#ifndef __SIMPLE_PROTOCOL_IMPL_H__
#define __SIMPLE_PROTOCOL_IMPL_H__

#include <cstdint>
#include <vector>
#include <type_traits>
#include <string>
#include <chrono>
#include <array>
#include <cstring>

#define SMP_PROTOCOL_TIMESTAMP_NULL 0      // 不支持时间戳
#define SMP_PROTOCOL_TIMESTAMP_STD_CPP 1   // 使用std::chrono::system_clock::now()获取时间戳
#define SMP_PROTOCOL_TIMESTAMP_ARDUINO 2   // 使用millis()获取时间戳
#define SMP_PROTOCOL_TIMESTAMP_FREERTOS 3  // 使用get_tick_count()获取时间戳
#define SMP_PROTOCOL_TIMESTAMP_STM32_HAL 4 // 使用HAL_GetTick()获取时间戳

// 时间戳函数
#ifndef SMP_PROTOCOL_TIMESTAMP_FUNC
#define SMP_PROTOCOL_TIMESTAMP_FUNC SMP_PROTOCOL_TIMESTAMP_STD_CPP
#endif

#if SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_ARDUINO
#include <Arduino.h>
#elif SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#elif SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_STM32_HAL
#if defined(STM32F0xx)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1xx)
#include "stm32f1xx_hal.h"
#elif defined(STM32F2xx)
#include "stm32f2xx_hal.h"
#elif defined(STM32F3xx)
#include "stm32f3xx_hal.h"
#elif defined(STM32F4xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F7xx)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0xx)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4xx)
#include "stm32g4xx_hal.h"
#elif defined(STM32H7xx)
#include "stm32h7xx_hal.h"
#elif defined(STM32L0xx)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1xx)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4xx)
#include "stm32l4xx_hal.h"
#elif defined(STM32L5xx)
#include "stm32l5xx_hal.h"
#elif defined(STM32U5xx)
#include "stm32u5xx_hal.h"
#elif defined(STM32WBxx)
#include "stm32wbxx_hal.h"
#elif defined(STM32WLxx)
#include "stm32wlxx_hal.h"
#else
#error "Unsupported STM32 series"
#endif
#endif

class SimpleProtocolImpl
{
public:
    /// @brief 数据包状态
    enum class FrameStatus
    {
        BUFFER_EMPTY = 0,      // 缓冲区为空
        BUFFER_INVALID = 1,    // 无效缓冲区，没有可以使用的数据
        BUFFER_TIMEOUT = 2,    // 缓冲区超时，连续一定时间没有接收到数据，但在缓存中有数据
        BUFFER_BROKEN = 3,     // 一帧数据同步头前面有未知意义的缓存
        BUFFER_NOT_ENOUGH = 4, // 缓冲区长度不足
        LENGTH_NOT_ENOUGH = 5, // 帧长度不足
        LENGTH_ERROR = 6,      // 帧长度错误
        CRC_ERROR = 7,         // 帧CRC错误
        INVALID = 8,           // 帧无效
        VALID = 9,             // 帧有效
    };

    /// @brief 解析出来的一帧数据
    struct frame_t
    {
        FrameStatus status;        // 当前帧的状态
        std::uint32_t channel;     // 通道号
        std::vector<uint8_t> data; // 帧数据

        /// @brief 将当前的帧数据转换为字符串
        /// @return 返回当前帧数据转换为字符串
        const std::string str_data()
        {
            return std::string{data.begin(), data.end()};
        }

        /// @brief 判断当前帧是否有效
        /// @return true: 有效，false: 无效
        bool is_valid() const
        {
            return status == FrameStatus::VALID;
        }

        frame_t(FrameStatus status, std::uint32_t channel, std::vector<uint8_t> data) : status(status), channel(channel), data(std::move(data))
        {
        }

        frame_t(const frame_t &&other)
        {
            status = other.status;
            channel = other.channel;
            data = std::move(other.data);
        }

        frame_t &operator=(const frame_t &&other)
        {
            status = other.status;
            channel = other.channel;
            data = std::move(other.data);
            return *this;
        }

        frame_t(const frame_t &other)
        {
            status = other.status;
            channel = other.channel;
            data = other.data;
        }

        frame_t &operator=(const frame_t &other)
        {
            status = other.status;
            channel = other.channel;
            data = other.data;
            return *this;
        }

