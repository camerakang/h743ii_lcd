/// @brief SDCP 接口定义
#ifndef __SDCP_IMPL_H__
#define __SDCP_IMPL_H__

#include "utools.h"
#include "sdcp_comm.h"
#include "sdcp_frame.h"
#include "sdcp_io_impl.h"
#include "sdcp_packer.h"
#include "sdcp_shcfg.h"
#include <algorithm>
#include <memory>
#include <queue>
#include <stdint.h>
#include <string>
#include <unordered_map>

namespace sdcp
{
    class sdcp_impl
    {
        friend class sdcp; // 友元类

    private:
        std::queue<std::shared_ptr<send_frame_t>> __send_queue;                                      // 发送队列
        std::unordered_map<port_t, std::queue<std::shared_ptr<send_frame_t>>> __port_send_queue_map; // 按端口号分类的发送队列

        std::vector<uint8_t> __recv_raw_buffer; // 接收原始数据缓冲区
        utools::mem::RingBuffer<uint8_t> __recv_raw_buffer_test{1024};
        utools::collab::Mutex __recv_raw_buffer_mutex;

        std::unordered_map<port_t, std::queue<std::shared_ptr<recv_frame_t>>> __port_recv_queue_map; // 按端口号分类的接收数据帧队列
        std::unordered_map<port_t, std::shared_ptr<recv_frame_t>> __preprocess_qos2_map;
        std::queue<std::shared_ptr<recv_frame_t>> __preprocess_queue;

        std::function<void(const port_t, std::vector<uint8_t> &)> __on_connect;                                          // 连接事件回调
        std::function<void(const port_t, std::vector<uint8_t> &)> __on_disconnect;                                       // 断开连接事件回调
        std::function<void(const port_t, const uint8_t *, const uint16_t)> __on_recv;                                    // 接收数据事件回调
        std::function<void(const port_t, const uint8_t *, const uint16_t)> __on_send;                                    // 发送数据事件回调
        std::function<void(const error_t, const port_t, const std::string &, std::shared_ptr<send_frame_t>)> __on_error; // 错误事件回调
        std::function<void(const error_t, const std::string &, std::shared_ptr<recv_frame_t>)> __on_analyse_error{
            [](const error_t err, const std::string &err_desc, std::shared_ptr<recv_frame_t> frm) -> void {

            }}; // 错误事件回调

        uint64_t __prev_update_time_stamp{0}; // 上一次更新时间戳
    public:
        sdcp_impl() = delete;

        /// @brief 构造函数
        /// @param io_impl 用于实现硬件层数据接收的接口，以托管的方式实现，当sdcp_impl对象销毁时，会自动销毁该对象
        /// @param sync_header 同步头
        /// @param max_packet_size 数据长度
        /// @param timeout_ms 超时时间
        //@param max_retry_times 最大重试次数
        sdcp_impl(IOImpl *io_impl,
                  const std::vector<uint8_t> &sync_header = std::vector<uint8_t>{{0xAA, 0xAA}},
                  const uint16_t max_packet_size = SDCP_MAX_PACKET_SIZE,
                  const int64_t timeout_ms = SDCP_FRAME_TIMEOUT_TS_MS,
                  const uint16_t max_retry_times = SDCP_FRAME_RETRY_TIMES)
        {
            SHCFG::__io_impl = io_impl;                 // 托管io_impl对象
            SHCFG::__sync_header = sync_header;         // 设置同步头
            SHCFG::__max_packet_size = max_packet_size; // 设置最大数据长度
            SHCFG::__timeout_ms = timeout_ms;           // 设置超时时间
            SHCFG::__max_retry_times = max_retry_times; // 设置最大重试次数
            SHCFG::__msgid_generator = 0;               // 初始化消息ID生成器
        }

