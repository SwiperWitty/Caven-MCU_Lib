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

#ifndef U8
    #define U8  unsigned char
    #define S8  signed char
    #define U16 unsigned short
    #define S16 signed short
    #define U32 unsigned int
    #define S32 signed int
    #define U64 unsigned long long

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

typedef struct
{
    uint32_t SYS_Sec;
    uint32_t SYS_Us;        // 这里最大 1000 000
}Caven_BaseTIME_Type;

// 颜色 4byte
typedef struct
{
    U8 REG;
    U8 GREEN;
    U8 BULE;
}Caven_Color_Type;


// Function
/*
	无传参函数指针
*/
typedef void (*V_pFun) (void);
/*
	有传参函数指针
*/
typedef void (*D_pFun) (void *data);
/*
	带数量传数据函数指针
*/
typedef void (*Send_pFun) (U8 *data,int length);
/*
	有通道函数指针
*/
typedef void (*ND_pFun) (int num,void *data);

/*
	这个是给APP做的管理结构体
1)app_ID 任务ID;layer当前的app的界面;cursor当前界面的光标坐落的选项
2)str_switch 是否需要更新str;string的主体,是初始化时提供的内存。
3)p_Data 绑定一个地址，地址内容是需要传给当前APP的数据
4)Watch 给app传递系统时间
*/
typedef struct
{
    int app_ID;             // APP任务
	int type;
    char layer;             // 层级
    char cursor;            // 光标位置
	int offset;
    char str_switch;
	char *string;           // 输出字符
    void *p_Data;           // 输入指针
    Caven_BaseTIME_Type Watch;  // 时间(里面有utc)
}Caven_App_Type;

/*
	x,y 是输入量
	botton 是按键量
	value 是其他数据输入 
*/
typedef struct
{
    int Control_x;
    int Control_y;
    int Control_botton;
    void *Control_value;
}Caven_Control_Type;

#endif
