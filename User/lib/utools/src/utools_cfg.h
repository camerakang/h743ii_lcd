/// @brief utools配置文件
/// @note 有ENABLE为结尾的宏，被定义为UTOOLS_TRUE时表示启用该功能，被定义为UTOOLS_FALSE时表示禁用该功能

#ifndef __UTOOLS_CFG_TEMPLATE_H__
#define __UTOOLS_CFG_TEMPLATE_H__

#include "utools_cfg_def.h"

// 如果用户自己定义了相关的配置文件，直接引入
#ifdef UTOOLS_USER_CONFIG_H
#include UTOOLS_USER_CONFIG_H
#elif __has_include("utools_config.h")
#include "utools_config.h"
#else
#warning "The user did not specify a configuration file for utools, and the default configuration will be used. \
 If you want to specify your own utools configuration, please write a configuration file called 'utools_config.h', \
 or configure your own configuration file through the 'UTOOLS_USER_CONFIG_H' macro definition."
#endif // UTOOLS_USER_CONFIG_H

// =======操作系统和硬件平台相关配置========
// 配置操作系统支持功能，如果没有定义操作系统，默认为无操作系统
#ifndef UTOOLS_OS_TYPE
#define UTOOLS_OS_TYPE UTOOLS_OS_TYPE_NULL
#endif // UTOOLS_OS_TYPE
// 配置无系统时所使用的嵌入式系统类型
#ifndef UTOOLS_EMBEDDED_PLATFORM
#define UTOOLS_EMBEDDED_PLATFORM UTOOLS_EMBEDDED_PLATFORM_ESP32
#endif // UTOOLS_EMBEDDED_PLATFORM
// 配置是否使用第三方的支持库
// TODO: 暂时没有实现
#ifndef UTOOLS_THIRD_PARTY_LIB_ENABLE
#define UTOOLS_THIRD_PARTY_LIB_ENABLE UTOOLS_FALSE
#endif // UTOOLS_THIRD_PARTY_LIB_ENABLE

// =======日志功能相关配置========
// UTOOSL::LOGGER功能开关
#ifndef UTOOLS_LOGGER_ENABLE
#define UTOOLS_LOGGER_ENABLE UTOOLS_TRUE
#endif // UTOOLS_LOGGER_ENABLE
// UTOOSL::LOGGER日志时间戳功能开关
#ifndef UTOOLS_LOGGER_TIMESTAMP_ENABLE
#define UTOOLS_LOGGER_TIMESTAMP_ENABLE UTOOLS_TRUE
#endif // UTOOLS_LOGGER_TIMESTAMP_ENABLE
// UTOOSL::LOGGER日志过滤功能开关
#ifndef UTOOLS_LOGGER_FILTER_ENABLE
#define UTOOLS_LOGGER_FILTER_ENABLE UTOOLS_FALSE
#endif // UTOOLS_LOGGER_FILTER_ENABLE
// UTOOSL::LOGGER日志模式开关，默认为同步模式
#ifndef UTOOLS_LOGGER_ASYNC_ENABLE
#define UTOOLS_LOGGER_ASYNC_ENABLE UTOOLS_FALSE
#endif // UTOOLS_LOGGER_MODE

// UTOOLS::COLAB协作相关功能开关
// 指定utools中使用的线程创建函数，用户可以不指定，只需要支持操作系统类型，utools会自动选择合适的线程创建函数
#ifndef UTOOLS_THREAD_FUNCATION
#if ((UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_CPP_STD) || (UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_WINDOWS))
#define UTOOLS_THREAD_FUNCATION UTOOLS_STD_THREAD_FUNCATION // CPP标准库和Windows使用std::thread库

#elif ((UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_LINUX) || (UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_MACOS))
#define UTOOLS_THREAD_FUNCATION UTOOLS_PTHREAD_FUNCATION // MacOS和Linux使用pthread库

#elif (UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_FREERTOS)
#define UTOOLS_THREAD_FUNCATION UTOOLS_FREERTOS_TASK_FUNCATION // FreeRTOS使用FreeRTOS的API
#ifndef UTOOLS_FREERTOS_INCLUDE_FREERTOS_H
#define UTOOLS_FREERTOS_INCLUDE_FREERTOS_H "FreeRTOS.h"
#endif // UTOOLS_FREERTOS_INCLUDE_FREERTOS_H
#ifndef UTOOLS_FREERTOS_INCLUDE_TASK_H
#define UTOOLS_FREERTOS_INCLUDE_TASK_H "task.h"
#endif // UTOOLS_FREERTOS_INCLUDE_TASK_H
#ifndef UTOOLS_FREERTOS_INCLUDE_SEMAPHORE_H
#define UTOOLS_FREERTOS_INCLUDE_SEMAPHORE_H "semphr.h"
#endif // UTOOLS_FREERTOS_INCLUDE_SEMAPHORE_H
#ifndef UTOOLS_FREERTOS_INCLUDE_TIMERS_H
#define UTOOLS_FREERTOS_INCLUDE_TIMERS_H "timers.h"
#endif // UTOOLS_FREERTOS_INCLUDE_TIMERS_H

