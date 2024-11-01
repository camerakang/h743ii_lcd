#ifndef __BYTE_STRUCT_H__
#define __BYTE_STRUCT_H__

#include "../utools_cfg.h"

#if UTOOLS_BYTES_STRUCT_ENABLE

#include <vector>
#include <string>
#include <cstdarg>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <cstdint>
#include <any>

namespace utools::code
{
    /// @brief 实现一个辅助使用的结构体打包和解包类
    /// @note 功能可以参考python的struct模块，其接口和使用方法类似
    class ByteStruct
    {
    public:
        /// @brief 判断本地字节序是否为小端序
        /// @return true 小端序，false 大端序
        inline static bool is_little_endian()
        {
            const uint16_t endian_type_code = 0xAABB;   // 0xAABB表示本地字节序
            return (endian_type_code & 0x00FF) == 0xBB; // 低位字节是 0xBB 表示小端
        }

        /// @brief 序列化函数
        /// @param args 变参列表
        /// @return 序列化后的字符串和字节数组
        template <typename... Args>
        static std::pair<std::string, std::vector<uint8_t>> serialize(Args &&...args)
        {
            std::vector<uint8_t> data;                                // 序列化后的字节数组
            std::string desc{is_little_endian() ? "<" : ">"};         // 序列化后的字符串描述
            (__serialize(std::forward<Args>(args), data, desc), ...); // 逐个序列化每个参数
            return {desc, data};
        }

        /// @brief 反序列化函数
        /// @param desc_data 字符串描述和字节数组
        /// @return 反序列化后的数据
        static std::vector<std::any> deserialize(const std::pair<std::string, std::vector<uint8_t>> &desc_data)
        {
            return unpack(desc_data.first, desc_data.second);
        }

