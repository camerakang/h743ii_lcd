/// @brief SDCP I/O implementation
///        用于实现物理接收和发送接口
///        整个功能分为两层，分别是【物理层】和【协议层】
///        物理层负责接收和发送数据，协议层负责解析数据
///        本文档定义了物理层接口，用于实现与硬件的交互

#ifndef __SDCP_IO_IMPL_H__
#define __SDCP_IO_IMPL_H__

#include <stdint.h>
#include <vector>
#include <string>

namespace sdcp
{
    /// @brief 物理层的参数类型
    enum class PhyLayerParamType : int32_t
    {
        MODULE_STATUS_CONFIG, // 模块状态配置
        MODULE_CTRL_CONFIG,   // 模块配置
        FREQ_CONFIG,          // 频率配置
        MODULATION_CONFIG,    // 调制方式配置
    };

    /// @brief 无线电波频率
    enum class PhyLayerCongig : int32_t
    {
        // 读取模块状态
        MODULE_STATUS_POWER_DOWN, // 模块断电状态
        MODULE_STATUS_RUNNING,    // 模块运行状态
        MODULE_STATUS_SLEEP,      // 模块休眠状态
        MODULE_STATUS_FAIL,       // 模块故障状态

        // 模块配置，MODULE_CONFIG
        MODULE_CTRL_POWER_UP,   // 模块上电
        MODULE_CTRL_RESET,      // 模块复位
        MODULE_CTRL_SLEEP,      // 模块休眠
        MODULE_CTRL_WAKEUP,     // 模块唤醒
        MODULE_CTRL_POWER_DOWN, // 模块断电

        // 频率相关配置，FREQ_CONFIG
        FREQ_MIXED,   // 混合模式
        FREQ_ISM24XX, // 2.4G模式
        FREQ_ISM9XX,  // 900M模式

        // 调制方式配置，MODULATION_CONFIG
        MODULATION_FIXED, // 指定频率
        MODULATION_ADAPT, // 自适应频率，选择选择
    };

    /// @brief 物理层信息
    enum class PhyLayerInfo : int32_t
    {
        SUCCESS = 0,      // 操作成功
        FAILED = -1,      // 操作失败
        ERROR_PARAM = -2, // 参数错误
    };

    /// @brief 物理层错误码
    enum class PhyLayerError : int32_t
    {
    };

    /// @brief 用户需要继承该类，实现自己的vtty IO操作
    class IOImpl
    {
    public:
        virtual ~IOImpl() {};

        /// @brief 由硬件层调用，将接收到的数据写入到缓存中
        /// @param data 硬件层接收到的数据
        /// @param len 数据长度
        virtual void write(const uint8_t *data, uint32_t len) = 0;

        /// @brief 由硬件层读取协议层中是否还有可以发送的数据
        /// @return 返回协议层在带发送的数据包数量
        virtual uint32_t available() = 0;

        /// @brief 由硬件层调用，读取协议层中的数据
        /// @return 读取一帧数据，如果读取失败，返回空
        virtual std::vector<uint8_t> read() = 0;

        /// @brief 控制物理层参数
        /// @param cfg 物理层配置
        /// @param in_param 配置参数，需要通知物理的信息，非必须
        /// @return 物理层信息
        virtual const PhyLayerInfo pyh_set(const PhyLayerParamType plpy, const PhyLayerCongig cfg, void *in_param = nullptr) = 0;

        /// @brief 获取物理层参数
        /// @param plpy 物理层参数类型
        /// @param out_param 物理层中具体要的参数，非必须
        /// @return 物理层参数
        virtual const PhyLayerCongig phy_get(PhyLayerParamType plpy, void *out_param = nullptr) = 0;

        /// @brief 获取错误信息
        /// @return 错误信息
        /// @note 此错误信息为最近一次错误，获取后，将自动清空相关错误缓存
        virtual const PhyLayerError get_error() = 0;

        /// @brief 获取错误信息字符串
        /// @return 错误信息字符串
        virtual const std::string get_error_str() = 0;

        /// @brief 清空输入到系统的缓存
        /// @note 函数会在初始化和退出时调用此功能
        virtual void flush_in() {};

        /// @brief 清空输出到系统的缓存
        /// @note 函数会在初始化和退出时调用此功能
        virtual void flush_out() {};
    };
}

#endif // __SDCP_IO_IMPL_H__
