#include "pqj.h"
#include "stdbool.h"
#include "math.h"
#include "usart.h"
#include "stdlib.h"
#include "LED.h"
#include "KEY.h"
#include "EXTI.h"
#include "TM1640.h"

/*************************

	喷浆机的基本设置
	长度默认单位：mm
	速度默认单位：mm/s
	时间默认单位：s

*************************/

//喷浆机流水线速度（默认值）
int pjj_speed = 10;
//喷浆机扫描分辨率
#define pjj_Scanning_speed 10
//喷浆机流水线滚轴半径
int pjj_R = 50;

//扫描区 通道数
const int smq_Number = 14;
//扫描区 通道间隔 （每一个扫描点之间的距离）
const int smq_interval = 48;
//扫描区 总长度（最好设置为8和10的公倍数）
const int smq_long = 1080;

//扫描区 分辨率 单位：每厘米	（设置太小CPU可能会炸）
const int smq_ResolvingPpower = 10;


/************************************************
	全局变量申明区
************************************************/
//喷盘通道	左右定点位置	x,y
struct weizhi{
	int x;
	int y;
}pp_life[smq_Number],pp_right[smq_Number];


//扫描块区间，记录定点落在哪个区间上
struct qvjian{
	int life;
	int right;
}smk_qvjian[smq_Number];

//喷盘计时器
struct shijian{
	int start;
	int timer;
}pp_timer[6];


//喷盘上的定点的斜率
double pp_K[smq_Number];

//扫描区通道位置	y;
int smq_tongdao_y[smq_Number];
//扫描分块后每一块的长度
double smk_long = 1;
//扫描分块数
int smk_Number = 0;

//扫描块
u8 smk[smq_Number][300];


//扫描计次	时钟中断多少次后算作扫描一次
int smk_timer=100;//不变
int smk_timer_Num=100;//应万变
//扫描列计数
int smk_Columns_Num_max = smq_long/10*smq_ResolvingPpower/8;

//喷射区间延迟矫正
int pp_delay = 5;

int saomiao_Num = 0;//当前扫描轮到的列数
int pp_speed = 180;//这玩意是角速度
int pp_fire[6];//默认6个喷机
int mp_angle_q = 90;//码盘角度
int mp_angle_h = -90;

//光敏和红外端口结构体
struct body GM_demo[14],HW_demo[6];

double unit_area = 0.048 * 0.008;//单位面积(长*宽)
double mianji = 0;


/************************************************
	全局变量引用区
************************************************/

extern struct body LED_demo[14];

extern int printf_Error(int Error_Code);


void TIME_scanning(void) {

    int i;
    u16 data;

    data = GPIO_ReadInputData(GM_demo[0].GPIOx);//以GM口1的端口为准
    data &= 0x3fff;//排除PF14 15位的影响

    for(i=0; i<smq_Number; i++) {
        if(data >> i & 0x0001){
			smk[i][saomiao_Num] = 1;
			mianji += unit_area;
		}
        else smk[i][saomiao_Num] = 0;
		//printf("%d",data >> i & 0x0001);//调试光电管用
    }
	//printf("\r\n");

	GPIO_Write(GPIOF,~data);//控制LED亮灭,已排除14,15位影响
	

    //指针后移
    if(--saomiao_Num <= 0) {
        saomiao_Num = smk_Columns_Num_max;
    }


    for(i=0; i<smq_Number; i++) {

        if(--smk_qvjian[i].life <= 0) {
            smk_qvjian[i].life = smk_Columns_Num_max;
        }

        if(--smk_qvjian[i].right <= 0) {
            smk_qvjian[i].right = smk_Columns_Num_max ;
        }
    }
}


void pp_spray(int i) { //接受到的变量为当前经过码盘的喷枪序号	码盘在90度的位置,i:0~5;

    int a = 0 ,b = 0,j = 0;
    double k1,k2,o1,o2;
    int temp = 0;

    //printf("\r\n--%d--\r\n",i);

    for(j=0; j<smq_Number; j++) {

        temp = smk_qvjian[i].life-pp_delay;
        if(temp<0)temp+=smk_Columns_Num_max;

        if(a==0&&smk[14-j-1][temp]==1) {
            a++;
            k1=pp_K[14-j-1];
        }
        if(b==0&&smk[j][temp]==1) {
            b++;
            k2=pp_K[j];
        }

    }


    if(a||b) { //只要有一个不为0时

        o1 = atan(k1)/3.14*180;
        o2 = atan(k2)/3.14*180;

        o1+=180;
        o2+=180;

        //装载计时值
        pp_timer[i].start = (o1-mp_angle_q)/(double)pp_speed/0.001;//0.001是1ms的中断间隔
        pp_timer[i].timer = fabs(o1-o2)/(double)pp_speed/0.001;
		printf("p:%d++%d\r\n",pp_timer[i].start,pp_timer[i].timer);

    }else{
		pp_timer[i].start = 0;
		pp_timer[i].timer = -1;
	}

    if(i>=3)i-=3;
    else i+=3;

    a=b=0;

    for(j=0; j<14; j++) {

        temp = smk_qvjian[14-j-1].right-pp_delay;
        if(temp<0)temp+=smk_Columns_Num_max;

        if(a==0&&smk[j][temp]==1) {
            a++;
            k1=pp_K[j];
        }
        if(b==0&&smk[14-j-1][temp]==1) {
            b++;
            k2=pp_K[14-j-1];
        }

    }

    if(a||b) {

        o1 = -(atan(k1)/3.14*180);
        o2 = -(atan(k2)/3.14*180);

        //装载计时值
        pp_timer[i].start = (o1-mp_angle_h)/(double)pp_speed/0.001;//0.01是10ms的中断间隔
        pp_timer[i].timer = fabs(o1-o2)/(double)pp_speed/0.001;
		
		printf("p:%d++%d\r\n",pp_timer[i].start,pp_timer[i].timer);

    }else{
		pp_timer[i].start = 0;
		pp_timer[i].timer = -1;
	}
	
	
}


