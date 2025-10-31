/*
 * Base_Flash.h
 *
 */

#ifndef __FLASH_H_
#define __FLASH_H_

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#ifdef Exist_FLASH
    #define FLASH_PAGE_SIZE ((uint32_t)256)         //一页的大小
    #define FLASH_AREA_SIZE (FLASH_PAGE_SIZE*16)    //区大小
    #define FLASH_SIZE  ((uint32_t)(64*1024))       //Flash大小64K

    #define FLASH_DATA_END  ((uint32_t)0x08006000)              //掉电保存【数据区】结束地址
    #define FLASH_DATA      (FLASH_DATA_END - FLASH_PAGE_SIZE)  //掉电保存【数据区】只有一页(请确保这个位置不在程序段)

    #define FLASH_CODE      FLASH_DATA_END              //Flash存放【APP的代码区】
    #define FLASH_CODE_END  (0x08000000 + FLASH_SIZE)   //Flash存放【APP的代码区】一直到最大Flash

    #define FLASH_END       FLASH_CODE_END              //Flash最终地址
#endif

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define Fast_Flash(x)   *(__IO uint16_t*)(x)
#define Fast_8_Flash(x)   *(__IO uint8_t*)(x)


int Read_Flash(int Address);
char Clear_Flash_Page(int Addr);
char Clear_Flash_Area(int addr_start,int addr_end);

char Save_Flash(int Addr,const uint16_t *Data,int Lenght);

#endif
