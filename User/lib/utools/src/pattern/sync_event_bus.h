#ifndef __UTOOLS_SYNC_EVENT_BUS_H__
#define __UTOOLS_SYNC_EVENT_BUS_H__

#include "../utools_cfg.h"

#if UTOOLS_SYNC_EVENT_BUS_ENABLE
#include <unordered_map>
#include <list>
#include <functional>
#include <memory>
#endif // UTOOLS_SYNC_EVENT_BUS_ENABLE

namespace utools::pattern
{
#if UTOOLS_SYNC_EVENT_BUS_ENABLE
    /// @brief 增加同步发布订阅模式，消息发布者与订阅者之间无依赖关系，
    ///        发布者发布消息，订阅者订阅消息，发布者无需知道订阅者是谁，订阅者无需知道发布者是谁
    ///        消息发布者发布消息后，同步接收到所有订阅者的响应，发布者可以等待订阅者响应完成后再继续执行
    /// @tparam EVENT_TYPE 消息主题类型
    /// @tparam MESSAGE_TYPE 消息类型，默认为std::shared_ptr<void>，可以指定为其他类型
    /// @note 发布者发布消息后，订阅者响应完成前，发布者阻塞等待，非线程安全的
    template <typename EVENT_TYPE, typename MESSAGE_TYPE = std::shared_ptr<void>>
    class SyncEventBus
    {
    public:
        typedef EVENT_TYPE event_type;
        typedef MESSAGE_TYPE message_type;
        typedef std::function<void(const event_type &, message_type)> callback_type;

        SyncEventBus() {};
        virtual ~SyncEventBus() = default;

        /// @brief 发布消息，将每个订阅的回调函数调用，传递数据
        /// @param topic 消息主题
        /// @param data 消息数据
        void publish(const event_type &topic, message_type &&message)
        {
            auto it = __topics.find(topic);
            if (it != __topics.end())
            {
                for (auto &entry : it->second)
                {
                    entry.callback(topic, message);
                }
            }
        }

        /// @brief 订阅消息主题
        /// @param topic 消息主题
        /// @param callback 回调函数
        int subscribe(const event_type &topic, callback_type callback)
        {
            int id = ++__current_id;
            __topics[topic].emplace_back(callback_entry_t{id, callback});
            return id;
        }

        /// @brief 取消订阅消息主题
        /// @param topic 消息主题
        /// @param callback_id 回调函数ID
        void unsubscribe(const event_type &topic, const int callback_id)
        {
            auto it = __topics.find(topic);
            if (it != __topics.end())
            {
                auto &entries = it->second;
                entries.remove_if([callback_id](const callback_entry_t &entry)
                                  { return entry.id == callback_id; });
            }
        }

    private:
        struct callback_entry_t
        {
            int id;
            callback_type callback;
        };

        std::unordered_map<event_type, std::list<callback_entry_t>> __topics;
        int __current_id = 0;
    };
#endif // UTOOLS_SYNC_EVENT_BUS_ENABLE
}

#endif // __UTOOLS_SYNC_EVENT_BUS_H__