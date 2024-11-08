#include "memory_ee.h"
#include "NimaLTD.I-CUBE-EE_conf.h"
#include <string.h>
#include <stdio.h>
#include <stdexcept>
#include <cstdio>

bool InitializeEEPROM(uint32_t size)
{
    static char eepromData[JSON_SIZE]; // 用于 EEPROM 存储的静态缓冲区
    if (EE_Init(eepromData, size))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void SaveJsonToEEPROM(const char *jsonStr)
{
    size_t len = strlen(jsonStr) + 1; // 包括终止符
    if (len <= JSON_SIZE)
    {
        // 将 JSON 字符串写入 EEPROM
        memcpy(eeHandle.pData, jsonStr, len);
        if (!EE_Write())
        {
            printf("Failed to write JSON to EEPROM\n");
        }
    }
    else
    {
        printf("JSON string too large for EEPROM\n");
    }
}

void LoadJsonFromEEPROM(char *buffer, uint32_t bufferSize)
{
    if (buffer != NULL && bufferSize >= JSON_SIZE)
    {
        EE_Read();                                 // 从 EEPROM 读取数据
        memcpy(buffer, eeHandle.pData, JSON_SIZE); // 复制到输出缓冲区
    }
    else
    {
        printf("Invalid buffer or buffer size\n");
    }
}

void SavePreferencesToEEPROM(JsonPreferences &prefs)
{
    // 将 JsonPreferences 对象转换为 JSON 字符串
    std::string jsonStr = prefs.toString();

    // 调用现有的函数保存到 EEPROM
    SaveJsonToEEPROM(jsonStr.c_str());
}

void LoadPreferencesFromEEPROM(JsonPreferences &prefs)
{
    char buffer[JSON_SIZE]; // 定义用于存储从 EEPROM 读取的数据的缓冲区

    // 从 EEPROM 读取 JSON 字符串
    LoadJsonFromEEPROM(buffer, sizeof(buffer));

    try
    {
        // 使用 fromString 方法解析缓冲区中的 JSON 字符串并更新偏好设置
        prefs = JsonPreferences::fromString(buffer);

        // 输出 JSON 对象的字符串表示形式
        printf("Loaded preferences: %s\n", prefs.toString().c_str());
    }
    catch (const std::runtime_error &e)
    {
        fprintf(stderr, "Failed to load preferences: %s\n", e.what());
    }
}