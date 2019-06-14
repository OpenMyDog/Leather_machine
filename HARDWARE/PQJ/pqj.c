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

	�罬���Ļ�������
	����Ĭ�ϵ�λ��mm
	�ٶ�Ĭ�ϵ�λ��mm/s
	ʱ��Ĭ�ϵ�λ��s

*************************/

//�罬����ˮ���ٶȣ�Ĭ��ֵ��
int pjj_speed = 10;
//�罬��ɨ��ֱ���
#define pjj_Scanning_speed 10
//�罬����ˮ�߹���뾶
int pjj_R = 50;

//ɨ���� ͨ����
const int smq_Number = 14;
//ɨ���� ͨ����� ��ÿһ��ɨ���֮��ľ��룩
const int smq_interval = 48;
//ɨ���� �ܳ��ȣ��������Ϊ8��10�Ĺ�������
const int smq_long = 1080;

//ɨ���� �ֱ��� ��λ��ÿ����	������̫СCPU���ܻ�ը��
const int smq_ResolvingPpower = 10;


/************************************************
	ȫ�ֱ���������
************************************************/
//����ͨ��	���Ҷ���λ��	x,y
struct weizhi{
	int x;
	int y;
}pp_life[smq_Number],pp_right[smq_Number];


//ɨ������䣬��¼���������ĸ�������
struct qvjian{
	int life;
	int right;
}smk_qvjian[smq_Number];

//���̼�ʱ��
struct shijian{
	int start;
	int timer;
}pp_timer[6];


//�����ϵĶ����б��
double pp_K[smq_Number];

//ɨ����ͨ��λ��	y;
int smq_tongdao_y[smq_Number];
//ɨ��ֿ��ÿһ��ĳ���
double smk_long = 1;
//ɨ��ֿ���
int smk_Number = 0;

//ɨ���
u8 smk[smq_Number][300];


//ɨ��ƴ�	ʱ���ж϶��ٴκ�����ɨ��һ��
int smk_timer=100;//����
int smk_timer_Num=100;//Ӧ���
//ɨ���м���
int smk_Columns_Num_max = smq_long/10*smq_ResolvingPpower/8;

//���������ӳٽ���
int pp_delay = 5;

int saomiao_Num = 0;//��ǰɨ���ֵ�������
int pp_speed = 180;//�������ǽ��ٶ�
int pp_fire[6];//Ĭ��6�����
int mp_angle_q = 90;//���̽Ƕ�
int mp_angle_h = -90;

//�����ͺ���˿ڽṹ��
struct body GM_demo[14],HW_demo[6];

double unit_area = 0.048 * 0.008;//��λ���(��*��)
double mianji = 0;


/************************************************
	ȫ�ֱ���������
************************************************/

extern struct body LED_demo[14];

extern int printf_Error(int Error_Code);


