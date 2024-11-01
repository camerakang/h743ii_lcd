#include "app_entry.h"
#include "utools.h"
#include "router.h"
#include "KD024VGFPD094.h"
#include "lcd_test.h"
void log_demo()
{
    while (1)
    {
        UTDEBUG("log_demo");
        LCD_Test_Clear();      // 清屏测试
        LCD_Test_Text();       //	文本显示测试
        LCD_Test_Variable();   // 变量显示，包括整数和小数
        LCD_Test_Color();      // 颜色测试
        LCD_Test_GrahicTest(); // 2D图形绘制
        LCD_Test_FillRect();   // 矩形填充测试
        LCD_Test_Image();      // 图片显示测试
        utcollab::Task::sleep_for(100);
    }
}

void setup()
{
    // 1、初始化外设
    // TODO: 初始化外设
    KD024VGFPD094_init();
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
    router::taskpool.assign(log_demo);
    // utcollab::Task(log_demo).detach(2048);

    // 2、读取配置参数
    // TODO: 从flash中读取参数
}

void loop()
{

    // printf("hello world\n");
    // UTDEBUG("loop");
    // HAL_Delay(100);
    // utcollab::Task::sleep_for(1000);
}
