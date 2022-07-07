#ifndef _MODE_H__
#define _MODE_H__

#include "lcd.h"			//显示输出

#include "BZZ.h"
#include "HC595.h"			//输出

#include "Steering_engine.h"
#include "motor.h"			//动力输出

#include "key.h"
#include "encoder.h"
#include "Tracing.h"		//外部物理直接输入条件

#include "Ultrasonic.h"
#include "mlx90614.h"		//外部需要模块才能输入的条件

#include "time.h"
#include "uart_handle.h"


extern struct _LCD LCD;

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
	struct _LCD LCD;
	struct Delay_ Delay;
};

void Mode_Index(void); //初始化Mode函数索引,真正功能的初始化请调用结构体中函数指针(Mode_Init)

#endif