        frame_t() = default;
    };

    /// @brief 打包器对象，提供给使用者更方面的进行数据打包
    struct packer_t
    {
    public:
        packer_t() = delete;
        packer_t(const std::vector<uint8_t> sync_header,
                 uint32_t channel, uint32_t channel_size,
                 uint32_t data_len_size,
                 size_t size,
                 bool crc_enable) : __need_crc16(crc_enable)
        {
            __data.reserve(size);
            __data.insert(__data.end(), sync_header.begin(), sync_header.end()); // 写入同步头
            for (size_t i = 0; i < data_len_size; ++i)
            {
                __data.push_back(0); // 写入数据长度
            }
            this->push_back(&channel, channel_size); // 写入通道号
            __sync_header_size = sync_header.size();
            __data_len_size = data_len_size;
        }

        packer_t(const packer_t &other) : __data(other.__data) {}

        packer_t(const packer_t &&other) : __data(std::move(other.__data)) {}

        packer_t &operator=(const packer_t &other)
        {
            __data = other.__data;
            return *this;
        }

        packer_t &operator=(const packer_t &&other)
        {
            __data = std::move(other.__data);
            return *this;
        }

        packer_t &operator=(const std::vector<uint8_t> &data)
        {
            __data = data;
            return *this;
        }

        packer_t &operator=(const std::vector<uint8_t> &&data)
        {
            __data = std::move(data);
            return *this;
        }

        /// @brief 写入数据
        /// @param data 数据
        /// @param size 数据大小
        /// @return (packer_t &)*this
        packer_t &push_back(const void *data, uint32_t size)
        {
            __data.insert(__data.end(),
                          reinterpret_cast<const uint8_t *>(data),
                          reinterpret_cast<const uint8_t *>(data) + size);
            return *this;
        }

        /// @brief 写入数据
        /// @param data 数据(uint8_t)
        /// @return (packer_t &)*this
        packer_t &push_back(const uint8_t data)
        {
            __data.push_back(data);
            return *this;
        }

        /// @brief 写入数据
        /// @param data 数据(uint16_t)
        /// @return (packer_t &)*this
        packer_t &push_back(const uint16_t data)
        {
            __data.insert(__data.end(),
                          reinterpret_cast<const uint8_t *>(&data),
                          reinterpret_cast<const uint8_t *>(&data) + 2);
            return *this;
        }

        /// @brief 写入数据
        /// @param data 数据(uint32_t)
        /// @return (packer_t &)*this
        packer_t &push_back(const uint32_t data)
        {
            __data.insert(__data.end(),
                          reinterpret_cast<const uint8_t *>(&data),
                          reinterpret_cast<const uint8_t *>(&data) + 4);
            return *this;
        }

        /// @brief 写入数据
        /// @param data 数据(std::vector<uint8_t>)
        /// @return (packer_t &)*this
        packer_t &push_back(const std::vector<uint8_t> &data)
        {
            __data.insert(__data.end(), data.begin(), data.end());
            return *this;
        }

        /// @brief 写入数据
        /// @param arra 标准库中的数据
        /// @return (packer_t &)*this
        template <typename ARR_DATA_TYPE, size_t ARR_LEN>
        packer_t &push_back(const std::array<ARR_DATA_TYPE, ARR_LEN> arr)
        {
            __data.insert(__data.end(),
                          reinterpret_cast<const uint8_t *>(arr.data()),
                          reinterpret_cast<const uint8_t *>(arr.data() + ARR_LEN));
            return *this;
        }

