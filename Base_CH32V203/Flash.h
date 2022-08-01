/*
 * Flash.h
 *
 *  Created on: 2022年8月1日
 *      Author: gxwl
 */

#ifndef __FLASH_H_
#define __FLASH_H_

#include "Items.h"

#ifdef Exist_FLASH
    #define FLASH_AREA          //Flash起始可用区

    #define FLASH_DATA      FLASH_AREA      //Flash存放掉电保存数据区
    #define FLASH_DATA_END  (FLASH_DATA + 1024)

    #define FLASH_CODE      FLASH_DATA_END  //Flash存放APP的代码区
    #define FLASH_CODE_END  (FLASH_CODE + 64 * 1024)
#endif

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#endif
