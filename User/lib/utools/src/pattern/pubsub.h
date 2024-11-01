/// @brief 发布订阅模式
#ifndef __UTOOLS_PUBSUB_H__
#define __UTOOLS_PUBSUB_H__

#include "../utools_cfg.h"

#if UTOOLS_PUBSUB_ENABLE
#include <unordered_map>
#include <list>
#include <functional>
#include <memory>
#include <type_traits>
#include <stdint.h>

#if UTOOLS_COLLAB_TASK_ENABLE
#include "../collab/utmutex.h"
#include "../collab/collab_task.h"
#include "../collab/sync_queue.h"
#else
#error "AsyncPubSub or PubSub requires UTOOLS_COLLAB_TASK_ENABLE."
#endif // UTOOLS_COLLAB_TASK_ENABLE
#endif // UTOOLS_EVENT_BUS_ENABLE

#if UTOOLS_SYNC_EVENT_BUS_ENABLE

#endif // UTOOLS_EVENT_BUS_ENABLE

namespace utools::pattern
{
#if (UTOOLS_PUBSUB_ENABLE && UTOOLS_COLLAB_TASK_ENABLE)
    template <typename TOPIC_TYPE>
    class PubSub
    {
    public:
        typedef TOPIC_TYPE topic_type;
        typedef std::shared_ptr<void> message_type;

        /// @brief 消息包装器，用于提供给回调函数使用，方便数据解析
        typedef struct message_wrapper_type
        {
            message_type ptr; // 消息内容

            message_wrapper_type(const message_type message) : ptr(message) {}

            /// @brief 获取消息内容
            /// @tparam T 需要要解析的消息类型
            /// @return 消息内容
            template <typename T>
            T &cast()
            {
                return *std::static_pointer_cast<T>(ptr);
            }

            /// @brief 判断是否有消息内容
            bool has_value()
            {
                return ptr != nullptr;
            }
        } message_wrapper_type;

        // 回调函数类型，接受消息主题、消息内容
        typedef std::function<void(const topic_type &, message_wrapper_type)> callback_type;

        /// @brief 构造函数
        /// @param auto_start 是否自动启动，默认为false
        /// @param task_stack_size 任务栈大小，默认为UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE
        PubSub(bool auto_start = false, size_t task_stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE)
        {
            if (auto_start)
            {
                start(task_stack_size);
            }
        }

        virtual ~PubSub()
        {
            stop();
        }

        /// @brief 发布消息
        /// @tparam T 用户自定义消息类型
        /// @param topic 消息主题
        /// @param message 消息内容
        template <typename T>
        void publish(const topic_type &topic, T &&message)
        {
            decltype(__topics.find(topic)) it = __topics.end();
            {
                ::utools::collab::LockGuard lock(__callback_mutex);
                it = __topics.find(topic);
                if (it == __topics.end())
                {
                    __topics[topic].last_msg = std::make_shared<typename std::decay<T>::type>(std::forward<T>(message));
                    return; // 初次创建，没有订阅者，直接返回
                }

                it->second.last_msg = std::static_pointer_cast<void>(
                    std::make_shared<typename std::decay<T>::type>(std::forward<T>(message)));
            }
            auto store_msg = it->second.last_msg;
            for (auto &entry : it->second.callbacks)
            {
                auto task = [entry, topic, store_msg]
                {
                    entry.callback(topic, message_wrapper_type(store_msg));
                };
                __task_queue.emplace(task); // 将任务加入队列，并通知等待线程执行
            }
        }

        /// @brief 订阅消息
        /// @param topic 订阅的主题
        /// @param callback 回调函数
        /// @return 返回订阅ID
        /// @note 订阅ID可用于取消订阅
        virtual int32_t subscribe(const topic_type &topic, callback_type callback)
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            int32_t id = ++__current_id;
            __topics[topic].callbacks.emplace_back(callback_entry_t{id, callback});
            return id;
        }

        /// @brief 订阅消息
        /// @param topic 订阅的主题
        /// @param callback 回调函数
        /// @param obj 回调函数所属对象
        /// @return 返回订阅ID
        /// @note 订阅ID可用于取消订阅
        template <typename T>
        int32_t subscribe(const topic_type &topic,
                          void (T::*func)(const topic_type &, message_wrapper_type), T *obj)
        {
            std::function<void(const topic_type &, message_wrapper_type)> callback =
                std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);

