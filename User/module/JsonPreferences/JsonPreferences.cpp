#include "JsonPreferences.h"

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

bool JsonPreferences::isKey(const char *key)
{
    // 检查键是否存在于 JSON 对象中
    return preferences.contains(key);
}

bool JsonPreferences::clear()
{
    // 清除所有键值对
    preferences.clear();
    return true; // 返回 true 表示清除成功
}
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
std::string JsonPreferences::toString() const
{
    return preferences.dump();
}
