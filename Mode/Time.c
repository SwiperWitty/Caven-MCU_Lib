#include "time.h"

volatile static int Delay_Time = 0;
volatile static char Daley_Falg = 0;
//防止被编译器优化，并且只能在本文件使用
/*
 * while(a);
 * 和
 * while(get(a));
 * 不一样
 */


struct _SYS_Time SYS_Time = {
    .Watch.hour = 23,
    .Watch.minutes = 59,
    .Watch.second = 56,
};

void Time_Init(int SET)
{
#ifdef Exist_SYS_TIME
    Sys_Time_Init (SET);

#endif
}

void Set_TIME (int second)
{
#ifdef Exist_SYS_TIME
    SET_SysTick((U64)second*(SystemCoreClock));

#endif
}

int Get_TIME (void)
{
#ifdef Exist_SYS_TIME
    int Time = 0;
    U64 temp = (GET_SysTick() / SystemCoreClock);
    Time =  temp % 86400;
    if(temp / 86400)        //下一天
    {
        Set_TIME (Time);    //重设时间戳

    }
    return Time;
#endif
}

void Delay_10us(int num)
{

}

void Delay_ms(int num)
{

}

void Delay_S(char num)
{

}
