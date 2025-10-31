#ifndef _BASE_H__
#define _BASE_H__

#ifdef DEFAULT
#include "Items.h"          // 默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"     // 自行设置功能，一般出现在本地文件的User中
#endif

/****************/

#include "Base_GPIO.h"      // items的附属（LED、BZZ之类的）

#if Exist_SYS_TIME          // 这种保护不占内存，所以尽可能写
#include "Base_Sys_Time.h"
#endif

#if Exist_UART
#include "Base_UART.h" 
#endif

#if Exist_SPI
#include "Base_SPI.h" 
#endif

#if Exist_IIC
#include "Base_IIC.h" 
#endif

#if Exist_USB
#include "Base_USB.h" 
#endif

#if Exist_CAN
#include "Base_CAN.h" 
#endif

#if Exist_ADC
#include "Base_ADC.h" 
#endif

#if Exist_DAC
#include "Base_DAC.h" 
#endif

#if Exist_PWM
#include "Base_PWM.h"
#endif

#if Exist_CAPTURE
#include "Base_Capture.h"
#endif

#if Exist_FLASH
#include "Base_Flash.h"
#endif

// Base文件没有结构体索引
#endif
