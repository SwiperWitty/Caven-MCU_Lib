#ifndef _MODE_H__
#define _MODE_H__
#include "Base.h"

#ifdef Exist_LCD
    #include "LCD.h"            //显示输出
#endif

#ifdef Exist_OLED
    #include "OLED.h"            //显示输出
#endif

#ifdef Exist_BZZ
    #include "BZZ.h"            //显示输出
#endif
#ifdef Exist_LED
    #include "LED.h"            //显示输出
#endif

#include "HC595.h"			//输出

#include "Steering_engine.h"
#include "motor.h"			//动力输出

#include "key.h"
#include "Tracing.h"		//外部物理直接输入条件

#include "Ultrasonic.h"
#include "mlx90614.h"		//外部需要模块才能输入的条件

#include "time.h"
#include "uart_handle.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

extern struct _Mode_Init Mode_Init;
extern struct _Mode_User Mode_User;


struct _Mode_Init
{
	void (*LCD)(int SET);	// FunctionalState 是一个枚举，你可以看成bit,只有0/1.做使能位
	void (*TIME)(int SET);
    void (*UART)(char Channel,int Baud,int SET);
};

struct _Mode_User
{
    #ifdef Exist_LCD
	struct _LCD LCD;
    #endif
    #ifdef Exist_SYS_TIME
	struct Delay_ Delay;
    #endif
};

void Mode_Index(void); //初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)

#endif
