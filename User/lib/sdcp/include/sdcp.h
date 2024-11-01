/// @brief 用户接口文件

#ifndef __SIMPLE_TRANSMISSION_PROTOCOL_H__
#define __SIMPLE_TRANSMISSION_PROTOCOL_H__

#include <vector>
#include <array>
#include <string>
#include <stdint.h>
#include <cstring>

#include "sdcp_def.h"
#include "sdcp_comm.h"
#include "sdcp_impl.h"

namespace sdcp
{
    class sdcp
    {
    public:
        sdcp_impl *mySdcpImpl{nullptr};
        sdcp(IOImpl *io_impl,
             const std::vector<uint8_t> &sync_header = std::vector<uint8_t>{{0xAA, 0xAA}},
             const uint16_t max_packet_size = SDCP_MAX_PACKET_SIZE,
             const int64_t timeout_ms = SDCP_FRAME_TIMEOUT_TS_MS,
             const uint16_t max_retry_times = SDCP_FRAME_RETRY_TIMES)
        {

            mySdcpImpl = new sdcp_impl{io_impl, sync_header, max_packet_size, timeout_ms, max_retry_times};
        }

        virtual ~sdcp()
        {
            if (mySdcpImpl)
            {
                delete mySdcpImpl;
            }
        }
        void handle()
        {
            mySdcpImpl->handle();
        }

        /// @brief 连接到指定的端口
        /// @param port 端口号
        /// @return 连接状态，如果连接成功，返回0，否则返回错误码
        const size_t connect(const port_t port, const void *data, const size_t size)
        {
            uint8_t *data_cast = static_cast<uint8_t *>(const_cast<void *>(data));
            mySdcpImpl->send_to_with_cfg(port, msg_type_t::CONNECT, qos_t::QoS0, data_cast, size);
            return 0;
        }

        /// @brief 断开与指定端口的连接
        /// @param port 端口号
        /// @return 断开状态，如果断开成功，返回0，否则返回错误码
        const size_t disconnect(const port_t port, const void *data, const size_t size)
        {
            uint8_t *data_cast = static_cast<uint8_t *>(const_cast<void *>(data));
            mySdcpImpl->send_to_with_cfg(port, msg_type_t::DISCONNECT, qos_t::QoS0, data_cast, size);
            return 0;
        }

        /// @brief 向指定的端口发送ping包
        /// @param port 端口号
        /// @param count 发送次数，默认为1，表示只发送一次
        /// @param timeout_ms 超时时间，单位为毫秒，默认为1000毫秒
        /// @return 发送状态，如果发送成功，返回0，否则返回错误码
        const size_t ping(const port_t port, const void *data, const size_t size, const uint32_t count = 1, const uint32_t timeout_ms = 1000)
        {
            for (int i; i < count; i++)
            {
                uint8_t *data_cast = static_cast<uint8_t *>(const_cast<void *>(data));
                mySdcpImpl->send_to_with_cfg(port, msg_type_t::PINGREQ, qos_t::QoS0, data_cast, size);
            }
            return 0;
        }

        /// @brief 将数据发送到指定的端口
        /// @param port 端口号
        /// @param data 数据
        /// @param size 数据大小
        /// @return 发送的字节数，如果发送失败，返回0
        const size_t sendto(const port_t port, const void *data, const size_t size, qos_t qos)
        {
            if (data == NULL)
            {
                return 0; // 或者抛出异常
            }

            uint8_t *data_cast = static_cast<uint8_t *>(const_cast<void *>(data));

            mySdcpImpl->send_to_with_cfg(port, msg_type_t::MSGPUSH, qos, data_cast, size);
            return size;
        }

        /// @brief 将字符串发送到指定的端口
        /// @param port 端口号
        /// @param cstr 字符串
        /// @return 发送的字节数，如果发送失败，返回0
        const size_t sendto(const port_t port, const char *cstr, qos_t qos)
        {
            return sendto(port, cstr, strlen(cstr), qos);
        }

