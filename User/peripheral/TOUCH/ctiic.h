#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32������
// ���ݴ�����-IIC ��������
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// ��������:2015/12/30
// �汾��V1.0
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//********************************************************************************
// ��
//////////////////////////////////////////////////////////////////////////////////

// IO��������
#define CT_SDA_IN()                      \
    {                                    \
        GPIOB->MODER &= ~(3 << (9 * 2)); \
        GPIOB->MODER |= (0 << (9 * 2));  \
    } // PB9 ����ģʽ
#define CT_SDA_OUT()                     \
    {                                    \
        GPIOB->MODER &= ~(3 << (9 * 2)); \
        GPIOB->MODER |= (1 << (9 * 2));  \
    } // PB9 ���ģʽ
// IO��������
#define CT_IIC_SCL(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)) // SCL
#define CT_IIC_SDA(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)) // SDA
#define CT_READ_SDA HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)                                                                               // ����SDA

// IIC���в�������
void CT_IIC_Init(void);                 // ��ʼ��IIC��IO��
void CT_IIC_Start(void);                // ����IIC��ʼ�ź�
void CT_IIC_Stop(void);                 // ����IICֹͣ�ź�
void CT_IIC_Send_Byte(u8 txd);          // IIC����һ���ֽ�
u8 CT_IIC_Read_Byte(unsigned char ack); // IIC��ȡһ���ֽ�
u8 CT_IIC_Wait_Ack(void);               // IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);                  // IIC����ACK�ź�
void CT_IIC_NAck(void);                 // IIC������ACK�ź�

#endif
