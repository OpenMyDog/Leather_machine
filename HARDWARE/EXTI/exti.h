#ifndef __EXTI_H
#define __EXTI_H
#include "sys.h"

#define kaiguan GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4)//读取喷盘接近开关信号;
#define liushuixian GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取流水线接近开关信号;


void EXTIX_Init(void);


#endif

