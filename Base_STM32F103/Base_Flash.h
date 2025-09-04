#ifndef BASE_FLASH_H__
#define BASE_FLASH_H__

#ifdef DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#if Exist_FLASH 

#define FLASH_START_ADDR        ((uint32_t)0x08000000)
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE       (0x800)	// 2048
  #define FLASH_END_ADDR          ((uint32_t)0x08040000)
#else
  #define FLASH_PAGE_SIZE       (0x400)	// 1024
  #define FLASH_END_ADDR          ((uint32_t)0x08010000)
#endif

#endif

int Base_Flash_Erase (int addr,int len);
int Base_Flash_Read (void *data,int addr,int len);
int Base_Flash_Write (void *data,int addr,int len);

#endif
