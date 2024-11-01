#ifndef __UTOOLS_COLLAB_TIMER_H__
#define __UTOOLS_COLLAB_TIMER_H__

#include "../utools_cfg.h"

#if UTOOLS_COLLAB_TIMER_ENABLE

#if UTOOLS_COLLAB_TASK_ENABLE != UTOOLS_TRUE
#error "UTOOLS_COLLAB_TASK_ENABLE must be enabled"
#endif

#include <stdint.h>
#include <memory>
#include <functional>
#include <list>

#include "collab_task.h"
#include "../utime/time_utils.h"

namespace utools::collab
{
    class Timer
    {
    public:
        /// @brief 定时器状态
        enum class status_type : uint8_t
        {
            UNITIALIZED, // 未初始化
            READY,       // 准备就绪
            RUNNING,     // 运行中
            FINISHED,    // 已完成
        };

        /// @brief 定时器模式
        enum class mode_type : uint8_t
        {
            ONCE,   // 单次模式
            REPEAT, // 重复模式
            COUNT,  // 计数模式
        };

        /// @brief 构造函数
        explicit Timer() noexcept
        {
        }

        /// @brief 构造函数：接受任意可调用对象
        template <typename Func, typename... Args,
                  typename = std::enable_if<!std::is_member_function_pointer<Func>::value>>
        explicit Timer(Func &&func, Args &&...args)
        {
            bind(std::forward<Func>(func), std::forward<Args>(args)...);
        }

        /// @brief 构造函数：接受类成员函数
        template <typename ReturnType, typename T, typename... Args>
        explicit Timer(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            bind(func, obj, std::forward<Args>(args)...);
        }

