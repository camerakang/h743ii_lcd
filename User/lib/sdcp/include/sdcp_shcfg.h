#ifndef __SDCP_IO_INST_H__
#define __SDCP_IO_INST_H__

#include "sdcp_io_impl.h"
#include <functional>

namespace sdcp
{
    /// @brief 共享配置类
    /// @note 该类用于存储共享配置信息，如IO实现、同步头、最大包大小等
    /// @note 采用全静态成员实现，使用SHCGF::xxx()获取参数值
    /// @note 该对象中保存的数据初始化和修改由sdcp_impl类负责，使用者无需关心
    class SHCFG
    {
        friend class sdcp_impl;

    private:
        static IOImpl *__io_impl;                      // IO实现
        static std::vector<uint8_t> __sync_header;     // 同步头
        static uint16_t __max_packet_size;             // 最大包大小
        static int64_t __timeout_ms;                   // 超时时间
        static uint16_t __max_retry_times;             // 最大重试次数
        static uint16_t __msgid_generator;             // 消息ID生成器
        static std::function<const int64_t()> __ts_ms; // 当前时间戳

    public:
        /// @brief 获取IO实现
        /// @return 返回IO实现
        inline static IOImpl &ioimpl()
        {
            return *__io_impl;
        }

        /// @brief 获取当前时间戳
        /// @return 返回当前时间戳
        inline static const int64_t ts_ms()
        {
            return __ts_ms();
        }

        /// @brief 获取同步头
        /// @return 返回同步头
        inline static const std::vector<uint8_t> sync_header()
        {
            return __sync_header;
        }

        /// @brief 获取包的最大值
        /// @return 返回包的最大值
        inline static const uint16_t max_packet_size()
        {
            return __max_packet_size;
        }

        /// @brief 获取超时时间
        /// @return 返回超时时间
        inline static const int64_t timeout_ms()
        {
            return __timeout_ms;
        }

        /// @brief 获取最大重试次数
        /// @return 返回最大重试次数
        inline static const uint16_t max_retry_times()
        {
            return __max_retry_times;
        }

        /// @brief 获取消息ID
        /// @return 返回一个新的消息ID
        inline static const uint16_t msgid_generator()
        {
            return ++__msgid_generator;
        }
    };
}

#endif // __SDCP_IO_INST_H__