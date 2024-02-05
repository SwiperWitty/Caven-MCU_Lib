#ifndef _CAVEN_TYPE__H_
#define _CAVEN_TYPE__H_

/*
 * Caven_Type.h file
 * 这是一个纯C的.h文件，是各个底层.h的 “共同语言”，上层管理函数的.h不需要包含
 *
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "math.h"

/*
 *          SDK->Items
 *                      \\
 *                      -->Base     -->Mode
 *                      //          //
 *  C(Lib)->Caven_Type->        API
*/

#ifndef u8
    #define U8  unsigned char
    #define S8  signed char
    #define U16 unsigned short
    #define S16 signed short
    #define U32 unsigned int
    #define S32 signed int
    #define U64 unsigned long long

    #define u8  unsigned char
    #define s8  signed char
    #define u16 unsigned short
    #define s16 signed short
    #define u32 unsigned int
    #define s32 signed int
    #define u64 unsigned long long

#endif 

#ifndef ENABLE
    #if (!defined (STM32F10X_HD) && !defined (STM32F10X_MD) && !defined (STM32F40X_MD))
    #define DISABLE 0
    #define ENABLE (!DISABLE)
    #endif
#endif

#ifndef MAX
    #define MAX(a,b)    ((a)>(b))?(a):(b)               // 比较函数返回最大值，防止过小
    #define MIN(a,b)    ((a)<(b))?(a):(b)               // 比较函数返回最小值，防止过大
#endif

#define DESTROY_DATA(X,N) memset((X),0,(N))             // 销毁 的地址 (X)  长度 (N)

#define BUFF_MAX 360


// 日期 8byte
typedef struct
{
    U16 year;
    U8 month;
    U8 day;
    int Days;
}Caven_Date_Type;

// 时间 8byte
typedef struct
{
    U8 hour;
    U8 minutes;
    U8 second;
    U32 time_us;            // 这里最大 1000 000
}Caven_Watch_Type;

// 系统运行总时长 8byte
typedef struct
{
    U32 SYS_Sec;
    U32 SYS_Us;             // 这里最大 1000 000
}Caven_BaseTIME_Type;

// 颜色 4byte
typedef struct
{
    U8 REG;
    U8 GREEN;
    U8 BULE;
}Caven_Color_Type;


// Function

typedef void (*V_pFun) (void);
typedef void (*D_pFun) (void *data);
typedef void (*Send_pFun) (U8 *data,int length);

typedef struct
{
    int app_ID;
    void *p_Data;
    char *string;
    Caven_Watch_Type Watch;
    Caven_BaseTIME_Type BaseTIME;
}Caven_App_Type;

#endif
