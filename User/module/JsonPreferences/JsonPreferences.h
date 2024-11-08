#ifndef _JSON_PREFERENCES_H_
#define _JSON_PREFERENCES_H_

#include "json.hpp"
#include <string>

using json = nlohmann::json;

/**
 * JsonPreferences 类用于管理基于JSON格式的偏好设置。
 * 提供了多种类型的键值对存储和读取方法，支持整型、浮点型、布尔型和字符串类型的数据。
 */
class JsonPreferences
{
public:
    /**
     * 构造函数，初始化JsonPreferences对象。
     */
    JsonPreferences();

    /**
     * 析构函数，释放JsonPreferences对象。
     */
    ~JsonPreferences();

    /**
     * 存储一个有符号字符类型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putChar(const char *key, int8_t value);

    /**
     * 存储一个无符号字符类型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putUChar(const char *key, uint8_t value);

    /**
     * 存储一个有符号短整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putShort(const char *key, int16_t value);

    /**
     * 存储一个无符号短整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putUShort(const char *key, uint16_t value);

    /**
     * 存储一个有符号整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putInt(const char *key, int32_t value);

    /**
     * 存储一个无符号整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putUInt(const char *key, uint32_t value);

    /**
     * 存储一个有符号长整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putLong(const char *key, int32_t value);

    /**
     * 存储一个无符号长整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putULong(const char *key, uint32_t value);

    /**
     * 存储一个有符号64位整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putLong64(const char *key, int64_t value);

    /**
     * 存储一个无符号64位整型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putULong64(const char *key, uint64_t value);

    /**
     * 存储一个浮点型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putFloat(const char *key, float value);

    /**
     * 存储一个双精度浮点型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putDouble(const char *key, double value);

    /**
     * 存储一个布尔型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putBool(const char *key, bool value);

    /**
     * 存储一个字符串类型的值到偏好设置中。
     *
     * @param key 键名
     * @param value 要存储的值
     * @return 存储操作成功与否，通常为1表示成功
     */
    size_t putString(const char *key, const std::string &value);

    /**
     * 从偏好设置中获取一个有符号字符类型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    int8_t getChar(const char *key, int8_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个无符号字符类型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    uint8_t getUChar(const char *key, uint8_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个有符号短整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    int16_t getShort(const char *key, int16_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个无符号短整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    uint16_t getUShort(const char *key, uint16_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个有符号整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    int32_t getInt(const char *key, int32_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个无符号整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    uint32_t getUInt(const char *key, uint32_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个有符号长整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    int32_t getLong(const char *key, int32_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个无符号长整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    uint32_t getULong(const char *key, uint32_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个有符号64位整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    int64_t getLong64(const char *key, int64_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个无符号64位整型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    uint64_t getULong64(const char *key, uint64_t defaultValue = 0);

    /**
     * 从偏好设置中获取一个浮点型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    float getFloat(const char *key, float defaultValue = NAN);

    /**
     * 从偏好设置中获取一个双精度浮点型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    double getDouble(const char *key, double defaultValue = NAN);

    /**
     * 从偏好设置中获取一个布尔型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    bool getBool(const char *key, bool defaultValue = false);

    /**
     * 从偏好设置中获取一个字符串类型的值。
     *
     * @param key 键名
     * @param defaultValue 如果键不存在或类型不匹配时返回的默认值
     * @return 对应键的值，如果不存在则返回默认值
     */
    std::string getString(const char *key, const std::string &defaultValue = "");

    /**
     * 检查偏好设置中是否包含指定的键。
     *
     * @param key 键名
     * @return 如果存在该键则返回true，否则返回false
     */
    bool isKey(const char *key);

    /**
     * 清除偏好设置中的所有键值对。
     *
     * @return 清除操作成功与否
     */
    bool clear();

    /**
     * 删除偏好设置中的指定键。
     *
     * @param key 键名
     * @return 删除操作成功与否
     */
    bool remove(const char *key); // 删除指定键

    /**
     * @brief 从 JSON 格式的字符串创建 JsonPreferences 对象
     *
     * 静态方法，用于解析 JSON 字符串并生成对应的 JsonPreferences 对象
     *
     * @param jsonStr JSON 格式的字符串
     * @return 解析后的 JsonPreferences 对象
     */
    static JsonPreferences fromString(const std::string &jsonStr);
    std::string toString() const;

private:
    json preferences; // JSON 对象存储数据
};

#endif // _JSON_PREFERENCES_H_