#elif (UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_CMSIS_V2)
#define UTOOLS_THREAD_FUNCATION UTOOLS_OSTHREADNEW_FUNCATION // CMSIS V2使用CMSIS V2的API

#elif (UTOOLS_OS_TYPE == UTOOLS_OS_TYPE_NULL)
#define UTOOLS_THREAD_FUNCATION UTOOLS_THREAD_FUNCATION_NULL // 如果没有指定操作系统类型，则使用NULL线程创建函数

#endif
#endif // UTOOLS_THREAD_FUNCATION

/// @brief 纳秒延时功能
/// @warning 此功能使用软件实现，精度很低，不建议使用
#ifndef UTOOLS_NANOSECONDS_SLEEP_ENABLE
#define UTOOLS_NANOSECONDS_SLEEP_ENABLE UTOOLS_FALSE
#endif // UTOOLS_NANOSECONDS_SLEEP_ENABLE

// 指定支持的CPU核心数量，默认为单核
#ifndef UTOOLS_SUPPORTED_CUP_CORE_NUM
#define UTOOLS_SUPPORTED_CUP_CORE_NUM 1
#endif // UTOOLS_SUPPORTED_CUP_CORE_NUM

// utools::collab::Task功能开关
#ifndef UTOOLS_COLLAB_TASK_ENABLE
#if UTOOLS_OS_TYPE != UTOOLS_OS_TYPE_NULL
#define UTOOLS_COLLAB_TASK_ENABLE UTOOLS_TRUE
#else
#define UTOOLS_COLLAB_TASK_ENABLE UTOOLS_FALSE
#endif // UTOOLS_OS_TYPE != UTOOLS_OS_TYPE_NULL
#endif // UTOOLS_COLLAB_TASK_ENABLE

// utools::collab::Task默认栈大小
#ifndef UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE
#define UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE ((size_t)4096)
#endif // UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE

// utools::collab::Timer功能开关
#ifndef UTOOLS_COLLAB_TIMER_ENABLE
#define UTOOLS_COLLAB_TIMER_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_TIMER_ENABLE

// utools::collab::TaskPool功能开关
#ifndef UTOOLS_COLLAB_TASK_POOL_ENABLE
#define UTOOLS_COLLAB_TASK_POOL_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_TASK_POOL_ENABLE

// utools::collab::TaskSequence功能开关
#ifndef UTOOLS_COLLAB_TASK_SEQUENCE_ENABLE
#define UTOOLS_COLLAB_TASK_SEQUENCE_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_TASK_SEQUENCE_ENABLE

// utools::collab::func_wrapper功能开关
#ifndef UTOOLS_COLLAB_FUNC_WRAPPER_ENABLE
#define UTOOLS_COLLAB_FUNC_WRAPPER_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_FUNC_WRAPPER_ENABLE

// utools::collab::SpinWait功能开关
#ifndef UTOOLS_COLLAB_SPIN_WAIT_ENABLE
#define UTOOLS_COLLAB_SPIN_WAIT_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_SPIN_WAIT_ENABLE

// utools::collab::SyncPartner功能开关
#ifndef UTOOLS_COLLAB_SYNC_PARTNER_ENABLE
#define UTOOLS_COLLAB_SYNC_PARTNER_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_SYNC_PARTNER_ENABLE

// utools::collab::SyncQueue功能开关
#ifndef UTOOLS_COLLAB_SYNC_QUEUE_ENABLE
#define UTOOLS_COLLAB_SYNC_QUEUE_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_SYNC_QUEUE_ENABLE

// utools::collab::WaitNotify功能开关
#ifndef UTOOLS_COLLAB_WAIT_NOTIFY_ENABLE
#define UTOOLS_COLLAB_WAIT_NOTIFY_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_WAIT_NOTIFY_ENABLE

// utools::collab::FlowTask功能开关
#ifndef UTOOLS_COLLAB_FLOWTASK_ENABLE
#define UTOOLS_COLLAB_FLOWTASK_ENABLE UTOOLS_FALSE
#endif // UTOOLS_COLLAB_FLOWTASK_ENABLE

