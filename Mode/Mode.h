#ifndef _MODE_H__
#define _MODE_H__

#include "Base.h"
#include "API.h"

#ifdef Exist_SYS_TIME
    #include "MODE_Time.h"
#endif

#ifdef Exist_LCD
    #include "MODE_LCD.h"            //显示输出
#endif
#ifdef Exist_OLED
    #include "MODE_OLED.h"               //显示输出
#endif

#ifdef Exist_BZZ
    #include "MODE_BZZ.h"            //BZZ输出
#endif
#ifdef Exist_LED
    #include "MODE_LED.h"            //LED输出
#endif

#ifdef Exist_UART
    #include "MODE_UART.h"
#endif

#ifdef Exist_HC595
    #include "HC595.h"          //驱动输出
#endif
#ifdef Exist_HC138
    #include "HC138.h"
#endif

#ifdef Exist_DS18B20
    #include "DS18B20.h"
#endif

#ifdef Exist_Voice
    #include "MODE_MP3.h"
#endif
#ifdef Exist_Steering_Engine            //动力输出
    #include "Steering_engine.h"
#endif
#ifdef Exist_Motor
    #include "MODE_motor.h"
#endif

#ifdef Exist_BUTTON
    #include "MODE_Button.h"            //按键输入
#endif

#ifdef Exist_FindLine
    #include "Tracing.h"        //外部需要模块才能输入的条件
#endif

#ifdef Exist_Ultrasonic
    #include "MODE_Ultrasonic.h"
#endif

#ifdef Exist_MLX90614
    #include "mlx90614.h"
#endif
#ifdef Exist_RTC_Clock
#include "Mode_RTC8564.h"
#endif


/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

extern struct _Mode_Init Mode_Init;
extern struct _Mode_Use Mode_Use;


struct _Mode_Init
{
    char empty;
#ifdef Exist_LCD
    int (*LCD)(int SET);
#endif
	
#ifdef Exist_OLED
    int (*OLED)(int SET);
#endif
	
#ifdef Exist_SYS_TIME
    int (*TIME)(int SET);
#endif

#ifdef Exist_UART
    int (*UART)(char Channel,int Baud,int SET);
#endif
#ifdef Exist_USB
    int (*USB)(int SET);
#endif
    
#ifdef Exist_LED
    void (*LED)(int SET);
#endif
#ifdef Exist_BZZ
    void (*BZZ)(int SET);
#endif
#ifdef Exist_HC595
    void (*HC_595)(int SET);
#endif
#ifdef Exist_HC138
    void (*HC_138)(int SET);
#endif

#ifdef Exist_DS18B20
    int (*DS18B20)(int SET);
#endif

#ifdef Exist_BUTTON
    void (*Button)(char Channel,int SET);
#endif
#ifdef Exist_Ultrasonic
    int (*Ultrasonic)(int SET);
#endif

#ifdef Exist_Voice
    int (*Voice) (int Set);
#endif
#ifdef Exist_Steering_Engine
    int (*Steering_Engine) (int Set);
#endif

#ifdef Exist_ADC
    int (*User_ADC) (int Set);
#endif

};

struct _Mode_Use
{
    void (*Debug_Out_Way)(uint8_t *data,int Length);    // 提倡写一个Debug

#ifdef Exist_SYS_TIME
    MODE_TIME_Way TIME;
#endif
#ifdef Exist_UART
    MODE_UART_Way UART;
#endif
#ifdef Exist_USB
    MODE_USB_Way USB_HID;
#endif 
#ifdef Exist_LCD
    MODE_LCD_Way LCD;
#endif
#ifdef Exist_OLED
    MODE_OLED_Way OLED;
#endif
#ifdef Exist_LED
    MODE_LED_Way LED;
#endif
#ifdef Exist_BZZ
    MODE_BZZ_Way BZZ;
#endif
#ifdef Exist_HC595
    MODE_HC595_Way HC595;
#endif
#ifdef Exist_HC138
    MODE_HC138_Way HC138;
#endif
#ifdef Exist_DS18B20
    MODE_DS18B20_Way DS18B20;
#endif
#ifdef Exist_BUTTON
    MODE_Button_Way Button;
#endif
#ifdef Exist_Ultrasonic
    MODE_Ultrasonic_Way Ultrasonic;
#endif

#ifdef Exist_Voice
    MODE_MP3_Way MP3;
#endif
#ifdef Exist_Steering_Engine
    MODE_Steering_Engine_Way Steering_Engine;
#endif

#ifdef Exist_ADC
    MODE_USER_ADC_Way USER_ADC;
#endif
};

void Mode_Index(void); //初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)
void Debug_Out(uint8_t *data,int Length);

#endif
