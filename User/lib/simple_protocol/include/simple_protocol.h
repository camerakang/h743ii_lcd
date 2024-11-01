#ifndef __SAMPLE_PROTOCOL_H__
#define __SAMPLE_PROTOCOL_H__

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
class SimpleProtocol : public SimpleProtocolImpl
{
private:
    const std::vector<uint8_t> __sync_header;
    const uint32_t __sync_header_size{static_cast<uint32_t>(__sync_header.size())}; // 同步头长度
    const uint32_t __data_len_size;                                                 // 数据长度长度
    const uint32_t __channel_size;                                                  // 同步通道长度
    const uint32_t __frame_len_size{__data_len_size};                               // 帧长度长度
    const bool __en_crc;                                                            // 是否启用CRC校验
    const uint32_t __max_frame_len;                                                 // 最大帧长度
    const uint32_t __timeout_ms{0};                                                 // 接收超时时间，单位毫秒

    const uint32_t __header_size{static_cast<uint32_t>(__sync_header.size() + __data_len_size + __channel_size)}; // 包关长度
    const uint32_t __crc16_size{static_cast<uint32_t>(__en_crc ? 2 : 0)};                                         // 校验和长度

    std::vector<uint8_t> __recv_buffer; // 接收数据的缓存

    uint64_t __prev_update_time_stamp{0}; // 上一次更新时间戳
public:
    SimpleProtocol &push_back(const uint8_t *data, size_t size) override;

    bool buffer_is_empty() const override;

    void buffer_flush() override;

    const frame_t parse() override;

    const std::vector<uint8_t> pack(const void *data, const size_t data_len, const uint32_t channel = 1) override;

    packer_t make_packer(const uint32_t channel, const size_t data_len = 16) override;

    SimpleProtocol() = delete;

    /// @brief 构造函数
    /// @param sync_header 同步头
    /// @param data_len_size 长度描述类型，最大长度为4字节
    /// @param channel_size 通道描述类型，最大长度为2字节
    /// @param en_crc 是否启用CRC校验，如果开启，将对 “FRAME_LEN | CHANNEL | DATA ”数据进行校验
    /// @param max_frame_len 最大包长度，包容长度“[SYNC_HEADER | FRAME_LEN | CHANNEL | DATA | CRC16]”
    SimpleProtocol(std::vector<uint8_t> sync_header = {0xAA, 0xAB},
                   const uint8_t data_len_size = 1,
                   const uint8_t channel_size = 1,
                   const bool en_crc = true,
                   const uint32_t max_frame_len = 256,
                   const uint32_t timeout_ms = 1000);

    virtual ~SimpleProtocol();

private:
    /// @brief 重新初始化缓存，将缓存中已解析的数据移除
    /// @return 返回一个帧数据，包含状态和数据
    /// @note 在进行此操作时，需要保证数据的同步头在缓存的最前面
    std::vector<uint8_t> __reinit_recv_buffer_header();
};

#endif // __SAMPLE_PROTOCOL_H__
