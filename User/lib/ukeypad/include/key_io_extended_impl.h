/// @brief 按键接口扩展实现，除了处理按下和释放外，还可以用于处理按键的单击、长按、双击、周期触发等复杂操作
#ifndef __KEY_IO_EXTENDED_IMPL_H__
#define __KEY_IO_EXTENDED_IMPL_H__

#include <cstdint>

class KeyIOExtendedImpl
{
public:
    /// @brief 按键状态枚举
    enum class KeyEvent
    {
        NONE,          // 无事件
        PRESSED,       // 按下事件
        RELEASED,      // 释放事件
        DOUBLE_CLICK,  // 双击事件
        LONG_PRESS,    // 长按事件
        PERIOD_TRIGGER // 周期触发事件
    };

    /// @brief 初始化按键接口
    /// @param pressed_delay_ms 按键按下延时长，单位为ms
    /// @param release_delay_ms 按键释放延时长，单位为ms
    /// @param double_click_interval_ms 双击间隔时间，单位为ms
    /// @param long_press_threshold_ms 长按触发时间，单位为ms
    /// @param period_trigger_interval_ms 周期触发间隔时间，单位为ms
    KeyIOExtendedImpl(int pressed_delay_ms = 10, int release_delay_ms = 10,
                      int double_click_interval_ms = 500, int long_press_threshold_ms = 1000,
                      int period_trigger_interval_ms = 1000)
        : pressed_delay(pressed_delay_ms), release_delay(release_delay_ms),
          double_click_interval(double_click_interval_ms), long_press_threshold(long_press_threshold_ms),
          period_trigger_interval(period_trigger_interval_ms), state(state_t::IDLE),
          last_time(0), last_click_time(0), long_press_active(false),
          period_trigger_next_interval(period_trigger_interval_ms) {}

    /// @brief 更新按键状态
    /// @param input 是否按下，true表示按下，false表示释放
    /// @param time_ms 当前时间，单位为ms
    /// @return 按键事件类型
    KeyEvent update(bool input, const int64_t &time_ms)
    {
        KeyEvent event = KeyEvent::NONE;

        switch (state)
        {
        case state_t::IDLE:
            if (input)
            {
                state = state_t::PRESSED_DELAY;
                last_time = time_ms;
            }
            break;
        case state_t::PRESSED_DELAY:
            if (!input)
            {
                state = state_t::IDLE;
            }
            else if (time_ms - last_time >= pressed_delay)
            {
                state = state_t::PRESSED_STABLE;
                event = KeyEvent::PRESSED; // 按下事件
            }
            break;
        case state_t::PRESSED_STABLE:
            if (!input)
            {
                state = state_t::RELEASE_DELAY;
                last_time = time_ms;
                long_press_active = false;
                period_trigger_next_interval = period_trigger_interval;
            }
            else
            {
                if (time_ms - last_time >= long_press_threshold && !long_press_active)
                {
                    long_press_active = true;
                    event = KeyEvent::LONG_PRESS;
                }
                else if (time_ms - last_time >= period_trigger_next_interval)
                {
                    period_trigger_next_interval += period_trigger_interval;
                    event = KeyEvent::PERIOD_TRIGGER;
                }
            }
            break;
        case state_t::RELEASE_DELAY:
            if (input)
            {
                state = state_t::PRESSED_STABLE;
            }
            else if (time_ms - last_time >= release_delay)
            {
                state = state_t::IDLE;
                event = __handle_double_click(time_ms) ? KeyEvent::DOUBLE_CLICK : KeyEvent::RELEASED; // 判断是否有双击事件
            }
            break;
        }
        return event;
    }

    int pressed_delay;           // 按下延迟
    int release_delay;           // 放开延迟
    int double_click_interval;   // 双击间隔时间
    int long_press_threshold;    // 长按触发时间
    int period_trigger_interval; // 周期触发间隔时间

private:
    /// @brief 按键输入状态
    enum class state_t
    {
        IDLE,           // 空闲状态，此时按键是释放的，是的于RELEASE_STABLE状态
        PRESSED_DELAY,  // 不稳定按下状态
        PRESSED_STABLE, // 稳定按下状态
        RELEASE_DELAY   // 不稳定放开状态
    };

    state_t state;                        // 当前状态，初始为空闲状态
    int64_t last_time;                    // 记录状态改变的时间戳
    int64_t last_click_time;              // 记录最后一次点击的时间戳
    bool long_press_active;               // 记录长按是否已触发
    int64_t period_trigger_next_interval; // 记录下次周期触发的时间间隔

    /// @brief 处理单击事件
    /// @param time_ms 当前时间，单位为ms
    /// @return 判断是否发生了双击事件
    bool __handle_double_click(const int64_t &time_ms)
    {
        if (last_click_time == 0 || time_ms - last_click_time > double_click_interval)
        {
            last_click_time = time_ms; // 记录最后一次点击的时间戳
            return false;
        }
        if (time_ms - last_click_time <= double_click_interval)
        {
            last_click_time = 0; // 重置双击时间
            return true;
        }
        return false;
    }
};

#endif // __KEY_IO_EXTENDED_IMPL_H__
