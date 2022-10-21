#ifndef _BASE_H__
#define _BASE_H__

#include "items.h"              //功能使能、指定文件（因MCU而异）
#include "Base_Exist_GPIO.h"    //items的附属（LED、BZZ之类的）

#ifdef Exist_SYS_TIME          //这种保护不占内存，所以尽可能写
#include "Base_sys_time.h"
#endif

#ifdef Exist_UART
#include "Base_UART.h" 
#endif

#ifdef Exist_SPI
#include "Base_SPI.h" 
#endif

#ifdef Exist_IIC
#include "Base_IIC.h" 
#endif

#ifdef Exist_ADC
#include "Base_ADC.h" 
#endif

#ifdef Exist_PWM
#include "Base_PWM.h"
#endif

//Base文件没有结构体索引
#endif
