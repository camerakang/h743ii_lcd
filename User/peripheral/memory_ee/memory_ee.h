#ifndef __MEMORY_EE__
#define __MEMORY_EE__
#include "JsonPreferences.h"
#include "ee.h"
#define JSON_SIZE 1024 // 根据实际需要定义 JSON 字符串的大小
extern EE_HandleTypeDef eeHandle;

bool InitializeEEPROM(uint32_t size);
void SaveJsonToEEPROM(const char *jsonStr);
void LoadJsonFromEEPROM(char *buffer, uint32_t bufferSize);
void SavePreferencesToEEPROM(JsonPreferences &prefs);
void LoadPreferencesFromEEPROM(JsonPreferences &prefs);
#endif // !__MEMORY_EE__