        /// @brief 构造函数：接受静态函数或自由函数
        template <typename ReturnType, typename... Args>
        explicit Timer(ReturnType (*func)(Args...), Args &&...args)
        {
            bind(func, std::forward<Args>(args)...);
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        explicit Timer(std::function<void()> &&func)
        {
            if (__status != status_type::RUNNING)
            {
                __task = std::move(func);
            }
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        explicit Timer(const std::function<void()> &func)
        {
            if (__status != status_type::RUNNING)
            {
                __task = func;
            }
        }

        /// @brief 析构函数
        virtual ~Timer() noexcept
        {
        }

        /// @brief 禁止复制构造和赋值操作
        Timer(const Timer &) = delete;
        Timer &operator=(const Timer &) = delete;

        /// @brief 设置运行任务
        /// @param func 可调用对象
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename Func, typename... Args,
                  typename = std::enable_if<!std::is_member_function_pointer<Func>::value>>
        inline Timer &bind(Func &&func, Args &&...args)
        {
            if (__status != status_type::RUNNING)
            {
                __task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
                __status = status_type::READY;
            }
            return *this;
        }

        /// @brief 设置运行任务
        /// @param func 类成员函数
        /// @param obj 类对象指针
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename T, typename... Args>
        inline Timer &bind(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            if (__status != status_type::RUNNING)
            {
                __task = std::bind(func, obj, std::forward<Args>(args)...);
                __status = status_type::READY;
            }
            return *this;
        }

        /// @brief 设置运行任务
        /// @param func 静态函数或自由函数
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename... Args>
        inline Timer &bind(ReturnType (*func)(Args...), Args &&...args)
        {
            if (__status != status_type::RUNNING)
            {
                __task = std::bind(func, std::forward<Args>(args)...);
                __status = status_type::READY;
            }
            return *this;
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        inline Timer &bind(std::function<void()> &&func)
        {
            if (__status != status_type::RUNNING)
            {
                __task = std::move(func);
                __status = status_type::READY;
            }
            return *this;
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        inline Timer &bind(const std::function<void()> &func)
        {
            if (__status != status_type::RUNNING)
            {
                __task = func;
                __status = status_type::READY;
            }
            return *this;
        }

        /// @brief 生成一个定时器，传入相关的参数
        /// @tparam ...Args 函数参数类型
        /// @param ...args 函数参数
        /// @return 返回一个定时器指针
        /// @note 其生成的定时器会独立运行，使用时可以不保存其指针或引用
        template <typename... Args>
        static std::shared_ptr<Timer> detach(Args &&...args)
        {
            std::shared_ptr<Timer> timer = std::make_shared<Timer>(std::forward<Args>(args)...);
            __detach_timers.push_back(timer);
            return timer;
        }

        /// @brief 获取所有已经被分离的定时器
        /// @return 返回一个定时器指针数组
        static std::list<std::shared_ptr<Timer>> &detached_timers()
        {
            return __detach_timers;
        }

        /// @brief 删除所有已经完成的任务
        /// @note 不会删除正在运行的定时器
        static void clear_finshed_detached_timer()
        {
            __detach_timers.remove_if([](std::shared_ptr<Timer> timer)
                                      { return timer->is_done(); });
        }

        /// @brief 设置一个一次性定时任务
        /// @param interval_ms 间隔时间，单位毫秒
        /// @param stack_size 栈大小，单位字节
        /// @return 对象本身
        Timer &once(uint64_t interval_ms, size_t stack_size = 512)
        {
            if (__status == status_type::RUNNING)
            {
                return *this;
            }
            __mode = mode_type::ONCE; // 单次模式
            __interval_ms = interval_ms;
            ::utools::collab::Task(
                [this]() -> void
                {
                    this->__status = status_type::RUNNING;
                    ::utools::collab::Task::sleep_for(this->__interval_ms);
                    __task();
                    __status = status_type::FINISHED;
                })
                .detach(stack_size);
            return *this;
        }

        /// @brief 设置一个可重复的定时任务，可以选择是否立即执行一次
        /// @param interval_ms 间隔时间，单位毫秒
        /// @param interval_include_calltime 调用的时间是否保存在间隔时间内
        /// @param delay_call_ms  第一次调用延迟时间，单位毫秒
        /// @param stack_size 栈大小，单位字节
        /// @return 对象本身
        Timer &repeat(uint64_t interval_ms,
                      uint64_t delay_call_ms = 0,
                      bool interval_include_calltime = true,
                      size_t stack_size = 512)
        {
            if (__status == status_type::RUNNING)
            {
                return *this;
            }
            __mode = mode_type::REPEAT; // 重复模式
            __interval_ms = interval_ms;

            if (interval_include_calltime)
            {
                ::utools::collab::Task(
                    [this, delay_call_ms]() -> void
                    {
                        this->__can_running = true;
                        this->__status = status_type::RUNNING;
                        if (delay_call_ms)
                        {
                            ::utools::collab::Task::sleep_for(delay_call_ms);
                        }
                        while (this->__can_running)
                        {
                            this->__timer_task_include_calltime();
                        }
                        this->__status = status_type::FINISHED;
                        this->__can_running = false;
                    })
                    .detach(stack_size);
            }
            else
            {
                ::utools::collab::Task(
                    [this, delay_call_ms]() -> void
                    {
                        this->__can_running = true;
                        this->__status = status_type::RUNNING;
                        if (delay_call_ms)
                        {
                            ::utools::collab::Task::sleep_for(delay_call_ms);
                        }
                        while (this->__can_running)
                        {
                            this->__timer_task_exclude_calltime();
                        }
                        this->__status = status_type::FINISHED;
                        this->__can_running = false;
                    })
                    .detach(stack_size);
            }
            return *this;
        }

        /// @brief 设置一个可重复的定时任务，可以选择是否立即执行一次，并指定执行次数
        /// @param interval_ms 间隔时间，单位毫秒
        /// @param count 执行次数
        /// @param delay_call_ms 第一次调用延迟时间，单位毫秒
        /// @param interval_include_calltime 调用的时间是否保存在间隔时间内
        /// @param stack_size 栈大小，单位字节
        /// @return 对象本身
        Timer &repeat_n(uint64_t interval_ms,
                        uint64_t count,
                        uint64_t delay_call_ms = 0,
                        bool interval_include_calltime = true,
                        size_t stack_size = 512)
        {
            if (__status == status_type::RUNNING && count > 0)
            {
                return *this;
            }
            __mode = mode_type::COUNT; // 计数模式
            __interval_ms = interval_ms;
            __count = count;

            if (interval_include_calltime)
            {
                ::utools::collab::Task(
                    [this, delay_call_ms]() -> void
                    {
                        this->__can_running = true;
                        this->__status = status_type::RUNNING;
                        if (delay_call_ms > 0)
                        {
                            ::utools::collab::Task::sleep_for(delay_call_ms);
                        }
                        while (this->__can_running && this->__count > this->__call_count++)
                        {
                            this->__timer_task_include_calltime();
                        }
                        this->__status = status_type::FINISHED;
                        this->__can_running = false;
                        this->__call_count = 0;
                    })
                    .detach(stack_size);
            }
            else
            {
                ::utools::collab::Task(
                    [this, delay_call_ms]() -> void
                    {
                        this->__can_running = true;
                        this->__status = status_type::RUNNING;
                        if (delay_call_ms > 0)
                        {
                            ::utools::collab::Task::sleep_for(delay_call_ms);
                        }
                        while (this->__can_running && this->__count > this->__call_count++)
                        {
                            this->__timer_task_exclude_calltime();
                        }
                        this->__status = status_type::FINISHED;
                        this->__can_running = false;
                        this->__call_count = 0;
                    })
                    .detach(stack_size);
            }
            return *this;
        }

        /// @brief 获取定时器的模式
        /// @returns 定时器的模式
        const mode_type mode() const noexcept
        {
            return __mode;
        }

        /// @brief 获取定时器的状态
        /// @returns 定时器的状态
        const status_type status() const noexcept
        {
            return __status;
        }

        /// @brief 取消当前的定时任务
        /// @note 任务会在下次执行前被取消
        void cancel() noexcept
        {
            __can_running = false;
        }

        /// @brief 判断定时器是否处于空闲状态
        /// @returns 定时器是否处于空闲状态
        bool is_timer_idle() const noexcept
        {
            return __can_running == false;
        }

        /// @brief 获取定时器设置的运行时间间隔
        /// @returns 设置的运动时间间隔
        const uint64_t count() const noexcept
        {
            return __count;
        }

        /// @brief 获取定时器叫用的次数
        /// @returns 定时器调用次数
        const uint64_t call_count() const noexcept
        {
            return __call_count;
        }

        /// @brief 获取定时器剩余的调用次数
        /// @returns 定时器剩余的调用次数
        const uint64_t remaining_count() const noexcept
        {
            return __count - __call_count;
        }

        /// @brief 获取间隔时间
        /// @returns 间隔时间
        const uint64_t interval_ms() const noexcept
        {
            return __interval_ms;
        }

        /// @brief 判断是否已经运行完成或退出
        /// @returns 是否已经运行完成或退出
        bool is_done() const noexcept
        {
            return __status == status_type::FINISHED;
        }

    private:
        status_type __status{status_type::UNITIALIZED}; // 定时器状态
        mode_type __mode{mode_type::REPEAT};            // 定时器模式
        std::function<void()> __task;                   // 存储绑定的任务

        bool __can_running{false}; // 定时器是否正在运行
        uint64_t __interval_ms{0}; // 定时器间隔时间
        uint64_t __count{0};       // 定时器执行次数
        uint64_t __call_count{0};  // 定时器调用次数

        inline static std::list<std::shared_ptr<Timer>> __detach_timers; // 定时器列表

        /// @brief 定时器任务
        /// @note 时间间隔考虑任务执行时间
        inline void __timer_task_include_calltime()
        {
            auto start_ts_ms{::utools::time::unix_ts()};
            __task();
            auto sleep_time_ms{__interval_ms - (::utools::time::unix_ts() - start_ts_ms)};
            if (sleep_time_ms > 0)
            {
                ::utools::collab::Task::sleep_for(sleep_time_ms);
            }
        }

        /// @brief 定时器任务
        /// @note 时间间隔不考虑任务执行时间
        inline void __timer_task_exclude_calltime()
        {
            __task();
            ::utools::collab::Task::sleep_for(__interval_ms);
        }
    };
}

#endif // UTOOLS_COLLAB_TIMER_ENABLE

#endif // __UTOOLS_COLLAB_TIMER_H__