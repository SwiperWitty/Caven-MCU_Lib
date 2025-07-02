#ifndef __BASE_GPIO__H_
#define __BASE_GPIO__H_

#ifdef DEFAULT
#include "Items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*
                GPIO_Init(Exist_GPIO)->
    SDK->Items->                       BASE->
                Peripheral(UART、ADC)->      \\
                                              -->[XXX]->MODE
                                             //
                        C(Lib)->Caven->API->
*/

/*
        本文件在上述关系图中属于 【GPIO_Init】
         这里存放的是【只使用GPIO外设】功能的初始化（KEY、LED、BZZ、超声波等）。
         其他功能的初始化（例如UART、IIC），除了先初始化【GPIO外设】还要【功能外设】，这种功能的初始化GPIO并不在本文件范围内。
         IIC有软件版本和硬件版本，所以它的GPIO跟本文件无关。
         但是LCD的驱动由SPI协议和DC（IO）接口组成，那么这个IO接口由本文件提供。

                                                                     ————202207.14
 */

#define READ_Config		0
#define WRITE_Config	1

//  User io
#define PD_CGF_A_L() GPIOD->IO_L_REG = GPIO_Pin_2
#define PD_CGF_A_H() GPIOD->IO_H_REG = GPIO_Pin_2
#define PD_CGF_B_L() GPIOC->IO_L_REG = GPIO_Pin_12
#define PD_CGF_B_H() GPIOC->IO_H_REG = GPIO_Pin_12

#define DC_5V_ON()	 GPIOC->IO_H_REG = GPIO_Pin_0
#define DC_5V_OFF()  GPIOC->IO_L_REG = GPIO_Pin_0
#define DC_OUT_ON()  GPIOC->IO_H_REG = GPIO_Pin_1
#define DC_OUT_OFF() GPIOC->IO_L_REG = GPIO_Pin_1
#define YG_KEY_STATE()  gpio_input_data_bit_read(GPIOC,GPIO_Pin_3)
//---User MODE---

#ifdef Exist_LCD
    #define LCD_DC_L()  GPIOA->IO_L_REG = GPIO_Pin_10  //DC_GPIO
    #define LCD_DC_H()  GPIOA->IO_H_REG = GPIO_Pin_10  //PA10
//    #define LCD_RES_L() GPIOB->IO_L_REG = GPIO_Pin_0   // RES 使用硬件复位
//    #define LCD_RES_H() GPIOB->IO_H_REG = GPIO_Pin_0
#endif

#ifdef Exist_OLED
    #define OLED_DC_L() GPIOA->IO_L_REG = GPIO_Pin_10   //DC_GPIO
    #define OLED_DC_H() GPIOA->IO_H_REG = GPIO_Pin_10   //PA10

#endif

#ifdef Exist_LED
    #define GPIO_LED    GPIO_Pin_4      //PA04
    #define GPIO_LEDR   GPIO_Pin_2      //PC02
    #define LED_L() GPIOA->IO_L_REG = GPIO_LED
    #define LED_H() GPIOA->IO_H_REG = GPIO_LED
    #define LEDR_L() GPIOC->IO_L_REG = GPIO_LEDR
    #define LEDR_H() GPIOC->IO_H_REG = GPIO_LEDR
#endif

#ifdef Exist_BZZ
    #define GPIO_BZZ    GPIO_Pin_5     //PB5
    #define BZZ_L() GPIOB->IO_L_REG = GPIO_BZZ
    #define BZZ_H() GPIOB->IO_H_REG = GPIO_BZZ
#endif

#ifdef Exist_BUTTON
    #define BUTTON_STATE() gpio_input_data_bit_read(GPIOC,GPIO_Pin_13)     // KEY,读
#endif

#ifdef Exist_HC595
    #define HC595_RCK       GPIO_Pin_1         // 门阀时钟
    #define HC595_SCK       GPIO_Pin_2         // 移动时钟
    #define HC595_Data      GPIO_Pin_3         // 数据
    
    #define HC595_RCK_L() GPIOC->IO_L_REG = HC595_RCK
    #define HC595_RCK_H() GPIOC->IO_H_REG = HC595_RCK
    #define HC595_SCK_L() GPIOC->IO_L_REG = HC595_SCK
    #define HC595_SCK_H() GPIOC->IO_H_REG = HC595_SCK
    #define HC595_Data_L() GPIOC->IO_L_REG = HC595_Data
    #define HC595_Data_H() GPIOC->IO_H_REG = HC595_Data
    
#endif

#ifdef Exist_HC138
    #define HC595_D1    GPIO_Pin_1
    #define HC595_D2    GPIO_Pin_2
    #define HC595_D3    GPIO_Pin_3         //数据
    
    #define HC138_D1_H() GPIOC->IO_H_REG = HC595_D1
    #define HC138_D1_L() GPIOC->IO_L_REG = HC595_D1
    #define HC138_D2_H() GPIOC->IO_H_REG = HC595_D2
    #define HC138_D2_L() GPIOC->IO_L_REG = HC595_D2
    #define HC138_D3_H() GPIOC->IO_H_REG = HC595_D3
    #define HC138_D3_L() GPIOC->IO_L_REG = HC595_D3
    
#endif

#ifdef Exist_DS18B20
    #define DS18B20_IO      GPIO_Pin_8
    #define DS18B20_Clock   GPIOC

    #define DS18B20_IO_H() DS18B20_Clock->IO_H_REG = DS18B20_IO
    #define DS18B20_IO_L() DS18B20_Clock->IO_L_REG = DS18B20_IO

    #define DS18B20_IO_R() gpio_input_data_bit_read(DS18B20_Clock,DS18B20_IO)     // 读
#endif

#ifdef Exist_STEP_Motor
    #define STEP_OUT1   GPIO_Pin_0
    #define STEP_OUT2   GPIO_Pin_1 
    #define STEP_OUT3   GPIO_Pin_2 
    #define STEP_OUT4   GPIO_Pin_3 
    #define STEP_Clock  GPIOC

    #define STEP_OUT_L(x) (STEP_Clock->IO_L_REG = 0x0001 << (x))
    #define STEP_OUT_H(x) (STEP_Clock->IO_H_REG = 0x0001 << (x))

#endif

#ifdef Exist_Ultrasonic
    #define Trig_IO_H() GPIOB->IO_H_REG = GPIO_Pin_0
    #define Trig_IO_L() GPIOB->IO_L_REG = GPIO_Pin_0
    #define Echo_Read() gpio_input_data_bit_read(GPIOB,GPIO_Pin_1)     // 读
#endif

/*  Init-Function    */

void LCD_GPIO_Init(int Set);

void LED_GPIO_Init(int Set);
void BZZ_GPIO_Init(int Set);

void HC138_GPIO_Init(int Set);
void HC595_GPIO_Init(int Set);

void Button_GPIO_Init(int Set);

void User_GPIO_Init(int Set);	//!!!!

/*  other    */
void DS18B20_IO_Config(int Set);
void STEP_Motor_GPIO_Init (int Set);
void Ultrasonic_GPIO_Init(int Set);

#endif
