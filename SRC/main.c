#include <reg52.h>
#include "LCD1602.h"
#include "DS18B20.h"
#include "key.h"
/***********************************
*����IO��
**********************************/
sbit LED3 = P2 ^ 3;
sbit LED4 = P2 ^ 4;
/***********************************
*�Զ��庯��
**********************************/
void initData(void);						   //��ʼ������
void LcdDisplay(int);						   //LCD��ʾ��ȡ�����¶�
void LcdDispChar(uchar x, uchar y, uchar dat); //LCD��ʾ�ַ�����
void LcdDispStr(uchar x, uchar y, uchar *str); //LCD��ʾ�ַ���
void Timer0Init(void);						   //��ʱ����ʼ��
void InitINT0(void);						   //�ⲿ�ж�0��ʼ��
void KeyContrl(uint kv);					   //���̿��ƴ���
void setTempThreshold(int mk, uint num);	   //�趨��ֵ
void outOfRange(int d1, int d2);			   //����¶��Ƿ񳬹���ֵ
void delay10ms(void);						   //��ʱ����
/***********************************
*ȫ�ֱ���
**********************************/
int tempState;						   //�Ƿ�ʼ�����¶� 0����1����
int mark;							   //���
int tempMax;						   //�¶����ֵ
int tempMin;						   //�¶����ֵ
uint KeyValue;						   //������Ŷ�ȡ���ļ�ֵ
uchar KeyState;						   //������Ű���״̬
uchar symbolMax;					   //���ֵ�¶ȷ���״̬ 0��+��1��-
uchar symbolMin;					   //��Сֵ�¶ȷ���״̬ 0��+��1��-
uchar dat[] = "0123456789";			   //������̰�����ʾ����
uchar showInit[] = " set:[+00,+00]C"; //���ý����ʼ��ʾ����
uchar tempSymbol[] = "+-";			   //�¶���ʾ�ķ���

/*******************************************************************************
* ������         : main
* ��������		 : ������
* ����           : ��
* ���         	 : ��
*******************************************************************************/
void main()
{
	initData();
	LcdInit(); //��ʼ��LCD1602
	Timer0Init();
	LcdDispStr(2, 1, "TEMP SETTING");
	LcdDispStr(0, 2, &showInit);
	while (1)
	{
		GPIO_KEY = 0xf0;
		if (KeyState == 1)
		{
			delay10ms();
			KeyState = 0;
			KeyValue = KeyPro();
			KeyContrl(KeyValue);
		}
		delay10ms();
		if (tempState)
			LcdDisplay(Ds18b20ReadTemp());
	}
}

/*******************************************************************************
* �� �� ��         : initData
* ��������		   : ��ʼ��ȫ�ֱ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void initData(void)
{
	tempState = 0;
	mark = 6;
	KeyState = 0;
	tempMax = 27;
	tempMin = 20;
	symbolMax = 0;
	symbolMin = 0;
}

/*******************************************************************************
* ������         : LcdDisplay()
* ��������		 : LCD��ʾ��ȡ�����¶�
* ����           : temp���¶�
* ���         	 : ��
*******************************************************************************/

void LcdDisplay(int temp) //lcd��ʾ
{
	uchar datas[] = {0, 0, 0, 0}; //���崢���¶�����
	float tp;
	if (temp < 0) //���¶�ֵΪ����
	{
		LcdDispChar(5, 1, tempSymbol[1]);
		//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
		temp = temp - 1;
		temp = ~temp;
		tp = temp;
		temp = tp * 0.0625 * 100 + 0.5;
		//*100������С���㣬+0.5����������
	}
	else
	{
		LcdDispChar(5, 1, tempSymbol[0]);
		tp = temp;
		//����¶���������ô������ԭ����ǲ���������
		temp = tp * 0.0625 * 100 + 0.5;
		//*100������С���㣬+0.5����������
	}
	datas[0] = temp % 10000 / 1000;
	datas[1] = temp % 1000 / 100;
	datas[2] = temp % 100 / 10;
	datas[3] = temp % 10;

	outOfRange(datas[0], datas[1]);

	LcdDispChar(6, 1, '0' + datas[0]); //ʮλ
	LcdDispChar(7, 1, '0' + datas[1]); //��λ
	LcdDispChar(9, 1, '0' + datas[2]); //��ʾС�����1λ
	LcdDispChar(10, 1, '0' + datas[3]); //��ʾС�����2λ
}
/*******************************************************************************
* �� �� ��       : LcdDispChar()
* ��������		 : LCD1602��ʾ�ַ�����
* ��    ��       : X:lcd����ֵ(ȡֵ��Χ��0-15),y:lcd����ֵ(ȡֵ��Χ��1-2)��datΪ��Ҫ��ʾ�ַ���Ӧ�ĵ�ַ������
* ��    ��       : ��
*******************************************************************************/
void LcdDispChar(uchar x, uchar y, uchar dat)
{
	uchar address;
	if (y == 1)
		address = 0x80 + x;
	else
		address = 0xc0 + x;
	LcdWriteCom(address);
	LcdWriteData(dat);
}

/*******************************************************************************
* �� �� ��       : LcdDispStr()
* ��������		 : LCD1602��ʾ�ַ�����
* ��    ��       : X��lcd����ֵ(ȡֵ��Χ��0-15),y:lcd����ֵ(ȡֵ��Χ��1-2)��strΪ��Ҫ��ʾ�ַ�����Ӧ��ָ�����
* ��    ��       : ��
*******************************************************************************/
void LcdDispStr(uchar x, uchar y, uchar *str)
{
	uchar address;
	if (y == 1)
		address = 0x80 + x;
	else
		address = 0xc0 + x;
	LcdWriteCom(address);
	while (*str != '\0')
	{
		LcdWriteData(*str);
		str++;
	}
}

