#ifndef __BASE_GPIO__H_
#define __BASE_GPIO__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif


/*
                GPIO_Init(Exist_GPIO)->
    SDK->Items->                       BASE->
                Peripheral(UART、ADC)->      \\
                                              -->[XXX]->MODE
                                             //
                        C(Lib)->Caven->API->
*/


#define READ_Config		0
#define WRITE_Config	1


/*  Init-Function    */
void User_GPIO_config(int gpiox,int pin,int set);
void User_GPIO_set(int gpiox,int pin,int set);
char User_GPIO_get(int gpiox,int pin);

// end 
#endif
