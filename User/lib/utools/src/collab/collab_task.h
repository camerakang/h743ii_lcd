#ifndef __UTOOLS_COLLAB_TASK_H__
#define __UTOOLS_COLLAB_TASK_H__

#include "../utools_cfg.h"

#if UTOOLS_COLLAB_TASK_ENABLE == UTOOLS_TRUE

#include <functional>
#include <stdint.h>
#include <string>

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
#include <thread>
#include <chrono>

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
#include UTOOLS_FREERTOS_INCLUDE_FREERTOS_H
#include UTOOLS_FREERTOS_INCLUDE_TASK_H

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
#include "cmsis_os.h"

#else
#warning "Unsupported thread function type"
#endif

namespace utools::collab
{

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
    static void __std_thread_detach_wrapper(void *param);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
    static void __freertos_task_wrapper(void *param);
    static void __freertos_task_detach_wrapper(void *param);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
    static void *__pthread_wrapper(void *arg);
    static void *__pthread_detach_wrapper(void *arg);
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
    static void __os_thread_wrapper(void *arg);
    static void __os_thread_detach_wrapper(void *arg);
#endif

    /// @brief 任务类型
    class Task
    {
    public:
        virtual ~Task()
        {
            stop();
        }

        /// @brief 使用默认构造函数
        Task() = default;

        /// @brief 禁止拷贝赋值
        Task(const Task &) = delete;
        Task &operator=(const Task &) = delete;

        /// @brief 移动构造函数
        /// @param other 另一个任务对象
        /// @warning 移动构造时，原任务对象不能处于运行状态，否则会导致未定义行为
        Task(Task &&other)
            : __task(std::move(other.__task)),
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
              __thread_handle(std::move(other.__thread_handle)),
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
              __freertos_task_handle(other.__freertos_task_handle),
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
              __pthread_handle(other.__pthread_handle),
#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
              __osthread_handle(other.__osthread_handle),
#endif
              __is_running(other.__is_running),
              __usr_task_id(other.__usr_task_id)
        {
            other.__is_running = false;
            other.__usr_task_id = -1;
#if UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            other.__freertos_task_handle = nullptr;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            other.__pthread_handle = nullptr;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            other.__osthread_handle = nullptr;
#endif
        }

        /// @brief 移动赋值运算符
        /// @param other 另一个任务对象
        /// @return 当前任务对象
        /// @warning 移动赋值时，原任务对象不能处于运行状态，否则会导致未定义行为
        Task &operator=(Task &&other)
        {
            if (this == &other)
            {
                return *this;
            }
            __task = std::move(other.__task);
            __is_running = other.__is_running;
            __usr_task_id = other.__usr_task_id;

            other.__is_running = false;
            other.__usr_task_id = -1;

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            __thread_handle = std::move(other.__thread_handle);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            __freertos_task_handle = other.__freertos_task_handle;
            other.__freertos_task_handle = nullptr;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            __pthread_handle = other.__pthread_handle;
            other.__pthread_handle = nullptr;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            __osthread_handle = other.__osthread_handle;
            other.__osthread_handle = nullptr;
#endif
            return *this;
        }

        /// @brief 构造函数：接受任意可调用对象
        template <typename Func, typename... Args,
                  typename = std::enable_if<!std::is_member_function_pointer<Func>::value>>
        Task(Func &&func, Args &&...args)
        {
            __task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
        }

        /// @brief 构造函数：接受类成员函数
        template <typename ReturnType, typename T, typename... Args>
        Task(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            __task = std::bind(func, obj, std::forward<Args>(args)...);
        }

        /// @brief 构造函数：接受静态函数或自由函数
        template <typename ReturnType, typename... Args>
        Task(ReturnType (*func)(Args...), Args &&...args)
        {
            __task = std::bind(func, std::forward<Args>(args)...);
        }

        /// @brief 右值引用构造函数
        /// @note 处理已经绑定好的函数
        Task(std::function<void()> &&func) : __task(std::move(func))
        {
        }

