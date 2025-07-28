#ifndef _MODE_H__
#define _MODE_H__

#include "Items.h"
#ifdef MCU_SYS_FREQ
	#include "Base.h"
#endif
#include "API.h"

#if Exist_SYS_TIME
    #include "MODE_Time.h"
#endif

#if Exist_LCD
    #include "MODE_LCD.h"           // 显示输出
#endif
#if Exist_OLED
    #include "MODE_OLED.h"          // 显示输出
#endif

#if Exist_BZZ
    #include "MODE_BZZ.h"            // BZZ输出
#endif
#if Exist_LED
    #include "MODE_LED.h"            // LED输出
#endif

#if Exist_UART
    #include "MODE_UART.h"
#endif

#if Exist_Voice
    #include "MODE_MP3.h"
#endif
#if Exist_Steering_Engine            // 动力输出
    #include "Steering_engine.h"
#endif
#if Exist_Motor_Engine
    #include "Motor_engine.h"
#endif
#if Exist_Motor_BYJ
    #include "Motor_BYJ.h"
#endif
#if Exist_Motor_STEP
    #include "Motor_Step.h"
#endif

#if Exist_BUTTON
    #include "MODE_Button.h"        // 按键输入
#endif

#if Exist_FindLine
    #include "Tracing.h"            // 外部需要模块才能输入的条件
#endif

#if Exist_Ultrasonic
    #include "MODE_Ultrasonic.h"
#endif

#if Exist_HC595
    #include "IC_HC595.h"           // 驱动输出
#endif
#if Exist_HC138
    #include "IC_HC138.h"
#endif

#if Exist_DS18B20
    #include "IC_DS18B20.h"
#endif

#if Exist_DHT11
    #include "IC_DHT11.h"
#endif

#if Exist_HX711
#include "IC_HX711.h"
#endif

#if Exist_MLX90614
    #include "IC_MLX90614.h"
#endif

#if Exist_RTC_Clock
#include "IC_RTC8564.h"
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
#if Exist_LCD
    int (*LCD)(int Set);
#endif
	
#if Exist_OLED
    int (*OLED)(int Set);
#endif
	
#if Exist_SYS_TIME
    int (*TIME)(int Set);
#endif

#if Exist_UART
    int (*UART)(char Channel,int Baud,int Set);
#endif
#if Exist_USB
    int (*USB)(int Set);
#endif
    
#if Exist_LED
    void (*LED)(int Set);
#endif
#if Exist_BZZ
    void (*BZZ)(int Set);
#endif
#if Exist_HC595
    void (*HC_595)(int Set);
#endif
#if Exist_HC138
    void (*HC_138)(int Set);
#endif

#if Exist_DS18B20
    int (*DS18B20)(int gpiox,int pin,int Set);
#endif

#if Exist_DHT11
    int (*DHT11)(int gpiox,int pin,int Set);
#endif

#if Exist_HX711
	int (*HX711) (int gpiox,int SCK_pin,int DOUT_pin,int Set);
#endif

#if Exist_BUTTON
    void (*Button)(char Channel,int Set);
#endif
#if Exist_Ultrasonic
    int (*Ultrasonic)(int Set);
#endif

#if Exist_Voice
    int (*Voice) (int Set);
#endif
#if Exist_Steering_Engine
    int (*Steering_Engine) (int Set);
#endif
#if Exist_Motor_BYJ
	void (*Motor_BYJ) (int Set);
#endif
#if Exist_Motor_STEP
    void (*Motor_STEP) (int gpiox,int PUL_pin,int DIR_pin,int Set);
#endif

#if Exist_ADC
    int (*Base_ADC) (int Set);
#endif

};

struct _Mode_Use
{
    void (*Debug_Out_Way)(uint8_t *data,int Length);    // 提倡写一个Debug

#if Exist_SYS_TIME
    MODE_TIME_Way TIME;
#endif
#if Exist_UART
    MODE_UART_Way UART;
#endif
#if Exist_USB
    MODE_USB_Way USB_HID;
#endif 
#if Exist_LCD
    MODE_LCD_Way LCD;
#endif
#if Exist_OLED
    MODE_OLED_Way OLED;
#endif
#if Exist_LED
    MODE_LED_Way LED;
#endif
#if Exist_BZZ
    MODE_BZZ_Way BZZ;
#endif
#if Exist_HC595
    MODE_HC595_Way HC595;
#endif
#if Exist_HC138
    MODE_HC138_Way HC138;
#endif
#if Exist_DS18B20
    MODE_DS18B20_Way DS18B20;
#endif
#if Exist_DHT11
    MODE_DHT11_Way DHT11;
#endif
#if Exist_HX711
	MODE_HX711_Way HX711;
#endif

#if Exist_BUTTON
    MODE_Button_Way Button;
#endif
#if Exist_Ultrasonic
    MODE_Ultrasonic_Way Ultrasonic;
#endif

#if Exist_Voice
    MODE_MP3_Way MP3;
#endif
#if Exist_Steering_Engine
    MODE_Steering_Engine_Way Steering_Engine;
#endif
#if Exist_Motor_BYJ
    MODE_Motor_BYJ_Way Motor_BYJ;
#endif
#if Exist_Motor_STEP
    MODE_Motor_Step_Way Motor_Step;
#endif

#if Exist_ADC
    MODE_USER_ADC_Way Base_ADC;
#endif
};

void Mode_Index(void);  // 初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)
void Debug_Out(uint8_t *data,int Length);

#endif