        /// @brief 写入数据
        /// @tparam T 写入的数据类型
        /// @param data 数据
        /// @return (packer_t &)*this
        template <typename T>
        packer_t &push_back(const T &data)
        {
            return push_back(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }

        /// @brief 获取数据
        /// @return 已经写入数据的对象
        std::vector<uint8_t> &operator()()
        {
            return data();
        }

        /// @brief 获取数据
        /// @return 已经写入数据的对象
        std::vector<uint8_t> &data()
        {
            return __data;
        }

        /// @brief 生成打包数据
        /// @return 打包后的数据，会根据是否有crc校验自动添加crc校验数据
        packer_t &end_pack()
        {
            size_t data_len{__data.size() - __sync_header_size - __data_len_size};
            if (__need_crc16)
            {
                data_len += 2;
            }
            std::copy((uint8_t *)&data_len, (uint8_t *)&data_len + __data_len_size, __data.begin() + __sync_header_size);
            if (__need_crc16)
            {
                auto crc{ crc16(__data.begin() + __sync_header_size, __data.end()) };
                __data.insert(__data.end(), (uint8_t *)&crc, (uint8_t *)&crc + 2);
            }
            return *this;
        }

    private:
        std::vector<uint8_t> __data{}; // 缓存区
        size_t __sync_header_size{2};  // 同步头大小
        size_t __data_len_size{1};     // 数据长度大小
        bool __need_crc16{true};       // 是否需要校验
    };

public:
    /// @brief 将接收到的数据写入到缓存中
    /// @param data 接收到的数据
    /// @return 返回自身引用
    SimpleProtocolImpl &push_back(const std::vector<uint8_t> &data)
    {
        return push_back(data.data(), data.size());
    }

    /// @brief 将接收到的数据写入到缓存中
    /// @param data 接收到的数据
    /// @param size 数据大小
    /// @return 返回自身引用
    virtual SimpleProtocolImpl &push_back(const uint8_t *data, size_t size) = 0;

    /// @brief 解析数据包
    /// @return 返回解析结果
    virtual const frame_t parse() = 0;

    /// @brief 解析数据包
    /// @param data 接收到的数据
    /// @return 返回解析结果
    virtual const frame_t parse(const std::vector<uint8_t> &data)
    {
        if (!data.empty())
        {
            push_back(data);
        }
        return parse();
    }

    /// @brief 解析数据包
    /// @param data 接收到的数据
    /// @param size 数据大小
    /// @return 返回解析结果
    virtual const frame_t parse(const uint8_t *data, size_t size)
    {
        if (data && size > 0)
        {
            push_back(data, size);
        }
        return parse();
    }

    /// @brief 解析数据包，直到解析到一个包可用的数据
    /// @return 返回解析结果，如果没有接到有可用的数据，将返回最后一次处理时的错误信息
    virtual const frame_t parse_until_valid()
    {
        while (true)
        {
            auto frame{parse()};
            if (frame.status == FrameStatus::VALID || !can_parsed(frame.status))
            {
                return frame;
            }
        }
    }

    /// @brief 解析n个数据包
    /// @param n 需要解析的数据包数量
    /// @return 返回解析结果，最大返回为n个数据包，最少为0个数据包
    virtual const std::vector<frame_t> parse_n(size_t n = 2)
    {
        std::vector<frame_t> frames;
        while (n-- > 0)
        {
            frame_t frame = parse();
            if (!can_parsed(frame.status))
            {
                break;
            }
            frames.push_back(frame);
        }
        return frames;
    }

    /// @brief 解析所有数据包
    /// @return 返回解析结果
    virtual const std::vector<frame_t> parse_all()
    {
        return parse_n(std::numeric_limits<size_t>::max());
    }

    /// @brief 是否需要继续解析数据
    /// @param prev_status 上一次解析的数据包的状态
    /// @return true：需要继续解析数据，false：不需要继续解析数据
    virtual inline bool can_parsed(const FrameStatus &prev_status)
    {
        return !(prev_status == FrameStatus::BUFFER_EMPTY ||
                 prev_status == FrameStatus::BUFFER_INVALID ||
                 prev_status == FrameStatus::BUFFER_TIMEOUT ||
                 prev_status == FrameStatus::BUFFER_NOT_ENOUGH ||
                 prev_status == FrameStatus::LENGTH_NOT_ENOUGH);
    }

    /// @brief 生成一个数据包
    /// @param data 需要打包的数据
    /// @param data_len 数据长度
    /// @param channel 通道号
    /// @return 打包后的数据
    /// @note 打包后的数据包含：同步头、长度信息、通道信息、数据、CRC16
    virtual const std::vector<uint8_t> pack(const void *data, const size_t data_len, const uint32_t channel) = 0;

    /// @brief 打包器生成器
    /// @param channel 发送数据的通道
    /// @param data_len 数据长度，默认16
    /// @return 打包器对象
    virtual packer_t make_packer(const uint32_t channel, const size_t data_len = 16) = 0;