        virtual ~sdcp_impl()
        {
            /* if (SHCFG::__io_impl != nullptr)
             {
                 delete SHCFG::__io_impl;
                 SHCFG::__io_impl = nullptr;
             }*/
        }
        void set_recv_raw_buffer() // std::vector<uint8_t> &buffer)
        {
            auto vec = SHCFG::ioimpl().read();
            __recv_raw_buffer_test.push_back(vec, vec.size());
            __prev_update_time_stamp = SHCFG::ts_ms();
        }
        /// @brief 发送数据帧
        /// @param port 端口号
        /// @param msg_type 消息类型
        /// @param qos QoS
        /// @param data 数据
        /// @param size 数据长度，此处不进行大小判断
        /// @return send_frame_t
        /// @note 此处不进行大小判断，由上层调用者保证
        /// @note 此处也不会处理过大的数据包，也由上一层进行处理
        std::shared_ptr<send_frame_t> send_to_with_cfg(port_t port, msg_type_t msg_type, qos_t qos, uint8_t *data, uint16_t size)
        {
            std::shared_ptr<send_frame_t> send_frame; // 发送数据帧

            switch (msg_type)
            {
            case msg_type_t::CONNECT:
                send_frame = send_frame_t::CONNECT(port, data, size);
                break;
            [[likely]] case msg_type_t::MSGPUSH:
                send_frame = send_frame_t::MSGPUSH(port, qos, data, size);
                break;
            case msg_type_t::PINGREQ:
                send_frame = send_frame_t::PINGREQ(port, data, size);
                break;
            case msg_type_t::DISCONNECT:
                send_frame = send_frame_t::DISCONNECT(port, data, size);
                break;
            default:
                break;
            }

            auto &send_queue = __port_send_queue_map[port];
            // NOTE:只有是QoS0的消息才会直接发送，同时还要满足其发送对列中没有等待的消息
            if (qos == qos_t::QoS0 && send_queue.empty())
            {
                __send_queue.push(send_frame); // 直接放在等待发送的对列中
            }
            else
            {
                send_queue.push(send_frame); // 放在对应端口的发送队列中
            }
            return send_frame;
        }

        std::shared_ptr<send_frame_t> send_to_with_msgID(port_t port, msg_type_t msg_type, msgid_t msgid, uint16_t size)
        {
            std::shared_ptr<send_frame_t> send_frame; // 发送数据帧
            switch (msg_type)
            {
            case msg_type_t::MSGACK:
                send_frame = send_frame_t::MSGACK(port, msgid, nullptr, size);
                break;
            case msg_type_t::MSGREC:
                send_frame = send_frame_t::MSGREC(port, msgid, nullptr, size);
                break;
            case msg_type_t::MSGCOMP:
                send_frame = send_frame_t::MSGCOMP(port, msgid, nullptr, size);
                break;
            default:
                break;
            }

            auto &send_queue = __port_send_queue_map[port];

            send_queue.push(send_frame); // 放在对应端口的发送队列中

            return send_frame;
        }

        /// @brief 根据端口号接收数据帧
        /// @param port 端口号
        /// @return 接收到的数据
        std::shared_ptr<recv_frame_t> recv_from(port_t port)
        {
            if (__port_recv_queue_map.find(port) == __port_recv_queue_map.end())
            {
                return nullptr;
            }
            auto &recv_queue = __port_recv_queue_map[port];
            if (recv_queue.empty())
            {
                return nullptr;
            }
            auto recv_frame = recv_queue.front();
            recv_queue.pop();
            return recv_frame;
        }

        /// @brief 读取一个数据帧
        /// @return 读取队列中最老的数据
        std::shared_ptr<recv_frame_t> recv()
        {
            auto prev_frm_ts_ms{SHCFG::ts_ms()};
            int32_t port{-1};
            for (auto &recv_frames : __port_recv_queue_map)
            {
                if (!recv_frames.second.empty())
                {
                    auto recv_frame = recv_frames.second.front();
                    if (recv_frame->ts < prev_frm_ts_ms)
                    {
                        port = recv_frames.first;
                        prev_frm_ts_ms = recv_frame->ts;
                    }
                }
            }
            if (port == -1)
            {
                return nullptr;
            }
            auto &recv_queue = __port_recv_queue_map[port];
            if (recv_queue.empty())
            {
                return nullptr;
            }
            auto recv_frame = recv_queue.front();
            recv_queue.pop();
            return recv_frame;
        }

