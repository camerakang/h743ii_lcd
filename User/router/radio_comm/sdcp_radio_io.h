#ifndef __SDCP_RADIO_IO_H__
#define __SDCP_RADIO_IO_H__

#include "sdcp_io_impl.h"

/// @brief SDCP协议层与硬件层之间的接口
/// @note 基于STM32H7与ESP32连接串口实现，stm32h7为uart4
class SDCPRadioIO : public sdcp::IOImpl
{
public:
    /// @brief 由硬件层调用，将接收到的数据写入到缓存中
    /// @param data 硬件层接收到的数据
    /// @param len 数据长度
    virtual void write(const uint8_t *data, uint32_t len) override
    {
    }

    /// @brief 由硬件层读取协议层中是否还有可以发送的数据
    /// @return 返回协议层在带发送的数据包数量
    virtual uint32_t available() override
    {
    }

    /// @brief 由硬件层调用，读取协议层中的数据
    /// @return 读取一帧数据，如果读取失败，返回空
    virtual std::vector<uint8_t> read() override
    {
    }

    /// @brief 控制物理层参数
    /// @param cfg 物理层配置
    /// @param in_param 配置参数，需要通知物理的信息，非必须
    /// @return 物理层信息
    virtual const sdcp::PhyLayerInfo pyh_set(
        const sdcp::PhyLayerParamType plpy, const sdcp::PhyLayerCongig cfg, void *in_param = nullptr) override
    {
    }

    /// @brief 获取物理层参数
    /// @param plpy 物理层参数类型
    /// @param out_param 物理层中具体要的参数，非必须
    /// @return 物理层参数
    virtual const sdcp::PhyLayerCongig phy_get(sdcp::PhyLayerParamType plpy, void *out_param = nullptr) override
    {
    }

    /// @brief 获取错误信息
    /// @return 错误信息
    /// @note 此错误信息为最近一次错误，获取后，将自动清空相关错误缓存
    virtual const sdcp::PhyLayerError get_error() override
    {
    }

    /// @brief 获取错误信息字符串
    /// @return 错误信息字符串
    virtual const std::string get_error_str() override
    {
    }
};

#endif // __SDCP_RADIO_IO_H__