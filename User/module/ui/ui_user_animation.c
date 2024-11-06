#include "ui_user_animation.h"
#include <stdio.h> // 用于 sprintf 函数

// 动画回调函数，用于更新标签的文本
void label_anim_cb(void * var, int32_t v)
{
    char buf[4]; // 用于保存整数到字符串的转换结果
    sprintf(buf, "%d", v); // 将整数转换为字符串
    lv_label_set_text((lv_obj_t *)var, buf); // 设置标签文本
}

// 动画结束回调函数，用于创建下一阶段的动画
void label_anim_ready_cb(lv_anim_t * a)
{
    int32_t start = strcmp(lv_label_get_text(a->var), "100") == 0 ? 100 : 0;
    int32_t end = start == 0 ? 100 : 0;

    lv_anim_init(a);
    lv_anim_set_var(a, a->var);
    lv_anim_set_values(a, start, end);
    lv_anim_set_time(a, 2000); // 动画时间，单位毫秒
    lv_anim_set_playback_delay(a, 0); // 设置回放延迟
    lv_anim_set_playback_time(a, 2000); // 设置回放时间
    lv_anim_set_repeat_delay(a, 0); // 设置重复延迟
    lv_anim_set_repeat_count(a, LV_ANIM_REPEAT_INFINITE); // 无限重复
    lv_anim_set_exec_cb(a, label_anim_cb);
    lv_anim_set_ready_cb(a, label_anim_ready_cb);
    lv_anim_start(a);
}

// 创建标签动画
void create_label_animation(lv_obj_t * label)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    lv_anim_set_values(&a, 0, 100); // 从 0 到 100
    lv_anim_set_time(&a, 2000); // 动画时间，单位毫秒
    lv_anim_set_playback_delay(&a, 0); // 设置回放延迟
    lv_anim_set_playback_time(&a, 2000); // 设置回放时间
    lv_anim_set_repeat_delay(&a, 0); // 设置重复延迟
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 无限重复
    lv_anim_set_exec_cb(&a, label_anim_cb);
    lv_anim_set_ready_cb(&a, label_anim_ready_cb);
    lv_anim_start(&a);
}

// 在适当的位置调用此函数来初始化动画



// 动画回调函数，用于更新滑块的值
void slider_anim_cb(void * var, int32_t v)
{
    lv_slider_set_value((lv_obj_t *)var, v, LV_ANIM_OFF);
}

// 动画结束回调函数，用于创建下一阶段的动画
void slider_anim_ready_cb(lv_anim_t * a)
{
    int32_t start = lv_slider_get_value(a->var) == 100 ? 100 : 0;
    int32_t end = start == 0 ? 100 : 0;

    lv_anim_init(a);
    lv_anim_set_var(a, a->var);
    lv_anim_set_values(a, start, end);
    lv_anim_set_time(a, 2000); // 动画时间，单位毫秒
    lv_anim_set_playback_delay(a, 0); // 设置回放延迟
    lv_anim_set_playback_time(a, 2000); // 设置回放时间
    lv_anim_set_repeat_delay(a, 0); // 设置重复延迟
    lv_anim_set_repeat_count(a, LV_ANIM_REPEAT_INFINITE); // 无限重复
    lv_anim_set_exec_cb(a, slider_anim_cb);
    lv_anim_set_ready_cb(a, slider_anim_ready_cb);
    lv_anim_start(a);
}

// 创建滑块动画
void create_slider_animation(lv_obj_t * slider)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, slider);
    lv_anim_set_values(&a, 0, 100); // 从 0 到 100
    lv_anim_set_time(&a, 2000); // 动画时间，单位毫秒
    lv_anim_set_playback_delay(&a, 0); // 设置回放延迟
    lv_anim_set_playback_time(&a, 2000); // 设置回放时间
    lv_anim_set_repeat_delay(&a, 0); // 设置重复延迟
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); // 无限重复
    lv_anim_set_exec_cb(&a, slider_anim_cb);
    lv_anim_set_ready_cb(&a, slider_anim_ready_cb);
    lv_anim_start(&a);
}
