#ifndef _JSON_PREFERENCES_H_
#define _JSON_PREFERENCES_H_

#include "json.hpp"
#include <string>

using json = nlohmann::json;

class JsonPreferences
{
public:
    JsonPreferences();
    ~JsonPreferences();

    size_t putChar(const char *key, int8_t value);
    size_t putUChar(const char *key, uint8_t value);
    size_t putShort(const char *key, int16_t value);
    size_t putUShort(const char *key, uint16_t value);
    size_t putInt(const char *key, int32_t value);
    size_t putUInt(const char *key, uint32_t value);
    size_t putLong(const char *key, int32_t value);
    size_t putULong(const char *key, uint32_t value);
    size_t putLong64(const char *key, int64_t value);
    size_t putULong64(const char *key, uint64_t value);
    size_t putFloat(const char *key, float value);
    size_t putDouble(const char *key, double value);
    size_t putBool(const char *key, bool value);
    size_t putString(const char *key, const std::string &value);

    int8_t getChar(const char *key, int8_t defaultValue = 0);
    uint8_t getUChar(const char *key, uint8_t defaultValue = 0);
    int16_t getShort(const char *key, int16_t defaultValue = 0);
    uint16_t getUShort(const char *key, uint16_t defaultValue = 0);
    int32_t getInt(const char *key, int32_t defaultValue = 0);
    uint32_t getUInt(const char *key, uint32_t defaultValue = 0);
    int32_t getLong(const char *key, int32_t defaultValue = 0);
    uint32_t getULong(const char *key, uint32_t defaultValue = 0);
    int64_t getLong64(const char *key, int64_t defaultValue = 0);
    uint64_t getULong64(const char *key, uint64_t defaultValue = 0);
    float getFloat(const char *key, float defaultValue = NAN);
    double getDouble(const char *key, double defaultValue = NAN);
    bool getBool(const char *key, bool defaultValue = false);
    std::string getString(const char *key, const std::string &defaultValue = "");
    bool JsonPreferences::isKey(const char *key);
    bool clear();                 // 清除所有键值对
    bool remove(const char *key); // 删除指定键
    std::string toString() const;

private:
    json preferences; // JSON 对象存储数据
};

#endif // _JSON_PREFERENCES_H_