void TIME_scanning(void) {

    int i;
    u16 data;

    data = GPIO_ReadInputData(GM_demo[0].GPIOx);//��GM��1�Ķ˿�Ϊ׼
    data &= 0x3fff;//�ų�PF14 15λ��Ӱ��

    for(i=0; i<smq_Number; i++) {
        if(data >> i & 0x0001){
			smk[i][saomiao_Num] = 1;
			mianji += unit_area;
		}
        else smk[i][saomiao_Num] = 0;
		//printf("%d",data >> i & 0x0001);//���Թ�����
    }
	//printf("\r\n");

	GPIO_Write(GPIOF,~data);//����LED����,���ų�14,15λӰ��
	

    //ָ�����
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


void pp_spray(int i) { //���ܵ��ı���Ϊ��ǰ�������̵���ǹ���	������90�ȵ�λ��,i:0~5;

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


    if(a||b) { //ֻҪ��һ����Ϊ0ʱ

        o1 = atan(k1)/3.14*180;
        o2 = atan(k2)/3.14*180;

        o1+=180;
        o2+=180;

        //װ�ؼ�ʱֵ
        pp_timer[i].start = (o1-mp_angle_q)/(double)pp_speed/0.001;//0.001��1ms���жϼ��
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

        //װ�ؼ�ʱֵ
        pp_timer[i].start = (o1-mp_angle_h)/(double)pp_speed/0.001;//0.01��10ms���жϼ��
        pp_timer[i].timer = fabs(o1-o2)/(double)pp_speed/0.001;
		
		printf("p:%d++%d\r\n",pp_timer[i].start,pp_timer[i].timer);

    }else{
		pp_timer[i].start = 0;
		pp_timer[i].timer = -1;
	}
	
	
}


void Numerical_value_Init(void) {

    int i,j;


    //���ȳ�ʼ������
    for(i=0; i<smq_Number; i++) {
        for(j=0; j<smk_Columns_Num_max; j++) {
            smk[i][j] = 0;
        }
    }

    printf("������%d\r\n",smk_Columns_Num_max);


    //����ͨ��y��ƫ��λ��
    if(smq_Number%2==0) { //ͨ����Ϊż
        int data = smq_interval/2;
        int Num =smq_Number/2;

        for(i=0; i<Num; i++) {
            smq_tongdao_y[i] = -(data+(Num-i-1)*smq_interval);

            printf("ͨ��%dƫ��:%d\r\n",i,smq_tongdao_y[i]);
        }

        for(; i<smq_Number; i++) {
            smq_tongdao_y[i] = data+(i-Num)*smq_interval;

            printf("ͨ��%dƫ��:%d\r\n",i,smq_tongdao_y[i]);
        }
    } else { //ͨ����Ϊ��
		printf_Error(1);
    }


    //����ͨ�������̵Ľ���
    for(i=0; i<smq_Number; i++) {

        if(fabs((double)smq_tongdao_y[i]) < pp_R + pp_y) {

            pp_life[i].x = -(int)(sqrt(pow(pp_R,2) - pow(smq_tongdao_y[i]-pp_y,2)) - pp_x);
            pp_life[i].y = smq_tongdao_y[i];

            pp_right[i].x = (int)(sqrt(pow(pp_R,2) - pow(smq_tongdao_y[i]-pp_y,2)) + pp_x);
            pp_right[i].y = pp_life[i].y;

            printf("�Ƕ�%d:L:%d,%d;R:%d,%d;\r\n",i,pp_life[i].x,pp_life[i].y,pp_right[i].x,pp_right[i].y);

        } else {

            pp_life[i].x = pp_right[i].x = 0;
            pp_life[i].y = pp_right[i].y = smq_tongdao_y[i];

            printf("�Ƕ�%d:L:%d,%d;R:%d,%d;\r\n",i,pp_life[i].x,pp_life[i].y,pp_right[i].x,pp_right[i].y);
        }
    }


    //ɨ�����ֿ鳤��
    smk_long = smq_long/(smq_long/10*smq_ResolvingPpower/8);//�ó�ֵΪ8mm
    smk_Number = smk_Columns_Num_max;//�ó�ֵΪ135��

    //�ж϶�����������
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


    //�ƴ�ɨ����
    smk_timer = 1 /( pjj_speed / smk_long) / (0.01);//0.01���жϼ��
    smk_timer_Num = smk_timer;

    printf("timer:%d\r\n",smk_timer_Num);

    //����ֱ�ߵļн�
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
	//��������ʱ��
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

	body_Init();//�ṹ���ʼ��
    GM_Init();//�����˿ڳ�ʼ��
    HW_Init();//����˿ڳ�ʼ��
	LED_Init();//LED�˿ڳ�ʼ��
	jjkg_Init();//�ӽ����س�ʼ��
	KEY_Init();//������ʼ��
	TM1640_Init();//��ʼ��TM1640
	
	EXTIX_Init();//�жϳ�ʼ��
}


void GM_Init(void) {//����������˿ڳ�ʼ��
    GPIO_InitTypeDef  GPIO_InitStructure;
    int i;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; 		 //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz

    for(i=0; i<smq_Number; i++) {
        GPIO_InitStructure.GPIO_Pin = GM_demo[i].GPIO_Pin;
        GPIO_Init(GM_demo[i].GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(GM_demo[i].GPIOx,GM_demo[i].GPIO_Pin);//����ߵ�ƽ
    }
}

void HW_Init(void) {//����������˿ڳ�ʼ��

    int i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz

    for(i=0; i<pq_Number; i++) {
        GPIO_InitStructure.GPIO_Pin = HW_demo[i].GPIO_Pin;
        GPIO_Init(HW_demo[i].GPIOx, &GPIO_InitStructure);
        GPIO_ResetBits(HW_demo[i].GPIOx,HW_demo[i].GPIO_Pin);//����͵�ƽ
    }

}





void jjkg_Init(void) //�ӽ����س�ʼ��
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ��PORTDʱ��

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//�ӽ���������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD

}



void body_Init(void) { //�õ��Ľṹ��ĳ�ʼ��

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


