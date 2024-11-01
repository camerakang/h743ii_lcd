#ifndef __UTOOLS_EVENT_BUS_H__
#define __UTOOLS_EVENT_BUS_H__

#include "../utools_cfg.h"

#if UTOOLS_EVENT_BUS_ENABLE
#if UTOOLS_COLLAB_TASK_ENABLE
#include <unordered_map>
#include <list>
#include <functional>
#include <memory>
#include "../collab/utmutex.h"
#include "../collab/collab_task.h"
#include "../collab/sync_queue.h"
#else
#error "EventBuS requires UTOOLS_COLLAB_TASK_ENABLE."
#endif
#endif

namespace utools::pattern
{
#if (UTOOLS_COLLAB_TASK_ENABLE && UTOOLS_EVENT_BUS_ENABLE)

    /// @brief 异步发布订阅模式
    /// @tparam EVENT_TYPE 主题类型，需要支持 == 操作符的类型
    /// @tparam MESSAGE_TYPE 消息类型，默认为std::shared_ptr<void>，可以指定为其他类型
    /// @note 采用异步队列方式实现，保证线程安全
    /// @note 为了保存发布时间和订阅调用顺序，采用队列与单线程实现
    template <typename EVENT_TYPE, typename MESSAGE_TYPE = std::shared_ptr<void>, int32_t STACK_SIZE = 1024 * 6>
    class EventBus
    {
    public:
        typedef EVENT_TYPE event_type;
        typedef MESSAGE_TYPE message_type;
        /// @brief 回调函数类型，接受消息主题、消息内容
        typedef std::function<void(const event_type &, message_type)> callback_type;

        /// @brief 构造函数
        /// @param auto_start 是否自动启动，默认为false
        EventBus(bool auto_start = false)
        {
            if (auto_start)
            {
                start();
            }
        }

        virtual ~EventBus()
        {
            stop();
        }

        /// @brief 开始处理队列中的事件
        void start()
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            if (__running)
            {
                return;
            }
            __running = true;
            ::utools::collab::Task(&EventBus::__process_queue, this).detach(STACK_SIZE);
        }

        /// @brief 判断是否正在处理队列中的事件
        /// @return true 正在处理，false 未处理
        bool is_running() const
        {
            return __running;
        }

        /// @brief 停止处理队列中的事件
        void stop()
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            if (!__running)
            {
                return;
            }
            __running = false;
            __task_queue.notify_one(); // 唤醒线程以便它可以安全退出
        }

        /// @brief 触发事件
        /// @param event 事件主题
        /// @param message 事件消息
        void publish(const event_type &event, message_type &&message)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            auto it = __topics.find(event);
            if (it != __topics.end())
            {
                for (auto &entry : it->second)
                {
                    auto task = [entry, event, message]
                    {
                        entry.callback(event, message);
                    };
                    __task_queue.emplace(task);
                }
            }
        }

        /// @brief 订阅消息
        /// @param event 事件主题
        /// @param callback 处理消息的回调函数
        /// @return 订阅ID
        int subscribe(const event_type &event, callback_type callback)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            int id = ++__current_id;
            __topics[event].emplace_back(callback_entry_t{id, callback});
            return id;
        }

        /// @brief 取消订阅
        /// @param event 事件主题
        /// @param callback_id 订阅ID
        void unsubscribe(const event_type &event, int callback_id)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            auto it = __topics.find(event);
            if (it != __topics.end())
            {
                auto &entries = it->second;
                entries.remove_if([callback_id](const callback_entry_t &entry)
                                  { return entry.id == callback_id; });
            }
        }

    private:
        void __process_queue()
        {
            while (__running)
            {
                __task_queue.pop_wait()();
            }
        }

        struct callback_entry_t
        {
            int id;
            callback_type callback;
        };

        std::unordered_map<event_type, std::list<callback_entry_t>> __topics;
        int __current_id = 0;

        ::utools::collab::SyncQueue<std::function<void()>> __task_queue; // 同步任务队列
        ::utools::collab::Mutex __queue_mutex;                           // 保护任务队列的互斥量
        bool __running;                                                  // 控制线程的运行状态
    };
#endif // (UTOOLS_COLLAB_TASK_ENABLE  && UTOOLS_EVENT_BUS_ENABLE)
}

#endif // __UTOOLS_EVENT_BUS_H__