#include "KD024VGFPD094.h"
#include <stdarg.h>
// 读取SDO数据
void read_data(unsigned char y, unsigned char *readValue)
{
    unsigned char j;
    unsigned char data = 0;

    unsigned char i;

    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);   // 使能CSB
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 开始时钟为低
    HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);   // D/C为低，表示写命令
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET);   // 时钟上升沿

    for (i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 下降沿准备数据
        HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);

        if (y & 0x80)
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET); // 上升沿发送数据

        y = y << 1; // 准备下一个比特
    }

    // 给25个SCL时钟信号，每个时钟信号的上升沿读取SDO
    for (j = 0; j < 25; j++)
    {
        HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);

        // 产生时钟下降沿
        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET);
        // 微小延迟, 如果需要的话
        // __NOP(); 或者任何合适的延迟函数

        // 产生时钟上升沿
        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET);

        // 在时钟线的上升沿读取SDO数据
        if (HAL_GPIO_ReadPin(SDO_GPIO_Port, SDO_Pin) == GPIO_PIN_SET)
        {
            // 如果读到的是高电平，则当前位设置为1
            data |= (1 << (24 - j));
        }
        else
        {
            // 如果读到的是低电平，则当前位保持为0，这一步其实可以省略不写
            data &= ~(1 << (24 - j));
        }

        // 微小延迟, 如果需要的话
        // __NOP(); 或者任何合适的延迟函数
    }

    // 禁用片选信号
    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);

    // 将读到的数据存储到提供的地址
    *readValue = data;
}

void write_command(unsigned char y)
{
    unsigned char i;

    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);   // 使能CSB
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 开始时钟为低
    HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);   // D/C为低，表示写命令
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET);   // 时钟上升沿

    for (i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 下降沿准备数据
        if (y & 0x80)
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET); // 上升沿发送数据

        y = y << 1; // 准备下一个比特
    }

    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET); // 禁用CSB
}