            return subscribe(topic, callback);
        }

        /// @brief 取消订阅消息主题
        /// @param topic 消息主题
        /// @param callback_id 回调函数ID
        virtual void unsubscribe(const topic_type &topic, const int32_t callback_id)
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            auto it = __topics.find(topic);
            if (it != __topics.end())
            {
                auto &entries = it->second.callbacks;
                entries.remove_if([callback_id](const callback_entry_t &entry)
                                  { return entry.id == callback_id; });
            }
        }

        /// @brief 删除指定主题的所有订阅者
        /// @param topic 主题
        virtual void remove(const topic_type &topic)
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            __topics.erase(topic);
        }

        /// @brief 更新消息主题中的数据
        /// @param topic 主题
        /// @param message 消息
        template <typename T>
        void update(const topic_type &topic, T &&message)
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            auto it = __topics.find(topic);
            if (it != __topics.end())
            {
                it->second.last_msg = std::static_pointer_cast<void>(
                    std::make_shared<typename std::decay<T>::type>(std::forward<T>(message)));
            }
            else
            {
                __topics[topic].last_msg = std::make_shared<typename std::decay<T>::type>(
                    std::forward<T>(message));
            }
        }

        /// @brief 获取指定主题中的最新消息
        /// @param topic 主题
        /// @return 最新消息
        template <typename T>
        T &get(const topic_type &topic) const
        {
            return *std::static_pointer_cast<T>(__topics.at(topic).last_msg);
        }

        /// @brief 获取指定主题中的最新消息
        /// @param topic 主题
        /// @return 最新消息
        message_type get_raw(const topic_type &topic) const
        {
            auto it = __topics.find(topic);
            if (it != __topics.end())
            {
                return it->second.last_msg;
            }
            return std::shared_ptr<void>();
        }

        /// @brief 判断缓存中是否存在指定主题的数据
        /// @param key 键
        /// @return bool 存在返回true，否则返回false
        bool contains(const topic_type &topic) const
        {
            return __topics.find(topic) != __topics.end();
        }

        /// @brief 启动订阅服务
        /// @param task_stack_size 任务栈大小
        void start(size_t task_stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE)
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            if (__running)
            {
                return;
            }
            __running = true;
            __work_task.bind(&PubSub::__process_task_queue, this).start(task_stack_size);
        }

        /// @brief 停止订阅服务
        void stop()
        {
            ::utools::collab::LockGuard lock(__callback_mutex);
            if (!__running)
            {
                return;
            }
            __running = false;
            __task_queue.emplace([]
                                 { return; });
            __task_queue.notify_one();
            __work_task.stop();
        }

        /// @brief 判断订阅服务是否正在运行
        /// @return bool 返回true表示正在运行，否则返回false
        bool is_running() const
        {
            return __running;
        }

    protected:
        struct callback_entry_t
        {
            int32_t id;             // 订阅ID
            callback_type callback; // 回调函数

            callback_entry_t(int32_t id, callback_type callback) : id(id), callback(callback) {}
        };

        struct _pubsub_node_t
        {
            message_type last_msg;                 // 最新消息
            std::list<callback_entry_t> callbacks; // 回调函数列表
        };

        ::utools::collab::Mutex __callback_mutex; // 保护任务队列的互斥量
        std::unordered_map<topic_type, _pubsub_node_t> __topics;

        ::utools::collab::SyncQueue<std::function<void()>> __task_queue; // 同步任务队列

        ::utools::collab::Task __work_task; // 工作线程任务

        int32_t __current_id{0}; // 订阅者ID
        bool __running{false};   // 是否运行

        void __process_task_queue()
        {
            while (__running)
            {
#if __cplusplus < 201703L
                auto task = __task_queue.pop_wait();
                if (task)
                {
                    task();
                }
#else  // >= C++17
                if (auto task = __task_queue.pop_wait_safety(); task.has_value())
                {
                    task.value()();
                }
#endif // C++17
            }
        }
    };

#endif // UTOOLS_PUBSUB_ENABLE && UTOOLS_COLLAB_TASK_ENABLE
}

#endif // __UTOOLS_PUBSUB_H__
