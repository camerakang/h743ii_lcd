#ifndef __SAMPLE_PROTOCOL_TPL_H__
#define __SAMPLE_PROTOCOL_TPL_H__

#include <string>
#include <cstdint>
#include <vector>
#include <queue>
#include <algorithm>
#include <array>

#include "simple_protocol_impl.h"

/// @brief 简易协议类型
/// @note 简易协议类型，用于定义简易协议的类型，包括同步头、通道描述类型、长度描述类型等。
/// @note [SYNC_HEADER | FRAME_LEN | CHANNEL | DATA | CRC16]
/// @note FRAME_LEN是CHANNEL、DATA和CRC16的长度；
/// @note CRC16是包含了FRAME_LEN、CHANNEL和DATA三个部分的CRC16校验值。
/// @tparam _CHANNEL_SIZE 通道描述类型，最大长度为4字节
/// @tparam _DATA_LEN_SIZE 长度描述类型，最大长度为4字节，每一次发送的最大数据包长度，不包含协议内容
/// @tparam _EN_CRC 是否启用CRC校验，如果开启，将对 “FRAME_LEN | CHANNEL | DATA ”数据进行校验
/// @tparam _TIMEOUT_MS 接收超时时间，单位毫秒，如果为0表示不使用此功能，默认为1000ms
/// @tparam _MAX_FRAME_LEN 最大包长度，包容长度“[SYNC_HEADER | FRAME_LEN | CHANNEL | DATA | CRC16]”
template <const uint8_t _DATA_LEN_SIZE = 1,
          const uint8_t _CHANNEL_SIZE = 1,
          const bool _EN_CRC = true,
          const uint32_t _MAX_FRAME_LEN = 256,
          const uint32_t _TIMEOUT_MS = 1000>
class SimpleProtocolTpl : public SimpleProtocolImpl
{
private:
    const std::vector<uint8_t> __sync_header;
    const uint32_t __sync_header_size{__sync_header.size()};                 // 同步头长度
    const uint32_t __channel_size{_CHANNEL_SIZE > 4 ? 4 : _CHANNEL_SIZE};    // 同步通道长度
    const uint32_t __data_len_size{_DATA_LEN_SIZE > 4 ? 4 : _DATA_LEN_SIZE}; // 数据长度长度
    const uint32_t __frame_len_size{__data_len_size};                        // 帧长度长度

    const uint32_t __header_size{__sync_header.size() + __data_len_size + __channel_size}; // 包关长度
    const uint32_t __crc16_size{_EN_CRC ? 2 : 0};                                          // 校验和长度

    std::vector<uint8_t> __recv_buffer; // 接收数据的缓存

    uint64_t __prev_update_time_stamp{0}; // 上一次更新时间戳
public:
    SimpleProtocolTpl &push_back(const uint8_t *data, size_t size) override
    {
        if constexpr (_TIMEOUT_MS > 0)
        {
            __prev_update_time_stamp = curr_timestamp_ms();
        }

        __recv_buffer.insert(__recv_buffer.end(), data, data + size);
        return *this;
    }

    bool buffer_is_empty() const override
    {
        return __recv_buffer.size() == 0;
    };

    void buffer_flush() override
    {
        __recv_buffer.clear();
    }

