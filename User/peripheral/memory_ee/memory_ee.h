#ifndef __MEMORY_EE__
#define __MEMORY_EE__

#include "ee.h"
#define JSON_SIZE 1024  // 根据实际需要定义 JSON 字符串的大小
bool InitializeEEPROM(uint32_t size);
void SaveJsonToEEPROM(const char* jsonStr);
void LoadJsonFromEEPROM(char* buffer, uint32_t bufferSize);
#endif // !__MEMORY_EE__