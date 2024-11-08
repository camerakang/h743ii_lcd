#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "ui_user.h"
#include "ft5206.h"
#include "lvgl.h"
#include "memory_ee.h"
#include "sys_config.h"
#include "JsonPreferences.h"
// JSON 字符串缓冲区
char jsonBuffer[JSON_SIZE];
void setup()
{
    // 1、初始化外设
    // TODO: 初始化外设
    // router::taskpool.assign(ui_display);
    // utcollab::Task(log_demo).detach(2048);
    ui_display();

    // 2、读取配置参数
    // TODO: 从flash中读取参数
    // EEPROM 初始化
    if (!InitializeEEPROM(JSON_SIZE))
    {
        printf("EEPROM initialization failed\n");
    }
    
    // 保存 JSON 到 EEPROM
    SaveJsonToEEPROM(custom_config_string.c_str());

    // 从 EEPROM 加载 JSON 字符串
    LoadJsonFromEEPROM(jsonBuffer, sizeof(jsonBuffer));
    printf("Loaded JSON: %s\n", jsonBuffer);
    custom_config = json::parse(jsonBuffer);

    JsonPreferences custom_config_1;
    custom_config_1.putChar("putChar", 1);
    custom_config_1.putString("putString", "hello");
    custom_config_1.putInt("putInt", 2);
    custom_config_1.putLong("putLong", 3);
    printf("%s\n", custom_config_1.toString().c_str());
}

void loop()
{
    lv_task_handler();
}
