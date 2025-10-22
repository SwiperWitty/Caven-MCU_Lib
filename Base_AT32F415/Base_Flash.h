#ifndef BASE_FLASH_H__
#define BASE_FLASH_H__

#ifdef DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#if Exist_FLASH 

#define FLASH_START_ADDR        ((uint32_t)0x08000000)
#ifdef AT32F415RCT7
#define FLASH_PAGE_SIZE			((uint16_t)0x800)	// 2048
#define FLASH_END_ADDR          ((uint32_t)0x08040000)
#elif AT32F415CBT7
#define FLASH_PAGE_SIZE			((uint16_t)0x400)	// 1024
#define FLASH_END_ADDR          ((uint32_t)0x08020000)
#endif

#endif

int Base_Flash_Erase (int addr,int len);
int Base_Flash_Read (void *data,int addr,int len);
int Base_Flash_Write (void *data,int addr,int len);

#endif
