#include "simple_protocol.h"

SimpleProtocol &SimpleProtocol::push_back(const uint8_t *data, size_t size)
{
    if (__timeout_ms > 0)
    {
        __prev_update_time_stamp = curr_timestamp_ms();
    }

    __recv_buffer.insert(__recv_buffer.end(), data, data + size);
    return *this;
}

bool SimpleProtocol::buffer_is_empty() const
{
    return __recv_buffer.size() == 0;
};

void SimpleProtocol::buffer_flush()
{
    __recv_buffer.clear();
}

const SimpleProtocol::frame_t SimpleProtocol::parse()
{
    // 1 处理缓存无效条件
    // 1.1 检测是否为空缓存
    if (__recv_buffer.size() == 0)
    {
        return {FrameStatus::BUFFER_EMPTY, 0, {}};
    }

    // 1.2 检查是否超时
    if (__timeout_ms > 0)
    {
        if (__prev_update_time_stamp + __timeout_ms < curr_timestamp_ms())
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
    if (length > (__max_frame_len - (__sync_header_size + __data_len_size)))
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
    if (__en_crc)
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

const std::vector<uint8_t> SimpleProtocol::pack(const void *data, const size_t data_len, const uint32_t channel)
{
    std::vector<uint8_t> ret;
    ret.reserve(__sync_header_size + __frame_len_size + __channel_size + data_len + __crc16_size);
    // 1 添加同步头
    ret.insert(ret.end(), __sync_header.begin(), __sync_header.end());
    // 2 添加长度信息
    uint32_t length{static_cast<uint32_t>(data_len + __channel_size + __crc16_size)};
    ret.insert(ret.end(), (uint8_t *)&length, (uint8_t *)&length + __frame_len_size);
    // 3 添加通道信息
    ret.insert(ret.end(), (uint8_t *)&channel, (uint8_t *)&channel + __channel_size);
    // 4 添加数据
    ret.insert(ret.end(), (uint8_t *)data, (uint8_t *)data + data_len);
    // 5 添加CRC
    if (__en_crc)
    {
        auto crc{crc16(ret.begin() + __sync_header_size, ret.end())};
        ret.insert(ret.end(), (uint8_t *)&crc, (uint8_t *)&crc + __crc16_size);
    }
    return ret;
}

SimpleProtocol::packer_t SimpleProtocol::make_packer(const uint32_t channel, const size_t data_len)
{
    return packer_t(__sync_header,
                    channel, __channel_size, __data_len_size,
                    __sync_header_size + __frame_len_size + __channel_size + __crc16_size + data_len,
                    __en_crc);
}

SimpleProtocol::SimpleProtocol(std::vector<uint8_t> sync_header,
                               const uint8_t data_len_size,
                               const uint8_t channel_size,
                               const bool en_crc,
                               const uint32_t max_frame_len,
                               const uint32_t timeout_ms)
    : __sync_header(std::move(sync_header)),
      __data_len_size(data_len_size > 4 ? 4 : data_len_size),
      __channel_size(channel_size > 4 ? 4 : channel_size),
      __en_crc(en_crc),
      __max_frame_len(max_frame_len),
      __timeout_ms(timeout_ms)
{
    __recv_buffer.reserve(max_frame_len * 2);
}

SimpleProtocol::~SimpleProtocol()
{
}

std::vector<uint8_t> SimpleProtocol::__reinit_recv_buffer_header()
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
