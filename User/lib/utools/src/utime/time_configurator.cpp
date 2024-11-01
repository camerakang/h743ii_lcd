/// @brief 时间功能配置器
///        为了适应不同的系统，需要对时间函数进行重新配置，本文件提供了默认配置

#include "../utools_cfg.h"
#include "time_utils.h"
#include <chrono>

#if UTOOLS_NEWLIB__GETTOPICTIME_ENABLE == UTOOLS_TRUE
#include <sys/time.h>

extern "C" int _gettimeofday(struct timeval *tv, void *tzvp)
{
    uint64_t time_us = static_cast<uint64_t>(
        ::utools::time::unix_ts<std::chrono::microseconds>()); // 获取当前的Tick计数

    if (tv)
    {
        tv->tv_sec = time_us / 1000000;  // 转换为秒
        tv->tv_usec = time_us % 1000000; // 剩余的微秒
    }
    return 0;
}
#endif // UTOOLS_NEWLIB__GETTOPICTIME_ENABLE

#if UTOOLS_NEWLIB_SLEEP_ENABLE == UTOOLS_TRUE
extern "C" void usleep(unsigned int useconds)
{
    if (useconds > 0)
    {
        ::utools::time::usleep(useconds);
    }
}

extern "C" unsigned int sleep(unsigned int seconds)
{
    if (seconds > 0)
    {
        ::utools::time::sleep(seconds);
    }
    return 0;
}
#endif // UTOOLS_NEWLIB_SLEEP_ENABLE
