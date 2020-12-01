#include <reg52.h>
#include "LCD1602.h"
#include "DS18B20.h"
#include "key.h"
/***********************************
*定义IO口
**********************************/
sbit LED3 = P2 ^ 3;
sbit LED4 = P2 ^ 4;
/***********************************
*自定义函数
**********************************/
void initData(void);						   //初始化数据
void LcdDisplay(int);						   //LCD显示读取到的温度
void LcdDispChar(uchar x, uchar y, uchar dat); //LCD显示字符数组
void LcdDispStr(uchar x, uchar y, uchar *str); //LCD显示字符串
void Timer0Init(void);						   //定时器初始化
void InitINT0(void);						   //外部中断0初始化
void KeyContrl(uint kv);					   //键盘控制处理
void setTempThreshold(int mk, uint num);	   //设定阈值
void outOfRange(int d1, int d2);			   //检查温度是否超过阈值
void delay10ms(void);						   //延时函数
/***********************************
*全局变量
**********************************/
int tempState;						   //是否开始测量温度 0：否；1：是
int mark;							   //标记
int tempMax;						   //温度最大值
int tempMin;						   //温度最大值
uint KeyValue;						   //用来存放读取到的键值
uchar KeyState;						   //用来存放按键状态
uchar symbolMax;					   //最大值温度符号状态 0：+；1：-
uchar symbolMin;					   //最小值温度符号状态 0：+；1：-
uchar dat[] = "0123456789";			   //矩阵键盘按键显示数据
uchar showInit[] = " set:[+00,+00]C"; //设置界面初始显示数据
uchar tempSymbol[] = "+-";			   //温度显示的符号

/*******************************************************************************
* 函数名         : main
* 函数功能		 : 主函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void main()
{
	initData();
	LcdInit(); //初始化LCD1602
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
* 函 数 名         : initData
* 函数功能		   : 初始化全局变量
* 输    入         : 无
* 输    出         : 无
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
* 函数名         : LcdDisplay()
* 函数功能		 : LCD显示读取到的温度
* 输入           : temp：温度
* 输出         	 : 无
*******************************************************************************/

void LcdDisplay(int temp) //lcd显示
{
	uchar datas[] = {0, 0, 0, 0}; //定义储存温度数组
	float tp;
	if (temp < 0) //当温度值为负数
	{
		LcdDispChar(5, 1, tempSymbol[1]);
		//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
		temp = temp - 1;
		temp = ~temp;
		tp = temp;
		temp = tp * 0.0625 * 100 + 0.5;
		//*100留两个小数点，+0.5是四舍五入
	}
	else
	{
		LcdDispChar(5, 1, tempSymbol[0]);
		tp = temp;
		//如果温度是正，那么正数的原码就是补码它本身
		temp = tp * 0.0625 * 100 + 0.5;
		//*100留两个小数点，+0.5是四舍五入
	}
	datas[0] = temp % 10000 / 1000;
	datas[1] = temp % 1000 / 100;
	datas[2] = temp % 100 / 10;
	datas[3] = temp % 10;

	outOfRange(datas[0], datas[1]);

	LcdDispChar(6, 1, '0' + datas[0]); //十位
	LcdDispChar(7, 1, '0' + datas[1]); //个位
	LcdDispChar(9, 1, '0' + datas[2]); //显示小数点后1位
	LcdDispChar(10, 1, '0' + datas[3]); //显示小数点后2位
}
/*******************************************************************************
* 函 数 名       : LcdDispChar()
* 函数功能		 : LCD1602显示字符数组
* 输    入       : X:lcd的列值(取值范围是0-15),y:lcd的行值(取值范围是1-2)，dat为所要显示字符对应的地址参数。
* 输    出       : 无
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
* 函 数 名       : LcdDispStr()
* 函数功能		 : LCD1602显示字符串组
* 输    入       : X：lcd的列值(取值范围是0-15),y:lcd的行值(取值范围是1-2)，str为所要显示字符串对应的指针参数
* 输    出       : 无
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
* 函 数 名         : Timer0Init
* 函数功能		   : 定时器T0中断
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer0Init(void)
{
	EA = 1;						//开总中断
	ET0 = 1;					//定时器T0中断允许
	TR0 = 1;					//启动定时器T0
	TH0 = (65536 - 1000) / 256; //定时器T0赋初值，每计数200次（217微秒）发送一次中断请求
	TL0 = (65536 - 1000) % 256; //定时器T0赋初值
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
* 函 数 名         : KeyContrl
* 函数功能		   : 矩阵键盘特殊键功能重
* 输    入         : kv 键值
* 输    出         : 无
*******************************************************************************/
void KeyContrl(uint kv)
{
	if (kv != -1)
	{
		if (kv > 9)	 //大于9是特殊按键
		{
			switch (kv)
			{
			case 10: //左
				LcdDispChar(7, 2, dat[0]);
				LcdDispChar(8, 2, dat[0]);
				mark = 6;
				tempMin = 0;
				break;
			case 11: //右
				LcdDispChar(11, 2, dat[0]);
				LcdDispChar(12, 2, dat[0]);
				mark = 10;
				tempMax = 0;
				break;
			case 12: //设置
				tempState = 0;
				LcdWriteCom(0x01); //清屏
				LcdWriteCom(0x80); //设置数据指针起点
				delay10ms();
				LcdDispStr(2, 1, "TEMP SETTING");
				showInit[7] = tempMin / 10 + 48;
				showInit[8] = tempMin % 10 + 48;
				showInit[11] = tempMax / 10 + 48;
				showInit[12] = tempMax % 10 + 48;
				LcdDispStr(0, 2, &showInit);
				break;
			case 13: //确认
				tempState = 1;
				LcdWriteCom(0x01); //清屏
				LcdWriteCom(0x80); //设置数据指针起点
				//*********初始化湿度显示区*********
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
				//输入完成
			}
		}
	}
}

/*******************************************************************************
* 函 数 名         : setTempThreshold
* 函数功能		   : 设置温度阈值
* 输    入         : mk：屏幕指针 num：数值
* 输    出         : 无
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
* 函 数 名         : outOfRange
* 函数功能		   : 检测温度是否越界
* 输    入         : d1:测得温度的十位；d2:测得温度的个位
* 输    出         : 无
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
* 函 数 名         : delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void delay10ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}