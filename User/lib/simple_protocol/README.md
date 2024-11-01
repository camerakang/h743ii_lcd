# SIMPLE PROTOCOL协议说明

## 1 概述
简易协议类型，用于定义简易协议的类型，包括同步头、通道描述类型、长度描述类型等。

协议组成：[SYNC_HEADER | FRAME_LEN | CHANNEL | DATA | CRC16]

​		  FRAME_LEN是CHANNEL、DATA和CRC16的长度；

​		  CRC16是包含了FRAME_LEN、CHANNEL和DATA三个部分的CRC16校验值。


## 2 使用方法

为了可以在不同的平台使用，本项目提供了模板实现```simple_protocol_tpl.h```和非模板实现```simple_protocol.h```

```simple_protocol_tpl.h```：在如MCU等资源有限的平台上使用，可以减少在解析数据时的判断开销；

```simple_protocol.h```：可以提供给其它的语言使用，如通过pybind11转换为python可以直接调用的库等。

```simple_protocol_impl.h```：文件中封装了```SimpleProtocolImpl```类，是```SimpleProtocolTpl```和```SimpleProtocol```的基类，相关接口都在这类中进行定义，使用时可以直接采用此类中的方法，对相关功能进行访问。



使用时只需要根据情况，通过```SimpleProtocolTpl```、```SimpleProtocol```的构造方法，进行实例化，并通过```SimpleProtocolImpl```中的接口进行读写操作即可。

## 3 例子

### 3.1 使用模板```SimpleProtocolTpl```实现

```c++
#include "simple_protocol_impl.h"
#include "simple_protocol_tpl.h"

SimpleProtocolTpl *protocol = dynamic_cast<SimpleProtocolImpl *>(new SimpleProtocolTpl<1, 1, true, 128, 200>({0xA5, 0xA5})); // 初始化协议解析功能
```

### 3.2 使用模板```SimpleProtocol```实现

```c++
#include "simple_protocol_impl.h"
#include "simple_protocol_tpl.h"

SimpleProtocolTpl *protocol = dynamic_cast<SimpleProtocolImpl *>(new SimpleProtocol({0xA5, 0xA5}, 1, 1, true, 128, 200)); // 初始化协议解析功能
```

### 3.3 功能调用相关的接口

```c++
class SimpleProtocolImpl
{
    /// @brief 将接收到的数据写入到缓存中
    /// @param data 接收到的数据
    /// @return 返回自身引用
    SimpleProtocolImpl &push_back(const std::vector<uint8_t> &data);
    
    /// @brief 将接收到的数据写入到缓存中
    /// @param data 接收到的数据
    /// @param size 数据大小
    /// @return 返回自身引用
    virtual SimpleProtocolImpl &push_back(const uint8_t *data, size_t size);
    
    /// @brief 解析数据包
    /// @return 返回解析结果
    virtual const frame_t parse();
    
    /// @brief 解析数据包
    /// @param data 接收到的数据
    /// @return 返回解析结果
    virtual const frame_t parse(const std::vector<uint8_t> &data);
    
    /// @brief 生成一个数据包
    /// @param data 需要打包的数据
    /// @param data_len 数据长度
    /// @param channel 通道号
    /// @return 打包后的数据
    /// @note 打包后的数据包含：同步头、长度信息、通道信息、数据、CRC16
    virtual const std::vector<uint8_t> pack(const void *data, const size_t data_len, const uint32_t channel);
    
    /// @brief 判断当前缓存是否为空
    /// @return 返回当前缓存是否为空
    virtual bool buffer_is_empty();
    
    /// @brief 清空缓存区域
    virtual void buffer_flush();
    
    const std::vector<uint8_t> pack(const std::vector<uint8_t> &data, const uint32_t channel = 1);
    const std::vector<uint8_t> pack(const std::string &data, const uint32_t channel = 1);
    const std::vector<uint8_t> pack(const uint32_t channel, const void *data, const size_t data_len);
    const std::vector<uint8_t> pack(const uint32_t channel, const std::vector<uint8_t> &data);
    const std::vector<uint8_t> pack(const uint32_t channel, const std::string &data);
    virtual const uint64_t curr_timestamp_ms();
    static uint16_t crc16(const void *buffer, size_t buffer_length);
    static uint16_t crc16(const std::vector<uint8_t> &data);
    static uint16_t crc16(const std::vector<uint8_t>::iterator &begin, const std::vector<uint8_t>::iterator &end);
}
```

### 3.4 ```frame_t```数据帧

```c++
class SimpleProtocolImpl
{
    /// @brief 数据包状态
    enum class FrameStatus
    {
        BUFFER_EMPTY = 0,      // 缓冲区为空
        BUFFER_INVALID = 1,    // 无效缓冲区，没有可以使用的数据
        BUFFER_TIMEOUT = 2,    // 缓冲区超时，连续一定时间没有接收到数据，但在缓存中有数据
        BUFFER_BROKEN = 3,     // 一帧数据开关前面有未知意义的缓存
        BUFFER_NOT_ENOUGH = 4, // 缓冲区长度不足
        LENGTH_NOT_ENOUGH = 5, // 帧长度不足
        LENGTH_ERROR = 6,      // 帧长度错误
        CRC_ERROR = 7,         // 帧CRC错误
        INVALID = 8,           // 帧无效
        VALID = 9,             // 帧有效
    };

	/// @brief 解析出来的一帧数据
    struct frame_t
    {
        FrameStatus status;        // 当前帧的状态
        std::uint32_t channel;     // 通道号
        std::vector<uint8_t> data; // 帧数据

        /// @brief 将当前的帧数据转换为字符串
        /// @return 返回当前帧数据转换为字符串
        const std::string str_data();
        frame_t(FrameStatus status, std::uint32_t channel, std::vector<uint8_t> data);
    };
}
```

