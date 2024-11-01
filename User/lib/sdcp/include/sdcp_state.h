/// @brief 收发状态管理

#ifndef __SDCP_STATE_H__
#define __SDCP_STATE_H__

#include "sdcp_comm.h"
#include "sdcp_shcfg.h"

#include <vector>
#include <stdint.h>
#include <functional>
#include <array>
#include <list>

namespace sdcp
{
    using frame_state_t = msg_type_t; // 状态类型

    /// @brief 帧状态管理基类
    struct frame_state_base_t
    {
        qos_t qos;              // 服务质量类型
        timestamp_t ts_ms;      // 时间戳，用于记录上一次发送或接收的时间
        frame_state_t state;    // 当前状态
        msgid_t msgid;          // 包ID，用于QoS1和QoS2消息的确认
        timestamp_t timeout_ms; // 超时时间，用于QoS1和QoS2消息的确认
        uint32_t retry;         // 重试次数

        frame_state_base_t(qos_t qos, frame_state_t state, msgid_t msgid,
                           timestamp_t ts_ms, timestamp_t timeout_ms, uint32_t retry)
            : qos(qos), state(state), msgid(msgid), ts_ms(ts_ms),
              timeout_ms(timeout_ms), retry(retry) {}

        frame_state_base_t(qos_t qos, frame_state_t state, msgid_t msgid, uint32_t retry)
            : qos(qos), state(state), msgid(msgid), ts_ms(SHCFG::ts_ms()),
              timeout_ms(SDCP_FRAME_TIMEOUT_TS_MS), retry(retry) {}

        frame_state_base_t(qos_t qos, frame_state_t state, msgid_t msgid)
            : qos(qos), state(state), msgid(msgid), ts_ms(SHCFG::ts_ms()),
              timeout_ms(SDCP_FRAME_TIMEOUT_TS_MS), retry(SDCP_FRAME_RETRY_TIMES) {}

        frame_state_base_t() = default;

        virtual ~frame_state_base_t() = default;

        /// @brief 周期刷新包的状态
        /// @param curr_ts 刷新的时间戳
        inline void update(const timestamp_t curr_ts)
        {
            if (curr_ts - ts_ms >= timeout_ms)
            {
                retry++;
                ts_ms = curr_ts;
                // TODO: 重试机制
            }
        }

        void update()
        {
            update(SHCFG::ts_ms());
        }
    };

    // CONNECT状态转移图
    struct connect_frame_state_t : public frame_state_base_t
    {
    };

    // QoS0 MSG状态转移图
    // struct qos0_frame_state_t : public frame_state_base_t
    // {
    //     using frame_state_base_t::frame_state_base_t;
    // };

    // QoS1 MSG状态转移图
    struct qos1_frame_state_t : public frame_state_base_t
    {
        qos1_frame_state_t(msgid_t msgid)
            : frame_state_base_t(qos_t::QoS1, frame_state_t::MSGPUSH, msgid) {}

        qos1_frame_state_t(msgid_t msgid, uint32_t retry)
            : frame_state_base_t(qos_t::QoS2, frame_state_t::MSGPUSH, msgid, retry) {}

        qos1_frame_state_t(msgid_t msgid, timestamp_t ts_ms, timestamp_t timeout_ms, uint32_t retry)
            : frame_state_base_t(qos_t::QoS1, frame_state_t::MSGPUSH, msgid, ts_ms, timeout_ms, retry) {}

        qos1_frame_state_t() = delete;

        virtual ~qos1_frame_state_t() = default;
    };

    // QoS2 MSG状态转移图
    struct qos2_frame_state_t : public frame_state_base_t
    {
        qos2_frame_state_t(msgid_t msgid)
            : frame_state_base_t(qos_t::QoS2, frame_state_t::MSGPUSH, msgid) {}

        qos2_frame_state_t(msgid_t msgid, uint32_t retry)
            : frame_state_base_t(qos_t::QoS2, frame_state_t::MSGPUSH, msgid, retry) {}

        qos2_frame_state_t(msgid_t msgid, timestamp_t ts_ms, timestamp_t timeout_ms, uint32_t retry)
            : frame_state_base_t(qos_t::QoS2, frame_state_t::MSGPUSH, msgid, ts_ms, timeout_ms, retry) {}

        qos2_frame_state_t() = delete;

        virtual ~qos2_frame_state_t() = default;
    };

    // PING 状态转移图

    /// @brief 发送状态管理
    struct sender_frame_state_t : public frame_state_base_t
    {
    };

    /// @brief 接收状态管理
    struct receiver_frame_state_t : public frame_state_base_t
    {
    };

    // TODO:所有帧的状态管理list，用于周期处理相相关的数据，如超时、重试等，主要保存的是QoS1和QoS2的帧，
    // PING、CONNECT的帧为QoS1的子类型
    // TODO:frame_t为数据帧，其中包含了所有需要处理的数据，如重试次数、超时时间等，对于QoS0的数据，不会保存在此处
    // std::list<frame_t> frame_state_list;
} // namespace sdcp

#endif // __SDCP_STATE_H__