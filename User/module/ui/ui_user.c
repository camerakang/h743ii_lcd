#include "ui_user.h"
#include "KD024VGFPD094.h"
#include "lcd_test.h"
#include "ui.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "sdram_user.h"
#include "fmc.h"
#include "ui_user_animation.h"
void ui_display()
{
    // SDRAM_Initialization_Sequence(&hsdram1, &command);// 控制指令
    KD024VGFPD094_init();
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    ui_init();
    create_slider_animation(ui_Screen1_Slider1);
    create_slider_animation(ui_Screen1_Slider2);
    create_slider_animation(ui_Screen1_Slider3);
    create_label_animation(ui_Screen1_Label8);
    create_label_animation(ui_Screen1_Label7);
    create_label_animation(ui_Screen1_Label4);
    // while (1)
    // {
    //     lv_task_handler();
    // }
}
