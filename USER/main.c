/************************************************
 喷漆机用程序
 别搞事情
 很高级的！！
************************************************/


#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "stdbool.h"
#include "pqj.h"
#include "wdg.h"
#include "stdio.h"
#include "TM1640.h"
#include "stdlib.h"


//中断间隔= （ARR+1)(PSC+1)/Tclk  Tclk=72M
#define Time_delay_arr 9
#define Time_delay_psc 7199

//串口波特率设置
#define UART1_bit 115200


/************************************************
	函数申明区
************************************************/

void printf_Error(int Error_Code);//程序出现错误时输出错误信息

int main(void)
{

	IWDG_Init(4,645);//初始化看门狗，1S周期
    delay_init();//延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(UART1_bit);	 //串口初始化为115200
	Hardware_Init();//硬件初始化
    TIM2_Int_Init(Time_delay_arr,Time_delay_psc);//1ms触发一次	中断间隔 = （ARR+1)(PSC+1)/Tclk（秒）  Tclk=72M
	TIM6_Int_Init(Time_delay_arr,Time_delay_psc);//1ms触发一次
    Numerical_value_Init();//数值初始化


    while(1)
    {
		delay_ms(400);
		IWDG_Feed();//喂狗
		TM1640_Show();//数码管显示
    }
}



void printf_Error(int Error_Code) {

    if(Error_Code) {
        switch(Error_Code) {

        case 1:
			printf("\r\n通道数出错\r\n");
            break;

        case 2:

            break;

        case 3:

            break;

        }

        printf("\r\n--------------Error output: %d--------------\r\n",Error_Code);
		while(1);
    } else ;
	
	
	
}

