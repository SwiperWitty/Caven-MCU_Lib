#include "MODE_Time.h"

/*
 *  防止被编译器优化，并且只能在本文件使用
 *
*/


static Real_TIME_Type Real_TIME = {0};
struct tm mode_date;
static int time_enable = 0;
static int SYNC_TIME_Fun (void);

int MODE_TIME_Init(int Set)
{
    time_enable = SET;
#if Exist_SYS_TIME
    SYS_Time_Init(SET);
	Real_TIME.SYNC_Flag = 0;
	Real_TIME.date = &mode_date;
    return 0;
#else
    return 1;
#endif
}

void MODE_TIME_Set_BaseTIME (Caven_BaseTIME_Type time)
{
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return ;
    }
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 1;
    Real_TIME.Time = time;
    SYNC_TIME_Fun ();
#endif
}

Caven_BaseTIME_Type MODE_TIME_Get_BaseTIME (void)
{
    Caven_BaseTIME_Type retval = {0};
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return retval;
    }
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 0;
    SYNC_TIME_Fun ();
    
    retval = Real_TIME.Time;
#endif
    return retval;
}

// timezone_s 为时区参数(s秒)，例如北京时间 timezone_s = 8 * 60 * 60
struct tm MODE_TIME_Get_Date (int timezone_s)
{
    struct tm retval = {0};
#if Exist_SYS_TIME
    if (time_enable == 0 || Real_TIME.date == NULL) {
        return retval;
    }
    struct tm *time_info;
    if(timezone_s >= 86400)
    {
        timezone_s = 86400;
    }
    time_t utc = Real_TIME.Time.SYS_Sec + timezone_s;
    while (Real_TIME.SYNC_Flag);
    Real_TIME.SYNC_Flag = 0;
    SYNC_TIME_Fun ();
    time_info = localtime(&utc);
	memcpy(Real_TIME.date,time_info,sizeof(struct tm));
    Real_TIME.date->tm_year += 1900;
    Real_TIME.date->tm_mon += 1;
    retval = *Real_TIME.date;
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
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return retval;
    }
    SYS_BaseTIME_Type time;
    if (Real_TIME.SYNC_Flag == 1) 
    {
        memcpy(&time,&Real_TIME.Time,sizeof(SYS_BaseTIME_Type));
        SYS_Time_Set(&time);
    }
    else
    {
        SYS_Time_Get(&time);
        memcpy(&Real_TIME.Time,&time,sizeof(SYS_BaseTIME_Type));
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
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return ;
    }
    SYS_Delay_us(num);
#endif
}
void MODE_Delay_Ms(int num)
{
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return ;
    }
    SYS_Delay_ms (num);
#endif
}
void MODE_Delay_S(int num)
{
#if Exist_SYS_TIME
    if (time_enable == 0) {
        return ;
    }
    for (int i = 0; i < num; i++)
    {
        SYS_Delay_ms (1000);
    }
#endif
}
#endif


