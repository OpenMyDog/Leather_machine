#include "TM1640.h"


unsigned char Display_Num[8] = {0x6f, 0x7f, 0x27, 0x7d, 0x6d, 0x3f};			//�����������ʾ������
u8 CODE[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9��ʾ����

int show_switch = 0;

extern int pjj_speed;//��ˮ���ٶ�
extern int pp_speed;//����ת��
extern double mianji;


//��ʾ����
void TM1640_Show(void){

	int Number = 0;
	double temp = 0;

	static int ge,shi,bai,qian,wan,xge,xshi;
	
	switch(show_switch%3){
	
		case 0:
			Number = pjj_speed;
		
			Display_Num[0] = 0x76;//"X"
			Display_Num[1] = 0x73;//"P"
			Display_Num[2] = 0x40;
			break;
	
		case 1:
			Number = pp_speed;
		
			Display_Num[0] = 0x73;//"P"
			Display_Num[1] = 0x67;//"Q"
			Display_Num[2] = 0x40;
			break;
		
		case 2:
			Number = (int)mianji;	
			temp = mianji;
			Display_Num[0] = 0x77;//"A"
			Display_Num[1] = 0x31;//"r"
			Display_Num[2] = 0x40;
			break;
	
		default:		
			Number = 66666;
			break;
	}
	
	if(Number/10000<10)wan = (int)(Number/10000);
	if(Number/1000%10<10)qian = (int)(Number/1000%10);
	if(Number/100%10<10)bai = (int)(Number/100%10);
	if(Number/10%10<10)shi = (int)(Number/10%10);
	if(Number%10<10)ge = (int)(Number%10);
	
	
	if(show_switch%3!=2){
		Display_Num[3] = CODE[wan];
		Display_Num[4] = CODE[qian];
		Display_Num[5] = CODE[bai];
		Display_Num[6] = CODE[shi];
		Display_Num[7] = CODE[ge];
	}else{
		
		Display_Num[3] = CODE[bai];
		Display_Num[4] = CODE[shi];
		Display_Num[5] = CODE[ge]|0x80;
		
		//ȡС����λ
		Number=(temp-(double)Number)*100;
		if(Number/10%10<10)xshi = (int)(Number/10%10);
		if(Number%10<10)xge = (int)(Number%10);
		
		Display_Num[6] = CODE[xshi];
		Display_Num[7] = CODE[xge];
	}
	
	
	
	//������
	//Display_Num[0]=Display_Num[1]=Display_Num[2]=Display_Num[3]=Display_Num[4] = Display_Num[5]=Display_Num[6]=Display_Num[7]= CODE[8];
	
//	Display_Num[0]=CODE[0];
//	Display_Num[1]=CODE[1];
//	Display_Num[2]=CODE[2];
//	Display_Num[3]=CODE[3];
//	Display_Num[4]=CODE[4];
//	Display_Num[5]=CODE[5];
//	Display_Num[6]=CODE[6];
//	Display_Num[7]=CODE[7];
	
	
	
	TM1640_Display();
}


//��ʼ��TM1640����
void TM1640_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ͷ�JTAG IO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/**************************************************
 ����  ������俪ʼ��־
 ����  ����
 ����ֵ: ��
 *************************************************/
void TM1640_Start(void)
{
	TM1640_DIN_HIGH;
	delay_us(2);
	
	TM1640_CLK_HIGH;
	delay_us(2);
		
	TM1640_DIN_LOW;
	delay_us(2);
	
	TM1640_CLK_LOW;
	delay_us(2);
}


/**************************************************
 ����  ������������־
 ����  ����
 ����ֵ: ��
 *************************************************/
void TM1640_End(void)
{
	TM1640_CLK_LOW;
	delay_us(2);
	
	TM1640_DIN_LOW;
	delay_us(2);
	
	TM1640_CLK_HIGH;
	delay_us(2);
	
	TM1640_DIN_HIGH;
	delay_us(2);
}


/**************************************************
 ����  ��д�ֽں���
 ����  ��
		dat --- ��Ҫд����ֽ�

 ����ֵ: ��
 *************************************************/
void TM1640_Write_Byte(unsigned char dat)
{
	uint8_t i;
	
	for (i = 0; i < 8; i++)
	{
		TM1640_CLK_LOW;			//CLK��ʱ���ź�Ϊ�͵�ƽʱ��DIN�ϵ��źŲ��ܸı�
		
		if (dat & 0x01)			//��λ��д
		{
			TM1640_DIN_HIGH;
		}
		else
		{
			TM1640_DIN_LOW;
		}
		delay_us(1);
		
		TM1640_CLK_HIGH;
		delay_us(1);
		
		dat >>= 1;
	}
}


/************************************************
 ������ �� TM1640_Display
 ����   �� ��
 ����ֵ :  ��
 ����   : �������ʾ��������ʾ�����Display_Num�е�����
 *************************************************/
void TM1640_Display(void)
{
	int	i;
	//��������
	TM1640_Start();
	TM1640_Write_Byte(DATA_COMMAND_Z);			//������ͨ����ַ�Զ�+1����
	TM1640_End();
	
	//���õ�ַ
	TM1640_Start();
	TM1640_Write_Byte(ADDR_START);				//��ʼ��ַ��0xco����ӦDIG1,Ȼ���ַ���Զ�����
	for(i = 0;i < 8; i++)						//6���������
	{
		TM1640_Write_Byte(Display_Num[i]);		//�������ʾ������{9 8 7 6 5}
	}	
	TM1640_End();

	//������ʾ
	TM1640_Start();
	TM1640_Write_Byte(DISP_OPEN);				//������ʾ	
	TM1640_End();
}