        /// @brief 左值引用构造函数
        /// @note 处理已经绑定好的函数
        Task(const std::function<void()> &func) : __task(func)
        {
        }

        /// @brief 设置运行任务
        /// @param func 可调用对象
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename Func, typename... Args,
                  typename = std::enable_if<!std::is_member_function_pointer<Func>::value>>
        Task &bind(Func &&func, Args &&...args)
        {
            __task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            return *this;
        }

        /// @brief 设置运行任务
        /// @param func 类成员函数
        /// @param obj 类对象指针
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename T, typename... Args>
        Task &bind(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            __task = std::bind(func, obj, std::forward<Args>(args)...);
            return *this;
        }

        /// @brief 设置运行任务
        /// @param func 静态函数或自由函数
        /// @param args 可变参数
        /// @return 返回任务对象本身
        template <typename ReturnType, typename... Args>
        Task &bind(ReturnType (*func)(Args...), Args &&...args)
        {
            __task = std::bind(func, std::forward<Args>(args)...);
            return *this;
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        Task &bind(std::function<void()> &&func)
        {
            __task = std::move(func);
            return *this;
        }

        /// @brief 设置运行任务
        /// @note 处理已经绑定好的函数
        Task &bind(const std::function<void()> &func)
        {
            __task = func;
            return *this;
        }

        /// @brief 启动任务
        /// @param stack_size 栈大小，默认为UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE
        /// @param priority 优先级，默认为正常优先级
        /// @param cpu_id CPU ID，默认为0，表示使用默认CPU
        /// @param usr_id 用户任务ID，默认为-1，表示不指定ID
        /// @return 返回任务对象本身
        Task &start(const size_t stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE,
#if UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
                    const int32_t priority = static_cast<int32_t>(osPriorityNormal),
#else
                    const int32_t priority = 0,
#endif
                    const int32_t cpu_id = 0,
                    const int32_t usr_id = -1)
        {
            if (__is_running)
            {
                return *this;
            }

#if (UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION || \
     UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION)
            if (usr_id != -1)
            {
                __usr_task_id = usr_id;
            }
            else
            {
                __usr_task_id = __next_task_id++;
            }
#else
            (void)usr_id;
#endif

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            (void)stack_size;
            (void)cpu_id;
            (void)priority;
            __thread_handle = std::thread(__task);
            __thread_handle.detach();

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
#if UTOOLS_SUPPORTED_CUP_CORE_NUM == 1 // 单核
            xTaskCreate(__freertos_task_wrapper,
                        ("ut" + std::to_string(__usr_task_id)).c_str(),
                        stack_size,
                        this,
                        priority,
                        &__freertos_task_handle);
#else                                  // 多核
            TaskHandle_t xHandle = nullptr;
            xTaskCreatePinnedToCore(
                __freertos_task_wrapper,
                ("ut" + std::to_string(__usr_task_id)).c_str(),
                stack_size,
                this,
                priority,
                &xHandle,
                cpu_id);
            __freertos_task_handle = xHandle;
#endif

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setstacksize(&attr, stack_size);
#if UTOOLS_SUPPORTED_CUP_CORE_NUM > 1 // 多核
            cpu_set_t cpus;
            CPU_ZERO(&cpus);
            CPU_SET(cpu_id, &cpus);
            pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
#endif
            // 设置优先级
            sched_param sch_params;
            sch_params.sched_priority = priority;
            pthread_attr_setschedparam(&attr, &sch_params);
            // 设置调度策略
            pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

            pthread_create(&__pthread_handle, &attr, __pthread_wrapper, this);
            pthread_detach(__pthread_handle);
            pthread_attr_destroy(&attr);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            // NOTE:必须使用下面的方式才能正确创建osThreadAttr_t，如果先定义，赋值无法启动线程
            osThreadAttr_t thread_attr = {};
            thread_attr.name = ("ut" + std::to_string(__usr_task_id)).c_str();
            thread_attr.stack_size = stack_size;
            thread_attr.priority = static_cast<osPriority_t>(priority);

            __osthread_handle = osThreadNew(__os_thread_wrapper, this, &thread_attr);

#endif // UTOOLS_THREAD_FUNCATION
            __is_running = true;
            return *this;
        }

