#ifndef __KEY_H_
#define __KEY_H_

/**********************************
����ͷ�ļ�
**********************************/
#include <reg52.h>
/**********************************
PIN�ڶ���
**********************************/
#define GPIO_KEY P1

//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

/**********************************
��������
**********************************/
uchar KeyScan(void);
uint KeyPro(void);
void delay(uchar i);
#endif