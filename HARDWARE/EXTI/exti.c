#include "exti.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "stdbool.h"
#include "pqj.h"
#include "key.h"
#include "TM1640.h"

extern int time6_Num;
extern int time6_lsx;
extern int pp_speed;
extern int pjj_speed;
extern int pp_time_Num[6] ;
extern int angle_delay;
extern int show_switch;

extern int pjj_R;
extern int smk_timer;
extern int smk_timer_Num;
extern double smk_long;

double pp_speed_delay = 1.05;//����У׼

double pjj_speed_delay = 1.1;//��ˮ���ٶ�У׼

//�ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
	int jsd;//��ˮ�����̽��ٶ�

    if(EXTI_GetITStatus(EXTI_Line0)) {//�ж��ж�0�Ƿ���
		
        delay_ms(3);//����
		
        if((int)liushuixian)
        {
			
            //��ˮ���ٶ����������
            jsd = 360.0/(double)time6_lsx*1000.0;//ͨ��ʱ�������ٶ�
			pjj_speed = 2 * 3.14 * pjj_R * (1.0/360.0*jsd);//���ٶ�ת���ٶ�
			
			pjj_speed *= pjj_speed_delay;
			
			smk_timer = 1 /( pjj_speed / smk_long) / (0.001);//0.001���жϼ��
			smk_timer_Num = smk_timer;//���üƴ�ʱ��
			
		    printf("pjj_speed:%d mm/s,time:%d ms;\r\n",pjj_speed,smk_timer_Num);

            time6_lsx=0;
			
			//GPIO_Write(GPIOF,(0));//��������LED��

        }
		EXTI_ClearITPendingBit(EXTI_Line0);  //���LINE0�ϵ��жϱ�־λ
    }
}


void EXTI1_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line1)) {//�ж��ж�4�Ƿ���
        delay_ms(5);//����		
		
        if((int)KEY1){//�л�����Ƥ�ٶ�
			printf("����1һ��\r\n");
			show_switch++;
        }
		
		EXTI_ClearITPendingBit(EXTI_Line1);  //���LINE1�ϵ��жϱ�־λ
    }
}

void EXTI2_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line2)) {//�ж��ж�2�Ƿ���
        delay_ms(5);//����
		
        if((int)KEY2){
			printf("����2һ��\r\n");
			show_switch++;
        }
		EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ
    }
}


void EXTI3_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line3)) {//�ж��ж�3�Ƿ���
        delay_ms(5);//����
		
        if((int)KEY3){
			printf("����3һ��\r\n");
			show_switch++;
        }
		EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ
    }
}

//ת���жϷ�����
void EXTI4_IRQHandler(void)
{
	
    int i;

    if(EXTI_GetITStatus(EXTI_Line4)) {//�ж��ж�4�Ƿ���
        delay_ms(3);//����
		
        if((int)kaiguan)
        {

            //ת���ٶ����������
            pp_speed = 360.0/(double)time6_Num*1000.0;
			
			pp_speed *=pp_speed_delay;//����У׼

            for(i=0; i<pq_Number; i++) {
                pp_time_Num[i] = (60 * i + angle_delay)/ (double)pp_speed / 0.001;//0.001��ʱ��Ƶ��
            }
			
			printf("pp_speed:%d\r\n",pp_speed);
            time6_Num=0;

        }
		EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ
    }
}



void EXTIX_Init(void)
{

    //Ex_NVIC_Config(GPIO_D,4,FTIR);//PG2,�½��ش���
    //MY_NVIC_Init(2,3,EXTI4_IRQn,2);	//��ռ2�������ȼ�3����2


    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

	//�ӽ��������ж�
    //GPIOE.2 �ж����Լ��жϳ�ʼ������   ���ش�����������PD4���жϴ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource4);
	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//GM�˿������жϣ������ؾ�����
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);  	  
	
	
	
	//����1��
	//GPIOA.1 �ж����Լ��жϳ�ʼ������   ���ش�����������PA1���жϴ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//�����˿������жϣ������ش���
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	  
	  
	  
	//����2��
	//GPIOA.2 �ж����Լ��жϳ�ʼ������   ���ش�����������PA2���жϴ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;	//�����˿������жϣ������ش���
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	  
	
	
	//����3��
	//GPIOA.3 �ж����Լ��жϳ�ʼ������   ���ش�����������PA3���жϴ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line3;	//�����˿������жϣ������ش���
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	


	//��ˮ��������
	//GPIOA.0 �ж����Լ��жϳ�ʼ������   ���ش�����������PA0���жϴ���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//��ˮ�����������жϣ������ش���
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  
	
}