        /// @brief 开始任务，并从对象中分离，独立运行
        /// @param stack_size 栈大小，默认为UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE
        /// @param priority 优先级，默认为正常优先级
        /// @param cpu_id CPU ID，默认为0，表示使用默认CPU
        /// @param usr_id 用户任务ID，默认为-1，表示不指定ID
        void detach(
            const size_t stack_size = UTOOLS_COLLAB_TASK_STACK_DEFAULT_SIZE,
#if UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            const int32_t priority = static_cast<int32_t>(osPriorityNormal),
#else
            const int32_t priority = 0,
#endif
            const int32_t cpu_id = 0,
            const int32_t usr_id = -1)
        {
#if (UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION || \
     UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION)
            int32_t usr_task_id = usr_id == -1 ? __next_task_id++ : usr_id;
#else
            (void)usr_id;
#endif

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            (void)stack_size;
            (void)cpu_id;
            (void)priority;
            (void)usr_id;
            std::thread(__std_thread_detach_wrapper, new std::function<void()>(__task)).detach();

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
#if UTOOLS_SUPPORTED_CUP_CORE_NUM == 1 // 单核
            xTaskCreate(__freertos_task_detach_wrapper,
                        ("ut" + std::to_string(usr_task_id)).c_str(),
                        stack_size,
                        new std::function<void()>(__task),
                        priority,
                        nullptr);
#else                                  // 多核
            TaskHandle_t xHandle = nullptr;
            xTaskCreatePinnedToCore(
                __pthread_detach_wrapper,
                ("ut" + std::to_string(usr_task_id)).c_str(),
                stack_size,
                this,
                priority,
                &xHandle,
                cpu_id);
#endif

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setstacksize(&attr, stack_size);
#if UTOOLS_SUPPORTED_CUP_CORE_NUM > 1 // 多核
            cpu_set_t cpus;
            CPU_ZERO(&cpus);
            CPU_SET(cpu_id, &cpus);
            pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
#endif
            // 设置优先级
            sched_param sch_params;
            sch_params.sched_priority = priority;
            pthread_attr_setschedparam(&attr, &sch_params);
            // 设置调度策略
            pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

            pthread_t __pthread_temp_handle;
            pthread_create(&__pthread_temp_handle, &attr, new std::function<void()>(__task), this);
            pthread_detach(__pthread_temp_handle);
            pthread_attr_destroy(&attr);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            // NOTE:必须使用下面的方式才能正确创建osThreadAttr_t，如果先定义，赋值无法启动线程
            osThreadAttr_t thread_attr = {};
            thread_attr.name = ("ut" + std::to_string(usr_task_id)).c_str();
            thread_attr.stack_size = stack_size;
            thread_attr.priority = static_cast<osPriority_t>(priority);

            osThreadNew(
                __os_thread_detach_wrapper,
                new std::function<void()>(__task),
                &thread_attr);
#endif
        }

        /// @brief 停止任务
        void stop()
        {
            if (!__is_running)
            {
                return;
            }
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            if (__thread_handle.joinable())
            {
                __thread_handle.join();
            }
            __is_running = false;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            if (__freertos_task_handle != nullptr)
            {
                vTaskDelete(__freertos_task_handle);
                __freertos_task_handle = nullptr;
            }

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            pthread_cancel(__pthread_handle);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            if (__osthread_handle != nullptr)
            {
                osThreadTerminate(__osthread_handle);
                __osthread_handle = nullptr;
            }
#endif
        }

