/// @brief 硬件入口程序，用于初始化硬件

#ifndef __HU_ENTRY_H__
#define __HU_ENTRY_H__
#ifdef __cplusplus

extern "C"
{
#endif

    /// @brief 硬件入口程序，用于初始化硬件
    /// @note 该函数应在主函数中调用
    void hu_entry(void);

#ifdef __cplusplus
}
#endif

#endif // __HU_ENTRY_H__
