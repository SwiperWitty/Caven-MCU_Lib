#ifndef __Exist_GPIO__H_
#define __Exist_GPIO__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
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
#define User_hot_off() GPIOB->Reg_IO_L = GPIO_PINS_6  
#define User_hot_on() GPIOB->Reg_IO_H = GPIO_PINS_6   
#define User_09V() GPIOA->Reg_IO_L = GPIO_PINS_1      
#define User_12V() GPIOA->Reg_IO_H = GPIO_PINS_1   

//---User---

#ifdef Exist_LCD
    #define LCD_DC_L() GPIOA->Reg_IO_L = GPIO_PINS_10    //DC_GPIO
    #define LCD_DC_H() GPIOA->Reg_IO_H = GPIO_PINS_10   //PA10
#endif

#ifdef Exist_OLED
    #define OLED_DC_L() GPIOA->Reg_IO_L = GPIO_PINS_10    //DC_GPIO
    #define OLED_DC_H() GPIOA->Reg_IO_H = GPIO_PINS_10   //PA10
//    #define LCD_RES_L()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_H()
#endif

#ifdef Exist_LED
    #define GPIO_LED    GPIO_Pin_4      //PA04
    #define LED_L() GPIOA->Reg_IO_L = LED_R
    #define LED_H() GPIOA->Reg_IO_H = LED_R
    #define LEDR_L() GPIOC->Reg_IO_L = GPIO_PINS_2
    #define LEDR_H() GPIOC->Reg_IO_H = GPIO_PINS_2
#endif

#ifdef Exist_BZZ
    #define GPIO_BZZ    GPIO_Pin_5     //PB5
    #define BZZ_L() GPIOB->Reg_IO_L = GPIO_BZZ
    #define BZZ_H() GPIOB->Reg_IO_H = GPIO_BZZ
#endif

#ifdef Exist_KEY
    #define KEY_R() gpio_input_data_bit_read(GPIOC,GPIO_Pin_13)     //KEY,读
#endif

#ifdef Exist_HC595
    #define LATCH_CLOCK     GPIO_PINS_1         //门阀时钟
    #define SHIFT_CLOCK     GPIO_PINS_2         //移动时钟
    #define HC595_Data      GPIO_PINS_3         //数据
    
    #define LATCH_CLOCK_L() GPIOC->Reg_IO_L = LATCH_CLOCK
    #define LATCH_CLOCK_H() GPIOC->Reg_IO_H = LATCH_CLOCK
    #define SHIFT_CLOCK_L() GPIOC->Reg_IO_L = SHIFT_CLOCK
    #define SHIFT_CLOCK_H() GPIOC->Reg_IO_H = SHIFT_CLOCK
    #define HC595_Data_L() GPIOC->Reg_IO_L = HC595_Data
    #define HC595_Data_H() GPIOC->Reg_IO_H = HC595_Data
    
#endif

#ifdef Exist_HC138
    #define HC595_D1    GPIO_Pin_1
    #define HC595_D2    GPIO_Pin_2
    #define HC595_D3    GPIO_Pin_3         //数据
    
    #define HC138_D1_H() GPIOC->Reg_IO_H = HC595_D1
    #define HC138_D1_L() GPIOC->Reg_IO_L = HC595_D1
    #define HC138_D2_H() GPIOC->Reg_IO_H = HC595_D2
    #define HC138_D2_L() GPIOC->Reg_IO_L = HC595_D2
    #define HC138_D3_H() GPIOC->Reg_IO_H = HC595_D3
    #define HC138_D3_L() GPIOC->Reg_IO_L = HC595_D3
    
#endif

#ifdef Exist_DS18B20
    #define DS18B20_IO    GPIO_PINS_8
    #define DS18B20_Clock    GPIOC

    #define DS18B20_IO_H() DS18B20_Clock->Reg_IO_H = DS18B20_IO
    #define DS18B20_IO_L() DS18B20_Clock->Reg_IO_L = DS18B20_IO

    #define DS18B20_IO_R() gpio_input_data_bit_read(DS18B20_Clock,DS18B20_IO)     // 读
#endif

#ifdef Exist_STEP_Motor
    #define STEP_OUT1   GPIO_Pin_0
    #define STEP_OUT2   GPIO_Pin_1 
    #define STEP_OUT3   GPIO_Pin_2 
    #define STEP_OUT4   GPIO_Pin_3 
    #define STEP_Clock  GPIOC

    #define STEP_OUT_L(x) (STEP_Clock->Reg_IO_L = 0x0001 << (x))
    #define STEP_OUT_H(x) (STEP_Clock->Reg_IO_H = 0x0001 << (x))

#endif


/*  Init-Function    */

void LCD_GPIO_Init(int Set);

void LED_GPIO_Init(int Set);
void BZZ_GPIO_Init(int Set);

void HC138_GPIO_Init(int Set);
void HC595_GPIO_Init(int Set);

void KEY_GPIO_Init(int Set);

void User_GPIO_Init(int Set);	//!!!!

/*  other    */
void DS18B20_IO_Config(int Set);
void STEP_Motor (int Set);

#endif
