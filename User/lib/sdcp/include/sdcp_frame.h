/// @brief 数据帧管理功能

#ifndef __SDCP_FRAME_H__
#define __SDCP_FRAME_H__

#include "utools.h"
#include "sdcp_def.h"
#include "sdcp_packer.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace sdcp
{
    /// @brief 原始数据帧
    struct recv_frame_raw_t
    {
        friend class sdcp_impl; // 允许 sdcp_impl 访问私有成员
        friend class sdcp;      // 允许 sdcp 访问私有成员

        enum class status_t : uint8_t
        {
            BUFFER_EMPTY = 0,      // 缓冲区为空
            BUFFER_INVALID = 1,    // 无效缓冲区，没有可以使用的数据
            BUFFER_TIMEOUT = 2,    // 缓冲区超时，连续一定时间没有接收到数据，但在缓存中有数据
            BUFFER_BROKEN = 3,     // 一帧数据同步头前面有未知意义的缓存
            BUFFER_NOT_ENOUGH = 4, // 缓冲区长度不足
            LENGTH_NOT_ENOUGH = 5, // 帧长度不足
            LENGTH_ERROR = 6,      // 帧长度错误
            CRC_ERROR = 7,         // 帧CRC错误
            INVALID = 8,           // 帧无效
            VALID = 9,             // 帧有效、
            MSGPUSH = 11,          // 帧MSGPUSH
            MSGREL = 12,           // 帧MSGREL
        };

    protected:
        status_t status; // 当前帧的状态
        qos_t qos;       // 当前帧的服务质量
        msgid_t msgid;

    public:
        port_t port;               // 通道号
        std::vector<uint8_t> data; // 帧数据
        timestamp_t ts;            // 接收时间戳

        recv_frame_raw_t(status_t status, qos_t qos, msgid_t msgid, port_t port, std::vector<uint8_t> &&data)
            : status(status), qos(qos), msgid(msgid), port(port), data(std::move(data)) {}

        recv_frame_raw_t(status_t status, qos_t qos, msgid_t msgid, port_t port, std::vector<uint8_t> &data)
            : status(status), qos(qos), msgid(msgid), port(port), data(data) {}

        recv_frame_raw_t(const recv_frame_raw_t &&other)
        {
            status = other.status;
            qos = other.qos;
            msgid = other.msgid;
            port = other.port;
            data = std::move(other.data);
        }

        recv_frame_raw_t &operator=(const recv_frame_raw_t &&other)
        {
            status = other.status;
            qos = other.qos;
            msgid = other.msgid;
            port = other.port;
            data = std::move(other.data);
            return *this;
        }

        recv_frame_raw_t(const recv_frame_raw_t &other)
        {
            status = other.status;
            qos = other.qos;
            msgid = other.msgid;
            port = other.port;
            data = other.data;
        }

        recv_frame_raw_t &operator=(const recv_frame_raw_t &other)
        {
            status = other.status;
            qos = other.qos;
            msgid = other.msgid;
            port = other.port;
            data = other.data;
            return *this;
        }

        recv_frame_raw_t() = default;
    };

    /// @brief 解析出来的一帧数据
    struct recv_frame_t : public recv_frame_raw_t
    {
        using recv_frame_raw_t::recv_frame_raw_t;

        /// @brief 从数据中解析出一帧数据
        /// @tparam ...Args 数据类型
        /// @param format 数据格式
        /// @return std::tuple<Args...> 解析出来的数据
        // template <typename... Args>
        // std::tuple<Args...> unpack(const std::string &format)
        // {
        //     return utools::coded::ByteStruct::unpack(format, data.data(), data.size());
        // }

        /// @brief 从数据中解析出一帧数据
        /// @tparam _OUT_TYPE 数据类型
        /// @param index 数据起始位置
        /// @param length 数据长度
        /// @return _OUT_TYPE 解析出来的数据
        template <typename _OUT_TYPE>
        _OUT_TYPE unpack_from(const uint32_t index, const uint32_t length)
        {
            _OUT_TYPE result;
            memcpy(&result, data.data() + index, length);
            return result;
        }

        /// @brief 将当前的帧数据转换为字符串
        /// @return 返回当前帧数据转换为字符串
        const std::string str_data()
        {
            return std::string{data.begin(), data.end()};
        }
    };

    struct send_frame_t : public packer_t
    {
        friend class sdcp_impl;

    public:
        msg_type_t msg_type;           // 当前消息类型
        qos_t qos;                     // 消息质量
        timestamp_t ts;                // 消息时间戳
        port_t port;                   // 消息端口，接收和发送方一致，使用相同的端口通讯
        msgid_t msgid;                 // 消息ID
        msg_extend_flag_t extend_flag; // 消息扩展标志

        std::vector<std::uint8_t> data; // 消息数据

        send_frame_t()
        {
        }

        static std::shared_ptr<send_frame_t>
        CONNECT(const port_t port, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = SHCFG::msgid_generator();
            frame->qos = qos_t::QoS1;
            frame->port = port;
            frame->msg_type = msg_type_t::CONNECT;
            frame->data = pack_with_msgid(msg_type_t::CONNECT, qos_t::QoS1, port, data, len, frame->msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> CONNACK(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = SHCFG::msgid_generator();
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::CONNACK;
            frame->data = pack_with_msgid(msg_type_t::CONNACK, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGPUSH(const port_t port, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = SHCFG::msgid_generator();
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGPUSH;
            frame->data = pack_without_msgid(msg_type_t::MSGPUSH, qos_t::QoS0, port, data, len);
            frame->ts = SHCFG::ts_ms();
            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGPUSH(const port_t port, const qos_t qos, const uint8_t *data, uint16_t len)
        {
            if (qos == qos_t::QoS0)
            {
                return send_frame_t::MSGPUSH(port, data, len);
            }

            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = SHCFG::msgid_generator();
            frame->qos = qos;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGPUSH;
            frame->data = pack_with_msgid(msg_type_t::MSGPUSH, qos, port, data, len);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGACK(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = msgid;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGACK;
            frame->data = pack_with_msgid(msg_type_t::MSGACK, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGREC(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = msgid;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGREC;
            frame->data = pack_with_msgid(msg_type_t::MSGREC, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGREL(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = msgid;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGREL;
            frame->data = pack_with_msgid(msg_type_t::MSGREL, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> MSGCOMP(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = msgid;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::MSGCOMP;
            frame->data = pack_with_msgid(msg_type_t::MSGCOMP, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> PINGREQ(const port_t port, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = SHCFG::msgid_generator();
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::PINGREQ;
            frame->data = pack_with_msgid(msg_type_t::PINGREQ, qos_t::QoS1, port, data, len, frame->msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> PINGRESP(const port_t port, const msgid_t msgid, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = msgid;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::PINGRESP;
            frame->data = pack_with_msgid(msg_type_t::PINGRESP, qos_t::QoS0, port, data, len, msgid);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        static std::shared_ptr<send_frame_t> DISCONNECT(const port_t port, const uint8_t *data, uint16_t len)
        {
            std::shared_ptr<send_frame_t> frame = std::make_shared<send_frame_t>();
            frame->msgid = 0;
            frame->qos = qos_t::QoS0;
            frame->port = port;
            frame->msg_type = msg_type_t::DISCONNECT;
            frame->data = pack_without_msgid(msg_type_t::DISCONNECT, qos_t::QoS0, port, data, len);
            frame->ts = SHCFG::ts_ms();

            frame->__tx_state = qos_tx_state_t::WAIT_TX_MSG; // 消息设置为等待推送状态
            return frame;
        }

        /// @brief 构造函数：接受任意可调用对象
        template <typename Func, typename... Args>
        send_frame_t &monitor(Func &&func, Args &&...args)
        {
            __monitor = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            return *this;
        }

        /// @brief 构造函数：接受类成员函数
        template <typename ReturnType, typename T, typename... Args>
        send_frame_t &monitor(ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            __monitor = std::bind(func, obj, std::forward<Args>(args)...);
            return *this;
        }

        /// @brief 构造函数：接受静态函数或自由函数
        template <typename ReturnType, typename... Args>
        send_frame_t &monitor(ReturnType (*func)(Args...), Args &&...args)
        {
            __monitor = std::bind(func, std::forward<Args>(args)...);
            return *this;
        }

    private:
        std::function<void(send_frame_t &, std::uint32_t)> __monitor{nullptr}; // 监控函数，用于将处理过程和接收过程通知给调用者
        qos_tx_state_t __tx_state{qos_tx_state_t::WAIT_TX_MSG};                // 当前消息状态，用于记录消息的发送和接收状态
        uint16_t __retry_count{0};                                             // 重试次数，用于记录消息的重试次数
        timestamp_t __send_time{0};                                            // 发送时间，用于记录消息的发送时间

        bool __invalid{true}; // 是否无效，用于标记消息是否有效
        send_frame_t(bool invalid) : __invalid(invalid) {}
    };
}

#endif // __SDCP_FRAME_H__
