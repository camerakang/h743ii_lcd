#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "ui_user.h"
#include "ft5206.h"
#include "lvgl.h"
#include "memory_ee.h"
#include "JsonPreferences.h"
// JSON 字符串缓冲区
JsonPreferences custom_config;

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

    custom_config.putChar("putChar", 1);
    custom_config.putString("putString", "hello");
    custom_config.putInt("putInt", 2);
    custom_config.putLong("putLong", 3);
    printf("%s\n", custom_config.toString().c_str());
    LoadPreferencesFromEEPROM(custom_config);
    custom_config.putChar("putchar1", 3);
    SavePreferencesToEEPROM(custom_config);
}

void loop()
{
    lv_task_handler();
}
