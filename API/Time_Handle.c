#include "Time_Handle.h"

//这个是 秒 级的转换，基于Caven_Watch
int Hourly_to_Seconds(struct Caven_Watch Item)
{
    int temp;
    temp = Item.second + (Item.minutes * 60) + (Item.hour * 3600); //设置的超时时差（秒级）
    temp = MIN(temp,86400);        // 60 * 60 * 24 = 86400
    return temp;
}

//这个是 秒 级的转换
struct Caven_Watch Seconds_to_Hourly(int Seconds)
{
    struct Caven_Watch temp;
    temp.hour = Seconds / 3600;
    temp.minutes = (Seconds % 3600) / 60;
    temp.second = Seconds % 60;
    
    temp.hour = MIN(temp.hour,24);
    temp.minutes = MIN(temp.minutes,60);
    temp.second = MIN(temp.second,60);
    return temp;
}


char Over_Time(struct _Over_time *Item)
{
    if (Item->last_data != *(int *)Item->Now_data) //数据在跳动
    {
        Item->last_data = *(int *)Item->Now_data;
        Item->load = 0;
        Item->Flag = 0;
    }
    else //没有跳动
    {
        if (Item->load == 0) //开始载入超时,如果数据未跳动或者未超时，那么他只载入一次
        {
            Item->load = 1;
            Item->Flag = 0;
            Item->last_Time = *Item->Now_Time;
        }
        else //超时判定
        {
            struct Caven_Watch Temp_Time = {0}; //用来装时差
            int temp_num[2] = {0};
            int temp_us = 0;
            if (Item->Now_Time->hour < Item->last_Time.hour)        //下一天了
            {
                Temp_Time.hour = Item->Now_Time->hour + 24;   //为现在的时间补充 24H
            }
            else
            {
                Temp_Time.hour = Item->Now_Time->hour - Item->last_Time.hour;
            }
            Temp_Time.minutes = Item->Now_Time->minutes - Item->last_Time.minutes;
            Temp_Time.second = Item->Now_Time->second - Item->last_Time.second;
            Temp_Time.time_us = Item->Now_Time->time_us - Item->last_Time.time_us;  //实际时差（微秒级）

            temp_us = Item->Now_Time->time_us - Item->last_Time.time_us;            //实际时差（毫秒）
            temp_num[0] = Hourly_to_Seconds(Temp_Time);                             //实际时差（秒级）可能为负,所以要取绝对值
            temp_num[1] = Hourly_to_Seconds(Item->Set_Time);                        //设置超时的时间秒数

            if (temp_us < Item->Set_Time.time_us)   //--
            {
                temp_num[0]--;
            }

            if ((temp_num[0] - temp_num[1]) >= 0) //秒超时
            {
                Item->Flag = 'p';                   //为 0 是常态，为 'p' 是瞬态
                Item->Flip = !(Item->Flip);         //为了让程序捕捉这个瞬态，引入一个翻转态
                Item->load = 0;                     //允许重装载
            }
            else                                    //没有超时
            {
                Item->Flag = 0; //为 0 是常态，为 'p' 是瞬态
            }
        }
    }
    return Item->Flag;
}
