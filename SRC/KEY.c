#include "KEY.h"
/*******************************************************************************
* �� �� ��         : KeyDown
* ��������		   : ����а������²���ȡ��ֵ
* ��    ��         : ��
* ��    ��         : Val
*******************************************************************************/
uchar KeyScan(void)
{
    unsigned char Val;
    GPIO_KEY = 0xf0;      //����λ�øߣ�����λ����
    if (GPIO_KEY != 0xf0) //��ʾ�а�������
    {
        delay(40); //ȥ��
        if (GPIO_KEY != 0xf0)
        {                    //��ʾ�а�������
            GPIO_KEY = 0xfe; //����һ��
            if (GPIO_KEY != 0xfe)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0e;
                while (GPIO_KEY != 0xfe)
                    ;
                delay(40); //ȥ��
                while (GPIO_KEY != 0xfe)
                    ;
                return Val;
            }
            GPIO_KEY = 0xfd; //���ڶ���
            if (GPIO_KEY != 0xfd)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0d;
                while (GPIO_KEY != 0xfd)
                    ;
                delay(40); //ȥ��
                while (GPIO_KEY != 0xfd)
                    ;
                return Val;
            }

            GPIO_KEY = 0xfb; //��������
            if (GPIO_KEY != 0xfb)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x0b;
                while (GPIO_KEY != 0xfb)
                    ;
                delay(40); //ȥ��
                while (GPIO_KEY != 0xfb)
                    ;
                return Val;
            }

            GPIO_KEY = 0xf7; //��������
            if (GPIO_KEY != 0xf7)
            {
                Val = GPIO_KEY & 0xf0;
                Val += 0x07;
                while (GPIO_KEY != 0xf7)
                    ;
                delay(40); //ȥ��
                while (GPIO_KEY != 0xf7)
                    ;
                return Val;
            }
        }
    }
    return 0xff;
}

/*******************************************************************************
* �� �� ��       : KeyPro()
* ��������		 : �Ի�ȡ���ļ�ֵ���б��0-15
* ��    ��       : ��
* ��    ��       : ���ڼ����
*******************************************************************************/
uint KeyPro(void) //���������Ӻ���
{
    switch (KeyScan())
    {
    case 0x7e:
        return 12; //����
        break;
    case 0xbe:
        return 13; //ȷ��
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
        return 11; //��
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
        return 10; //��
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