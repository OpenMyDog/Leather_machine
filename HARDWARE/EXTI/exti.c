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

double pp_speed_delay = 1.05;//喷射校准

double pjj_speed_delay = 1.1;//流水线速度校准

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	int jsd;//流水线码盘角速度

    if(EXTI_GetITStatus(EXTI_Line0)) {//判断中断0是否发生
		
        delay_ms(3);//消抖
		
        if((int)liushuixian)
        {
			
            //流水线速度在这里计算
            jsd = 360.0/(double)time6_lsx*1000.0;//通过时间计算角速度
			pjj_speed = 2 * 3.14 * pjj_R * (1.0/360.0*jsd);//角速度转线速度
			
			pjj_speed *= pjj_speed_delay;
			
			smk_timer = 1 /( pjj_speed / smk_long) / (0.001);//0.001是中断间隔
			smk_timer_Num = smk_timer;//重置计次时间
			
		    printf("pjj_speed:%d mm/s,time:%d ms;\r\n",pjj_speed,smk_timer_Num);

            time6_lsx=0;
			
			//GPIO_Write(GPIOF,(0));//控制所有LED亮

        }
		EXTI_ClearITPendingBit(EXTI_Line0);  //清除LINE0上的中断标志位
    }
}


void EXTI1_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line1)) {//判断中断4是否发生
        delay_ms(5);//消抖		
		
        if((int)KEY1){//切换至行皮速度
			printf("按了1一下\r\n");
			show_switch++;
        }
		
		EXTI_ClearITPendingBit(EXTI_Line1);  //清除LINE1上的中断标志位
    }
}

void EXTI2_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line2)) {//判断中断2是否发生
        delay_ms(5);//消抖
		
        if((int)KEY2){
			printf("按了2一下\r\n");
			show_switch++;
        }
		EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位
    }
}


void EXTI3_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line3)) {//判断中断3是否发生
        delay_ms(5);//消抖
		
        if((int)KEY3){
			printf("按了3一下\r\n");
			show_switch++;
        }
		EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位
    }
}

//转盘中断服务函数
void EXTI4_IRQHandler(void)
{
	
    int i;

    if(EXTI_GetITStatus(EXTI_Line4)) {//判断中断4是否发生
        delay_ms(3);//消抖
		
        if((int)kaiguan)
        {

            //转盘速度在这里计算
            pp_speed = 360.0/(double)time6_Num*1000.0;
			
			pp_speed *=pp_speed_delay;//喷射校准

            for(i=0; i<pq_Number; i++) {
                pp_time_Num[i] = (60 * i + angle_delay)/ (double)pp_speed / 0.001;//0.001是时钟频率
            }
			
			printf("pp_speed:%d\r\n",pp_speed);
            time6_Num=0;

        }
		EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位
    }
}



void EXTIX_Init(void)
{

    //Ex_NVIC_Config(GPIO_D,4,FTIR);//PG2,下降沿触发
    //MY_NVIC_Init(2,3,EXTI4_IRQn,2);	//抢占2，子优先级3，组2


    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//接近开关用中断
    //GPIOE.2 中断线以及中断初始化配置   边沿触发，仅接受PD4的中断触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource4);
	
	
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;	//GM端口输入中断，上下沿均触发
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);  	  
	
	
	
	//按键1用
	//GPIOA.1 中断线以及中断初始化配置   边沿触发，仅接受PA1的中断触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//按键端口输入中断，上升沿触发
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	  
	  
	  
	//按键2用
	//GPIOA.2 中断线以及中断初始化配置   边沿触发，仅接受PA2的中断触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;	//按键端口输入中断，上升沿触发
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	  
	
	
	//按键3用
	//GPIOA.3 中断线以及中断初始化配置   边沿触发，仅接受PA3的中断触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line3;	//按键端口输入中断，上升沿触发
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  	


	//流水线码盘用
	//GPIOA.0 中断线以及中断初始化配置   边沿触发，仅接受PA0的中断触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	  
	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//流水线码盘输入中断，上升沿触发
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			
	NVIC_Init(&NVIC_InitStructure);  
	
}

