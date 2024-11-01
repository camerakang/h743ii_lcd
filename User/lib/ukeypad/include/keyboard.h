#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <functional>
#include <vector>
#include <initializer_list>
#include <chrono>
#include <algorithm>

#include "key_io_extended.h"

class Keyboard
{
public:
    using Key = KeyIOExtended;                     // 按键类型
    using Event = KeyIOExtended::KeyEvent;         // 事件类型
    using KeyAttr = KeyIOExtended::trigger_attr_t; // 按键属性

    static Keyboard &instance()
    {
        static Keyboard instance;
        return instance;
    }

    virtual ~Keyboard() {};

    /// @brief 绑定事件处理函数
    /// @param event_handle 事件处理函数
    /// @return 自身引用
    Keyboard &bind_event_handle(std::function<void(const int32_t, const Event &)> event_handle)
    {
        __event_handle = event_handle;
        return *this;
    }

    Keyboard &bind_event_handle(void (*event_handle)(const int32_t, const Event &))
    {
        __event_handle = event_handle;
        return *this;
    }

    template <typename T>
    Keyboard &bind_event_handle(void (T::*event_handle)(const int32_t, const Event &), T *obj)
    {
        __event_handle = [obj, event_handle](const int32_t key_id, const Event &event)
        {
            (obj->*event_handle)(key_id, event);
        };
        return *this;
    }

    /// @brief 周期性调用更新所有按键状态
    void update()
    {
        auto ts_ms{std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count()};
        update(ts_ms);
    }

    /// @brief 周期性调用更新所有按键状态
    /// @param ts_ms 时间戳
    void update(const int64_t &ts_ms)
    {
        for (auto &key : __keys)
        {
            auto key_state = key.update(ts_ms);
            if (key_state != Event::NONE)
            {
                __event_handle(key.key_id, key_state);
                if (key_state == Event::DOUBLE_CLICK)
                {
                    __event_handle(key.key_id, Event::RELEASED);
                }
            }
        }
    }

    /// @brief 获取所有key
    /// @return 所有key
    std::vector<KeyIOExtended> &get_keys()
    {
        return __keys;
    }

    /// @brief 获取指定的key
    /// @param key_id key id
    /// @return nullptr 或 key
    KeyIOExtended *get_key(const int32_t &key_id)
    {
        auto it = std::find_if(__keys.begin(), __keys.end(), [key_id](KeyIOExtended &key)
                               { return key.key_id == key_id; });
        return it != __keys.end() ? &(*it) : nullptr; // 返回 nullptr 如果没找到
    }

    /// @brief 添加key
    /// @param key key
    /// @return 自身引用
    Keyboard &add_key(KeyIOExtended &key)
    {
        __keys.push_back(key);
        return *this;
    }

    /// @brief 添加key
    /// @param key key
    /// @return 自身引用
    Keyboard &add_key(KeyIOExtended &&key)
    {
        __keys.push_back(std::move(key));
        return *this;
    }

    /// @brief 添加多个key
    /// @param keys keys
    /// @return 自身引用
    Keyboard &add_keys(std::initializer_list<KeyIOExtended> keys)
    {
        __keys.insert(__keys.end(), keys.begin(), keys.end());
        return *this;
    }

    /// @brief 移除key
    /// @param key_id key id
    /// @return 自身引用
    Keyboard &remove_key(const int32_t &key_id)
    {
        auto it = std::find_if(__keys.begin(), __keys.end(), [key_id](KeyIOExtended &key)
                               { return key.key_id == key_id; });
        if (it != __keys.end())
        {
            __keys.erase(it);
        }
        return *this;
    }

    /// @brief 移除多个key
    /// @param keys keys
    /// @return 自身引用
    Keyboard &remove_keys(std::initializer_list<int32_t> keys)
    {
        for (auto key_id : keys)
        {
            remove_key(key_id);
        }
        return *this;
    }

    /// @brief 清空所有key
    /// @return 自身引用
    Keyboard &clear_keys()
    {
        __keys.clear();
        return *this;
    }

private:
    Keyboard() {};

    std::vector<KeyIOExtended> __keys; // 所有按键
    std::function<void(const int32_t, const Event &)> __event_handle{
        [](const int32_t, const Event &) -> void {}}; // 事件处理函数
};

#endif // __KEYBOARD_H__
