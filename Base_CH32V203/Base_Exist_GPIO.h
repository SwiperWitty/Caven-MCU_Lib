#ifndef __Exist_GPIO__H_
#define __Exist_GPIO__H_

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/*
        本文件在上述关系图中属于 【GPIO_Init】
         这里存放的是【只使用GPIO外设】功能的初始化（KEY、LED、BZZ、超声波等）。
         其他功能的初始化（例如UART、IIC），除了先初始化【GPIO外设】还要【功能外设】，这种功能的初始化GPIO并不在本文件范围内。
         IIC有软件版本和硬件版本，所以它的GPIO跟本文件无关。
         但是LCD的驱动由SPI协议和DC（IO）接口组成，那么这个IO接口由本文件提供。

                                                                     ————202207.14
 */

#ifdef Exist_LCD
    #define OLED_DC_L() GPIO_ResetBits(GPIOA, GPIO_Pin_10)  //DC_GPIO
    #define OLED_DC_H() GPIO_SetBits(GPIOA, GPIO_Pin_10)    //PA10

    #define LCD_RES_L() GPIO_ResetBits(GPIOB, GPIO_Pin_0)   //RES
    #define LCD_RES_H() GPIO_SetBits(GPIOB, GPIO_Pin_0)     //PB0
#endif

#ifdef Exist_OLED
    #define OLED_DC_L() GPIO_ResetBits(GPIOA, GPIO_Pin_10)  //DC_GPIO
    #define OLED_DC_H() GPIO_SetBits(GPIOA, GPIO_Pin_10)    //PA10

    #define LCD_RES_L() GPIO_ResetBits(GPIOB, GPIO_Pin_0)   //RES
    #define LCD_RES_H() GPIO_SetBits(GPIOB, GPIO_Pin_0)     //PB0
#endif

#ifdef Exist_LED
    #define LED_L()     GPIO_ResetBits(GPIOB, GPIO_Pin_4)   //LED
    #define LED_H()     GPIO_SetBits(GPIOB, GPIO_Pin_4)     //PB04
    #define LEDBLUE_L()  GPIO_ResetBits(GPIOB, GPIO_Pin_0)   //LED_bluetooth
    #define LEDBLUE_H()  GPIO_SetBits(GPIOB, GPIO_Pin_0)     //PB00
    #define LEDWIFI_L() GPIO_ResetBits(GPIOB, GPIO_Pin_1)   //LED
    #define LEDWIFI_H() GPIO_SetBits(GPIOB, GPIO_Pin_1)     //PB01
#endif
#ifdef Exist_BZZ
    #define BZZ_L() GPIO_ResetBits(GPIOB, GPIO_Pin_7)   //LED
    #define BZZ_H() GPIO_SetBits(GPIOB, GPIO_Pin_7)     //PB07
#endif

#ifdef Exist_BUTTON
    #define BUTTON_STATE() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)     // Button
#endif


/*  Init-Function    */

void LCD_GPIO_Init(int SET);

void LED_GPIO_Init(int SET);
void BZZ_GPIO_Init(int SET);

void KEY_GPIO_Init(int SET);

#endif
