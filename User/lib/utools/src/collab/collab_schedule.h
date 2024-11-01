#ifndef __COLLAB_SCHEDULE_H__
#define __COLLAB_SCHEDULE_H__

#include "../utools_cfg.h"

#include <cstdint>
#include <type_traits>
#include <functional>
#include <memory>
#include <list>

namespace utools::collab
{
    class Schedule
    {
    public:
        class Every
        {
        public:
            Every(int64_t interval) : __interval_without_unit(interval), __interval_ms(interval) {};
            Every() = delete;
            virtual ~Every() = default;

            /// @brief 设置时间间隔为毫秒
            /// @return *this
            Every &milliseconds()
            {
                __interval_ms = __interval_without_unit;
                return *this;
            }

            /// @brief 设置时间间隔为秒
            /// @return *this
            Every &seconds()
            {
                __interval_ms = __interval_without_unit * 1000;
                return *this;
            }

            /// @brief 设置时间间隔为分钟
            /// @return *this
            Every &minutes()
            {
                __interval_ms = __interval_without_unit * 60 * 1000;
                return *this;
            }

            /// @brief 设置时间间隔为小时
            /// @return *this
            Every &hours()
            {
                __interval_ms = __interval_without_unit * 60 * 60 * 1000;
                return *this;
            }

            /// @brief 设置时间间隔为天
            /// @return *this
            Every &days()
            {
                __interval_ms = __interval_without_unit * 24 * 60 * 60 * 1000;
                return *this;
            }

            /// @brief 设置时间间隔为周
            /// @return *this
            Every &weeks()
            {
                __interval_ms = __interval_without_unit * 7 * 24 * 60 * 60 * 1000;
                return *this;
            }

            /// @brief 设置时间间隔为月
            /// @return *this
            Every &months()
            {
                __interval_ms = __interval_without_unit * 30 * 24 * 60 * 60 * 1000;
                return *this;
            }

            /// @brief 滞后指定的时间后进行第一次调用
            /// @return *this
            Every &deffer(int64_t deffer_ms)
            {
                __last_run = deffer_ms; // TODO：此处逻辑还需要处理，临时写的
                return *this;
            }

            /// @brief 构造函数：接受任意可调用对象
            template <typename Func, typename... Args,
                      typename = std::enable_if<!std::is_member_function_pointer<Func>::value>>
            void run(Func &&func, Args &&...args)
            {
                __task = bind(std::forward<Func>(func), std::forward<Args>(args)...);
            }

            /// @brief 构造函数：接受类成员函数
            template <typename ReturnType, typename T, typename... Args>
            void run(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
            {
                __task = bind(func, obj, std::forward<Args>(args)...);
            }

            /// @brief 构造函数：接受静态函数或自由函数
            template <typename ReturnType, typename... Args>
            void run(ReturnType (*func)(Args...), Args &&...args)
            {
                __task = bind(func, std::forward<Args>(args)...);
            }

            /// @brief 设置运行任务
            /// @note 处理已经绑定好的函数
            void run(std::function<void()> &&func)
            {
                __task = std::move(func);
            }

            /// @brief 设置运行任务
            /// @note 处理已经绑定好的函数
            void run(const std::function<void()> &func)
            {
                __task = func;
            }

        private:
            int64_t __interval_without_unit{0}; // 时间间隔，没有单位
            int64_t __interval_ms{0};           // 时间间隔，单位为毫秒
            int64_t __last_run{0};              // 滞后时间，单位为毫秒
            std::function<void()> __task;       // 存储绑定的任务
        };

        Schedule() = default;
        virtual ~Schedule() = default;

        std::shared_ptr<Every> every(int64_t interval)
        {
            auto task = std::make_shared<Every>(interval);
            __tasks.push_back(task);
            return task;
        }

    private:
        inline static std::list<std::shared_ptr<Every>> __tasks;
    };
}

#endif // __COLLAB_SCHEDULE_H__