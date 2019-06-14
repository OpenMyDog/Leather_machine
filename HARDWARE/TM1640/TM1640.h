#ifndef __TM1640_H
#define __TM1640_H

#include "stm32f10x.h"
#include "delay.h"

#define TM1640_CLK_HIGH 	GPIO_SetBits(GPIOB, GPIO_Pin_5)				//��TM1640��CLK����Ϊ����ߵ�ƽ
#define TM1640_CLK_LOW  	GPIO_ResetBits(GPIOB, GPIO_Pin_5)			//��TM1640��CLK����Ϊ����͵�ƽ
#define TM1640_DIN_HIGH 	GPIO_SetBits(GPIOB, GPIO_Pin_4)				//��TM1640��DIN����Ϊ����ߵ�ƽ
#define TM1640_DIN_LOW  	GPIO_ResetBits(GPIOB,GPIO_Pin_4)			//��TM1640��DIN����Ϊ����͵�ƽ

#define DATA_COMMAND_Z		0x40					//�����Զ���ַ��1ģʽ
#define DATA_COMMAND_G		0x44					//���ù̶���ַģʽ
#define ADDR_START			0xc0					//��ʼ��ַ
#define DISP_CLOSE 			0x80					//�ر���ʾ
#define DISP_OPEN			0x8f					//������ʾ
	
void TM1640_Show(void);								//�������ʾ��Ӧ����
void TM1640_Init(void);								//��ʼ��TM1640
void TM1640_Start(void);							//����俪ʼ
void TM1640_End(void);								//��������
void TM1640_Write_Byte(unsigned char dat);			//д���ֽ�
void TM1640_Display(void);							//�������ʾ

#endif
