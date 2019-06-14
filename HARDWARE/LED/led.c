#include "led.h"
#include "pqj.h"

struct body LED_demo[14];

//ɨ���� ͨ����
extern const int smq_Number;

void LED_Init(void) {//����������˿ڳ�ʼ��

	int i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PF�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz

	for(i=0;i<smq_Number;i++){
		GPIO_InitStructure.GPIO_Pin = LED_demo[i].GPIO_Pin;
		GPIO_Init(LED_demo[i].GPIOx, &GPIO_InitStructure);
		GPIO_ResetBits(LED_demo[i].GPIOx,LED_demo[i].GPIO_Pin);//����ߵ�ƽ
	}
	
	//while(1){}
}
 
