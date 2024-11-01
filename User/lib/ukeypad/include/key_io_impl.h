/// @brief 基础按键接口，只支持按下和释放两种状态

#ifndef __KEY_IO_IMPL_H__
#define __KEY_IO_IMPL_H__

#include <cstdint>

class KeyIOImpl
{
public:
    /// @brief 初始化按键接口
    /// @param pressed_delay_ms 按键按下延时长，单位为ms
    /// @param release_delay_ms 按键释放延时长，单位为ms
    KeyIOImpl(int pressed_delay_ms = 10, int release_delay_ms = 10)
        : pressed_delay(pressed_delay_ms), release_delay(release_delay_ms), state(state_t::IDLE), last_time(0) {}

    /// @brief 更新按键状态
    /// @param input 是否按下，true表示按下，false表示释放
    /// @param time_ms 当前时间，单位为ms
    /// @return true表示按键按下，false表示按键释放
    bool update(bool input, const int64_t &time_ms)
    {
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
                output = true;
            }
            break;
        case state_t::PRESSED_STABLE:
            if (!input)
            {
                state = state_t::RELEASE_DELAY;
                last_time = time_ms;
            }
            break;
        case state_t::RELEASE_DELAY:
            if (input)
            {
                state = state_t::PRESSED_STABLE;
                output = true;
            }
            else if (time_ms - last_time >= release_delay)
            {
                state = state_t::IDLE;
                output = false;
            }
            break;
        }
        return output;
    }

private:
    /// @brief 按键输入状态
    enum class state_t
    {
        IDLE,           // 空闲状态，此时按键是释放的，是的于RELEASE_STABLE状态
        PRESSED_DELAY,  // 不稳定按下状态
        PRESSED_STABLE, // 稳定按下状态
        RELEASE_DELAY   // 不稳定放开状态
    };

    int pressed_delay{15};        // 按下延迟
    int release_delay{15};        // 放开延迟
    state_t state{state_t::IDLE}; // 当前状态，初始为空闲状态
    int64_t last_time;            // 记录状态改变的时间戳
    bool output{false};           // 输出状态，用于输出按键是否被按下
};

#endif // __KEY_IO_IMPL_H__