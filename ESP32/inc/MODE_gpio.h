#ifndef _MODE_GPIO__H_
#define _MODE_GPIO__H_

/*
    this is MODE_gpio.h file    
    这是个模板文件

    2023.9.13   
*/
#include "Items.h"
#include "Caven_Type.h"

#include "driver/gpio.h"
#ifdef Exist_GPIO
    #if (BOARD_NAME == ESP32_CAVEND)
        #define LED_T_IO    GPIO_NUM_2
        #define LCD_POW_IO  GPIO_NUM_2
        #define POW_KILL_IO GPIO_NUM_32
        #define KEY_IO      GPIO_NUM_36
        #define LED_T    1
    #elif (BOARD_NAME == EY1001)

    #endif
#endif

void User_GPIO_config(int gpiox,int pin,int set);
void User_GPIO_set(int gpiox,int pin,int set);
char User_GPIO_get(int gpiox,int pin);


#endif
