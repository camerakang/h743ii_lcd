#ifndef __UTOOLS_TASK_SEQUENCE_H__
#define __UTOOLS_TASK_SEQUENCE_H__

#include "../utools_cfg.h"

#if UTOOLS_COLLAB_TASK_SEQUENCE_ENABLE
#if UTOOLS_COLLAB_TASK_ENABLE == UTOOLS_FALSE
#error "UTOOLS_COLLAB_TASK_ENABLE must be true"

#else
#include <functional>
#include <vector>
#include <cstdint>
#include "collab_task.h"
#include "../utime/stable_interval_invoker.h"
#include "utmutex.h"

#endif // UTOOLS_COLLAB_TASK_ENABLE == UTOOLS_FALSE
#endif

namespace utools::collab
{
#if (UTOOLS_COLLAB_TASK_SEQUENCE_ENABLE && UTOOLS_COLLAB_TASK_ENABLE)
    class TaskSequence
    {
    public:
        /// @brief 构造函数
        /// @param interval_ms 调用间隔时间，单位毫秒
        TaskSequence(int interval_ms = 1000)
        {
            set_interval(interval_ms);
        }

        virtual ~TaskSequence() = default;

        /// @brief 增加运行任务
        /// @param func 可调用对象
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename Func, typename... Args>
        TaskSequence &add(Func &&func, Args &&...args)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __tasks.emplace_back(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
            return *this;
        }

        /// @brief 增加运行任务
        /// @param func 类成员函数
        /// @param obj 类对象指针
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename T, typename... Args>
        TaskSequence &add(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __tasks.emplace_back(std::bind(func, obj, std::forward<Args>(args)...));
            return *this;
        }

        /// @brief 增加运行任务
        /// @param func 静态函数或自由函数
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename... Args>
        TaskSequence &add(ReturnType (*func)(Args...), Args &&...args)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __tasks.emplace_back(std::bind(func, std::forward<Args>(args)...));
            return *this;
        }

        /// @brief 删除任务
        /// @param index 任务索引
        /// @return 返回任务对象本身
        TaskSequence &remove(size_t index)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __tasks.erase(__tasks.begin() + index);
            return *this;
        }

        /// @brief 清空任务
        /// @return 返回任务对象本身
        TaskSequence &clear()
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __tasks.clear();
            return *this;
        }

        /// @brief 设置任务调用频率
        /// @param frequency 调用频率，如果换算的周期小于于1ms，则延时为0
        /// @return 返回任务对象本身
        TaskSequence &set_frequency(int frequency)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            __invoker_loop.set_invoker_interval(frequency > 0 ? static_cast<int64_t>(1000 / frequency) : 0);
            return *this;
        }

        /// @brief 设置任务调用间隔
        /// @param interval 调用间隔，单位为毫秒
        /// @return 返回任务对象本身
        TaskSequence &set_interval(int interval)
        {
            __invoker_loop.set_invoker_interval(interval);
            return *this;
        }

        /// @brief 启动任务
        /// @param stack_size 任务栈大小
        /// @param priority 任务优先级，-1为默认优先级
        bool start(const size_t stack_size = 1024 * 4, const int32_t priority = -1)
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            if (is_running())
            {
                return false;
            }
            __is_exit = false;
            if (priority == -1)
            {
                ::utools::collab::Task(&TaskSequence::__handle, this).detach(stack_size);
            }
            else
            {
                ::utools::collab::Task(&TaskSequence::__handle, this).detach(stack_size, priority);
            }
            return true;
        }

        /// @brief 结束任务
        void stop()
        {
            __invoker_loop.stop();
        }

        /// @brief 是否在运行
        /// @return true: 在运行，false: 未运行
        bool is_running()
        {
            return __invoker_loop.is_running() && !__is_exit;
        }

        /// @brief 获取任务数量
        /// @return 任务数量
        size_t size()
        {
            return __tasks.size();
        }

    private:
        std::vector<std::function<void()>> __tasks; // 任务列表
        ::utools::collab::Mutex __queue_mutex;      // 保护任务队列的互斥量

        bool __is_exit{true}; // 是否已经退出

        /// @brief 任务处理函数
        inline void __invoker_func()
        {
            ::utools::collab::LockGuard lock(__queue_mutex);
            for (const auto &task : __tasks)
            {
                task();
            }
        };

        ::utools::time::StableIntervalInvoker<std::function<void()>> __invoker_loop{
            ::utools::time::make_stable_interval_invoker(&TaskSequence::__invoker_func, this)}; // 任务调用循环

        inline void __handle()
        {
            __invoker_loop.run_forever();
            __is_exit = true; // 设置为已退出
        }
    };
#endif // UTOOLS_COLLAB_TASK_ENABLE
}

#endif // __UTOOLS_TASK_SEQUENCE_H__