    /// @brief 判断当前缓存是否为空
    /// @return 返回当前缓存是否为空
    virtual bool buffer_is_empty() const
    {
        return false;
    }

    /// @brief 清空缓存区域
    virtual void buffer_flush() {};

    const std::vector<uint8_t> pack(const std::vector<uint8_t> &data, const uint32_t channel = 1)
    {
        return pack(data.data(), data.size(), channel);
    }

    const std::vector<uint8_t> pack(const std::string &data, const uint32_t channel = 1)
    {
        return pack((void *)data.data(), data.size(), channel);
    }

    const std::vector<uint8_t> pack(const uint32_t channel, const void *data, const size_t data_len)
    {
        return pack(data, data_len, channel);
    }

    const std::vector<uint8_t> pack(const uint32_t channel, const std::vector<uint8_t> &data)
    {
        return pack(channel, data.data(), data.size());
    }

    const std::vector<uint8_t> pack(const uint32_t channel, const std::string &data)
    {
        return pack(channel, data);
    }

    /// @brief 获取当时时间戳
    /// @return 当前时间戳，单位毫秒
    virtual const uint64_t curr_timestamp_ms()
    {
#if SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_STD_CPP
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#elif SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_ARDUINO
        return millis();
#elif SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_FREERTOS
        return xTaskGetTickCount();
#elif SMP_PROTOCOL_TIMESTAMP_FUNC == SMP_PROTOCOL_TIMESTAMP_STM32_HAL
        return HAL_GetTick();
#else
        return 0;
#endif
    }

    SimpleProtocolImpl() = default;

    virtual ~SimpleProtocolImpl() = default;

    static uint16_t crc16(const void *buffer, size_t buffer_length)
    {
        static const uint8_t __table_crc_hi[]{
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

        static const uint8_t __table_crc_lo[]{
            0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
            0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
            0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
            0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
            0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
            0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
            0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
            0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
            0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
            0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
            0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
            0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
            0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
            0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
            0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
            0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
            0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
            0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
            0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
            0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
            0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
            0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
            0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
            0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
            0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
            0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

        uint8_t crc_hi = 0xFF; // 高位CRC byte 初始化
        uint8_t crc_lo = 0xFF; // 低位CRC byte 初始化
        unsigned int i;

        uint8_t *ucbuf = (uint8_t *)buffer;

        while (buffer_length--)
        {
            i = crc_hi ^ *ucbuf++; // 计算CRC
            crc_hi = crc_lo ^ __table_crc_hi[i];
            crc_lo = __table_crc_lo[i];
        }

        return (crc_hi << 8 | crc_lo);
    }

    static uint16_t crc16(const std::vector<uint8_t> &data)
    {
        return crc16(data.data(), data.size());
    }

#if __cplusplus >= 201703L
    // 辅助函数模板，用于检查是否存在某个成员函数
    template <typename T, typename = void>
    struct has_dereference : std::false_type
    {
    };

    template <typename T>
    struct has_dereference<T, std::void_t<decltype(std::declval<T &>().operator*())>> : std::true_type
    {
    };

    template <typename T, typename = void>
    struct has_increment : std::false_type
    {
    };

    template <typename T>
    struct has_increment<T, std::void_t<decltype(std::declval<T &>().operator++())>> : std::true_type
    {
    };

    template <typename T, typename = void>
    struct has_decrement : std::false_type
    {
    };

    template <typename T>
    struct has_decrement<T, std::void_t<decltype(std::declval<T &>().operator--())>> : std::true_type
    {
    };

    // 判断是否为有序迭代器
    template <typename T>
    struct is_bidirectional_iterator
        : std::conjunction<has_dereference<T>, has_increment<T>, has_decrement<T>>
    {
    };

    template <typename _IT_TYPE,
              typename = std::enable_if<is_bidirectional_iterator<_IT_TYPE>::value>>
    static uint16_t crc16(const _IT_TYPE &begin, const _IT_TYPE &end)
    {
        return crc16(&*begin, end - begin);
    }
#else
    static uint16_t crc16(const std::vector<uint8_t>::iterator &begin, const std::vector<uint8_t>::iterator &end)
    {
        return crc16(&*begin, end - begin);
    }
#endif
};

#endif // __SIMPLE_PROTOCOL_IMPL_H__
