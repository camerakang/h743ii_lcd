#ifndef __APP_ENTRY_H__
#define __APP_ENTRY_H__

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief 初始化配置，主要是自己的启动配置等，非外设初始化
    void setup();

    /// @brief 主循环
    void loop();

#ifdef __cplusplus
}
#endif

#endif // __APP_ENTRY_H__
