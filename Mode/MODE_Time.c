#include "MODE_Time.h"

/*
 * 防止被编译器优化，并且只能在本文件使用
 * while(a);和while(get(a));不一样
 *
*/


static Real_TIME_Type Real_TIME = {0};
static int SYNC_TIME_Fun (void);

int MODE_TIME_Init(int SET)
{
#ifdef Exist_SYS_TIME
    SYS_Time_Init(SET);
    return 0;
#else
    return 1;
#endif
    
}

// set 
void MODE_TIME_Set_Date_Fun(Caven_Date_Type time)
{
#ifdef Exist_SYS_TIME
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 2;
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
    retval.day = retval.SYS_Day % 30;
    retval.month = ((retval.SYS_Day / 30) % 12) + 1;
    retval.year = retval.SYS_Day / 365;
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

/* 
 * sync
 * SYNC_Flag = 0; BaseTIME -> Watch+Date
 * SYNC_Flag = 1; Watch+Date -> BaseTIME
 *
*/
int SYNC_TIME_Fun (void)
{
    int retval = 0;
#ifdef Exist_SYS_TIME
    SYS_BaseTIME_Type BaseTIME;
    int i,j,k;
    if (Real_TIME.SYNC_Flag == 1) 
    {
        i = API_Hourly_to_Seconds(Real_TIME.Watch);
        if (i == (Real_TIME.Watch.SYS_Sec % 86400))
        {
            i = 0;
            j = Real_TIME.Watch.SYS_Sec;
        }
        else
        {
            Real_TIME.Watch.SYS_Sec = i;        // 更新SYS_Sec
            j = 0;
        }
        k = i + j;
        BaseTIME.SYS_Sec = k;
        BaseTIME.SYS_Us = Real_TIME.Watch.time_us;
//        printf("SYNC_TIME : set i: %d,j: %d,sec：%d,us:%d \n",i,j,Real_TIME.TIME.SYS_Sec,Real_TIME.Watch.time_us);
        SYS_Time_Set(&BaseTIME);
    }
    else if(Real_TIME.SYNC_Flag == 2)
    {
        i = Real_TIME.Date.SYS_Day * 86400;
        j = API_Hourly_to_Seconds(Real_TIME.Watch);
        k = i + j;
        Real_TIME.Watch.SYS_Sec = k;        // 更新SYS_Sec
        BaseTIME.SYS_Sec = k;
        BaseTIME.SYS_Us = Real_TIME.Watch.time_us;
        SYS_Time_Set(&BaseTIME);
    }
    else
    {
        SYS_Time_Get(&BaseTIME);

        i = BaseTIME.SYS_Sec % 86400;
        Real_TIME.Watch = API_Seconds_to_Hourly (i);
        Real_TIME.Watch.time_us = BaseTIME.SYS_Us;
        Real_TIME.Watch.SYS_Sec = i;

        Real_TIME.Date.SYS_Day = i / 86400;
    }
    Real_TIME.SYNC_Flag = 0;
#else
    retval = -1;
#endif
    return retval;
}

// delay
#if 1
void MODE_Delay_Us(int num)
{
#ifdef Exist_SYS_TIME
    SYS_Delay_us(num);
#else
    while(1);
#endif
}
void MODE_Delay_Ms(int num)
{
#ifdef Exist_SYS_TIME
    SYS_Delay_ms (num);
#else
    while(1);
#endif
}
void MODE_Delay_S(int num)
{
#ifdef Exist_SYS_TIME
    for (int i = 0; i < num; i++)
    {
        SYS_Delay_ms (1000);
    }
#else
    while(1);
#endif
}
#endif