void Numerical_value_Init(void) {

    int i,j;


    //首先初始化数组
    for(i=0; i<smq_Number; i++) {
        for(j=0; j<smk_Columns_Num_max; j++) {
            smk[i][j] = 0;
        }
    }

    printf("列数：%d\r\n",smk_Columns_Num_max);


    //计算通道y轴偏移位置
    if(smq_Number%2==0) { //通道数为偶
        int data = smq_interval/2;
        int Num =smq_Number/2;

        for(i=0; i<Num; i++) {
            smq_tongdao_y[i] = -(data+(Num-i-1)*smq_interval);

            printf("通道%d偏移:%d\r\n",i,smq_tongdao_y[i]);
        }

        for(; i<smq_Number; i++) {
            smq_tongdao_y[i] = data+(i-Num)*smq_interval;

            printf("通道%d偏移:%d\r\n",i,smq_tongdao_y[i]);
        }
    } else { //通道数为奇
		printf_Error(1);
    }


    //计算通道与喷盘的交点
    for(i=0; i<smq_Number; i++) {

        if(fabs((double)smq_tongdao_y[i]) < pp_R + pp_y) {

            pp_life[i].x = -(int)(sqrt(pow(pp_R,2) - pow(smq_tongdao_y[i]-pp_y,2)) - pp_x);
            pp_life[i].y = smq_tongdao_y[i];

            pp_right[i].x = (int)(sqrt(pow(pp_R,2) - pow(smq_tongdao_y[i]-pp_y,2)) + pp_x);
            pp_right[i].y = pp_life[i].y;

            printf("角度%d:L:%d,%d;R:%d,%d;\r\n",i,pp_life[i].x,pp_life[i].y,pp_right[i].x,pp_right[i].y);

        } else {

            pp_life[i].x = pp_right[i].x = 0;
            pp_life[i].y = pp_right[i].y = smq_tongdao_y[i];

            printf("角度%d:L:%d,%d;R:%d,%d;\r\n",i,pp_life[i].x,pp_life[i].y,pp_right[i].x,pp_right[i].y);
        }
    }


    //扫描区分块长度
    smk_long = smq_long/(smq_long/10*smq_ResolvingPpower/8);//得出值为8mm
    smk_Number = smk_Columns_Num_max;//得出值为135个

    //判断定点所处分区
    for(i=0; i<smq_Number; i++) {
        if(pp_life[0].x!=0) {

            smk_qvjian[i].life = (pp_life[i].x/smk_long) + 1;
            smk_qvjian[i].right = (pp_right[i].x/smk_long) + 1;

            printf("qvj%d:L:%d,R:%d\r\n",i,smk_qvjian[i].life,smk_qvjian[i].right);

        } else {
            smk_qvjian[i].life = smk_qvjian[i].right=0;

            printf("qvj%d:L:%d,R:%d\r\n",i,smk_qvjian[i].life,smk_qvjian[i].right);
        }
    }


    //计次扫描数
    smk_timer = 1 /( pjj_speed / smk_long) / (0.01);//0.01是中断间隔
    smk_timer_Num = smk_timer;

    printf("timer:%d\r\n",smk_timer_Num);

    //计算直线的夹角
    for(i=0; i<smq_Number; i++) {
        pp_K[i]=((double)pp_life[i].y-pp_y)/((double)pp_life[i].x-pp_x);
        printf("K%d:%lf\r\n",i,pp_K[i]);
    }
}



void fire(void) {

    int i ;

    for(i=0; i<pq_Number; i++) {
        if(pp_fire[i])GPIO_ResetBits(HW_demo[i].GPIOx,HW_demo[i].GPIO_Pin);
        else GPIO_SetBits(HW_demo[i].GPIOx,HW_demo[i].GPIO_Pin);
    }
}


