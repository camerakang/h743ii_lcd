#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "ui_user.h"
#include "ft5206.h"
void touch_sacn()
{
    FT5206_Init();
    printf("touch_sacn\n");

    while (1)
    {

        FT5206_Scan(0);
        utcollab::Task::sleep_for(1);
    }
}
void setup()
{
    // 1、初始化外设
    // TODO: 初始化外设
    router::taskpool.assign(touch_sacn);
    router::taskpool.assign(ui_display);
    // utcollab::Task(log_demo).detach(2048);

    // 2、读取配置参数
    // TODO: 从flash中读取参数
}

void loop()
{

    utcollab::Task::sleep_for(1);
}
