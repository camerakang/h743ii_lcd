#ifndef __FZI_MAP_H__
#define __FZI_MAP_H__

#include "utools.h"
#include "router.h"
#include <map>
#include <functional>

class FZIMap : public utpattern::Singleton<FZIMap>
{
public:
    FZIMap()
    {
        router::pubsub.subscribe(router::topic_type::KNOB_NORMALIZED_VALUE, &FZIMap::__on_knob_change, this);
        router::pubsub.subscribe(router::topic_type::ROCKER_NORMALIZED_VALUE, &FZIMap::__on_rocker_change, this);
        router::pubsub.subscribe(router::topic_type::SLIDER_NORMALIZED_VALUE, &FZIMap::__on_slider_change, this);
        router::pubsub.subscribe(router::topic_type::KNOB_FUNC_MAP_CONFIG, &FZIMap::__on_func_map_change, this);
        router::pubsub.subscribe(router::topic_type::ROCKER_FUNC_MAP_CONFIG, &FZIMap::__on_func_map_change, this);
        router::pubsub.subscribe(router::topic_type::SLIDER_FUNC_MAP_CONFIG, &FZIMap::__on_func_map_change, this);
    }

    virtual ~FZIMap() = default;

private:
    /// @brief 功能映射表，用于更新数据使用
    std::map<uint8_t, std::function<void(uint8_t)>> __funcs{
        {FUNC_FOCUS, std::bind(&decltype(router::pubsub)::publish<float>,
                               &router::pubsub, router::topic_type::FOCUS_VALUE, std::placeholders::_1)},
        {FUNC_ZOOM, std::bind(&decltype(router::pubsub)::publish<float>,
                              &router::pubsub, router::topic_type::ZOOM_VALUE, std::placeholders::_1)},
        {FUNC_IRIS, std::bind(&decltype(router::pubsub)::publish<float>,
                              &router::pubsub, router::topic_type::IRIS_VALUE, std::placeholders::_1)}};

    /// @brief 功能与传感器的映射关系
    std::map<uint8_t, std::function<void(uint8_t)>> __func_map{
        {SENSOR_KNOB, __funcs[FUNC_FOCUS]},
        {SENSOR_SLIDER, __funcs[FUNC_ZOOM]},
        {SENSOR_ROCKER, __funcs[FUNC_IRIS]}};

    /// @brief 接收knob消息
    /// @param topic 主题类型
    /// @param msgwrapper 数据包装器
    void __on_knob_change(const router::topic_type &topic, router::message_wrapper_type msgwrapper)
    {
        __func_map[SENSOR_KNOB](msgwrapper.cast<float>());
    }

    /// @brief 接收Roker消息
    /// @param topic 主题类型
    /// @param msgwrapper 数据包装器
    void __on_rocker_change(const router::topic_type &topic, router::message_wrapper_type msgwrapper)
    {
        __func_map[SENSOR_ROCKER](msgwrapper.cast<float>());
    }

    /// @brief 接收Slider消息
    /// @param topic 主题类型
    /// @param msgwrapper 数据包装器
    void __on_slider_change(const router::topic_type &topic, router::message_wrapper_type msgwrapper)
    {
        __func_map[SENSOR_SLIDER](msgwrapper.cast<float>());
    }

    /// @brief 接收功能映射变化消息
    /// @param topic 主题类型
    /// @param msgwrapper 数据包装器
    /// @note 1表示FOCUS，2表示ZOOM，3表示IRIS
    void __on_func_map_change(const router::topic_type &topic, router::message_wrapper_type msgwrapper)
    {
        switch (topic)
        {
        case router::table_type::KNOB_FUNC_MAP_CONFIG:
            __func_map[SENSOR_KNOB] = __funcs[msgwrapper.cast<int>()];
            break;
        case router::table_type::SLIDER_FUNC_MAP_CONFIG:
            __func_map[SENSOR_SLIDER] = __funcs[msgwrapper.cast<int>()];
            break;
        case router::table_type::ROCKER_FUNC_MAP_CONFIG:
            __func_map[SENSOR_ROCKER] = __funcs[msgwrapper.cast<int>()];
            break;
        default:
            break;
        }
    }
};

#endif // __FZI_MAP_H__