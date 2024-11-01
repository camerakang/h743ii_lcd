
/*
A、名称：递推平均滤波法（又称滑动平均滤波法）
B、方法：
    把连续取得的N个采样值看成一个队列，队列的长度固定为N，
    每次采样到一个新数据放入队尾，并扔掉原来队首的一次数据（先进先出原则），
    把队列中的N个数据进行算术平均运算，获得新的滤波结果。
    N值的选取：流量，N=12；压力，N=4；液面，N=4-12；温度，N=1-4。
C、优点：
    对周期性干扰有良好的抑制作用，平滑度高；
    适用于高频振荡的系统。
D、缺点：
    灵敏度低，对偶然出现的脉冲性干扰的抑制作用较差；
    不易消除由于脉冲干扰所引起的采样值偏差；
    不适用于脉冲干扰比较严重的场合；
    比较浪费RAM。
*/
#ifndef __MOVING_AVERAGE_QUICK_FLITER_HPP__
#define __MOVING_AVERAGE_QUICK_FLITER_HPP__

#include <cstdint>

namespace utools::fliter
{
    /// @brief 递推平均滤波法
    /// @tparam _InputType 输出值的数据类型
    /// @tparam _BufSize 缓存的大小
    template <class _InputType, unsigned short _BufSize>
    class MovingAverageQuick
    {
    protected:
        _InputType *_buf{nullptr};                                    // 数据缓存
        uint16_t _buf_pos{0};                                         // 当前操作数据的位置
        uint16_t _buf_size{static_cast<uint16_t>(_BufSize)};          // 缓存大小
        uint16_t _buf_size_mask{static_cast<uint16_t>(_BufSize - 1)}; // 缓存掩码
        int32_t _result{0};                                           // 滤波结果

    public:
        MovingAverageQuick(_InputType init_val = 0)
        {
            if (!__is_power_of_two(_BufSize))
            {
                _buf_size = __roundup_power_of_two(_BufSize);
            }
            _buf_size_mask = _buf_size - 1;
            _buf = new _InputType[_buf_size];
            reinit(init_val);
        }

        ~MovingAverageQuick()
        {
            delete[] _buf;
        }

        /// @brief 写入数据
        /// @param val 数据值
        inline void push(_InputType val)
        {
            _buf[++_buf_pos & _buf_size_mask] = val;
        }

        /// @brief 读取当前的滤波结果
        /// @tparam _Tr 指定返回值的类型
        /// @return 滤波结果
        template <class _Tre>
        inline const _Tre calc()
        {
            int32_t sum{0};
            for (uint16_t i = 0; i < _buf_size; i++)
            {
                sum += _buf[i];
            }
            _result = sum / _buf_size;
            return static_cast<_Tre>(_result);
        }

        /// @brief 读取当前的滤波结果
        /// @return 滤波结果
        inline const _InputType calc()
        {
            return calc<_InputType>();
        }

        /// @brief 增加新的数据，并返回计算结果
        /// @tparam _Tre 返回值类型
        /// @param val 输入值
        /// @return 滤波结果
        template <class _Tre>
        inline const _Tre calc(_InputType val)
        {
            push(val);
            return calc<_Tre>();
        }

        /// @brief 增加新的数据，并返回计算结果，计算结果类型与输出数据一样
        /// @param val 输入值
        /// @return 滤波结果
        const _InputType calc(_InputType val)
        {
            return calc<_InputType>(val);
        }

        /// @brief 获取当前的滤波结果
        /// @return 滤波结果
        inline const _InputType get()
        {
            return _result;
        }

        /// @brief 重新初始化缓存
        /// @param init_val 初始化值
        void reinit(_InputType init_val = 0)
        {
            for (uint16_t i = 0; i < _buf_size; i++)
            {
                _buf[i] = init_val;
            }
        }

    private:
        /// @brief 计算为2的平方数
        /// @param num 需要判读的值
        /// @return 返回是否成功
        inline const int __is_power_of_two(uint32_t num) const
        {
            if (num < 2)
            {
                return 0;
            }
            return (num & (num - 1)) == 0;
        }

        /// @brief 寻找最接近参数的2的平方数
        /// @param num 需要处理的值
        /// @return 2的平方数
        inline uint32_t __roundup_power_of_two(uint32_t num)
        {
            if (num == 0)
            {
                return 2;
            }
            int i = 0;
            for (; num != 0; ++i)
            {
                num >>= 1;
            }
            return 1U << i;
        }
    };
}

#endif // __MOVING_AVERAGE_QUICK_FLITER_HPP__
