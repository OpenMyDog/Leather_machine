#include "led.h"
#include "pqj.h"

struct body LED_demo[14];

//扫描区 通道数
extern const int smq_Number;

void LED_Init(void) {//红外用输入端口初始化

	int i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //使能PF端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz

	for(i=0;i<smq_Number;i++){
		GPIO_InitStructure.GPIO_Pin = LED_demo[i].GPIO_Pin;
		GPIO_Init(LED_demo[i].GPIOx, &GPIO_InitStructure);
		GPIO_ResetBits(LED_demo[i].GPIOx,LED_demo[i].GPIO_Pin);//输出高电平
	}
	
	//while(1){}
}
 
