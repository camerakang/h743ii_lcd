#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "ui_user.h"
#include "ft5206.h"
#include "lvgl.h"
#include "memory_ee.h"
#include "sys_config.h"

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
}

void loop()
{
    lv_task_handler();
}
