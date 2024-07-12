#ifndef _MODE_ST7789_DEVER_H_
#define _MODE_ST7789_DEVER_H_

#include "stdint.h"
#include "string.h"


#ifdef CONFIG_IDF_TARGET_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

    #define PIN_LCD_MOSI    (13) 
    #define PIN_LCD_MISO    (12)
    #define PIN_LCD_CLK     (14)
    #define PIN_LCD_CS      (-1)        /* -1 是不使用	*/

    #define PIN_LCD_DC      (33)
    #define PIN_LCD_RST     (32)
    #define PIN_LCD_BCKL    NULL        /* 直接接到vdd */

    #ifdef CONFIG_IDF_TARGET_ESP32
        #define LCD_HOST    SPI2_HOST 
    #else
        #define LCD_HOST    SPI3_HOST 
    #endif
    #define Exist_LCD
#else
#include "Base.h"

    #define PIN_LCD_CS      (-1)        /* -1 是不使用	*/

    #define PIN_LCD_DC      (10)        // PA10
    #define PIN_LCD_RST     (0)         // PB0
    #define PIN_LCD_BCKL    (-1)        /* 直接接到vdd */

#endif


/*

*/

int MODE_st7789_dever_Init(int set);

int MODE_st7789_dever_Set_TargetModel(char set);
int MODE_st7789_dever_Set_Horizontal(char set);

void MODE_st7789_dever_Set_Address (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void MODE_st7789_dever_WR_Data (uint16_t data);
void MODE_st7789_dever_Send_Data (uint8_t *data,int num);

#endif
