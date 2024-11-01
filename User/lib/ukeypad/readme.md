# uKeyPad

## 简介

uKeyPad 是一个基于 ESP32 的键盘模块，它可以通过蓝牙与电脑进行通信，模拟键盘按键操作。uKeyPad 支持各类型的按键，只需要提供相应的检测函数即可。

## 功能


## 使用方法

```c++
#include "keyboard.h"
#include "utools.h"

/// @brief 处理按键相关的功能，每20ms处理一次
/// @param vpram void* 参数
static void update_key_event(void *vpram)
{
    utools::logger_info("Registered buttons:", Keyboard::instance().get_keys().size());

    utools::time::make_stable_interval_invoker(
        static_cast<void (Keyboard::*)(void)>(&Keyboard::update), &Keyboard::instance())
        .set_invoker_interval(10)
        .run_forever();
}

static void on_key_event(const int32_t key_id, const Keyboard::Event event)
{
    switch (key_id)
    {
    case KEY_CTRL_MODE:
        if (event == Keyboard::Event::RELEASED)
        {
            utools::logger_info("Device mode changed to: ", device_mode);
        }
        break;
    case KEY_CTRL_RECENT:
        if (event == Keyboard::Event::RELEASED)
        {
            utools::logger_info("Device mode set to recenter.");
        }
        break;
    case KEY_CTRL_POWER:
        if (event == Keyboard::Event::RELEASED)
        {
            utools::logger_info("Power button RELEASED.");
        }
        else if (event == Keyboard::Event::PRESSED)
        {
            utools::logger_info("Power button PRESSED.");
        }
        else if (event == Keyboard::Event::DOUBLE_CLICK)
        {
            utools::logger_info("Power button DOUBLE_CLICK.");
        }
        else if (event == Keyboard::Event::LONG_PRESS)
        {
            utools::logger_info("Power button LONG_PRESS.");
        }
        break;
    case KEY_PAN_REVERSE_LEFT:
        if (event == Keyboard::Event::PRESSED)
        {
            utools::logger_info("PAN_REVERSE_LEFT pressed.");
        }
        else if (event == Keyboard::Event::RELEASED)
        {
            utools::logger_info("PAN_REVERSE_LEFT released.");
        }
        break;
    case KEY_TILT_REVERSE_RIGHT:
        if (event == Keyboard::Event::PRESSED)
        {
            utools::logger_info("TILT_REVERSE_RIGHT pressed.");
        }
        else if (event == Keyboard::Event::RELEASED)
        {
            utools::logger_info("TILT_REVERSE_RIGHT released.");
        }
        break;
    default:
        break;
    }
}

void setup()
{
    // 配置按键相关的功能
    Keyboard::instance()
        .bind_event_handle(on_key_event)
        .add_keys(
            {Keyboard::Key(KEY_CTRL_MODE, Keyboard::KeyAttr(), HIGH, digitalRead, CTRL_BTN_MODE),
             Keyboard::Key(KEY_CTRL_RECENT, Keyboard::KeyAttr(), HIGH, digitalRead, CTRL_BTN_RECENT),
             Keyboard::Key(KEY_CTRL_POWER, Keyboard::KeyAttr(), LOW, digitalRead, CTRL_BTN_POWER),
             Keyboard::Key(KEY_PAN_REVERSE_LEFT, Keyboard::KeyAttr(), HIGH, digitalRead, PAN_REVERSE_SWITCH),
             Keyboard::Key(KEY_TILT_REVERSE_RIGHT, Keyboard::KeyAttr(), HIGH, digitalRead, TILT_REVERSE_SWITCH)});
    // 创建任务
    xTaskCreate(update_key_event, "update_key_event", 4096, NULL, 1, NULL);
}

int loop()
{
    delay(1000);
}

```
