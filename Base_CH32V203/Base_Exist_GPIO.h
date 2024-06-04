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
                                                                     ————2022.07.14
 */

#ifdef Exist_LCD
    #define OLED_DC_L() GPIO_ResetBits(GPIOA, GPIO_Pin_10)  //DC_GPIO
    #define OLED_DC_H() GPIO_SetBits(GPIOA, GPIO_Pin_10)    //PA10

    #define LCD_RES_L() GPIO_ResetBits(GPIOB, GPIO_Pin_0)   //RES
    #define LCD_RES_H() GPIO_SetBits(GPIOB, GPIO_Pin_0)     //PB0

    void LCD_GPIO_Init(int SET);
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
    #define RUN_LED_L() GPIO_ResetBits(GPIOB, GPIO_Pin_12)  //
    #define RUN_LED_H() GPIO_SetBits(GPIOB, GPIO_Pin_12)    //PB12
    #define ERROR_LED_L()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)  //
    #define ERROR_LED_H()   GPIO_SetBits(GPIOB, GPIO_Pin_13)    //PB13
    #define RFID_LED_L()   GPIO_ResetBits(GPIOB, GPIO_Pin_15)  //
    #define RFID_LED_H()   GPIO_SetBits(GPIOB, GPIO_Pin_15)    //PB15
    void LED_GPIO_Init(int SET);
#endif
#ifdef Exist_BZZ
    #define BZZ_L() GPIO_ResetBits(GPIOB, GPIO_Pin_7)       //BZZ
    #define BZZ_H() GPIO_SetBits(GPIOB, GPIO_Pin_7)         //PB07

    void BZZ_GPIO_Init(int SET);
#endif

#ifdef Exist_BUTTON
    #define BUTTON_STATE() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)     // Button
    #define BUTTON1_STATE() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)     // Button
    #define BUTTON2_STATE() GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)     // Button
    #define BUTTON3_STATE() GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)     // Button
    #define BUTTON4_STATE() GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)     // Button
    void Button_GPIO_Init(int SET);
#endif

#ifdef Exist_HC595
    #define HC595_SCK_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_13)    //
    #define HC595_SCK_H()    GPIO_SetBits(GPIOB, GPIO_Pin_13)      //PB13
    #define HC595_Data_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)    //
    #define HC595_Data_H()    GPIO_SetBits(GPIOB, GPIO_Pin_14)      //PB14
    #define HC595_RCK_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_12)   //
    #define HC595_RCK_H()    GPIO_SetBits(GPIOB, GPIO_Pin_12)     //PB12

    void HC595_GPIO_Init(int SET);
#endif

#ifdef Exist_CustomIO
    #define RSTIC_L()   GPIO_ResetBits(GPIOA, GPIO_Pin_8)   //RST (NPN)
    #define RSTIC_H()   GPIO_SetBits(GPIOA, GPIO_Pin_8)     //PA08  // 复位ESP32
    #define WIG0_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_3)   //WIG0 (NPN)
    #define WIG0_H()    GPIO_SetBits(GPIOB, GPIO_Pin_3)     //PB03
    #define WIG1_L()    GPIO_ResetBits(GPIOA, GPIO_Pin_15)  //WIG1 (NPN)
    #define WIG1_H()    GPIO_SetBits(GPIOA, GPIO_Pin_15)    //PA15

// GPIO_OUT
    #define GPO1_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_8)   //GPO1 (NPN)
    #define GPO1_H()    GPIO_SetBits(GPIOB, GPIO_Pin_8)     //PB8
    #define GPO2_L()    GPIO_ResetBits(GPIOB, GPIO_Pin_9)   //GPO2 (NPN)
    #define GPO2_H()    GPIO_SetBits(GPIOB, GPIO_Pin_9)     //PB9
    #define GPO3_L()    GPIO_ResetBits(GPIOA, GPIO_Pin_12)  //GPO3 (NPN)
    #define GPO3_H()    GPIO_SetBits(GPIOA, GPIO_Pin_12)    //PA12
    #define GPO4_L()    GPIO_ResetBits(GPIOA, GPIO_Pin_11)  //GPO4 (NPN)
    #define GPO4_H()    GPIO_SetBits(GPIOA, GPIO_Pin_11)    //PA11

// GPIO_IN
    #define GPI1_STATE() GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)     // PB05
    #define GPI2_STATE() GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)     // PB06

    void Custom_GPIO_Init(int SET);
#endif

/*  Init-Function    */
//


#endif
