#ifndef __UTOOLS_LIMIT_VARIABLE_RATE_FLITER_H__
#define __UTOOLS_LIMIT_VARIABLE_RATE_FLITER_H__

namespace utools::fliter
{
    template <class _InputType>
    class LimitVariableRateFliter
    {
    protected:
        _InputType _upper_limit_rate{1}; // 增加速度上限
        _InputType _lower_limit_rate{1}; // 减少速度下限
        _InputType _prev_input_val{0};   // 前序输入值

    public:
        /// @brief 只有初始化的构造
        /// @param init_val 初始化值
        LimitVariableRateFliter(_InputType init_val = 0) : _prev_input_val(init_val)
        {
        }

        /// @brief 设置范围的构造
        /// @param lower_limit_rate 下限速率值
        /// @param upper_limit_rate 上限速率值
        /// @param init_val 初始化值
        LimitVariableRateFliter(_InputType lower_limit_rate, _InputType upper_limit_rate, _InputType init_val = 0) : _upper_limit_rate(upper_limit_rate), _lower_limit_rate(lower_limit_rate), _prev_input_val(init_val)
        {
        }

        /// @brief 设置上下限范围
        /// @param lower_limit_rate 上限值
        /// @param upper_limit_rate 下限值
        void set_limit_rate(_InputType lower_limit_rate, _InputType upper_limit_rate)
        {
            _upper_limit_rate = upper_limit_rate;
            _lower_limit_rate = lower_limit_rate;
        }

        /// @brief 重新初始化缓存
        /// @param init_val 初始化值
        void reinit(_InputType init_val)
        {
            _prev_input_val = init_val;
        }

        /// @brief 进行滤波输出，如果是周期性调用，使用此方法
        /// @param val 输入值
        /// @return 判断后输出相应的值
        const _InputType &calc(_InputType val)
        {
            auto val_rate{val - _prev_input_val};
            if (val_rate != 0)
            {
                if ((val_rate <= _upper_limit_rate) && (val_rate >= _lower_limit_rate))
                {
                    _prev_input_val = val;
                }
                else
                {
                    // NODE:引入数据可信度指标，适用于连续变化的值
                    float feasibility{std::abs(val_rate > 0 ? (float)_upper_limit_rate / val_rate : (float)_lower_limit_rate / val_rate)};
                    if (feasibility - 0.10 > 0.001f)
                    {
                        _prev_input_val += val_rate > 0 ? _upper_limit_rate : _lower_limit_rate;
                    }
                    else
                    {
                        _prev_input_val += (val_rate > 0 ? _upper_limit_rate : _lower_limit_rate) * 0.5;
                    }
                }
            }
            return _prev_input_val;
        }
    };
}

#endif // __UTOOLS_LIMIT_VARIABLE_RATE_FLITER_H__
