#ifndef __EXTI_H
#define __EXTI_H
#include "sys.h"

#define kaiguan GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)//��ȡ���̽ӽ������ź�;
#define liushuixian GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ��ˮ�߽ӽ������ź�;


void EXTIX_Init(void);


#endif

