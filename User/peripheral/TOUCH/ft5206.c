#include "ft5206.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
#include "gt9147.h"
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32开发板
// 7寸电容触摸屏-FT5206 驱动代码
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2015/12/28
// 版本：V1.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

// 向FT5206写入一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:写数据长度
// 返回值:0,成功;1,失败.
u8 FT5206_WR_Reg(u16 reg, u8 *buf, u8 len)
{
    u8 i;
    u8 ret = 0;
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_WR); // 发送写命令
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg & 0XFF); // 发送低8位地址
    CT_IIC_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        CT_IIC_Send_Byte(buf[i]); // 发数据
        ret = CT_IIC_Wait_Ack();
        if (ret)
            break;
    }
    CT_IIC_Stop(); // 产生一个停止条件
    return ret;
}
// 从FT5206读出一次数据
// reg:起始寄存器地址
// buf:数据缓缓存区
// len:读数据长度
void FT5206_RD_Reg(u16 reg, u8 *buf, u8 len)
{
    u8 i;
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_WR); // 发送写命令
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg & 0XFF); // 发送低8位地址
    CT_IIC_Wait_Ack();
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_RD); // 发送读命令
    CT_IIC_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        buf[i] = CT_IIC_Read_Byte(i == (len - 1) ? 0 : 1); // 发数据
    }
    CT_IIC_Stop(); // 产生一个停止条件
}
u8 CIP[5]; // 用来存放触摸IC-GT911
// 初始化FT5206触摸屏
// 返回值:0,初始化成功;1,初始化失败
u8 FT5206_Init(void)
{
    u8 temp[2];
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE(); // 开启GPIOH时钟
    __HAL_RCC_GPIOI_CLK_ENABLE(); // 开启GPIOI时钟

    // PE3=INT
    GPIO_Initure.Pin = GPIO_PIN_3;                  // PE3
    GPIO_Initure.Mode = GPIO_MODE_INPUT;            // 输入
    GPIO_Initure.Pull = GPIO_PULLUP;                // 上拉
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 高速
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);            // 初始化

    // PE2=RST
    GPIO_Initure.Pin = GPIO_PIN_2;           // PE2
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; // 推挽输出
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);     // 初始化

    CT_IIC_Init(); // 初始化电容屏的I2C总线
    FT_RST(0);     // 复位
    delay_ms(20);
    FT_RST(1); // 释放复位
    delay_ms(50);

    temp[0] = 0;
    FT5206_WR_Reg(FT_DEVIDE_MODE, temp, 1); // 进入正常操作模式
    FT5206_WR_Reg(FT_ID_G_MODE, temp, 1);   // 查询模式

    return 0;
}
const u16 FT5206_TPX_TBL[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
// GT911属于GT9xx系列，所以直接调用gt9147的相关宏定义和调用相关函数
const u16 GT911_TPX_TBL[5] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};
u8 g_gt_tnum = 5; // 默认支持的触摸屏点数(5点触摸)
// 扫描触摸屏(采用查询方式)
// mode:0,正常扫描.
// 返回值:当前触屏状态.
// 0,触屏无触摸;1,触屏有触摸
u8 FT5206_Scan(u8 mode)
{
    u8 buf[4];
    u8 i = 0;
    u8 res = 0;
    u8 temp;
    static u8 t = 0; // 控制查询间隔,从而降低CPU占用率
    t++;
    if ((t % 10) == 0 || t < 10) // 空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
    {
        FT5206_RD_Reg(FT_REG_NUM_FINGER, &mode, 1); // 读取触摸点的状态
        if ((mode & 0XF) && ((mode & 0XF) < 6))
        {
            temp = 0XFF << (mode & 0XF); // 将点的个数转换为1的位数,匹配tp_dev.sta定义
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            for (i = 0; i < 5; i++)
            {
                if (tp_dev.sta & (1 << i)) // 触摸有效?
                {
                    FT5206_RD_Reg(FT5206_TPX_TBL[i], &buf[0], 4); // 读取XY坐标值
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x01,&buf[1],1);	//读取XY坐标值
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x02,&buf[2],1);	//读取XY坐标值
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x03,&buf[3],1);	//读取XY坐标值

                    tp_dev.x[i] = ((u16)(buf[0] & 0X0F) << 8) + buf[1]; // X坐标
                    tp_dev.y[i] = ((u16)(buf[2] & 0X0F) << 8) + buf[3]; // Y坐标

                    printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
                }
            }
            res = 1;
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0)
                mode = 0; // 读到的数据都是0,则忽略此次数据
            t = 0;        // 触发一次,则会最少连续监测10次,从而提高命中率
        }
    }
    if ((mode & 0X1F) == 0) // 无触摸点按下
    {
        if (tp_dev.sta & TP_PRES_DOWN) // 之前是被按下的
        {
            tp_dev.sta &= ~(1 << 7); // 标记按键松开
        }
        else // 之前就没有被按下
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE0; // 清除点有效标记
        }
    }
    if (t > 240)
        t = 10; // 重新从10开始计数
    return res;
}

// 通过INT中断扫描触摸函数，在按键播放模式下被调用,只组合第一点坐标
void FT5206_Scan_point(u8 mode)
{
    u8 buf[4];
    u8 i = 0;
    u8 int_bit;

    if (FT_INT == 0)
        int_bit = 1; // 有触摸按下INT为低电平
    else
        int_bit = 0;
    if (int_bit == 1)
    {
        FT5206_RD_Reg(FT_REG_NUM_FINGER, &mode, 1); // 读取触摸点的状态
        if ((mode & 0XF) && ((mode & 0XF) < 6))
        {
            FT5206_RD_Reg(FT5206_TPX_TBL[0], &buf[0], 1);        // 读取XY坐标值
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x01, &buf[1], 1); // 读取XY坐标值
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x02, &buf[2], 1); // 读取XY坐标值
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x03, &buf[3], 1); // 读取XY坐标值

            tp_dev.x[1]++;       // 计数变量不能用该函数里面的变量，否则会导致进来此函数变量就会为0从而导致不会执行组合坐标
            if (tp_dev.x[1] > 4) // 手指按一下会进来很多次，为了防止误报，进来5次才算一次
            {
                tp_dev.x[1] = 0;
                tp_dev.x[i] = ((u16)(buf[0] & 0X0F) << 8) + buf[1]; // X1坐标
                tp_dev.y[i] = ((u16)(buf[2] & 0X0F) << 8) + buf[3]; // Y1坐标

                printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
            }
            else
            {
                tp_dev.x[0] = 0xFFFF;
                tp_dev.y[0] = 0xFFFF;
            }
        }
    }

    if ((mode & 0X1F) == 0) // 无触摸点按下
    {
        tp_dev.x[0] = 0xFFFF;
        tp_dev.y[0] = 0xFFFF;
    }
}