void fire_time(void){
	int i;
	//喷机亮灭的时间
	for(i=0;i<pq_Number;i++){
		//if(i==3)printf("%d\r\n",pp_timer[3][0]);
		
		if(pp_timer[i].start-- == 0){
			pp_timer[i].start = 0;
			
			if(pp_timer[i].timer-- >= 0){
				pp_fire[i]=1;
			}else{
				pp_fire[i]=0;
			}
		}
	}
}


void Hardware_Init(void){

	body_Init();//结构体初始化
    GM_Init();//光敏端口初始化
    HW_Init();//红外端口初始化
	LED_Init();//LED端口初始化
	jjkg_Init();//接近开关初始化
	KEY_Init();//按键初始化
	TM1640_Init();//初始化TM1640
	
	EXTIX_Init();//中断初始化
}


void GM_Init(void) {//光敏用输入端口初始化
    GPIO_InitTypeDef  GPIO_InitStructure;
    int i;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能PC端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; 		 //浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz

    for(i=0; i<smq_Number; i++) {
        GPIO_InitStructure.GPIO_Pin = GM_demo[i].GPIO_Pin;
        GPIO_Init(GM_demo[i].GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(GM_demo[i].GPIOx,GM_demo[i].GPIO_Pin);//输出高电平
    }
}

void HW_Init(void) {//红外用输入端口初始化

    int i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz

    for(i=0; i<pq_Number; i++) {
        GPIO_InitStructure.GPIO_Pin = HW_demo[i].GPIO_Pin;
        GPIO_Init(HW_demo[i].GPIOx, &GPIO_InitStructure);
        GPIO_ResetBits(HW_demo[i].GPIOx,HW_demo[i].GPIO_Pin);//输出低电平
    }

}





void jjkg_Init(void) //接近开关初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能PORTD时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//接近开关引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD

}



void body_Init(void) { //用到的结构体的初始化

    int i;

    for(i=0; i<smq_Number; i++) {
        GM_demo[i].GPIOx = GPIOC;
    }

    GM_demo[0].GPIO_Pin = GPIO_Pin_0;
    GM_demo[1].GPIO_Pin = GPIO_Pin_1;
    GM_demo[2].GPIO_Pin = GPIO_Pin_2;
    GM_demo[3].GPIO_Pin = GPIO_Pin_3;
    GM_demo[4].GPIO_Pin = GPIO_Pin_4;
    GM_demo[5].GPIO_Pin = GPIO_Pin_5;
    GM_demo[6].GPIO_Pin = GPIO_Pin_6;
    GM_demo[7].GPIO_Pin = GPIO_Pin_7;
    GM_demo[8].GPIO_Pin = GPIO_Pin_8;
    GM_demo[9].GPIO_Pin = GPIO_Pin_9;
    GM_demo[10].GPIO_Pin = GPIO_Pin_10;
    GM_demo[11].GPIO_Pin = GPIO_Pin_11;
    GM_demo[12].GPIO_Pin = GPIO_Pin_12;
    GM_demo[13].GPIO_Pin = GPIO_Pin_13;

    for(i=0; i<pq_Number; i++) {
        HW_demo[i].GPIOx = GPIOD;
    }

    HW_demo[0].GPIO_Pin = GPIO_Pin_9;
    HW_demo[1].GPIO_Pin = GPIO_Pin_10;
    HW_demo[2].GPIO_Pin = GPIO_Pin_11;
    HW_demo[3].GPIO_Pin = GPIO_Pin_12;
    HW_demo[4].GPIO_Pin = GPIO_Pin_13;
    HW_demo[5].GPIO_Pin = GPIO_Pin_14;


    for(i=0; i<smq_Number; i++) {
        LED_demo[i].GPIOx=GPIOF;
    }

    LED_demo[0].GPIO_Pin = GPIO_Pin_0;
    LED_demo[1].GPIO_Pin = GPIO_Pin_1;
    LED_demo[2].GPIO_Pin = GPIO_Pin_2;
    LED_demo[3].GPIO_Pin = GPIO_Pin_3;
    LED_demo[4].GPIO_Pin = GPIO_Pin_4;
    LED_demo[5].GPIO_Pin = GPIO_Pin_5;
    LED_demo[6].GPIO_Pin = GPIO_Pin_6;
    LED_demo[7].GPIO_Pin = GPIO_Pin_7;
    LED_demo[8].GPIO_Pin = GPIO_Pin_8;
    LED_demo[9].GPIO_Pin = GPIO_Pin_9;
    LED_demo[10].GPIO_Pin = GPIO_Pin_10;
    LED_demo[11].GPIO_Pin = GPIO_Pin_11;
    LED_demo[12].GPIO_Pin = GPIO_Pin_12;
    LED_demo[13].GPIO_Pin = GPIO_Pin_13;


	for(i=0; i<smq_Number; i++) {
        pp_life[i].x=0;
		pp_life[i].y=0;
    }
	
	for(i=0; i<smq_Number; i++) {
        pp_right[i].x=0;
		pp_right[i].y=0;
    }
	
	for(i=0; i<pq_Number; i++) {
        pp_timer[i].start=0;
		pp_timer[i].timer=0;
    }

	
}


