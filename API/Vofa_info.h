#ifndef _VOFA_INFO__H_
#define _VOFA_INFO__H_

/*
    Vofa_info.h
    C语言环境即可运行
    2024.1.1

*/

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#endif

int Vofa_JustFloat_Init_Fun (Send_pFun send_fun);
int Vofa_JustFloat_Show_Fun (char ch_num,float *f_num);

#endif
