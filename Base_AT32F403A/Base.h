#ifndef _BASE_H__
#define _BASE_H__

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#include "uart.h"
#include "Base_SPI.h"
#include "IIC.h"

#include "adc.h"

#include "sys_time.h"
#include "PWM.h"

#include "Base_Exist_GPIO.h"

/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/
/*
    底层（Base）中有不少东西让人迷惑，他们大致分布在 system_xx32xxx.c 以及 xx32xxxx_it.h 和 xx32xxxx.h
    如果查阅这里，问题都没有解决，那么这就是一个不该由你解决的问题。
  Base层只能和 MCU SDK有关
*/


#endif
