#include "sdcp_shcfg.h"
#include "sdcp_def.h"
#include <chrono>

sdcp::IOImpl *sdcp::SHCFG::__io_impl;

std::vector<uint8_t> sdcp::SHCFG::__sync_header = std::vector<uint8_t>{{0xAA, 0xAA}}; // 用具体的数据初始化

uint16_t sdcp::SHCFG::__max_packet_size = SDCP_MAX_PACKET_SIZE; // 假设最大包大小为1024字节

int64_t sdcp::SHCFG::__timeout_ms = SDCP_FRAME_TIMEOUT_TS_MS; // 假设超时时间为5000毫秒

uint16_t sdcp::SHCFG::__max_retry_times = SDCP_FRAME_RETRY_TIMES; // 假设最大重试次数为3次

uint16_t sdcp::SHCFG::__msgid_generator = 0; // 消息ID生成器初始化为0，每次生成消息ID时应该递增

std::function<const int64_t()> sdcp::SHCFG::__ts_ms = []() -> const int64_t
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
};