/*******************************************************************************
* �� �� ��         : Timer0Init
* ��������		   : ��ʱ��T0�ж�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Timer0Init(void)
{
	EA = 1;						//�����ж�
	ET0 = 1;					//��ʱ��T0�ж�����
	TR0 = 1;					//������ʱ��T0
	TH0 = (65536 - 1000) / 256; //��ʱ��T0����ֵ��ÿ����200�Σ�217΢�룩����һ���ж�����
	TL0 = (65536 - 1000) % 256; //��ʱ��T0����ֵ
}

void Time0_serve(void) interrupt 1
{
	if ((GPIO_KEY & 0xf0) != 0xf0)
	{
		KeyState = 1;
	}
	TH0 = (65536 - 1000) / 256;
	TL0 = (65536 - 1000) % 256;
}

/*******************************************************************************
* �� �� ��         : KeyContrl
* ��������		   : ������������������
* ��    ��         : kv ��ֵ
* ��    ��         : ��
*******************************************************************************/
void KeyContrl(uint kv)
{
	if (kv != -1)
	{
		if (kv > 9)	 //����9�����ⰴ��
		{
			switch (kv)
			{
			case 10: //��
				LcdDispChar(7, 2, dat[0]);
				LcdDispChar(8, 2, dat[0]);
				mark = 6;
				tempMin = 0;
				break;
			case 11: //��
				LcdDispChar(11, 2, dat[0]);
				LcdDispChar(12, 2, dat[0]);
				mark = 10;
				tempMax = 0;
				break;
			case 12: //����
				tempState = 0;
				LcdWriteCom(0x01); //����
				LcdWriteCom(0x80); //��������ָ�����
				delay10ms();
				LcdDispStr(2, 1, "TEMP SETTING");
				showInit[7] = tempMin / 10 + 48;
				showInit[8] = tempMin % 10 + 48;
				showInit[11] = tempMax / 10 + 48;
				showInit[12] = tempMax % 10 + 48;
				LcdDispStr(0, 2, &showInit);
				break;
			case 13: //ȷ��
				tempState = 1;
				LcdWriteCom(0x01); //����
				LcdWriteCom(0x80); //��������ָ�����
				//*********��ʼ��ʪ����ʾ��*********
				LcdDispStr(3, 1, "T:");
				LcdDispStr(6, 1, "TT.TTC");
				LcdDispStr(0, 2, "[");
				LcdDispChar(1, 2, tempSymbol[symbolMin]);
				LcdDispChar(2, 2, '0' + tempMin / 10);
				LcdDispChar(3, 2, '0' + tempMin % 10);
				LcdDispStr(4, 2, ",");
				LcdDispChar(5, 2, tempSymbol[symbolMax]);
				LcdDispChar(6, 2, '0' + tempMax / 10);
				LcdDispChar(7, 2, '0' + tempMax % 10);
				LcdDispStr(8, 2, "]");
				delay10ms();
				break;
			case 14: //+
				if (mark == 6)
				{
					symbolMin = 0;
					LcdDispChar(6, 2, tempSymbol[symbolMin]);
				}else
				{
					symbolMax = 0;
					LcdDispChar(10, 2, tempSymbol[symbolMax]);
				}
				mark++;
				break;
			case 15: //-
				if (mark == 6)
				{
					symbolMin = 1;
					LcdDispChar(6, 2, tempSymbol[symbolMin]);
				}else
				{
					symbolMax = 1;
					LcdDispChar(10, 2, tempSymbol[symbolMax]);
				}
				mark++;
				break;
			}
		}
		else
		{
			if (mark == 7|| mark == 11)
			{
				LcdDispChar(mark, 2, kv + 48);
				setTempThreshold(mark, kv);
				mark++;
			}
			else if (mark == 8)
			{
				LcdDispChar(mark, 2, kv + 48);
				setTempThreshold(mark, kv);
				mark = 10;
			}
			else if(mark==12)
			{
				LcdDispChar(mark, 2, kv + 48);
				setTempThreshold(mark, kv);
				//�������
			}
		}
	}
}

/*******************************************************************************
* �� �� ��         : setTempThreshold
* ��������		   : �����¶���ֵ
* ��    ��         : mk����Ļָ�� num����ֵ
* ��    ��         : ��
*******************************************************************************/
void setTempThreshold(int mk, uint num)
{

	switch (mk)
	{
	case 7:
		tempMin = num * 10;
		break;
	case 8:
		tempMin = tempMin + num;
		break;
	case 11:
		tempMax = num * 10;
		break;
	case 12:
		tempMax = tempMax + num;
		break;
	}
}

/*******************************************************************************
* �� �� ��         : outOfRange
* ��������		   : ����¶��Ƿ�Խ��
* ��    ��         : d1:����¶ȵ�ʮλ��d2:����¶ȵĸ�λ
* ��    ��         : ��
*******************************************************************************/
void outOfRange(int d1, int d2)
{	  
	int t = 0;
	t = t + d1 * 10 + d2;
	if (t == 85)
		t = 0;	
	LcdDispStr(10, 2, "      ");
	if (t > tempMax)
	{	
		LcdDispStr(10, 2, "Hight!");
		LED3 = 0;
		LED4 = 1;
	}
	if (t <= tempMax && t >= tempMin)
	{  				
		LcdDispStr(10, 2, "Normal");
		LED3 = 1;
		LED4 = 1;
	}
	if (t < tempMin)
	{		 
		LcdDispStr(12, 2, "Low!");
		LED3 = 1;
		LED4 = 0;
	}
}

 
/*******************************************************************************
* �� �� ��         : delay10ms
* ��������		   : ��ʱ��������ʱ10ms
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void delay10ms(void)   //��� 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}