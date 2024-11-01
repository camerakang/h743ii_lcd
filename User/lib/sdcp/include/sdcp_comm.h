/// @brief SDCP协议使用到的一个宏定义和类型定义

#ifndef __SDCP_COMM_H__
#define __SDCP_COMM_H__

#include <stdint.h>
#include "sdcp_def.h"

namespace sdcp
{
#define SDCP_VERSION 0x01

    /// @brief SDCP协议的端口类型定义
    using port_t = uint16_t;     // 端口号
    using msgid_t = uint16_t;    // 消息ID
    using timestamp_t = int64_t; // 时间戳
    using seq_t = uint32_t;      // 序列号

    // 协议内容相关的宏定义------------------------------------------------
    enum class msg_type_t : uint8_t
    {
        CONNECT = SDCP_MSG_TYPE_CONNECT,
        CONNACK = SDCP_MSG_TYPE_CONNACK,
        MSGPUSH = SDCP_MSG_TYPE_MSGPUSH,
        MSGACK = SDCP_MSG_TYPE_MSGACK,
        MSGREC = SDCP_MSG_TYPE_MSGREC,
        MSGREL = SDCP_MSG_TYPE_MSGREL,
        MSGCOMP = SDCP_MSG_TYPE_MSGCOMP,
        // ... [7,12] reserved 预留
        PINGREQ = SDCP_MSG_TYPE_PINGREQ,
        PINGRESP = SDCP_MSG_TYPE_PINGRESP,
        DISCONNECT = SDCP_MSG_TYPE_DISCONNECT
    };

    /// @brief QoS定义
    enum class qos_t : uint8_t
    {
        QoS0 = SDCP_MSG_QOS0,
        QoS1 = SDCP_MSG_QOS1,
        QoS2 = SDCP_MSG_QOS2
    };

    /// @brief SDCP数据长度
    enum class msg_len_t : uint8_t
    {
        LEVEL0 = SDCP_MSG_LEN_LEVEL0,
        LEVEL1 = SDCP_MSG_LEN_LEVEL1
    };

    /// @brief SDCP协议的扩展标记
    enum class msg_extend_flag_t : uint8_t
    {
        EXTEND0 = SDCP_MSG_EXTEND_FLAG0,
        EXTEND1 = SDCP_MSG_EXTEND_FLAG1
    };

    // 协议运行状态定义------------------------------------------------------
    enum class qos_rx_state_t : uint8_t
    {
        // 所有QoS都有的状态
        RX_MSG_READY, // 接收完成

        // QoS1
        QoS1_WAIT_TX_MSGACK, // 等待发送ACK状态

        // QoS2
        QoS2_WAIT_TX_MSGREC,  // 等待发送MSGREC状态
        QoS2_WAIT_RX_MSGREL,  // 等待接收MSGREL状态
        QoS2_WAIT_TX_MSGCOMP, // 等待发送MSGCOMP状态

        // 所有的QoS都有的状态
        WAIT_RELEASE, // 等待释放状态
    };

    enum class qos_tx_state_t : uint8_t
    {
        // 所有的QoS都有的状态
        WAIT_TX_MSG, // 等待发送数据状态

        // QoS1
        QoS1_WAIT_RX_MSGACK, // 等待消息ACK状态

        // QoS2
        QoS2_WAIT_RX_MSGREC,  // 等待MSGREC状态
        QoS2_WAIT_TX_MSGREL,  // 等待发送MSGREL状态
        QoS2_WAIT_RX_MSGCOMP, // 等待MSGCOMP状态

        // 所有的QoS都有的状态
        WAIT_RELEASE, // 等待释放状态
    };

    // 发送或接收数据的错误信号------------------------------------------------------
    enum class error_t : int32_t
    {
        SUCCESS = 0,      // 正常
        FAILE = -1,       // 错误
        TIMEOUT = -2,     // 超时
        INVALID = -3,     // 无效
        BROKEN = -4,      // 损坏
        REPEAT = -5,      // 重复
        RETYROUT = -6,    // 超出尝试次数
        NO_MSGACK = -7,   // 没有收到ACK
        NO_MSGREL = -8,   // 没有收到MSGREL
        NO_MSGCOMP = -9,  // 没有收到MSGCOMP
        NO_MSGREC = -10,  // 没有收到MSGREC
        NO_MSGPUSH = -11, // 没有收到MSGPUSH
        NO_RELEASE = -12, // 没有收到RELEASE
    };
}

#endif // __SDCP_COMM_H__