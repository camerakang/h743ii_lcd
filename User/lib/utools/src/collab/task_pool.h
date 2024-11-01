/// @brief 任务池

#ifndef __UTOOLS_TASK_POOL_H__
#define __UTOOLS_TASK_POOL_H__

#include "../utools_cfg.h"

#if UTOOLS_COLLAB_TASK_POOL_ENABLE

#if UTOOLS_COLLAB_TASK_ENABLE != UTOOLS_TRUE
#error "UTOOLS_COLLAB_TASK_ENABLE must be enabled"
#endif

#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <memory>
#include <type_traits>
#include "collab_task.h"
#include "sync_queue.h"

namespace utools::collab
{
    class TaskPool
    {
    public:
        template <typename T>
        class task_type
        {
        public:
            friend class TaskPool;

            /// @brief 返回值类型
            using ResultType = typename std::conditional<std::is_void<T>::value, void, T>::type;

            /// @brief 获取返回值
            /// @return 返回值
            ResultType get_result()
            {
                if constexpr (std::is_void<T>::value)
                {
                    /// @note void类型直接返回，什么都不用做
                }
                else
                {
                    return __result;
                }
            }

            /// @brief 获取任务是否完成
            /// @return true 成功放弃任务，false 任务正在运行
            bool discard()
            {
                if (__done == 0)
                {
                    __done = 2;
                    return true;
                }
                return false;
            }

            /// @brief 获取任务是否正在运行
            /// @return 任务是否正在运行
            bool is_running() const { return __done == 1; }

            /// @brief 获取任务是否完成
            /// @return 任务是否完成
            bool is_done() const { return __done == 2; }

            /// @brief 获取任务是否被放弃
            /// @return 任务是否被放弃
            bool is_discard() const { return __done == 3; }

            /// @brief 获取任务状态码
            /// @return 任务状态码
            /// @note 0:未开始 1:正在运行 2:已完成 3:已放弃
            uint8_t status_code() { return __done; }

            task_type() : __done{0} {}
            task_type(std::function<T()> callback) : __callback{callback}, __done{0} {}

        private:
            std::function<T()> __callback;
            typename std::conditional<std::is_void<T>::value, uint8_t, T>::type __result;
            volatile uint8_t __done;
        };

        /// @brief 构造任务池
        /// @param max_works_num 最大工作线程数
        /// @param stack_size 每个工作线程的栈大小
        TaskPool(const size_t max_workers_num = UTOOLS_SUPPORTED_CUP_CORE_NUM * 2 + 1,
                 const size_t stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE,
                 bool auto_start = false)
            : __max_workers_num(max_workers_num), __stack_size(stack_size)
        {
            if (auto_start)
            {
                start(__stack_size);
            }
        }

        /// @brief 析构任务池
        virtual ~TaskPool()
        {
            __running = false;
            for (auto &worker : __workers)
            {
                worker.stop();
            }
            __task_queue.release();
        }

        /// @brief 启动线程池服务
        /// @param task_stack_size 任务栈大小
        void start(const size_t max_workers_num = UTOOLS_SUPPORTED_CUP_CORE_NUM * 2 + 1,
                   size_t task_stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE)
        {
            __stack_size = task_stack_size;
            __max_workers_num = max_workers_num;
            __running = true;
            for (size_t i = 0; i < __max_workers_num; ++i)
            {
                __workers.push_back(::utools::collab::Task(&TaskPool::__worker, this));
            }
            for (auto &worker : __workers)
            {
                worker.start(__stack_size);
            }
        }

        /// @brief 停止线程池服务
        void stop()
        {
            __running = false;
            for (auto &worker : __workers)
            {
                worker.stop();
            }
        }

        /// @brief 判断系统池是否在运行
        /// @return true 正在运行, false 停止运行
        bool is_running() const
        {
            return __running;
        }

        /// @brief 分配任务
        template <typename Func, typename... Args>
        auto assign(Func &&func, Args &&...args) -> std::shared_ptr<task_type<decltype(func(args...))>>
        {
            using ReturnType = decltype(func(args...));
            auto task = std::make_shared<task_type<ReturnType>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
            auto callable_task = __worker_wrapper(task);
            __task_queue.push(std::make_shared<std::function<void()>>(callable_task));
            return task; // 返回智能指针
        }

        /// @brief 分配任务
        template <typename ReturnType, typename T, typename... Args>
        auto assign(ReturnType (T::*func)(Args...), T *obj, Args &&...args) -> std::shared_ptr<task_type<ReturnType>>
        {
            auto task = std::make_shared<task_type<ReturnType>>(std::bind(func, obj, std::forward<Args>(args)...));
            auto callable_task = __worker_wrapper(task);
            __task_queue.push(std::make_shared<std::function<void()>>(callable_task));
            return task; // 返回智能指针
        }

        /// @brief 分配任务
        template <typename ReturnType, typename... Args>
        auto assign(ReturnType (*func)(Args...), Args &&...args) -> std::shared_ptr<task_type<ReturnType>>
        {
            auto task = std::make_shared<task_type<ReturnType>>(std::bind(func, std::forward<Args>(args)...));
            auto callable_task = __worker_wrapper(task);
            __task_queue.push(std::make_shared<std::function<void()>>(callable_task));
            return task; // 返回智能指针
        }

        /// @brief 清空任务队列中所有的任务
        void clear_tasks()
        {
            __task_queue.clear();
        }

        /// @brief 获取任务队列中任务的数量
        /// @return 任务数量
        size_t get_tasks_num()
        {
            return static_cast<size_t>(__task_queue.size());
        }

    private:
        size_t __max_workers_num{2};
        size_t __stack_size{UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE};
        std::vector<::utools::collab::Task> __workers;                                     // 工作线程
        utools::collab::SyncQueue<std::shared_ptr<std::function<void()>>, 0> __task_queue; // 任务队列

        bool __running{false}; // 标志线程池是否停止

        template <typename Func>
        inline std::function<void()> __worker_wrapper(Func &&task)
        {
            return [task]()
            {
                if (task->__done != 0)
                {
                    return;
                }
                task->__done = 1; // 标记为正在执行
                if constexpr (std::is_void<typename std::result_of<decltype(task->__callback)()>::type>::value)
                {
                    task->__callback(); // 调用回调
                }
                else
                {
                    task->__result = task->__callback(); // 调用回调
                }
                task->__done = 2; // 标记为完成
                // FIXME: 这里没有做任务执行异常的处理，主要是为了简化在mcu上的实现
            };
        }

        void __worker()
        {
            while (__running)
            {
#if __cplusplus < 201703L
                auto task = __task_queue.pop_wait();
                if (task)
                {
                    task->operator()();
                }
#else  // >= C++17
                if (auto task = __task_queue.pop_wait_safety(); task.has_value())
                {
                    task.value()->operator()();
                }
#endif // C++17
            }
        }
    };
}

#endif // UTOOLS_COLLAB_TASK_POOL_ENABLE

#endif // __UTOOLS_TASK_POOL_H__
