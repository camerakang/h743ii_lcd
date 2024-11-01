#ifndef __KNOB_DJI_DRIVER_H__
#define __KNOB_DJI_DRIVER_H__

#include "normalized_sensor_impl.h"
#include "utools.h"
#include "sensor_def.h"

class KnobDJIDriver : public NormalizedSensorImpl, public utpattern::Singleton<KnobDJIDriver>
{
private:
    utfliter::MovingAverageQuick<int32_t, 8> __in_filter{0}; // 控制输入的平滑度

public:
    KnobDJIDriver() : utpattern::Singleton<KnobDJIDriver>()
    {
        // 初始化硬件
        analogReadResolution(12);                    // 设置为 12 位分辨率
        analogSetPinAttenuation(KNOB_PIN, ADC_11db); // 设置衰减为11 dB，测量范围 0 - 3.9V。

        // 初始化滤波啊器
        auto init_sensor_val{analogRead(KNOB_PIN)};
        set_input_range(110, 3815);  // 设置输入范围
        set_accuracy(-0.002, 0.002); // 设置精度
        init(init_sensor_val);       // 初始化为当前值
    }

    InputType read_raw() override
    {
        return __in_filter.calc();
    }

    void reset(OutputType init_value) override
    {
        __in_filter.reinit(init_value);
    }

    void update() override
    {
        __in_filter.push(analogRead(KNOB_PIN));
    }
};

#endif // __KNOB_DJI_DRIVER_H__