// 这个函数发送数据
void write_data(unsigned char w)
{
    unsigned char i;

    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);   // 使能CSB
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 开始时钟为低
    HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);     // D/C为高，表示写数据
    HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET);   // 时钟上升沿

    for (i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_RESET); // 下降沿准备数据

        if (w & 0x80)
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(SDI_GPIO_Port, SDI_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(SCLB_GPIO_Port, SCLB_Pin, GPIO_PIN_SET); // 上升沿发送数据

        w = w << 1; // 准备下一个比特
    }

    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET); // 禁用CSB
}
void KD024VGFPD094_init(void)
{
    // res = 1;
    // HAL_GPIO_WritePin(SPI1_RST_GPIO_Port, SPI1_RST_Pin, SET);
    HAL_Delay(1);
    // res = 0;
    HAL_GPIO_WritePin(SPI1_RST_GPIO_Port, SPI1_RST_Pin, RESET);
    HAL_Delay(10);
    // res = 1;
    HAL_GPIO_WritePin(SPI1_RST_GPIO_Port, SPI1_RST_Pin, SET);
    HAL_Delay(120);
    //***************************************************************//LCD SETING
    write_command(0xFF); // PAGE3
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x13);

    write_command(0xEF);
    write_data(0x08);

    write_command(0xFF); // PAGE01
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x10);

    write_command(0xC0); // Display Line Setting
    write_data(0x4F);
    write_data(0x00);

    write_command(0xC1); // Porch Control
    write_data(0x10);
    write_data(0x0C);

    write_command(0xC2); // Inversion set
    write_data(0x01);
    write_data(0x14);

    write_command(0xCC);
    write_data(0x10);

    write_command(0xB0); // Positive Voltage Gamma Control
    write_data(0x00);
    write_data(0x0B);
    write_data(0x13);
    write_data(0x0D);
    write_data(0x10);
    write_data(0x07);
    write_data(0x02);
    write_data(0x08);
    write_data(0x07);
    write_data(0x1F);
    write_data(0x04);
    write_data(0x11);
    write_data(0x0F);
    write_data(0x28);
    write_data(0x2F);
    write_data(0x1F);

    write_command(0xB1); // Negative Voltage Gamma Control
    write_data(0x00);
    write_data(0x0C);
    write_data(0x13);
    write_data(0x0C);
    write_data(0x10);
    write_data(0x05);
    write_data(0x02);
    write_data(0x08);
    write_data(0x08);
    write_data(0x1E);
    write_data(0x05);
    write_data(0x13);
    write_data(0x11);
    write_data(0x27);
    write_data(0x30);
    write_data(0x1F);

    write_command(0xFF); // PAGE1
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x11);

    write_command(0xB0); // Vop Amplitude setting
    write_data(0x4D);    //

    write_command(0xB1); // VCOM amplitude setting
    write_data(0x3D);    //

    write_command(0xB2); // VGH Voltage setting
    write_data(0x87);

    write_command(0xB3); // TEST Command Setting
    write_data(0x80);

    write_command(0xB5); // VGL Voltage setting
    write_data(0x45);

    write_command(0xB7); // Power Control 1
    write_data(0x85);

    write_command(0xB8); // Power Control 2
    write_data(0x20);

    write_command(0xC0);
    write_data(0x09);

    write_command(0xC1); // Source pre_drive timing set1
    write_data(0x78);

    write_command(0xC2); // Source EQ2 Setting
    write_data(0x78);

    write_command(0xD0); // Source EQ2 Setting
    write_data(0x88);

    HAL_Delay(100);
    //*********GIP SET*************//
    write_command(0xE0);
    write_data(0x00);
    write_data(0x00);
    write_data(0x02);

    write_command(0xE1);
    write_data(0x04);
    write_data(0xB0);
    write_data(0x06);
    write_data(0xB0);
    write_data(0x05);
    write_data(0xB0);
    write_data(0x07);
    write_data(0xB0);
    write_data(0x00);
    write_data(0x44);
    write_data(0x44);

    write_command(0xE2);
    write_data(0x20);
    write_data(0x20);
    write_data(0x44);
    write_data(0x44);
    write_data(0x96);
    write_data(0xA0);
    write_data(0x00);
    write_data(0x00);
    write_data(0x96);
    write_data(0xA0);
    write_data(0x00);
    write_data(0x00);

    write_command(0xE3);
    write_data(0x00);
    write_data(0x00);
    write_data(0x22);
    write_data(0x22);

    write_command(0xE4);
    write_data(0x44);
    write_data(0x44);

    write_command(0xE5);
    write_data(0x0C);
    write_data(0x90);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x0E);
    write_data(0x92);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x08);
    write_data(0x8C);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x0A);
    write_data(0x8E);
    write_data(0xB0);
    write_data(0xA0);

    write_command(0xE6);
    write_data(0x00);
    write_data(0x00);
    write_data(0x22);
    write_data(0x22);

    write_command(0xE7);
    write_data(0x44);
    write_data(0x44);

    write_command(0xE8);
    write_data(0x0D);
    write_data(0x91);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x0F);
    write_data(0x93);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x09);
    write_data(0x8D);
    write_data(0xB0);
    write_data(0xA0);
    write_data(0x0B);
    write_data(0x8F);
    write_data(0xB0);
    write_data(0xA0);

    write_command(0xE9);
    write_data(0x36);
    write_data(0x00);

    write_command(0xEB);
    write_data(0x00);
    write_data(0x00);
    write_data(0xE4);
    write_data(0xE4);
    write_data(0x44);
    write_data(0x88);
    write_data(0x40);

    write_command(0xED);
    write_data(0xC1);
    write_data(0xA2);
    write_data(0xBF);
    write_data(0x0F);
    write_data(0x67);
    write_data(0x45);
    write_data(0xFF);
    write_data(0xFF);
    write_data(0xFF);
    write_data(0xFF);
    write_data(0x54);
    write_data(0x76);
    write_data(0xF0);
    write_data(0xFB);
    write_data(0x2A);
    write_data(0x1C);

    write_command(0xEF);
    write_data(0x10);
    write_data(0x0D);
    write_data(0x04);
    write_data(0x08);
    write_data(0x3F);
    write_data(0x1F);

    write_command(0xFF);
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x13);

    write_command(0xE8);
    write_data(0x00);
    write_data(0x0E);

    write_command(0xFF);
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);

    write_command(0x11);
    HAL_Delay(120);

    write_command(0xFF);
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x13);

    write_command(0xE8);
    write_data(0x00);
    write_data(0x0C);
    HAL_Delay(10);

    write_command(0xE8);
    write_data(0x00);
    write_data(0x00);

    write_command(0xFF);
    write_data(0x77);
    write_data(0x01);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);

    write_command(0x3a);
    write_data(0x70); // 24BIT

    write_command(0x29); // DISPLAY ON
    HAL_Delay(50);

    printf("LCD init done\n");
}
//*******************************************
void EnterSleep(void)
{
    write_command(0x28);
    HAL_Delay(10);
    write_command(0x10);
}

//*********************************************************
void ExitSleep(void)

{
    write_command(0x11);
    HAL_Delay(120);
    write_command(0x29);
}
void SSD_SEND(uint8_t numArgs, ...)
{
    va_list args;
    va_start(args, numArgs);

    // 选择SPI设备
    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_RESET);

    uint8_t data;
    for (uint8_t i = 0; i < numArgs; ++i)
    {
        // 获取下一个参数并发送
        data = va_arg(args, int); // Note: va_arg第二个参数是你要获取的类型

        // 发送数据
        write_data(&data);
    }

    // 取消选择SPI设备
    HAL_GPIO_WritePin(CSB_GPIO_Port, CSB_Pin, GPIO_PIN_SET);

    va_end(args);
}