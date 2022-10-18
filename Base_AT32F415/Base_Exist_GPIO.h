#ifndef __Exist_GPIO__H_
#define __Exist_GPIO__H_

#include "Items.h"

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

#ifdef Exist_LCD
    #define LCD_DC_Clr() GPIOA->clr = GPIO_PINS_10    //DC_GPIO
    #define LCD_DC_Set() GPIOA->scr = GPIO_PINS_10   //PA10
//    #define LCD_RES_Clr()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_Set()
#endif

#ifdef Exist_OLED

    #define OLED_DC_Clr() GPIOA->clr = GPIO_PINS_10    //DC_GPIO
    #define OLED_DC_Set() GPIOA->scr = GPIO_PINS_10   //PA10

//    #define LCD_RES_Clr()                                     //RES               //Caven 使用硬件复位
//    #define LCD_RES_Set()
#endif

#ifdef Exist_LED
    #define LED_Clr() GPIOA->clr = GPIO_PINS_4       //LED
    #define LED_Set() GPIOA->scr = GPIO_PINS_4      //PA04
    #define LEDR_Clr() GPIOC->clr = GPIO_PINS_2      //PA04
    #define LEDR_Set() GPIOC->scr = GPIO_PINS_2      //PA04
#endif

#ifdef Exist_BZZ
    #define BZZ_Clr() GPIOB->clr = GPIO_PINS_5    //BZZ
    #define BZZ_Set() GPIOB->scr = GPIO_PINS_5   //PB05
#endif

#ifdef Exist_KEY
    #define KEY_IN() GPIO_ReadInputDataBit(GPIOC,GPIO_PINS_13)     //KEY,读
#endif

#ifdef Exist_HC595
    #define LATCH_CLOCK     GPIO_PINS_1         //门阀时钟
    #define SHIFT_CLOCK     GPIO_PINS_2         //移动时钟
    #define HC595_Data      GPIO_PINS_3         //数据
    
    #define LATCH_CLOCK_Clr() GPIOC->clr = LATCH_CLOCK
    #define LATCH_CLOCK_Set() GPIOC->scr = LATCH_CLOCK
    #define SHIFT_CLOCK_Clr() GPIOC->clr = SHIFT_CLOCK
    #define SHIFT_CLOCK_Set() GPIOC->scr = SHIFT_CLOCK
    #define HC595_Data_Clr() GPIOC->clr = HC595_Data
    #define HC595_Data_Set() GPIOC->scr = HC595_Data
    
#endif

#ifdef Exist_HC138
    #define HC595_D1    GPIO_PINS_1
    #define HC595_D2    GPIO_PINS_2
    #define HC595_D3    GPIO_PINS_3         //数据
    
    #define HC138_D1_H() GPIOC->scr = HC595_D1
    #define HC138_D1_L() GPIOC->clr = HC595_D1
    #define HC138_D2_H() GPIOC->scr = HC595_D2
    #define HC138_D2_L() GPIOC->clr = HC595_D2
    #define HC138_D3_H() GPIOC->scr = HC595_D3
    #define HC138_D3_L() GPIOC->clr = HC595_D3
    
#endif


/*  Init-Function    */

void LCD_GPIO_Init(int Set);

void LED_GPIO_Init(int Set);
void BZZ_GPIO_Init(int Set);

void HC138_GPIO_Init(int Set);
void HC595_GPIO_Init(int Set);
void GXIC_GPIO_Init(int Set);

void KEY_GPIO_Init(int Set);




#endif