        /// @brief 打包函数
        /// @param format 格式化字符串，如 "IiB"
        /// @param ... 变参列表
        /// @return 打包后的字节数组
        static std::vector<uint8_t> pack(const char *format, ...)
        {
            std::vector<uint8_t> buffer;
            size_t format_pos{0}; // 当前解析到的格式字符
            size_t ele_size{0};   // 当前解析到的元素大小
            size_t format_length{strlen(format)};

            if (format_length == 0)
            {
                return buffer;
            }

            // 跳过空格
            while (format_pos < format_length && std::isspace(format[format_pos]))
            {
                ++format_pos;
            }

            // 判断是否需要字节序转换
            bool endian_swap = false;
            char first_chr = format[format_pos];
            if (first_chr == '@' || first_chr == '=' || first_chr == '<' || first_chr == '>')
            {
                endian_swap = is_little_endian() != (first_chr == '>' || first_chr == '!');
                format_pos++;
            }

            va_list args;
            va_start(args, format);

            for (; format_pos < format_length; ++format_pos)
            {
                // 解析元素大小
                ele_size = 0;
                while (format[format_pos] >= '0' && format[format_pos] <= '9')
                {
                    ele_size = ele_size * 10 + (format[format_pos] - '0');
                    ++format_pos;
                }
                if (ele_size == 0)
                {
                    ele_size = 1;
                }

                // 处理结束条件判断
                if (format_pos >= format_length)
                {
                    break;
                }

                // 解析格式字符
                char type = format[format_pos];
                switch (type)
                {
                case 'x': // 占位符
                    buffer.insert(buffer.end(), ele_size, 0);
                    break;
                case 'c':
                case 'b':
                case 'B':
                case '?':
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        buffer.push_back(static_cast<uint8_t>(va_arg(args, int)));
                    }
                    break;
                case 'h':
                case 'H':
                case 'e':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_integer<uint16_t>(buffer, static_cast<uint16_t>(va_arg(args, int)), endian_swap);
                    }
                    break;
                }
                case 'i':
                case 'I':
                case 'l':
                case 'L':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_integer<uint32_t>(buffer, static_cast<uint32_t>(va_arg(args, int)), endian_swap);
                    }
                    break;
                }
                case 'q':
                case 'Q':
                case 'n':
                case 'N':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_integer<uint64_t>(buffer, static_cast<uint64_t>(va_arg(args, int)), endian_swap);
                    }
                    break;
                }
                case 'f':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_float(buffer, static_cast<float>(va_arg(args, double)), endian_swap);
                    }
                    break;
                }
                case 'd':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_double(buffer, va_arg(args, double), endian_swap);
                    }
                    break;
                }
                case 's':
                {
                    const char *str = va_arg(args, const char *);
                    buffer.insert(buffer.end(), str, str + ele_size);
                    break;
                }
                case 'p':
                {
                    void *ptr = va_arg(args, void *);
                    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(ptr), reinterpret_cast<const uint8_t *>(ptr) + ele_size);
                    break;
                }
                case 'P':
                {
                    for (size_t i = 0; i < ele_size; ++i)
                    {
                        __pack_ptr(buffer, reinterpret_cast<uintptr_t>(va_arg(args, void *)), endian_swap);
                    }
                    break;
                }
                default:
                    break;
                }
            }

            va_end(args);
            return buffer;
        }

        /// @brief 从数据中解析出一帧数据
        /// @param data 数据
        /// @param start_idx 开始解析的索引
        /// @param length 解析的长度
        /// @return 解析出来的帧数据
        template <typename _OUT_TYPE>
        static _OUT_TYPE unpack_from(const uint8_t *data, uint32_t start_idx, uint32_t length)
        {
            if (length == 0 || sizeof(_OUT_TYPE) > length)
            {
                return _OUT_TYPE();
            }
            _OUT_TYPE ret;
            memcpy(&ret, &data[start_idx], length);
            return ret;
        }

        template <typename _OUT_TYPE>
        static _OUT_TYPE unpack_from(const std::vector<uint8_t> &data, uint32_t start_idx, uint32_t length)
        {
            return unpack_from<_OUT_TYPE>(data.data(), start_idx, length);
        }

        /// @brief 解包函数
        /// @param format 格式字符串
        /// @param data 数据缓冲区
        /// @param length 数据长度
        /// @return 解包结果
        static std::vector<std::any> unpack(const std::string &format, const uint8_t *data, size_t length)
        {
            std::vector<std::any> result;
            size_t data_offset{0}; // 当前解析到的位置
            size_t ele_size{0};    // 当前解析到的元素大小
            size_t format_pos{0};  // 当前解析到的格式字符

            // 跳过空格
            while (format_pos < format.length() && std::isspace(format[format_pos]))
            {
                ++format_pos;
            }

            // 判断是否需要字节序转换
            bool endian_swap = false;
            char first_chr = format[format_pos];
            if (first_chr == '@' || first_chr == '=' || first_chr == '<' || first_chr == '>')
            {
                endian_swap = is_little_endian() != (first_chr == '>' || first_chr == '!');
                format_pos++;
            }

            while (data_offset < length && format_pos < format.length())
            {
                // 解析元素大小
                ele_size = 0;
                while (format[format_pos] >= '0' && format[format_pos] <= '9')
                {
                    ele_size = ele_size * 10 + (format[format_pos] - '0');
                    ++format_pos;
                }
                if (ele_size == 0)
                {
                    ele_size = 1;
                }

                // 解析格式字符
                char type = format[format_pos];
                switch (type)
                {
                case 'x':
                    data_offset += ele_size;
                    break;
                case 'c':
                case 'b':
                {
                    if (ele_size == 1)
                        result.emplace_back<char>(static_cast<char>(data[data_offset++]));
                    else
                        result.emplace_back<std::vector<char>>(__unpack_bytes<char>(data, data_offset, ele_size));
                    break;
                }
                case 'B':
                {
                    if (ele_size == 1)
                        result.emplace_back<char>(static_cast<uint8_t>(data[data_offset++]));
                    else
                        result.emplace_back<std::vector<uint8_t>>(__unpack_bytes<uint8_t>(data, data_offset, ele_size));
                    break;
                }
                case '?':
                {
                    if (ele_size == 1)
                        result.emplace_back<bool>(data[data_offset++] != 0);
                    else
                        result.emplace_back<std::vector<bool>>(__unpack_bool_bytes(data, data_offset, ele_size));
                    break;
                }
                case 'h':
                {
                    if (ele_size == 1)
                        result.emplace_back<int16_t>(__unpack_integer<int16_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<int16_t>>(__unpack_integer_array<int16_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'H':
                case 'e':
                {
                    if (ele_size == 1)
                        result.emplace_back<uint16_t>(__unpack_integer<uint16_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<uint16_t>>(__unpack_integer_array<uint16_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'i':
                case 'l':
                {
                    if (ele_size == 1)
                        result.emplace_back<int32_t>(__unpack_integer<int32_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<int32_t>>(__unpack_integer_array<int32_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'I':
                case 'L':
                {
                    if (ele_size == 1)
                        result.emplace_back<uint32_t>(__unpack_integer<uint32_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<uint32_t>>(__unpack_integer_array<uint32_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'q':
                case 'n': // ssize_t
                {
                    if (ele_size == 1)
                        result.emplace_back<int64_t>(__unpack_integer<int64_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<int64_t>>(__unpack_integer_array<int64_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'Q':
                case 'N': // size_t
                {
                    if (ele_size == 1)
                        result.emplace_back<uint64_t>(__unpack_integer<uint64_t>(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<uint64_t>>(__unpack_integer_array<uint64_t>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'f':
                {
                    if (ele_size == 1)
                        result.emplace_back<float>(__unpack_float(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<float>>(__unpack_float_array<float>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 'd':
                {
                    if (ele_size == 1)
                        result.emplace_back<float>(__unpack_float(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<double>>(__unpack_float_array<double>(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case 's':
                case 'p':
                {
                    result.emplace_back<std::string>(__unpack_string(data, data_offset, ele_size));
                    break;
                }
                case 'P':
                {
                    if (ele_size == 1)
                        result.emplace_back<void *>(__unpack_pointer(data, data_offset, endian_swap));
                    else
                        result.emplace_back<std::vector<void *>>(__unpack_pointer_array(data, data_offset, ele_size, endian_swap));
                    break;
                }
                case ' ':
                    break; // 空白字符，忽略
                default:
                    return std::vector<std::any>{}; // 返回一个空的数据向量
                    break;
                }

                ++format_pos;
            }
            return result;
        }

        static std::vector<std::any> unpack(const std::string &format, const std::vector<uint8_t> &data)
        {
            return unpack(format, data.data(), data.size());
        }

    private:
        inline static uint16_t __swap16(uint16_t value)
        {
            return (value >> 8) | (value << 8);
        }

        inline static uint32_t __swap32(uint32_t value)
        {
            return ((value & 0xFF000000) >> 24) |
                   ((value & 0x00FF0000) >> 8) |
                   ((value & 0x0000FF00) << 8) |
                   ((value & 0x000000FF) << 24);
        }

        inline static uint64_t __swap64(uint64_t value)
        {
            return ((value & 0xFF00000000000000) >> 56) |
                   ((value & 0x00FF000000000000) >> 40) |
                   ((value & 0x0000FF0000000000) >> 24) |
                   ((value & 0x000000FF00000000) >> 8) |
                   ((value & 0x00000000FF000000) << 8) |
                   ((value & 0x0000000000FF0000) << 24) |
                   ((value & 0x000000000000FF00) << 40) |
                   ((value & 0x00000000000000FF) << 56);
        }

        inline static uintptr_t __swap_ptr(uintptr_t value)
        {
            if constexpr (sizeof(uintptr_t) == 4)
            {
                return __swap32(static_cast<uint32_t>(value));
            }
            return __swap64(static_cast<uint64_t>(value));
        }

        template <typename T>
        static void __pack_integer(std::vector<uint8_t> &buffer, T value, bool endian_swap)
        {
            if (endian_swap)
            {
                if constexpr (std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value)
                {
                    value = static_cast<T>(__swap32(static_cast<uint32_t>(value)));
                }
                else if constexpr (std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value)
                {
                    value = static_cast<T>(__swap16(static_cast<uint16_t>(value)));
                }
                else if constexpr (std::is_same<T, uint64_t>::value)
                {
                    value = __swap64(value);
                }
            }
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(&value) + sizeof(value));
        }

        static void __pack_float(std::vector<uint8_t> &buffer, float value, bool endian_swap)
        {
            uint32_t temp;
            std::memcpy(&temp, &value, sizeof(float));
            if (endian_swap)
            {
                temp = __swap32(temp);
            }
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&temp), reinterpret_cast<uint8_t *>(&temp) + sizeof(temp));
        }

        static void __pack_double(std::vector<uint8_t> &buffer, double value, bool endian_swap)
        {
            uint64_t temp;
            std::memcpy(&temp, &value, sizeof(double));
            if (endian_swap)
            {
                temp = __swap64(temp);
            }
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&temp), reinterpret_cast<uint8_t *>(&temp) + sizeof(temp));
        }

        static void __pack_ptr(std::vector<uint8_t> &buffer, uintptr_t value, bool endian_swap)
        {
            if (endian_swap)
            {
                value = __swap_ptr(value);
            }
            buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(&value) + sizeof(value));
        }

        template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
        static T __unpack_integer(const uint8_t *data, size_t &offset, bool endian_swap)
        {
            T value;
            std::memcpy(&value, &data[offset], sizeof(T));
            offset += sizeof(T);
            if (endian_swap)
            {
                if constexpr (std::is_same<T, int32_t>::value || std::is_same<T, uint32_t>::value)
                {
                    value = static_cast<T>(__swap32(static_cast<uint32_t>(value)));
                }
                else if constexpr (std::is_same<T, int16_t>::value || std::is_same<T, uint16_t>::value)
                {
                    value = static_cast<T>(__swap16(static_cast<uint16_t>(value)));
                }
                else if constexpr (std::is_same<T, uint64_t>::value)
                {
                    value = __swap64(value);
                }
            }
            return value;
        }

        static float __unpack_float(const uint8_t *data, size_t &offset, bool endian_swap)
        {
            float value;
            std::memcpy(&value, &data[offset], sizeof(float));
            offset += sizeof(float);
            if (endian_swap)
            {
                value = static_cast<float>(__swap32(static_cast<uint32_t>(value)));
            }
            return value;
        }

        static double __unpack_double(const uint8_t *data, size_t &offset, bool endian_swap)
        {
            double value;
            std::memcpy(&value, &data[offset], sizeof(double));
            offset += sizeof(double);
            if (endian_swap)
            {
                value = static_cast<double>(__swap64(static_cast<uint64_t>(value)));
            }
            return value;
        }

        static void *__unpack_pointer(const uint8_t *data, size_t &offset, bool endian_swap)
        {
            uintptr_t address{reinterpret_cast<uintptr_t>(data + offset)};
            if (endian_swap)
            {
                address = __swap_ptr(address);
            }
            offset += sizeof(uintptr_t);
            return reinterpret_cast<void *>(address);
        }

        template <typename T,
                  typename = std::enable_if<std::is_same<int8_t, T>::value ||
                                            std::is_same<uint8_t, T>::value ||
                                            std::is_same<char, T>::value>>
        inline static std::vector<T> __unpack_bytes(const uint8_t *data, size_t &offset, size_t count)
        {
            std::vector<T> value = std::vector<T>{data + offset, data + offset + count};
            offset += count;
            return value;
        }

        inline static std::vector<bool> __unpack_bool_bytes(const uint8_t *data, size_t &offset, size_t count)
        {
            std::vector<bool> value;
            value.reserve(count);
            for (size_t i = 0; i < count; ++i)
            {
                value.push_back(data[offset + i] != 0);
            }
            offset += count;
            return value;
        }

        template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
        static std::vector<T> __unpack_integer_array(const uint8_t *data, size_t &offset, size_t count, bool endian_swap)
        {
            std::vector<T> values;
            for (size_t i = 0; i < count; ++i)
            {
                values.push_back(__unpack_integer<T>(data, offset, endian_swap));
            }
            return values;
        }

        template <typename T, typename = std::enable_if<std::is_floating_point<T>::value>>
        static std::vector<T> __unpack_float_array(const uint8_t *data, size_t &offset, size_t count, bool endian_swap)
        {
            std::vector<T> values;
            if constexpr (std::is_same<T, double>::value)
            {
                for (size_t i = 0; i < count; ++i)
                {
                    values.push_back(__unpack_double(data, offset, endian_swap));
                }
            }
            else if constexpr (std::is_same<T, float>::value)
            {
                for (size_t i = 0; i < count; ++i)
                {
                    values.push_back(__unpack_float(data, offset, endian_swap));
                }
            }
            return values;
        }

        static std::vector<void *> __unpack_pointer_array(const uint8_t *data, size_t &offset, size_t count, bool endian_swap)
        {
            std::vector<void *> values;
            for (size_t i = 0; i < count; ++i)
            {
                values.push_back(reinterpret_cast<void *>(__unpack_pointer(data, offset, endian_swap)));
            }
            return values;
        }

        static std::string __unpack_string(const uint8_t *data, size_t &offset, size_t length)
        {
            std::string value(reinterpret_cast<const char *>(data + offset), length);
            offset += length;
            return value;
        }

        /// @brief 根据类型描述符解析数据
        /// @tparam T 类型
        /// @return 描述符解析后的数据
        template <typename T>
        static std::string __get_type_desc()
        {
            if constexpr (std::is_same<T, bool>::value)
            {
                return "?"; // bool 类型使用 ? 表示
            }
            else if constexpr (std::is_same<T, char>::value)
            {
                return "c"; // char 类型使用 c 表示
            }
            else if constexpr (std::is_same<T, int8_t>::value)
            {
                return "b"; // int8_t 类型使用 b 表示
            }
            else if constexpr (std::is_same<T, uint8_t>::value)
            {
                return "B"; // uint8_t 类型使用 B 表示
            }
            else if constexpr (std::is_same<T, int16_t>::value)
            {
                return "h"; // int16_t 类型使用 h 表示
            }
            else if constexpr (std::is_same<T, uint16_t>::value)
            {
                return "H"; // uint16_t 类型使用 H 表示
            }
            else if constexpr (std::is_same<T, int32_t>::value)
            {
                return "i"; // int32_t 类型使用 i 表示
            }
            else if constexpr (std::is_same<T, uint32_t>::value)
            {
                return "I"; // uint32_t 类型使用 I 表示
            }
            else if constexpr (std::is_same<T, long>::value)
            {
                return "l"; // long 类型使用 l 表示
            }
            else if constexpr (std::is_same<T, unsigned long>::value)
            {
                return "L"; // unsigned long 类型使用 L 表示
            }
            else if constexpr (std::is_same<T, int64_t>::value)
            {
                return "q"; // int64_t 类型使用 q 表示
            }
            else if constexpr (std::is_same<T, uint64_t>::value)
            {
                return "Q"; // uint64_t 类型使用 Q 表示
            }
            // else if constexpr (std::is_same<T, ssize_t>::value)
            //{
            //     return "n"; // ssize_t 类型使用 n 表示
            // }
            else if constexpr (std::is_same<T, size_t>::value)
            {
                return "N"; // size_t 类型使用 N 表示
            }
            else if constexpr (std::is_same<T, float>::value)
            {
                return "f"; // float 类型使用 f 表示
            }
            else if constexpr (std::is_same<T, double>::value)
            {
                return "d"; // double 类型使用 d 表示
            }
            else if constexpr (std::is_same<T, std::string>::value || std::is_same<T, char *>::value)
            {
                return "s"; // std::string 类型使用 s 表示
            }
            else if constexpr (std::is_pointer<T>::value)
            {
                return "P"; // 指针类型使用 P 表示
            }
            else
            {
                return "x"; // 其他类型使用 x 表示
            }
        }

        template <typename T,
                  typename = std::enable_if<std::is_integral<T>::value || std::is_pointer<T>::value>>
        static void __serialize(const T &value, std::vector<uint8_t> &out, std::string &desc)
        {
            desc += __get_type_desc<T>();
            const uint8_t *byte = reinterpret_cast<const uint8_t *>(&value);
            out.insert(out.end(), byte, byte + sizeof(T));
        }

        static void __serialize(const std::string &value, std::vector<uint8_t> &out, std::string &desc)
        {
            size_t length = value.size();
            desc += std::to_string(length) + "s";
            out.insert(out.end(), value.begin(), value.end()); // 将字符串内容序列化
        }

        static void __serialize(const char *value, std::vector<uint8_t> &out, std::string &desc)
        {
            size_t length = strlen(value);
            desc += std::to_string(length) + "s";
            out.insert(out.end(), value, value + length); // 将字符串内容序列化
        }

        template <typename T>
        static void __serialize(const std::vector<T> &value, std::vector<uint8_t> &out, std::string &desc)
        {
            desc += std::to_string(value.size()) + __get_type_desc<T>();
            for (const auto &item : value)
            {
                const uint8_t *byte = reinterpret_cast<const uint8_t *>(&item);
                out.insert(out.end(), byte, byte + sizeof(T));
            }
        }

        template <typename T, size_t N>
        static void __serialize(const std::array<T, N> &value, std::vector<uint8_t> &out, std::string &desc)
        {
            desc += std::to_string(N) + __get_type_desc<T>();
            for (const auto &item : value)
            {
                const uint8_t *byte = reinterpret_cast<const uint8_t *>(&item);
                out.insert(out.end(), byte, byte + sizeof(T));
            }
        }
    };
}

#endif // UTOOLS_BYTES_STRUCT_ENABLE
#endif // __BYTE_STRUCT_H__