        /// @brief 调用不同平台的sleep功能，提供给线程内使用
        /// @param ms 毫秒数
        inline static void sleep_for(const uint32_t ms)
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            vTaskDelay(ms / portTICK_PERIOD_MS);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            struct timespec ts;
            ts.tv_sec = ms / 1000;
            ts.tv_nsec = (ms % 1000) * 1000000;
            nanosleep(&ts, nullptr);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            osDelay(ms);
#endif
        }

        /// @brief 增加支持chrono的sleep_for
        /// @tparam _Rep 时钟类型
        /// @tparam _Period 周期
        /// @param dtime 延时时长
        /// @note 目前仅支持std::chrono::milliseconds，其中类型将被转换为毫秒
        template <typename _Rep, typename _Period>
        static void sleep_for(const std::chrono::duration<_Rep, _Period> &dtime)
        {
            sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(dtime).count());
        }

        /// @brief 让出 CPU
        static void yeild()
        {
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
            std::this_thread::yield();

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
            taskYIELD();

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
            sched_yield();

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
            osThreadYield();

#endif
        }

        /// @brief 获取用户任务ID
        /// @return 用户任务ID
        const uint32_t get_usr_task_id() const
        {
            return __usr_task_id;
        }

        /// @brief 判断任务是否正在运行
        /// @return 任务是否正在运行
        bool is_running() const
        {
            return __is_running;
        }

        /// @brief 判断任务是否可用
        /// @return 任务是否可用
        bool is_available() const
        {
            return __task ? true : false;
        }

    private:
        inline static int32_t __next_task_id{0}; // 用户任务ID生成器

        std::function<void()> __task; // 存储绑定的任务

#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
        std::thread __thread_handle;

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
        TaskHandle_t __freertos_task_handle{nullptr};
        friend void __freertos_task_wrapper(void *param);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
        pthread_t __pthread_handle{nullptr};
        friend void *__pthread_wrapper(void *arg);

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
        osThreadId_t __osthread_handle{nullptr};
        friend void __os_thread_wrapper(void *arg);
#endif

        bool __is_running{false};  // 任务是否正在运行
        int32_t __usr_task_id{-1}; // 用户任务ID
    };
#if UTOOLS_THREAD_FUNCATION == UTOOLS_STD_THREAD_FUNCATION
    void __std_thread_detach_wrapper(void *param)
    {
        auto f = (std::function<void()> *)param;
        f->operator()();
        delete f;
    }

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_FREERTOS_TASK_FUNCATION
    void __freertos_task_wrapper(void *param)
    {
        utools::collab::Task *task = static_cast<utools::collab::Task *>(param);
        task->__task();
        task->__is_running = false;
        vTaskDelete(NULL);
    }

    void __freertos_task_detach_wrapper(void *param)
    {
        auto f = (std::function<void()> *)param;
        f->operator()();
        delete f;
        vTaskDelete(NULL);
    }

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_PTHREAD_FUNCATION
    void *__pthread_wrapper(void *arg)
    {
        utools::collab::Task *task = static_cast<utools::collab::Task *>(arg);
        task->__task();
        task->__is_running = false;
        return nullptr;
    }

    void *__pthread_detach_wrapper(void *arg)
    {
        auto f = (std::function<void()> *)arg;
        f->operator()();
        delete f;
        return nullptr;
    }

#elif UTOOLS_THREAD_FUNCATION == UTOOLS_OSTHREADNEW_FUNCATION
    void __os_thread_wrapper(void *arg)
    {
        utools::collab::Task *task = static_cast<utools::collab::Task *>(arg);
        task->__task();
        task->__is_running = false;
        osThreadExit();
    }

    void __os_thread_detach_wrapper(void *arg)
    {
        auto f = (std::function<void()> *)arg;
        f->operator()();
        delete f;
        osThreadExit();
    }
#endif
}
#endif // UTOOLS_COLLAB_TASK_ENABLE
#endif // __UTOOLS_COLLAB_TASK_H__
