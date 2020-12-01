#ifndef __KEY_H_
#define __KEY_H_

/**********************************
包含头文件
**********************************/
#include <reg52.h>
/**********************************
PIN口定义
**********************************/
#define GPIO_KEY P1

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

/**********************************
函数声明
**********************************/
uchar KeyScan(void);
uint KeyPro(void);
void delay(uchar i);
#endif