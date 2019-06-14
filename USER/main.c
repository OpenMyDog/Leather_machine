/************************************************
 ������ó���
 �������
 �ܸ߼��ģ���
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


//�жϼ��= ��ARR+1)(PSC+1)/Tclk  Tclk=72M
#define Time_delay_arr 9
#define Time_delay_psc 7199

//���ڲ���������
#define UART1_bit 115200


/************************************************
	����������
************************************************/

void printf_Error(int Error_Code);//������ִ���ʱ���������Ϣ

int main(void)
{

	IWDG_Init(4,645);//��ʼ�����Ź���1S����
    delay_init();//��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(UART1_bit);	 //���ڳ�ʼ��Ϊ115200
	Hardware_Init();//Ӳ����ʼ��
    TIM2_Int_Init(Time_delay_arr,Time_delay_psc);//1ms����һ��	�жϼ�� = ��ARR+1)(PSC+1)/Tclk���룩  Tclk=72M
	TIM6_Int_Init(Time_delay_arr,Time_delay_psc);//1ms����һ��
    Numerical_value_Init();//��ֵ��ʼ��


    while(1)
    {
		delay_ms(400);
		IWDG_Feed();//ι��
		TM1640_Show();//�������ʾ
    }
}



void printf_Error(int Error_Code) {

    if(Error_Code) {
        switch(Error_Code) {

        case 1:
			printf("\r\nͨ��������\r\n");
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

