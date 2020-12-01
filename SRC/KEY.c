#include "KEY.h"
/*******************************************************************************
* 函 数 名         : KeyDown
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : Val
*******************************************************************************/
uchar KeyScan(void)
{
    unsigned char Val;
    GPIO_KEY = 0xf0;      //高四位置高，低四位拉低
    if (GPIO_KEY != 0xf0) //表示有按键按下
    {
        delay(40); //去抖
        if (GPIO_KEY != 0xf0)
        {                    //表示有按键按下
            GPIO_KEY = 0xfe; //检测第一行
            if (GPIO_KEY != 0xfe)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0e;
                while (GPIO_KEY != 0xfe)
                    ;
                delay(40); //去抖
                while (GPIO_KEY != 0xfe)
                    ;
                return Val;
            }
            GPIO_KEY = 0xfd; //检测第二行
            if (GPIO_KEY != 0xfd)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0d;
                while (GPIO_KEY != 0xfd)
                    ;
                delay(40); //去抖
                while (GPIO_KEY != 0xfd)
                    ;
                return Val;
            }

            GPIO_KEY = 0xfb; //检测第三行
            if (GPIO_KEY != 0xfb)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0b;
                while (GPIO_KEY != 0xfb)
                    ;
                delay(40); //去抖
                while (GPIO_KEY != 0xfb)
                    ;
                return Val;
            }

            GPIO_KEY = 0xf7; //检测第四行
            if (GPIO_KEY != 0xf7)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x07;
                while (GPIO_KEY != 0xf7)
                    ;
                delay(40); //去抖
                while (GPIO_KEY != 0xf7)
                    ;
                return Val;
            }
        }
    }
    return 0xff;
}

/*******************************************************************************
* 函 数 名       : KeyPro()
* 函数功能		 : 对获取到的键值进行编号0-15
* 输    入       : 无
* 输    出       : 对于键编号
*******************************************************************************/
uint KeyPro(void) //按键处理子函数
{
    switch (KeyScan())
    {
    case 0x7e:
        return 12; //设置
        break;
    case 0xbe:
        return 13; //确认
        break;
    case 0xde:
        return 14; //+
        break;
    case 0xee:
        return 15; //-
        break;
    case 0x7d:
        return 9;
        break;
    case 0xbd:
        return 6;
        break;
    case 0xdd:
        return 3;
        break;
    case 0xed:
        return 11; //右
        break;
    case 0x7b:
        return 8;
        break;
    case 0xbb:
        return 5;
        break;
    case 0xdb:
        return 2;
        break;
    case 0xeb:
        return 0;
        break;
    case 0x77:
        return 7;
        break;
    case 0xb7:
        return 4;
        break;
    case 0xd7:
        return 1;
        break;
    case 0xe7:
        return 10; //左
        break;
    default:
        return -1;
        break;
    }
}

void delay(uchar i)
{
    uchar j, k;
    for (j = 50; j > 0; j--)
        for (k = i; k > 0; k--)
            ;
}