        /// @brief 处理接收队列
        void handle_recv()
        {
            // TODO：加入测试随机数
            //  1 处理缓存无效条件
            //  1.1 检测是否为空缓存

            if (__recv_raw_buffer_test.is_empty() == 1)
            {
                // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::BUFFER_EMPTY));
                return;
            }

            // 1.2 检查是否超时
            if (SHCFG::__timeout_ms > 0)
            {
                auto curr_ts_ms = SHCFG::ts_ms();
                if (__prev_update_time_stamp + SHCFG::__timeout_ms < curr_ts_ms)
                {
                    std::vector<uint8_t> dptr{__recv_raw_buffer_test.pop_head(__recv_raw_buffer_test.len())};
                    recv_frame_t ret{recv_frame_t::status_t::BUFFER_TIMEOUT, qos_t::QoS0, 0, 0, dptr};
                    __preprocess_queue.push(std::make_shared<recv_frame_t>(std::move(ret)));
                    return;
                }
            }
            auto __crc16_size = 2;
            auto fixed_header_size = 1;
            // 1.3 如果数据的长度太短，不能立刻处理
            if (__recv_raw_buffer_test.size() < SHCFG::__sync_header.size() + __crc16_size + fixed_header_size)
            {
                // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::BUFFER_NOT_ENOUGH));
                return;
            }

            // 2 查找同步头
            auto start_it = __recv_raw_buffer_test.search(SHCFG::__sync_header.data(), SHCFG::__sync_header.size());
            // 2.1 删除无同步头时的缓存
            if (start_it < 0) // 没找到同步头，清空缓存
            {
                std::vector<uint8_t> dptr{__recv_raw_buffer_test.pop_head(__recv_raw_buffer_test.len())};
                recv_frame_t ret{recv_frame_t::status_t::BUFFER_INVALID, qos_t::QoS0, 0, 0, dptr};
                __preprocess_queue.push(std::make_shared<recv_frame_t>(std::move(ret)));
                return;
            }
            // 2.2 移除同步头前面的任何数据
            if (start_it > 0)
            {
                std::vector<uint8_t> dptr{__recv_raw_buffer_test.pop_head(start_it)};
                recv_frame_t ret{recv_frame_t::status_t::BUFFER_BROKEN, qos_t::QoS0, 0, 0, dptr};
                __preprocess_queue.push(std::make_shared<recv_frame_t>(ret));
                return;
            }

            // 固定头
            uint8_t fixed_header{__recv_raw_buffer_test[SHCFG::__sync_header.size()]};
            // memcpy(&fixed_header, &__recv_raw_buffer_test[SHCFG::__sync_header.size()], fixed_header_size);
            uint8_t coml, comh;
            auto frame_id_size{0};
            msgid_t frame_id{0};
            uint32_t __data_len_leve{0};
            uint16_t length{__recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size]};
            qos_t qos;
            coml = 0x0c & fixed_header;
            comh = 0x02 & fixed_header;
            if (comh == 0x00)
            {
                __data_len_leve = 1;
                // 包体长度，CHANNEL、DATA和CRC16的长度
            }
            else if (comh == 0x02)
            {
                __data_len_leve = 2;
                length = __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size + 1] << 8 | __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size];
            }
            if (coml == 0x00)
            {
                frame_id_size = 0;
                qos = qos_t::QoS0;
            }
            else if (coml == 0x04)
            {
                qos = qos_t::QoS1;
                frame_id_size = 2;
                frame_id = __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve + 1] << 8 | __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve];
            }
            else if (coml == 0x08)
            {
                qos = qos_t::QoS2;
                frame_id_size = 2;
                frame_id = __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve + 1] << 8 | __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve];
            }

            // 3 检查数据长度是否足够读取长度信息，以进行下步的处理
            // 3.1 解析长度
            // uint8_t length{ __recv_raw_buffer_test[SHCFG::__sync_header.size() + fixed_header_size] };

            // 3.2 检查长度是否足够，如果不够长，返回，等待下次处理
            if (__recv_raw_buffer_test.size() < SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve + length)
            {
                // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::LENGTH_NOT_ENOUGH));
                return;
            }

            // 3.3 处理长度异常（如果超出理论最大值）
            if (length > (SHCFG::__max_packet_size - (SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve)))
            {
                // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::LENGTH_ERROR));
                return;
            }

            // 4 提取数据
            // 端口号
            uint16_t __prot_num_size{2};
            uint16_t prot_num{static_cast<uint16_t>(
                static_cast<uint16_t>(__recv_raw_buffer_test[SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve + 1] << 8) |
                static_cast<uint16_t>(__recv_raw_buffer_test[SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve]))};

            // 4.2 解析数据
            auto data_len = length - frame_id_size - __prot_num_size - __crc16_size;
            std::vector<uint8_t> frame_data{__recv_raw_buffer_test.read(SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve + __prot_num_size, data_len)};

            // 5 计算CRC
            uint16_t crc{sdcp::crc16(__recv_raw_buffer_test.read(SHCFG::__sync_header.size() - 2, __recv_raw_buffer_test.size() - 2))};
            // printf("%p, %p", &__recv_raw_buffer_test[0], &__recv_raw_buffer_test[SHCFG::__sync_header.size()]);
            // std::vector test_vec{ __recv_raw_buffer_test[SHCFG::__sync_header.size()], __recv_raw_buffer_test.begin() + frame_id_size + fixed_header_size + __data_len_leve + __prot_num_size + data_len };
            uint16_t received_crc{static_cast<uint16_t>(
                static_cast<uint16_t>(__recv_raw_buffer_test[SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve + __prot_num_size + data_len + 1]) << 8 |
                static_cast<uint16_t>(__recv_raw_buffer_test[SHCFG::__sync_header.size() + frame_id_size + fixed_header_size + __data_len_leve + __prot_num_size + data_len]))};
            if (crc != received_crc) // 检查CRC
            {
                // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::CRC_ERROR));
                __reinit_recv_buffer_header();
                return;
            }

            // 6 数据有效，从缓冲区移除已解析的数据
            __recv_raw_buffer_test.drain(SHCFG::__sync_header.size() + fixed_header_size + __data_len_leve + length);
            recv_frame_t recv; // { recv_frame_t::status_t::VALID, qos, prot_num, frame_id, frame_data };
            recv.status = recv_frame_t::status_t::VALID;
            recv.qos = qos;
            recv.port = prot_num;
            recv.msgid = frame_id;
            recv.data = frame_data;
            // __preprocess_queue.push(std::make_shared<sdcp::recv_frame_t>(recv_frame_t::status_t::VALID));
            __preprocess_queue.push(std::make_shared<recv_frame_t>(recv));
            return;
            // TODO:解析__recv_raw_buffer_test中的数据，recv_frame
            // TODO:根据recv_frame中的数据类型，更新发送队列中的状态
            // TODO:将recv_frame放入接收队列__port_recv_queue_map中，等待用户调用recv_from获取
        }

        /// @brief 处理发送队列
        void handle_send()
        {
            // 从map中取数据，放在发送队列中
            // FIXME:此处理方式效率不高，需要优化，也包括删除不需要的数据分支
            auto curr_ts_ms = SHCFG::ts_ms();
            for (auto &send_frames : __port_send_queue_map)
            {
                if (!send_frames.second.empty())
                {
                    // FIXME:此处需要根据QoS进行不同的处理，过程中可能会删除__port_send_queue_map中的整个分支，存在一定的风险
                    handle_send_with_qos(curr_ts_ms, send_frames.second);
                }
            }

            // 实现发送队列的处理
            if (!__send_queue.empty())
            {
                auto frame = __send_queue.front();
                SHCFG::ioimpl().write(frame->data.data(), frame->data.size());
                __send_queue.pop();
                if (frame->qos != qos_t::QoS0)
                {
                    frame->__send_time = SHCFG::ts_ms(); // 记录发送时间
                }
            }
        }

        /// @brief 处理需要通知的事件
        void handle_notify()
        {
            msgid_t __temp_msgid{0};
            if (__preprocess_queue.empty())
                return;
            auto __front_element = __preprocess_queue.front();
            switch (__front_element->status)
            {
            case recv_frame_t::status_t::BUFFER_TIMEOUT:
                __preprocess_queue.pop();
                __on_analyse_error(error_t::TIMEOUT,
                                   "TIMEOUT", __front_element);
                // std::queue<std::shared_ptr<recv_frame_t>>().swap(__preprocess_queue);
                return;
            case recv_frame_t::status_t::BUFFER_INVALID:
                __preprocess_queue.pop();
                __on_analyse_error(error_t::INVALID,
                                   "INVALID", __front_element);
                // std::queue<std::shared_ptr<recv_frame_t>>().swap(__preprocess_queue);
                return;
            case recv_frame_t::status_t::BUFFER_BROKEN:
                __preprocess_queue.pop();
                __on_analyse_error(error_t::BROKEN,
                                   "BROKEN", __front_element);
                // std::queue<std::shared_ptr<recv_frame_t>>().swap(__preprocess_queue);
                return;
            default:
                break;
            }

            if (__front_element->qos == qos_t::QoS0)
            {
                __port_recv_queue_map[__front_element->port].push(__front_element);
                return;
            }
            else if (__front_element->qos == qos_t::QoS1)
            {
                auto __secend_element = __preprocess_queue.back();
                static uint8_t front = 1;
                if (front || __front_element->msgid != __secend_element->msgid)
                {
                    front = 0;
                    send_to_with_msgID(__front_element->port, msg_type_t::MSGACK, __front_element->msgid, 0);
                    __port_recv_queue_map[__front_element->port].push(__front_element);
                }
                else
                {
                    __preprocess_queue.pop();
                    // recv__on_error(error_t::REPEAT, __front_element->port,
                    //                "REPEAT", __front_element);
                    // std::queue<std::shared_ptr<recv_frame_t>>().swap(__preprocess_queue);
                }
                return;
            }
            else if (__front_element->qos == qos_t::QoS2)
            {
                auto __secend_element = __preprocess_qos2_map[__front_element->port];
                if (__port_recv_queue_map[__front_element->port].empty())
                {
                    __front_element->status = recv_frame_t::status_t::MSGPUSH;
                    send_to_with_msgID(__front_element->port, msg_type_t::MSGREC, __front_element->msgid, 0);
                    __preprocess_qos2_map[__front_element->port] = (__front_element);
                }
                else if (__secend_element->status == recv_frame_t::status_t::MSGPUSH && __front_element->port == __secend_element->port)
                {
                    __front_element->status = recv_frame_t::status_t::MSGREL;
                    send_to_with_msgID(__front_element->port, msg_type_t::MSGCOMP, __front_element->msgid, 0);
                    __port_recv_queue_map[__secend_element->port].push(__secend_element);
                    __preprocess_qos2_map[__front_element->port].reset();
                }
                else
                {
                    __preprocess_qos2_map[__front_element->port].reset();
                    // recv__on_error(error_t::NO_MSGCOMP, __front_element->port,
                    //                "NO_MSGCOMP", __front_element);
                    // std::queue<std::shared_ptr<recv_frame_t>>().swap(__preprocess_queue);
                }

                return;
            }

            // TODO:实现需要通知的事件
        }
        void handle_time_out()
        {
        }
        /// @brief 单线程的处理函数
        void handle()
        {
            // 1、处理接收队列
            handle_recv();
            // 2、处理需要通知的事件
            handle_notify();
            // 3、处理发送队列
            handle_send();
            handle_time_out();
        }

    private:
        /// @brief 根据qos等级处理发送数据
        inline void handle_send_with_qos(const int64_t &curr_ts_ms, std::queue<std::shared_ptr<send_frame_t>> &send_queue)
        {
            auto frame = send_queue.front();
            switch (frame->qos)
            {
            case qos_t::QoS1:
                switch (frame->__tx_state)
                {
                case qos_tx_state_t::WAIT_TX_MSG:
                    __send_queue.push(frame);
                    frame->__tx_state = qos_tx_state_t::QoS1_WAIT_RX_MSGACK;
                    return;
                    break;
                case qos_tx_state_t::QoS1_WAIT_RX_MSGACK:
                    if (__send_timeout_check(curr_ts_ms, frame))
                    {
                        send_queue.pop(); // 将数据从发送队列中移除，传递给错误处理功能
                        __on_error(error_t::NO_MSGACK, frame->port,
                                   "No MSGACK message received, drop all messages in the channel",
                                   frame);
                        std::queue<std::shared_ptr<send_frame_t>>().swap(send_queue); // 清空队列
                        return;
                    }
                    break;
                case qos_tx_state_t::WAIT_RELEASE:
                    send_queue.pop(); // 释放
                    break;
                default:
                    break;
                }
                break;
            case qos_t::QoS2:
                switch (frame->__tx_state)
                {
                case qos_tx_state_t::WAIT_TX_MSG:
                    __send_queue.push(frame);
                    frame->__tx_state = qos_tx_state_t::QoS1_WAIT_RX_MSGACK;

                    break;
                case qos_tx_state_t::QoS2_WAIT_RX_MSGREC:
                    if (__send_timeout_check(curr_ts_ms, frame))
                    {
                        send_queue.pop(); // 将数据从发送队列中移除，传递给错误处理功能
                        __on_error(error_t::NO_MSGREC, frame->port,
                                   "No MSGREC message received, drop all messages in the channel",
                                   frame);
                        std::queue<std::shared_ptr<send_frame_t>>().swap(send_queue); // 清空队列
                        return;
                    }
                    break;
                case qos_tx_state_t::QoS2_WAIT_TX_MSGREL:
                    // TODO:生成MSGREL数据，发送给对方，并等待对方回复
                    frame->__tx_state = qos_tx_state_t::QoS2_WAIT_RX_MSGCOMP;
                    break;
                case qos_tx_state_t::QoS2_WAIT_RX_MSGCOMP:
                    if (__send_timeout_check(curr_ts_ms, frame))
                    {
                        send_queue.pop(); // 将数据从发送队列中移除，传递给错误处理功能
                        __on_error(error_t::NO_MSGCOMP, frame->port,
                                   "No MSGCOMP message received, drop all messages in the channel",
                                   frame);
                        std::queue<std::shared_ptr<send_frame_t>>().swap(send_queue); // 清空队列
                        return;
                    }
                    break;
                case qos_tx_state_t::WAIT_RELEASE:
                    send_queue.pop(); // 释放
                    break;
                default:
                    break;
                }
                break;
            case qos_t::QoS0:
                __send_queue.push(frame);
                send_queue.pop();
                break;
            default:
                break;
            }
        }

        /// @brief 检测是否出现了超时和超次的情况，如果可以发送，则尝试重发
        /// @param curr_ts_ms 用于比较的时间戳
        /// @param frame 需要判断的帧
        /// @return true表示出现了超时和超次，false表示没有
        inline bool __send_timeout_check(const int64_t &curr_ts_ms, std::shared_ptr<send_frame_t> &frame)
        {
            if (frame->__send_time && (curr_ts_ms - frame->__send_time > SHCFG::timeout_ms()))
            {
                if (++frame->__retry_count < SHCFG::max_retry_times())
                {
                    frame->__send_time = 0; // 重置时间，避免因数据在等待发送对列等待发送导致再次进入超时处理
                    __send_queue.push(frame);
                }
                else
                {
                    return true; // 超过重试次数，则放弃
                }
            }
            return false;
        }
        inline std::vector<uint8_t> __reinit_recv_buffer_header()
        {
            std::vector<uint8_t> ret{};
            auto start_it = __recv_raw_buffer_test.search(SHCFG::__sync_header.data(), SHCFG::__sync_header.size(), 1); // 跳过最前面的同步头搜索
            if (start_it < 0)                                                                                           // 没找到新的同步头，清空缓存
            {
                __recv_raw_buffer_test.pop_head(ret, __recv_raw_buffer_test.size());
                __recv_raw_buffer_test.drain(__recv_raw_buffer_test.size());
            }
            else
            {
                __recv_raw_buffer_test.pop_head(ret, start_it);
                __recv_raw_buffer_test.drain(start_it);
            }
            return ret;
        }
    };
} // namespace sdcp

#endif // __SDCP_IMPL_H__