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

#ifdef Exist_UART
    #include "uart_handle.h"
#endif

#ifdef Exist_HC595
#include "HC595.h"			//输出
#endif

#ifdef Exist_Steering_Engine
#include "Steering_engine.h"
#endif
#ifdef Exist_Motor
#include "motor.h"			//动力输出
#endif

#ifdef Exist_KEY
#include "key.h"
#endif


#include "Tracing.h"		//外部物理直接输入条件

#include "Ultrasonic.h"
#include "mlx90614.h"		//外部需要模块才能输入的条件

#include "time.h"


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
#ifdef Exist_LCD
	void (*LCD)(int SET);
#endif
#ifdef Exist_SYS_TIME
	void (*TIME)(int SET);
#endif

#ifdef Exist_UART
	void (*UART)(char Channel,int Baud,int SET);
#endif
#ifdef Exist_LED
	void (*LED)(int SET);
#endif

};

struct _Mode_User
{
    void (*Debug_Out)(const char *String);          //提倡写一个Debug
#ifdef Exist_LCD
    struct _LCD LCD;
#endif
#ifdef Exist_SYS_TIME
    struct Delay_ Delay;
#endif
#ifdef Exist_UART
    struct Uart_ UART;
#endif
#ifdef Exist_LED
    struct LED_ LED;
#endif
};

void Mode_Index(void); //初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)
void Debug_Out(const char *String);

#endif
