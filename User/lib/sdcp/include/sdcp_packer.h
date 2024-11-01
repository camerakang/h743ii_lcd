/// @brief SDCP协议打包

#ifndef __SDCP_PACKER_H__
#define __SDCP_PACKER_H__

#include "sdcp_crc16.h"
#include <vector>
#include "sdcp_def.h"
#include "sdcp_comm.h"
#include "sdcp_shcfg.h"

namespace sdcp
{
    /// @brief 打包器对象，提供给使用者更方面的进行数据打包
    struct packer_t
    {
        static std::vector<uint8_t> pack_with_msgid(
            msg_type_t msg_type,
            qos_t qos,
            const port_t port,
            const uint8_t *data, uint16_t len,
            msgid_t msgid = 0,
            msg_extend_flag_t extend_flag = msg_extend_flag_t::EXTEND0)
        {
            auto sync_header{SHCFG::sync_header()};

            // 数据长度  [ 帧识别码 + 端口号长度 + 用户数据长度 + 校验码长度 ]
            size_t data_len = 2 + sizeof(port_t) + len + 2;

            // 数据缓冲区
            std::vector<uint8_t> raw_data;
            raw_data.reserve(sync_header.size() + 1 + data_len);

            // 同步头
            raw_data.insert(raw_data.end(), sync_header.begin(), sync_header.end());

            // 固定头
            uint8_t fixed_header{
                static_cast<uint8_t>(msg_type) << 4 |
                static_cast<uint8_t>(qos) << 2 |
                (data_len <= 255 ? SDCP_MSG_LEN_LEVEL0_IN_FIXED_HEADER : SDCP_MSG_LEN_LEVEL1_IN_FIXED_HEADER) |
                static_cast<uint8_t>(extend_flag)};
            raw_data.push_back(fixed_header);

            // 数据长度
            if (data_len <= 255) [[linkely]]
            {
                raw_data.push_back(static_cast<uint8_t>(data_len));
            }
            else
            {
                raw_data.insert(raw_data.end(),
                                reinterpret_cast<const uint8_t *>(&data_len),
                                reinterpret_cast<const uint8_t *>(&data_len) + 2);
            }

            // [帧识别码]
            raw_data.insert(raw_data.end(),
                            reinterpret_cast<const uint8_t *>(&msgid),
                            reinterpret_cast<const uint8_t *>(&msgid) + sizeof(msgid_t));

            // 端口号
            raw_data.insert(raw_data.end(),
                            reinterpret_cast<const uint8_t *>(&port),
                            reinterpret_cast<const uint8_t *>(&port) + sizeof(port));

            // 数据
            if (data && len > 0)
            {
                raw_data.insert(raw_data.end(), data, data + len);
            }

            // CRC校验
            raw_data.push_back(sdcp::crc16(raw_data.data(), raw_data.size()));
            return raw_data;
        }

        static std::vector<uint8_t> pack_without_msgid(
            msg_type_t msg_type,
            qos_t qos,
            const port_t port,
            const uint8_t *data, uint16_t len,
            msgid_t msgid = 0,
            msg_extend_flag_t extend_flag = msg_extend_flag_t::EXTEND0)
        {
            auto sync_header{SHCFG::sync_header()};

            // 数据长度  [ 帧识别码 + 端口号长度 + 用户数据长度 + 校验码长度 ]
            size_t data_len = sizeof(port_t) + len + 2;

            // 数据缓冲区
            std::vector<uint8_t> raw_data;
            raw_data.reserve(sync_header.size() + 1 + data_len);

            // 同步头
            raw_data.insert(raw_data.end(), sync_header.begin(), sync_header.end());

            // 固定头
            uint8_t fixed_header{
                static_cast<uint8_t>(msg_type) << 4 |
                static_cast<uint8_t>(qos) << 2 |
                (data_len <= 255 ? SDCP_MSG_LEN_LEVEL0_IN_FIXED_HEADER : SDCP_MSG_LEN_LEVEL1_IN_FIXED_HEADER) |
                static_cast<uint8_t>(extend_flag)};
            raw_data.push_back(fixed_header);

            // 数据长度
            if (data_len <= 255) [[linkely]]
            {
                raw_data.push_back(static_cast<uint8_t>(data_len));
            }
            else
            {
                raw_data.insert(raw_data.end(),
                                reinterpret_cast<const uint8_t *>(&data_len),
                                reinterpret_cast<const uint8_t *>(&data_len) + 2);
            }

            // 端口号
            raw_data.insert(raw_data.end(),
                            reinterpret_cast<const uint8_t *>(&port),
                            reinterpret_cast<const uint8_t *>(&port) + sizeof(port));

            // 数据
            if (data && len > 0)
            {
                raw_data.insert(raw_data.end(), data, data + len);
            }

            // CRC校验
            // raw_data.push_back(sdcp::crc16(raw_data.data(), raw_data.size()));
            uint16_t crc116_val{sdcp::crc16(raw_data.data(), raw_data.size())};
            raw_data.insert(raw_data.end(), reinterpret_cast<uint8_t *>(&crc116_val), reinterpret_cast<uint8_t *>(&crc116_val) + 2);

            return raw_data;
        }
    };
}
#endif // __SDCP_PACKER_H__