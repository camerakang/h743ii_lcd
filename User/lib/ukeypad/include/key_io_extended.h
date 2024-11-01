/// @brief 按键功能扩展，将按键与检测功能合并，更方便调用

#ifndef __KEY_IO_EXTENDED_H__
#define __KEY_IO_EXTENDED_H__

#include <functional>
#include <chrono>
#include "key_io_extended_impl.h"

class KeyIOExtended : public KeyIOExtendedImpl
{
public:
    int32_t key_id;                       // 按键ID号
    std::function<bool(void)> is_pressed; // 按键按下回调

    struct trigger_attr_t
    {
        int32_t pressed_delay_ms{10};
        int32_t released_delay_ms{10};
        int32_t double_click_interval_ms{500};
        int32_t long_press_threshold_ms{1000};
        int32_t period_trigger_interval_ms{1000};

        trigger_attr_t(int pressed_delay_ms = 10, int release_delay_ms = 10,
                       int double_click_interval_ms = 500, int long_press_threshold_ms = 1000,
                       int period_trigger_interval_ms = 1000)
            : pressed_delay_ms(pressed_delay_ms),
              released_delay_ms(release_delay_ms),
              double_click_interval_ms(double_click_interval_ms),
              long_press_threshold_ms(long_press_threshold_ms),
              period_trigger_interval_ms(period_trigger_interval_ms) {};
    };

    /// @brief 构造函数
    /// @param key_id 按键ID号
    KeyIOExtended(uint32_t key_id) : KeyIOExtendedImpl(), key_id(key_id) {}

    /// @brief 构造函数
    /// @param key_id 按键ID号
    /// @param trigger_attr 触发属性
    KeyIOExtended(uint32_t key_id, const trigger_attr_t &trigger_attr)
        : KeyIOExtendedImpl(trigger_attr.pressed_delay_ms, trigger_attr.released_delay_ms,
                            trigger_attr.double_click_interval_ms, trigger_attr.long_press_threshold_ms,
                            trigger_attr.period_trigger_interval_ms),
          key_id(key_id)
    {
    }

    /// @brief 增加一个新的按键
    /// @tparam PRED_VAL 判断值类型
    /// @tparam PRED_FUN 判断函数
    /// @tparam ...Args 参数类型
    /// @param name 按键名称
    /// @param pred_fun 判断是否按下的函数
    /// @param ...args 输入参数
    /// @return keypad对象
    template <typename PRED_VAL, typename PRED_FUN, typename... Args>
    KeyIOExtended(uint32_t key_id, const trigger_attr_t &trigger_attr,
                  const PRED_VAL pred_val, PRED_FUN is_pressed_func, Args &&...args)
        : KeyIOExtended(key_id, trigger_attr)
    {
        bind_pressed_pred(pred_val, is_pressed_func, std::forward<Args>(args)...);
    };

    /// @brief 增加一个新的按键
    /// @tparam ReturnType 回调函数的返回值类型
    /// @tparam ...Args 参数类型
    /// @param name 按键名称
    /// @param is_pressed 判断是否按下的函数
    /// @param ...args 输入参数
    /// @return keypad对象
    template <typename ReturnType, typename... Args>
    KeyIOExtended(uint32_t key_id, const trigger_attr_t &trigger_attr,
                  const ReturnType pred_val, ReturnType (*is_pressed_func)(Args...), Args &&...args)
        : KeyIOExtended(key_id, trigger_attr)
    {
        bind_pressed_pred(pred_val, is_pressed_func, std::forward<Args>(args)...);
    };

    /// @brief 增加一个新的按键
    /// @tparam ReturnType 回调函数的返回值类型
    /// @tparam T 类对象
    /// @tparam ...Args 参数类型
    /// @param name 按键名称
    /// @param is_pressed_func 判断是否按下的函数
    /// @param obj 类对象
    /// @param ...args 输入参数
    /// @return keypad对象
    template <typename ReturnType, typename T, typename... Args>
    KeyIOExtended(uint32_t key_id, const trigger_attr_t &trigger_attr,
                  const ReturnType pred_val, ReturnType (T::*is_pressed_func)(Args...), T *obj, Args &&...args)
        : KeyIOExtended(key_id, trigger_attr)
    {
        bind_pressed_pred(pred_val, is_pressed_func, obj, std::forward<Args>(args)...);
    }

    template <typename PRED_VAL, typename PRED_FUN, typename... Args>
    void bind_pressed_pred(PRED_VAL pred_val, PRED_FUN is_pressed_func, Args &&...args)
    {
        auto bound_func = std::bind(is_pressed_func, std::forward<Args>(args)...);
        is_pressed = [pred_val, bound_func]() -> bool
        {
            return bound_func() == pred_val;
        };
    }

    template <typename ReturnType, typename... Args>
    void bind_pressed_pred(ReturnType pred_val, ReturnType (*is_pressed_func)(Args...), Args &&...args)
    {
        auto bound_func = std::bind(is_pressed_func, std::forward<Args>(args)...);
        is_pressed = [pred_val, bound_func]() -> bool
        {
            return bound_func() == pred_val;
        };
    }

    template <typename ReturnType, typename T, typename... Args>
    void bind_pressed_pred(ReturnType pred_val, ReturnType (T::*is_pressed_func)(Args...), T *obj, Args &&...args)
    {
        auto bound_func = std::bind(is_pressed_func, obj, std::forward<Args>(args)...);
        is_pressed = [pred_val, bound_func]() -> bool
        {
            return bound_func() == pred_val;
        };
    }

    /// @brief 读取按键的触发属性
    /// @return 触发属性结构体
    trigger_attr_t get_trigger_attr()
    {
        return trigger_attr_t{pressed_delay,
                              release_delay,
                              double_click_interval,
                              long_press_threshold,
                              period_trigger_interval};
    }

    /// @brief 设置按键的触发属性
    /// @param trigger_attr 触发属性结构体
    void set_trigger_attr(const trigger_attr_t &trigger_attr)
    {
        pressed_delay = trigger_attr.pressed_delay_ms;
        release_delay = trigger_attr.released_delay_ms;
        double_click_interval = trigger_attr.double_click_interval_ms;
        long_press_threshold = trigger_attr.long_press_threshold_ms;
        period_trigger_interval = trigger_attr.period_trigger_interval_ms;
    }

    /// @brief 更新按键状态
    /// @param time_ms 当前时间，单位为ms
    /// @return 按键事件类型
    KeyEvent update(const int64_t &time_ms)
    {
        return KeyIOExtendedImpl::update(is_pressed(), time_ms);
    }

    KeyEvent update()
    {
        return KeyIOExtendedImpl::update(
            is_pressed(),
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }

    virtual ~KeyIOExtended() = default;

    KeyIOExtended() = delete;
};

#endif // __KEY_IO_EXTENDED_H__