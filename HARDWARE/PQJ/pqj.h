#ifndef __PQJ_H
#define __PQJ_H
#include "sys.h"
#include "stdbool.h"



//����	�뾶
#define pp_R 375
//����	��ǹ��
#define pq_Number 6
//����	x��λ��ƫ��
#define pp_x 670
//����	y��λ��ƫ�� �����ĵ�Ĭ��Ϊɨ�����е㣩
#define pp_y 0


#define HW1 PDout(9)
#define HW2 PDout(10)
#define HW3 PDout(11)
#define HW4 PDout(12)
#define HW5 PDout(13)
#define HW6 PDout(14)


#define GM0 PEin(2)
#define GM1 PEin(4)
#define GM2 PEin(6)
#define GM3 PCin(14)
#define GM4 PFin(0)
#define GM5 PFin(2)
#define GM6 PFin(4)

#define GM7 PFin(3)
#define GM8 PFin(1)
#define GM9 PCin(15)
#define GM10 PCin(13)
#define GM11 PEin(5)
#define GM12 PEin(3)
#define GM13 PEin(1)

//#define GM16 PFin(4);

void Hardware_Init(void);
void fire(void);

void TIME_scanning(void);
void pp_spray(int i);
void GM_Init(void);
void HW_Init(void);
void GM_shuru(void);
void Numerical_value_Init(void);//��ʼ�������ֵ
void jjkg_Init(void);
void body_Init(void);


struct body//������һ�������ܻ���LED�ĸ�������
{
    uint16_t GPIO_Pin;
    GPIO_TypeDef* GPIOx;
};

void fire_time(void);

#endif