    const frame_t parse() override
    {
        // 1 处理缓存无效条件
        // 1.1 检测是否为空缓存
        if (__recv_buffer.size() == 0)
        {
            return {FrameStatus::BUFFER_EMPTY, 0, {}};
        }

        // 1.2 检查是否超时
        if constexpr (_TIMEOUT_MS > 0)
        {
            if (__prev_update_time_stamp + _TIMEOUT_MS < curr_timestamp_ms())
            {
                frame_t ret{FrameStatus::BUFFER_TIMEOUT, 0, std::vector<uint8_t>{__recv_buffer.begin(), __recv_buffer.end()}};
                __recv_buffer.clear();
                return ret;
            }
        }

        // 1.3 如果数据的长度太短，不能立刻处理
        if (__recv_buffer.size() < __header_size + __crc16_size)
        {
            return {FrameStatus::BUFFER_NOT_ENOUGH, 0, {}};
        }

        // 2 查找同步头
        auto start_it = std::search(__recv_buffer.begin(), __recv_buffer.end(), __sync_header.begin(), __sync_header.end());
        // 2.1 删除无同步头时的缓存
        if (start_it == __recv_buffer.end()) // 没找到同步头，清空缓存
        {
            frame_t ret{FrameStatus::BUFFER_INVALID, 0, std::vector<uint8_t>{__recv_buffer.begin(), __recv_buffer.end()}};
            __recv_buffer.clear();
            return ret;
        }

        // 2.2 移除同步头前面的任何数据
        size_t start = std::distance(__recv_buffer.begin(), start_it);
        if (start > 0)
        {
            frame_t ret{FrameStatus::BUFFER_BROKEN, 0, std::vector<uint8_t>{__recv_buffer.begin(), __recv_buffer.begin() + start}};
            __recv_buffer.erase(__recv_buffer.begin(), start_it);
            return ret;
        }

        // 3 检查数据长度是否足够读取长度信息，以进行下步的处理
        // 3.1 解析长度
        uint32_t length{0}; // 包体长度，CHANNEL、DATA和CRC16的长度
        memcpy(&length, &__recv_buffer[__sync_header_size], __frame_len_size);

        // 3.2 检查长度是否足够，如果不够长，返回，等待下次处理
        if (__recv_buffer.size() < __sync_header_size + __data_len_size + length)
        {
            return {FrameStatus::LENGTH_NOT_ENOUGH, 0, {}};
        }

        // 3.3 处理长度异常（如果超出理论最大值）
        if (length > (_MAX_FRAME_LEN - (__sync_header_size + __data_len_size)))
        {
            return {FrameStatus::LENGTH_ERROR, 0, __reinit_recv_buffer_header()};
        }

        // 4 提取数据
        // 4.1 解析通道
        uint32_t channel{0};
        memcpy(&channel, &__recv_buffer[__sync_header_size + __frame_len_size], __channel_size);

        // 4.2 解析数据
        auto data_len = length - __channel_size - __crc16_size;
        std::vector<uint8_t> frame_data{
            __recv_buffer.begin() + __sync_header_size + __frame_len_size + __channel_size,
            __recv_buffer.begin() + __sync_header_size + __frame_len_size + __channel_size + data_len};

        // 5 计算CRC
        if (_EN_CRC)
        {
            uint16_t crc{crc16(&__recv_buffer[__sync_header_size], __frame_len_size + __channel_size + data_len)};
            uint16_t received_crc{0};
            memcpy(&received_crc, &__recv_buffer[__sync_header_size + __frame_len_size + __channel_size + data_len], __crc16_size);
            if (crc != received_crc) // 检查CRC
            {
                return {FrameStatus::CRC_ERROR, 0, __reinit_recv_buffer_header()};
            }
        }

        // 6 数据有效，从缓冲区移除已解析的数据
        __recv_buffer.erase(__recv_buffer.begin(), __recv_buffer.begin() + __sync_header_size + __frame_len_size + length);

        return {FrameStatus::VALID, channel, frame_data};
    }

    const std::vector<uint8_t> pack(const void *data, const size_t data_len, const uint32_t channel = 1) override
    {
        std::vector<uint8_t> ret;
        ret.reserve(__sync_header_size + __frame_len_size + __channel_size + data_len + __crc16_size);
        // 1 添加同步头
        ret.insert(ret.end(), __sync_header.begin(), __sync_header.end());
        // 2 添加长度信息
        uint32_t length{data_len + __channel_size + __crc16_size};
        ret.insert(ret.end(), (uint8_t *)&length, (uint8_t *)&length + __frame_len_size);
        // 3 添加通道信息
        ret.insert(ret.end(), (uint8_t *)&channel, (uint8_t *)&channel + __channel_size);
        // 4 添加数据
        ret.insert(ret.end(), (uint8_t *)data, (uint8_t *)data + data_len);
        // 5 添加CRC
        if (_EN_CRC)
        {
            auto crc{crc16(ret.begin() + __sync_header_size, ret.end())};
            ret.insert(ret.end(), (uint8_t *)&crc, (uint8_t *)&crc + __crc16_size);
        }
        return ret;
    }

    packer_t make_packer(const uint32_t channel, const size_t data_len = 16) override
    {
        return packer_t(__sync_header,
                        channel, __channel_size, __data_len_size,
                        __sync_header_size + __frame_len_size + __channel_size + __crc16_size + data_len,
                        _EN_CRC);
    }

    /// @brief 构造函数
    /// @param sync_header 同步头
    SimpleProtocolTpl(std::vector<uint8_t> sync_header = {0xAA, 0xAB}) : __sync_header(std::move(sync_header))
    {
        __recv_buffer.reserve(_MAX_FRAME_LEN * 2);
    }

    virtual ~SimpleProtocolTpl()
    {
    }

private:
    /// @brief 重新初始化缓存，将缓存中已解析的数据移除
    /// @return 返回一个帧数据，包含状态和数据
    /// @note 在进行此操作时，需要保证数据的同步头在缓存的最前面
    std::vector<uint8_t> __reinit_recv_buffer_header()
    {
        std::vector<uint8_t> ret{};
        auto start_it = std::search(__recv_buffer.begin() + 1, __recv_buffer.end(), __sync_header.begin(), __sync_header.end()); // 跳过最前面的同步头搜索
        if (start_it == __recv_buffer.end())                                                                                     // 没找到新的同步头，清空缓存
        {
            ret.insert(ret.end(), __recv_buffer.begin(), __recv_buffer.end());
            __recv_buffer.clear();
        }
        else
        {
            ret.insert(ret.end(), __recv_buffer.begin(), start_it);
            __recv_buffer.erase(__recv_buffer.begin(), start_it);
        }
        return ret;
    }
};

#endif // __SAMPLE_PROTOCOL_H__
