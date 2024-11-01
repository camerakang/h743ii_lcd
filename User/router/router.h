/// @brief Router，用于处理路由请求

#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "utools.h"
#include "routering_table.h"
#include "global_def.h"
#include "radio_comm.h"

class router
{
public:
    /// @brief 路由表，不同的数据总线，采用同一个路由表
    enum class table_type
    {
#define RUTERINT_TABLE_X(name) name = routing_table::name,
        RUTERING_TABLE_VALUES
#undef RUTERINT_TABLE_X
    };

    /// @brief 映射函数，用于将枚举类型转换为字符串
    /// @param e 枚举类型
    /// @return 字符串
    static const char *router_name(::router::table_type e)
    {
        switch (e)
        {
#define RUTERINT_TABLE_X(name)       \
    case ::router::table_type::name: \
        return #name;
            RUTERING_TABLE_VALUES
#undef RUTERINT_TABLE_X
        default:
            return "UNKONWN";
        }
    }

    /// @brief 共享缓存，用户保存状态数据
    inline static utmem::SharedCacheSafety<table_type> scache;

    /// @brief 发布订阅模式，用于通知状态变化，记录总线上最新状态，支持读写
    /// @note pubsub的回调函数采用方面的方式定义
    /// @note void callback_func(const router::topic_type &topic, router::message_wrapper_type msgwrapper)
    /// @note 在回调函数中，msgwrapper.cast<T>()获取消息，T为消息类型
    /// @note 在回调函数中，也可以通过msgwrapper.ptr获取消息指针，然后自行转换
    inline static utpattern::PubSub<table_type> pubsub{};
    typedef utpattern::PubSub<table_type>::topic_type topic_type;
    typedef utpattern::PubSub<table_type>::message_type message_type;
    typedef utpattern::PubSub<table_type>::message_wrapper_type message_wrapper_type;

    /// @brief 任务池，用于处理任务
    inline static utcollab::TaskPool taskpool{};

    /// @brief 无线电通信功能
    inline static RadioComm radio{};
};

#endif // __ROUTER_H__
