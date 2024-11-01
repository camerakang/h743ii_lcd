/// @brief 路由表，用于存储路由信息，需要在程序内进行路由的功能都在此进行配置

#ifndef __ROUTERING_TABLE_H__
#define __ROUTERING_TABLE_H__

/// @brief 路由表宏定义
/// @note KNOB_NORMALIZED_VALUE：手轮归一化值，取值范围[0,1]，float类型
/// @note ROCKER_NORMALIZED_VALUE：摇杆归一化值，取值范围[0,1]，float类型
/// @note ROCKER_PUSH_ROD_VALUE：摇杆推杆量归一化值，取值范围[-1,1]，float类型，向上为正，向下为负
/// @note SLIDER_NORMALIZED_VALUE：滑条归一化值，取值范围[0,1]，float类型
/// @note PWRMANG_BATTERY_LEVEL：电源管理电池电量，取值范围[0,100]，int类型
/// @note PWRMANG_VBUS_SUPPLY：电源管理VBUS供电状态，取值范围[0|1]，int类型，0表示未供电，1表示供电
/// @note KNOB_FUNC_MAP_CONFIG：手轮功能映射配置，1表示FOCUS，2表示ZOOM，3表示IRIS
/// @note ROCKER_FUNC_MAP_CONFIG：摇杆功能映射配置，1表示FOCUS，2表示ZOOM，3表示IRIS
/// @note SLIDER_FUNC_MAP_CONFIG：滑条功能映射配置，1表示FOCUS，2表示ZOOM，3表示IRIS
/// @note FOCUS_VALUE：焦距值，取值范围[0,1]，float类型
/// @note ZOOM_VALUE：变焦值，取值范围[0,1]，float类型
/// @note IRIS_VALUE：光圈值，取值范围[0,1]，float类型
#define RUTERING_TABLE_VALUES                 \
    RUTERINT_TABLE_X(KNOB_NORMALIZED_VALUE)   \
    RUTERINT_TABLE_X(ROCKER_NORMALIZED_VALUE) \
    RUTERINT_TABLE_X(ROCKER_PUSH_ROD_VALUE)   \
    RUTERINT_TABLE_X(SLIDER_NORMALIZED_VALUE) \
    RUTERINT_TABLE_X(PWRMANG_BATTERY_LEVEL)   \
    RUTERINT_TABLE_X(PWRMANG_VBUS_SUPPLY)     \
    RUTERINT_TABLE_X(KNOB_FUNC_MAP_CONFIG)    \
    RUTERINT_TABLE_X(ROCKER_FUNC_MAP_CONFIG)  \
    RUTERINT_TABLE_X(SLIDER_FUNC_MAP_CONFIG)  \
    RUTERINT_TABLE_X(FOCUS_VALUE)             \
    RUTERINT_TABLE_X(ZOOM_VALUE)              \
    RUTERINT_TABLE_X(IRIS_VALUE)              \
// TODO：在此处添加新的路由表项

/// @brief 路由表定义
typedef enum
{
#define RUTERINT_TABLE_X(name) name,
    RUTERING_TABLE_VALUES
#undef RUTERINT_TABLE_X
} routing_table;

#endif // __ROUTERING_TABLE_H__s
