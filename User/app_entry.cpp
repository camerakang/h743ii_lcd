#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "ui_user.h"

void setup()
{
    // 1、初始化外设
    // TODO: 初始化外设

    router::taskpool.assign(ui_display);
    // utcollab::Task(log_demo).detach(2048);

    // 2、读取配置参数
    // TODO: 从flash中读取参数
}

void loop()
{

    utcollab::Task::sleep_for(1);
}
