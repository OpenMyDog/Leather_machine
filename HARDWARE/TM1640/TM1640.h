#ifndef __TM1640_H
#define __TM1640_H

#include "stm32f10x.h"
#include "delay.h"

#define TM1640_CLK_HIGH 	GPIO_SetBits(GPIOB, GPIO_Pin_5)				//将TM1640的CLK设置为输出高电平
#define TM1640_CLK_LOW  	GPIO_ResetBits(GPIOB, GPIO_Pin_5)			//将TM1640的CLK设置为输出低电平
#define TM1640_DIN_HIGH 	GPIO_SetBits(GPIOB, GPIO_Pin_4)				//将TM1640的DIN设置为输出高电平
#define TM1640_DIN_LOW  	GPIO_ResetBits(GPIOB,GPIO_Pin_4)			//将TM1640的DIN设置为输出低电平

#define DATA_COMMAND_Z		0x40					//采用自动地址加1模式
#define DATA_COMMAND_G		0x44					//采用固定地址模式
#define ADDR_START			0xc0					//开始地址
#define DISP_CLOSE 			0x80					//关闭显示
#define DISP_OPEN			0x8f					//开启显示
	
void TM1640_Show(void);								//数码管显示相应内容
void TM1640_Init(void);								//初始化TM1640
void TM1640_Start(void);							//命令传输开始
void TM1640_End(void);								//命令传输结束
void TM1640_Write_Byte(unsigned char dat);			//写入字节
void TM1640_Display(void);							//数码管显示

#endif