        /// @brief 将字符串发送到指定的端口
        /// @param port 端口号
        /// @param str 字符串
        /// @return 发送的字节数，如果发送失败，返回0
        const size_t sendto(const port_t port, const std::string &str, qos_t qos)
        {
            return sendto(port, str.c_str(), str.size(), qos);
        }

        /// @brief 发送数组到指定的端口
        /// @param port 端口号
        /// @param data 数据
        /// @return 发送的字节数，如果发送失败，返回0
        const size_t sendto(const port_t port, const std::vector<uint8_t> &data, qos_t qos)
        {
            return sendto(port, data.data(), data.size(), qos);
        }

        /// @brief 发送数组到指定的端口
        /// @param port 端口号
        /// @param data 数据
        /// @return 发送的字节数，如果发送失败，返回0
        const size_t sendto(const port_t port, std::vector<uint8_t> &&data, qos_t qos)
        {
            return sendto(port, data.data(), data.size(), qos);
        }

        /// @brief 发送数组到指定的端口
        /// @tparam DataType 数组类型
        /// @tparam ArrSize 数组长度
        /// @param port 端口号
        /// @param data 数据
        /// @return 发送的字节数，如果发送失败，返回0
        template <typename DataType, size_t ArrSize>
        const size_t sendto(const port_t port, const std::array<DataType, ArrSize> &data, qos_t qos)
        {
            return sendto(port, data.data(), data.size() * sizeof(DataType), qos);
        }

        /// @brief 发送数组到指定的端口
        /// @tparam DataType 数组类型
        /// @tparam ArrSize 数组长度
        /// @param port 端口号
        /// @param data 数据
        /// @return 发送的字节数，如果发送失败，返回0
        template <typename DataType, size_t ArrSize>
        const size_t sendto(const port_t port, std::array<DataType, ArrSize> &&data, qos_t qos)
        {
            return sendto(port, data.data(), data.size() * sizeof(DataType), qos);
        }

        /// @brief 从指定的端口读取数据
        /// @param port 端口号
        /// @return 读取到的数据，如果读取失败，返回空向量
        std::shared_ptr<recv_frame_t> recv_from(const port_t port)
        {
            return mySdcpImpl->recv_from(port);
        }

        /// @brief 从任意端口读取数据
        /// @return 读取到的数据，如果读取失败，返回空向量
        std::shared_ptr<recv_frame_t> recv()
        {
            return mySdcpImpl->recv();
        }

        /// @brief 监听指定的端口，程序会在接收到相关数据时调用指定的函数
        /// @tparam Func 可调用对象类型
        /// @tparam Args 可调用对象参数类型
        /// @param port 端口号
        /// @param func 可调用对象
        /// @param args 可调用对象参数
        template <typename Func, typename... Args>
        sdcp &listen(const port_t port, Func &&func, Args &&...args)
        {
            // __monitor = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            // TODO:未来实现相关功能，包括接收函数的参数
            return *this;
        }

        /// @brief 监听指定的端口，程序会在接收到相关数据时调用指定的函数
        template <typename ReturnType, typename T, typename... Args>
        sdcp &listen(const port_t port, ReturnType (T::*func)(Args...), T *obj, Args &&...args)
        {
            // __monitor = std::bind(func, obj, std::forward<Args>(args)...);
            // TODO:未来实现相关功能，包括接收函数的参数
            return *this;
        }

        /// @brief 监听指定的端口，程序会在接收到相关数据时调用指定的函数
        template <typename ReturnType, typename... Args>
        sdcp &listen(const port_t port, ReturnType (*func)(Args...), Args &&...args)
        {
            // __monitor = std::bind(func, std::forward<Args>(args)...);
            // TODO:未来实现相关功能，包括接收函数的参数
            return *this;
        }
    };
} // namespace sdcp

#endif // __SIMPLE_TRANSMISSION_PROTOCOL_H__