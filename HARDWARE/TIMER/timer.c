#include "timer.h"
#include "led.h"
#include "pqj.h"
#include "usart.h"

int time6_Num = 0;
int time6_lsx = 0;
int pp_time_Num[pq_Number]={0};
int time2_delay = 0;
int angle_delay = 1;

extern int smk_timer;
extern int smk_timer_Num;
extern int smk_Columns_Num_max;
extern int pp_speed;

extern int pp_fire[6];//默认6个喷机

//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM2中断，喷机亮灭
{

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
    {
		
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志

		time2_delay++;
		
		if(time2_delay >= 1000){
			time2_delay = 0;
			//printf("OK\r\n");			
		}
		
		//是否开始扫描
        if(!(smk_timer_Num--)) {
			smk_timer_Num = smk_timer;
			TIME_scanning();//保存当前扫描值
        }
		
		
		fire_time();
    }
}


//定时器6中断服务程序
void TIM6_IRQHandler(void)   //TIM6中断，定时扫描，喷机收放
{
	
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM6更新中断发生与否
    {
		int i;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  //清除TIMx更新中断标志

		time6_Num++;
		time6_lsx++;
		
		for(i=0;i<pq_Number;i++){//计算喷射时间
			if(time6_Num == pp_time_Num[i]){
				pp_spray(i);
			}
		}
		fire();
    }
	
}



void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


    TIM_Cmd(TIM2, ENABLE);  //使能TIMx
}

void TIM6_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


    TIM_Cmd(TIM6, ENABLE);  //使能TIMx
}


