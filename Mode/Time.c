#include "time.h"

static int Delay_Time;
static char Daley_Falg = 0;

struct _SYS_Time SYS_Time = {
        .Date.day = 0,
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
static void Hourly_Handle(struct Caven_Watch *Item)         //此函数仅供 中断 调用
{
    Item->time_num++;
    if (Item->time_num > 100000)
    {
        Item->time_num = 1;
        Item->second++;
        if (Item->second > 59)
        {
            Item->second = 0;
            Item->minutes++;
            if (Item->minutes > 59)
            {
                Item->minutes = 0;
                Item->hour++;
                if (Item->hour > 23)
                {
                    Item->hour = 0;
                    Item->date = !Item->date;
                }
            }
        }
    }
}

#ifdef Exist_SYS_TIME
void SYS_Time_Interrupt()                       //这是中断（定时器/滴答）
{
    #ifdef Exist_SYS_Time_Falg                  //有中断标志位
        if (SYS_Time_Interrupt_Flag() != 0)
        {
            SYS_Time_Interrupt_FlagClear();
            Hourly_Handle(&SYS_Time.Watch);
            if (Daley_Falg)
                Delay_Time++;
        }
    #else
        Hourly_Handle(&SYS_Time.Watch);
        if (Daley_Falg)
            Delay_Time++;
    #endif
}
#endif

void Delay_10us(int num)
{
    Delay_Time = 0;
    Daley_Falg = 1;
    while(Delay_Time < num);
    Daley_Falg = 0;
}

void Delay_ms(int num)
{
    int n = num;
    while((n--) > 0)
    {
        Delay_Time = 0;
        Daley_Falg = 1;
        while(Delay_Time < 100);
        Daley_Falg = 0;
    };
}

void Delay_S(char num)
{
    int n = num;
    while((n--) > 0)
    {
        Delay_ms(1000);
    };
}
