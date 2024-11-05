#include "ft5206.h"
#include "touch.h"
#include "ctiic.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
#include "gt9147.h"
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32������
// 7����ݴ�����-FT5206 ��������
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// ��������:2015/12/28
// �汾��V1.0
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

// ��FT5206д��һ������
// reg:��ʼ�Ĵ�����ַ
// buf:���ݻ�������
// len:д���ݳ���
// ����ֵ:0,�ɹ�;1,ʧ��.
u8 FT5206_WR_Reg(u16 reg, u8 *buf, u8 len)
{
    u8 i;
    u8 ret = 0;
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_WR); // ����д����
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg & 0XFF); // ���͵�8λ��ַ
    CT_IIC_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        CT_IIC_Send_Byte(buf[i]); // ������
        ret = CT_IIC_Wait_Ack();
        if (ret)
            break;
    }
    CT_IIC_Stop(); // ����һ��ֹͣ����
    return ret;
}
// ��FT5206����һ������
// reg:��ʼ�Ĵ�����ַ
// buf:���ݻ�������
// len:�����ݳ���
void FT5206_RD_Reg(u16 reg, u8 *buf, u8 len)
{
    u8 i;
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_WR); // ����д����
    CT_IIC_Wait_Ack();
    CT_IIC_Send_Byte(reg & 0XFF); // ���͵�8λ��ַ
    CT_IIC_Wait_Ack();
    CT_IIC_Start();
    CT_IIC_Send_Byte(FT_CMD_RD); // ���Ͷ�����
    CT_IIC_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        buf[i] = CT_IIC_Read_Byte(i == (len - 1) ? 0 : 1); // ������
    }
    CT_IIC_Stop(); // ����һ��ֹͣ����
}
u8 CIP[5]; // ������Ŵ���IC-GT911
// ��ʼ��FT5206������
// ����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
u8 FT5206_Init(void)
{
    u8 temp[2];
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE(); // ����GPIOHʱ��
    __HAL_RCC_GPIOI_CLK_ENABLE(); // ����GPIOIʱ��

    // PE3=INT
    GPIO_Initure.Pin = GPIO_PIN_3;                  // PE3
    GPIO_Initure.Mode = GPIO_MODE_INPUT;            // ����
    GPIO_Initure.Pull = GPIO_PULLUP;                // ����
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // ����
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);            // ��ʼ��

    // PE2=RST
    GPIO_Initure.Pin = GPIO_PIN_2;           // PE2
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; // �������
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);     // ��ʼ��

    CT_IIC_Init(); // ��ʼ����������I2C����
    FT_RST(0);     // ��λ
    delay_ms(20);
    FT_RST(1); // �ͷŸ�λ
    delay_ms(50);

    temp[0] = 0;
    FT5206_WR_Reg(FT_DEVIDE_MODE, temp, 1); // ������������ģʽ
    FT5206_WR_Reg(FT_ID_G_MODE, temp, 1);   // ��ѯģʽ

    return 0;
}
const u16 FT5206_TPX_TBL[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
// GT911����GT9xxϵ�У�����ֱ�ӵ���gt9147����غ궨��͵�����غ���
const u16 GT911_TPX_TBL[5] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};
u8 g_gt_tnum = 5; // Ĭ��֧�ֵĴ���������(5�㴥��)
// ɨ�败����(���ò�ѯ��ʽ)
// mode:0,����ɨ��.
// ����ֵ:��ǰ����״̬.
// 0,�����޴���;1,�����д���
u8 FT5206_Scan(u8 mode)
{
    u8 buf[4];
    u8 i = 0;
    u8 res = 0;
    u8 temp;
    static u8 t = 0; // ���Ʋ�ѯ���,�Ӷ�����CPUռ����
    t++;
    if ((t % 10) == 0 || t < 10) // ����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
    {
        FT5206_RD_Reg(FT_REG_NUM_FINGER, &mode, 1); // ��ȡ�������״̬
        if ((mode & 0XF) && ((mode & 0XF) < 6))
        {
            temp = 0XFF << (mode & 0XF); // ����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta����
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            for (i = 0; i < 5; i++)
            {
                if (tp_dev.sta & (1 << i)) // ������Ч?
                {
                    FT5206_RD_Reg(FT5206_TPX_TBL[i], &buf[0], 4); // ��ȡXY����ֵ
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x01,&buf[1],1);	//��ȡXY����ֵ
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x02,&buf[2],1);	//��ȡXY����ֵ
                    //					FT5206_RD_Reg(FT5206_TPX_TBL[i]+0x03,&buf[3],1);	//��ȡXY����ֵ

                    tp_dev.x[i] = ((u16)(buf[0] & 0X0F) << 8) + buf[1]; // X����
                    tp_dev.y[i] = ((u16)(buf[2] & 0X0F) << 8) + buf[3]; // Y����

                    printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
                }
            }
            res = 1;
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0)
                mode = 0; // ���������ݶ���0,����Դ˴�����
            t = 0;        // ����һ��,��������������10��,�Ӷ����������
        }
    }
    if ((mode & 0X1F) == 0) // �޴����㰴��
    {
        if (tp_dev.sta & TP_PRES_DOWN) // ֮ǰ�Ǳ����µ�
        {
            tp_dev.sta &= ~(1 << 7); // ��ǰ����ɿ�
        }
        else // ֮ǰ��û�б�����
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE0; // �������Ч���
        }
    }
    if (t > 240)
        t = 10; // ���´�10��ʼ����
    return res;
}

// ͨ��INT�ж�ɨ�败���������ڰ�������ģʽ�±�����,ֻ��ϵ�һ������
void FT5206_Scan_point(u8 mode)
{
    u8 buf[4];
    u8 i = 0;
    u8 int_bit;

    if (FT_INT == 0)
        int_bit = 1; // �д�������INTΪ�͵�ƽ
    else
        int_bit = 0;
    if (int_bit == 1)
    {
        FT5206_RD_Reg(FT_REG_NUM_FINGER, &mode, 1); // ��ȡ�������״̬
        if ((mode & 0XF) && ((mode & 0XF) < 6))
        {
            FT5206_RD_Reg(FT5206_TPX_TBL[0], &buf[0], 1);        // ��ȡXY����ֵ
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x01, &buf[1], 1); // ��ȡXY����ֵ
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x02, &buf[2], 1); // ��ȡXY����ֵ
            FT5206_RD_Reg(FT5206_TPX_TBL[0] + 0x03, &buf[3], 1); // ��ȡXY����ֵ

            tp_dev.x[1]++;       // �������������øú�������ı���������ᵼ�½����˺��������ͻ�Ϊ0�Ӷ����²���ִ���������
            if (tp_dev.x[1] > 4) // ��ָ��һ�»�����ܶ�Σ�Ϊ�˷�ֹ�󱨣�����5�β���һ��
            {
                tp_dev.x[1] = 0;
                tp_dev.x[i] = ((u16)(buf[0] & 0X0F) << 8) + buf[1]; // X1����
                tp_dev.y[i] = ((u16)(buf[2] & 0X0F) << 8) + buf[3]; // Y1����

                printf("x[%d]:%d,y[%d]:%d\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
            }
            else
            {
                tp_dev.x[0] = 0xFFFF;
                tp_dev.y[0] = 0xFFFF;
            }
        }
    }

    if ((mode & 0X1F) == 0) // �޴����㰴��
    {
        tp_dev.x[0] = 0xFFFF;
        tp_dev.y[0] = 0xFFFF;
    }
}
