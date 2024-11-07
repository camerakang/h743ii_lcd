#include "memory_ee.h"
#include "NimaLTD.I-CUBE-EE_conf.h"
#include <string.h>
#include <stdio.h>


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