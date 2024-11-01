#include "hu_entry.h"
#include "utools.h"
#include "router.h"
#include "usart.h"

void hu_entry(void)
{
    // 1、配置日志打印为uart1
    auto print_func = [](const char *message)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    };
    utlog::bind_print(print_func);
    utlog::start_async(512);

    // 2、初始化文件系统
    // TODO: 文件系统初始化

    // 3、启动线程池
    // if (!router::taskpool.is_running())
    {
        router::taskpool.start(3, 1024); // 启动线程池
    }

    // 4、配置路由
    // if (!router::pubsub.is_running())
    {
        router::pubsub.start(2048); // 启动发布订阅模式
    }
}