#ifndef __NORMALIZED_SENSOR_IMPL_H__
#define __NORMALIZED_SENSOR_IMPL_H__

#include "utools.h"
#include <cstdint>

class NormalizedSensorImpl
{
public:
    typedef int32_t InputType;
    typedef float OutputType;

    NormalizedSensorImpl(InputType input_max = 4095, InputType input_min = 0)
        : __input_max(input_max), __input_min(input_min) {}
    virtual ~NormalizedSensorImpl() = default;

    /// @brief 初始化传感器
    /// @param init_value 初始化值，默认为0
    virtual void init(OutputType init_value = 0)
    {
        reset(init_value);
        __accuracy_filter.reinit(init_value);
    }

    /// @brief 读取传感器原始数据
    /// @return int32_t 传感器数据
    virtual InputType read_raw() = 0;

    /// @brief 重置传感器数据
    /// @param init_value 初始化值，默认为0
    virtual void reset(OutputType init_value = 0) = 0;

    /// @brief 更新传感器值
    virtual void update() = 0;

    /// @brief 读取传感器数据
    /// @return float 传感器数据,范围0-1
    virtual OutputType read()
    {
        return utmath::clamp(
            __accuracy_filter.calc(
                utmath::linear_map(
                    utmath::clamp(read_raw(), __input_min, __input_max),
                    {__input_min, __input_max},
                    {__lower_limit_val, __upper_limit_val})),
            0.0f, 1.0f);
    }

    /// @brief 设置传感器精度输入范围
    /// @param min 最小输入值
    /// @param max 最大输入值
    void set_input_range(InputType min, InputType max)
    {
        __input_min = min;
        __input_max = max;
    }

    /// @brief 设置传感器精度输出范围
    /// @param upper_limit_val 最大误差
    /// @param lower_limit_val 最小误差
    void set_accuracy(OutputType lower_limit_val, OutputType upper_limit_val)
    {
        __upper_limit_val = 1.0f + upper_limit_val; // 重新计算边界值
        __lower_limit_val = 0.0f + lower_limit_val; // 重新计算边界值
        __accuracy_filter.set_limit_range(lower_limit_val, upper_limit_val);
    }

private:
    InputType __input_min{0};
    InputType __input_max{4095};

    OutputType __lower_limit_val{0.0f};
    OutputType __upper_limit_val{1.0f};

    utfliter::LimitBreadth<OutputType> __accuracy_filter{-0.00001, 0.00001}; // 控制输出的精度
};

#endif // __NORMALIZED_SENSOR_IMPL_H__