// utools::time相关功能开关
// utools::time::StableIntervalInvoker功能开关
#ifndef UTOOLS_STABLE_INTERVAL_INVOKER_ENABLE
#define UTOOLS_STABLE_INTERVAL_INVOKER_ENABLE UTOOLS_FALSE
#endif // UTOOLS_STABLE_INTERVAL_INVOKER_ENABLE

/// @note 编译时提示找不到usleep或sleep函数时，尝试开启此功能来解决问题
#ifndef UTOOLS_NEWLIB_SLEEP_ENABLE
#define UTOOLS_NEWLIB_SLEEP_ENABLE UTOOLS_FALSE
#endif // UTOOLS_NEWLIB_SLEEP_ENABLE

/// @note 编译时提示找不到_gettopictime或_gettopicdate函数时，尝试开启此功能来解决问题
#ifndef UTOOLS_NEWLIB__GETTOPICTIME_ENABLE
#define UTOOLS_NEWLIB__GETTOPICTIME_ENABLE UTOOLS_FALSE
#endif // UTOOLS_NEWLIB__GETTOPICTIME_ENABLE

// utools::pool相关功能开关
#ifndef UTOOLS_MEMPOLL_EANBLE
#define UTOOLS_MEMPOLL_EANBLE UTOOLS_FALSE
#endif // UTOOLS_MEMPOLL_EANBLE

// utools::about功能开关
#ifndef UTOOLS_ABOUT_ENABLE
#define UTOOLS_ABOUT_ENABLE UTOOLS_FALSE
#endif // UTOOLS_ABOUT_ENABLE

// 如果打开了utools::about功能，可以使用UTOOLS_ABOUT_INCLUDE来指定自己定义的about.h文件
// #define UTOOLS_ABOUT_INCLUDE "../../include/about.h"

// =======编码功能相关配置========
// utools::code::uuid功能开关
#ifndef UTOOLS_CODE_UUID_ENABLE
#define UTOOLS_CODE_UUID_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_UUID_ENABLE

// utools::code::base64功能开关
#ifndef UTOOLS_CODE_BASE64_ENABLE
#define UTOOLS_CODE_BASE64_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_BASE64_ENABLE

// utools::code::crc功能开关
#ifndef UTOOLS_CODE_CRC_ENABLE
#define UTOOLS_CODE_CRC_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_CRC_ENABLE

// utools::code::endian功能开关
#ifndef UTOOLS_CODE_ENDIAN_ENABLE
#define UTOOLS_CODE_ENDIAN_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_ENDIAN_ENABLE

// utools::code::hexstr功能开关
#ifndef UTOOLS_CODE_HEXSTR_ENABLE
#define UTOOLS_CODE_HEXSTR_ENABLE UTOOLS_TRUE
#endif // UTOOLS_CODE_HEXSTR_ENABLE

// utools::code::md5功能开关
#ifndef UTOOLS_CODE_MD5_ENABLE
#define UTOOLS_CODE_MD5_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_MD5_ENABLE

// utools::code::sha1功能开关
#ifndef UTOOLS_CODE_SHA1_ENABLE
#define UTOOLS_CODE_SHA1_ENABLE UTOOLS_FALSE
#endif // UTOOLS_CODE_SHA1_ENABLE

// utools::code::ByteStruct功能开关
#ifndef UTOOLS_BYTES_STRUCT_ENABLE
#define UTOOLS_BYTES_STRUCT_ENABLE UTOOLS_FALSE
#endif // UTOOLS_BYTES_STRUCT_ENABLE

// =======设计模式相关配置========
// utools::pattern::PubSub功能开关，异步，支持数据缓存
#ifndef UTOOLS_PUBSUB_ENABLE
#define UTOOLS_PUBSUB_ENABLE UTOOLS_FALSE
#endif // UTOOLS_PUBSUB_ENABLE

// utools::pattern::EventBus功能开关，异步，不支持数据缓存
#ifndef UTOOLS_EVENT_BUS_ENABLE
#define UTOOLS_EVENT_BUS_ENABLE UTOOLS_FALSE
#endif // UTOOLS_EVENT_BUS_ENABLE

// utools::pattern::SyncEventBus功能开关，同步，不支持数据缓存
#ifndef UTOOLS_SYNC_EVENT_BUS_ENABLE
#define UTOOLS_SYNC_EVENT_BUS_ENABLE UTOOLS_FALSE
#endif // UTOOLS_SYNC_EVENT_BUS_ENABLE

#endif // __UTOOLS_CFG_TEMPLATE_H__