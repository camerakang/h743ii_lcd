#include "JsonPreferences.h"
#include <iostream>
#include <stdexcept>
JsonPreferences::JsonPreferences()
{
    // 初始化 JSON 对象为空
    preferences = json::object();
}

JsonPreferences::~JsonPreferences() {}

size_t JsonPreferences::putChar(const char *key, int8_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putUChar(const char *key, uint8_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putShort(const char *key, int16_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putUShort(const char *key, uint16_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putInt(const char *key, int32_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putUInt(const char *key, uint32_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putLong(const char *key, int32_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putULong(const char *key, uint32_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putLong64(const char *key, int64_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putULong64(const char *key, uint64_t value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putFloat(const char *key, float value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putDouble(const char *key, double value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putBool(const char *key, bool value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

size_t JsonPreferences::putString(const char *key, const std::string &value)
{
    if (!key)
        return 0;
    preferences[key] = value;
    return 1;
}

int8_t JsonPreferences::getChar(const char *key, int8_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_integer())
    {
        return preferences[key].get<int8_t>();
    }
    return defaultValue;
}

uint8_t JsonPreferences::getUChar(const char *key, uint8_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_unsigned())
    {
        return preferences[key].get<uint8_t>();
    }
    return defaultValue;
}

int16_t JsonPreferences::getShort(const char *key, int16_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_integer())
    {
        return preferences[key].get<int16_t>();
    }
    return defaultValue;
}

uint16_t JsonPreferences::getUShort(const char *key, uint16_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_unsigned())
    {
        return preferences[key].get<uint16_t>();
    }
    return defaultValue;
}

int32_t JsonPreferences::getInt(const char *key, int32_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_integer())
    {
        return preferences[key].get<int32_t>();
    }
    return defaultValue;
}

uint32_t JsonPreferences::getUInt(const char *key, uint32_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_unsigned())
    {
        return preferences[key].get<uint32_t>();
    }
    return defaultValue;
}

int32_t JsonPreferences::getLong(const char *key, int32_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_integer())
    {
        return preferences[key].get<int32_t>();
    }
    return defaultValue;
}

uint32_t JsonPreferences::getULong(const char *key, uint32_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_unsigned())
    {
        return preferences[key].get<uint32_t>();
    }
    return defaultValue;
}

int64_t JsonPreferences::getLong64(const char *key, int64_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_integer())
    {
        return preferences[key].get<int64_t>();
    }
    return defaultValue;
}

uint64_t JsonPreferences::getULong64(const char *key, uint64_t defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_unsigned())
    {
        return preferences[key].get<uint64_t>();
    }
    return defaultValue;
}

float JsonPreferences::getFloat(const char *key, float defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_float())
    {
        return preferences[key].get<float>();
    }
    return defaultValue;
}

double JsonPreferences::getDouble(const char *key, double defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_number_float())
    {
        return preferences[key].get<double>();
    }
    return defaultValue;
}

bool JsonPreferences::getBool(const char *key, bool defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_boolean())
    {
        return preferences[key].get<bool>();
    }
    return defaultValue;
}

std::string JsonPreferences::getString(const char *key, const std::string &defaultValue)
{
    if (preferences.contains(key) && preferences[key].is_string())
    {
        return preferences[key];
    }
    return defaultValue;
}

/**
 * @brief 检查给定的键是否存在于 JSON 对象中
 *
 * 该函数用于确定一个键是否存在于 JSON 对象中，以便决定是否需要更新或添加该键的值
 *
 * @param key 要检查的键名
 * @return true 如果键存在于 JSON 对象中
 * @return false 如果键不存在于 JSON 对象中
 */
bool JsonPreferences::isKey(const char *key)
{
    // 检查键是否存在于 JSON 对象中
    return preferences.contains(key);
}

/**
 * 清除所有偏好设置
 *
 * 该函数将从偏好设置中移除所有已存储的键值对， essentially 重置 the 偏好设置到其初始状态
 * 没有参数传递给这个函数，它返回一个布尔值来指示操作是否成功
 *
 * @return 总是返回 true，因为 clear 操作在逻辑上不涉及失败的情形
 */
bool JsonPreferences::clear()
{
    // 清除所有键值对
    preferences.clear();
    return true; // 返回 true 表示清除成功
}

/**
 * @brief 删除指定键的偏好设置
 *
 * 该函数接受一个键名字符串作为参数，从偏好设置中删除与该键名关联的值
 * 如果键名为空或者键不存在于偏好设置中，则函数返回 false
 * 否则，函数将成功删除键对应的值，并返回 true
 *
 * @param key 要删除的键名，键名不能为空
 * @return true 如果键存在且删除成功
 * @return false 如果键为空或不存在
 */
bool JsonPreferences::remove(const char *key)
{
    if (!key)
        return false; // 如果键为空，返回 false
    // 检查键是否存在，如果存在则删除
    if (preferences.contains(key))
    {
        preferences.erase(key);
        return true; // 返回 true 表示删除成功
    }
    return false; // 返回 false 表示键不存在
}

/**
 * @brief 从字符串解析JsonPreferences对象
 *
 * 该函数接受一个JSON格式的字符串作为输入，尝试将其解析为一个JsonPreferences对象如果输入字符串不符合JSON格式，
 * 则抛出一个runtime_error异常这个函数的主要工作是调用json库的parse函数来执行实际的解析操作，
 * 并将结果存储在返回的对象中
 *
 * @param jsonStr JSON格式的字符串
 * @return JsonPreferences 解析后的JsonPreferences对象
 * @throws std::runtime_error 如果输入字符串不符合JSON格式
 * 需要在cxxFlags: 中添加- -fexceptions， -fno-exceptions 和 -fexceptions是冲突的，单独使用-fexceptions
 */
JsonPreferences JsonPreferences::fromString(const std::string &jsonStr)
{
    JsonPreferences prefs; // 创建新的 JsonPreferences 对象
    try
    {
        // 解析 JSON 字符串
        prefs.preferences = json::parse(jsonStr);
    }
    catch (const std::exception &e)
    {
        // 捕获解析异常并抛出 runtime_error
        throw std::runtime_error("Invalid JSON format");
    }
    return prefs; // 返回解析后的对象
}
/**
 * 将JsonPreferences对象转换为字符串表示形式。
 *
 * 此函数用于生成当前JsonPreferences对象的JSON字符串表示。
 * 它通过调用底层json对象的dump方法来实现，该方法负责序列化JSON对象为字符串。
 *
 * @return 返回包含JSON对象序列化结果的字符串。
 */
std::string JsonPreferences::toString() const
{
    return preferences.dump();
}
