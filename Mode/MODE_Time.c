#include "MODE_Time.h"

/*
 * 防止被编译器优化，并且只能在本文件使用
 * while(a);和while(get(a));不一样
 *
*/



Real_TIME_Type Real_TIME = {
    .Watch.hour = 23,
    .Watch.minutes = 59,
    .Watch.second = 56,
    
    .Date.Days = 0,

    .SYNC_Flag = 0,
};


int MODE_TIME_Init(int SET)
{
#ifdef Exist_SYS_TIME
    SYS_Time_Init(SET);
    return 0;
#endif
    return 1;
}

// set 
void MODE_TIME_Set_Date_Fun(Caven_Date_Type time)
{
#ifdef Exist_SYS_TIME
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 1;
    Real_TIME.Date = time;
    SYNC_TIME_Fun ();
#endif
}
void MODE_TIME_Set_Watch_Fun(Caven_Watch_Type time)
{
#ifdef Exist_SYS_TIME
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 1;
    Real_TIME.Watch = time;
    SYNC_TIME_Fun ();
#endif
}

// get 
Caven_Date_Type MODE_TIME_Get_Date_Fun(void)
{
    Caven_Date_Type retval = {0};
#ifdef Exist_SYS_TIME
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 0;
    SYNC_TIME_Fun ();
    retval = Real_TIME.Date;
#endif
    return retval;
}

Caven_Watch_Type MODE_TIME_Get_Watch_Fun(void)
{
    Caven_Watch_Type retval = {0};
#ifdef Exist_SYS_TIME
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 0;
    SYNC_TIME_Fun ();
    retval = Real_TIME.Watch;
#endif
    return retval;
}

Caven_Watch_Type  MODE_TIME_Get_differ_Fun(Caven_Watch_Type temp_a,Caven_Watch_Type temp_b)
{
    Caven_Watch_Type retval = {0};
#ifdef Exist_SYS_TIME
    int i,j,k;
    i = API_Hourly_to_Seconds(temp_a);
    j = API_Hourly_to_Seconds(temp_b);
    if (i > j)
    {
        j += 86400;
    }
    k = j - i;
    
    i = temp_a.time_us;
    j = temp_b.time_us;
    if (i > j)
    {
        j += 1000000;
        k--;
    }
    retval = API_Seconds_to_Hourly (k);

    k = j - i;
    // MIN(k,1000000);
    retval.time_us = k;
#endif
    return retval;
}

/* 
 * sync
 * SYNC_Flag = 0; TIME -> Watch Date
 * SYNC_Flag = 1; Watch Date -> TIME
 *
*/
int SYNC_TIME_Fun (void)
{
    int retval = 0;
#ifdef Exist_SYS_TIME
    int i,j,k;
    if (Real_TIME.SYNC_Flag) 
    {
        i = API_Hourly_to_Seconds(Real_TIME.Watch);
        j = Real_TIME.Date.Days * 86400;
        k = i + j;
        Real_TIME.TIME.SYS_Sec = k;
        Real_TIME.TIME.SYS_Us = Real_TIME.Watch.time_us;

        SYS_Time_Set(&Real_TIME.TIME);
    }
    else
    {
        SYS_Time_Get(&Real_TIME.TIME);

        i = Real_TIME.TIME.SYS_Sec % 86400;
        Real_TIME.Watch = API_Seconds_to_Hourly (i);
        Real_TIME.Watch.time_us = Real_TIME.TIME.SYS_Us;

        Real_TIME.Date.Days = Real_TIME.TIME.SYS_Sec / 86400;
        
    }
    Real_TIME.SYNC_Flag = 0;
#else
    retval = -1;
#endif
    return retval;
}

// delay
#if 1
void Delay_Us(int num)
{
#ifdef Exist_SYS_TIME
    SYS_Delay_us(num);
#elif
    while(1);
#endif
}
void Delay_Ms(int num)
{
#ifdef Exist_SYS_TIME
    SYS_Delay_ms (num);
#elif
    while(1);
#endif
}
void Delay_S(int num)
{
#ifdef Exist_SYS_TIME
    for (int i = 0; i < num; i++)
    {
        SYS_Delay_ms (1000);
    }
#elif
    while(1);
#endif
}
#endif


