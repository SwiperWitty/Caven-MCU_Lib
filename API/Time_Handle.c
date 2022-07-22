#include "Time_Handle.h"

int Hourly_to_Seconds(struct Caven_Watch Item)
{
    int temp;
    temp = Item.second + (Item.minutes * 60) + (Item.hour * 3600); //设置的超时时差（秒级）
    return temp;
}

struct Caven_Watch Seconds_to_Hourly(int Seconds)
{
    struct Caven_Watch temp;
    temp.hour = Seconds / 3600;
    temp.minutes = (Seconds % 3600) / 60;
    temp.second = Seconds % 60;
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

            Temp_Time.hour = Item->Now_Time->hour - Item->last_Time.hour;
            if (Item->last_Time.date != Item->Now_Time->date) //如果这已然是下一天
            {
                Temp_Time.hour += 24; //为现在的时间补充 24H
            }
            Temp_Time.minutes = Item->Now_Time->minutes - Item->last_Time.minutes;
            Temp_Time.second = Item->Now_Time->second - Item->last_Time.second;
            Temp_Time.time_num = Item->Now_Time->time_num - Item->last_Time.time_num;   //实际时差（微秒级）
            temp_num[0] = Hourly_to_Seconds(Temp_Time);                                 //实际时差（秒级）可能为负,所以要取绝对值
            temp_num[1] = Hourly_to_Seconds(Item->Set_Time);
            if ((temp_num[0] - temp_num[1]) >= 0) //可能超时
            {
                if ((temp_num[0] - temp_num[1]) > 0) //秒超时，直接溢出(其他就不用看了)
                {
                    Item->Flag = 'p';           //为 0 是常态，为 'p' 是瞬态
                    Item->Flip = !(Item->Flip); //为了让程序捕捉这个瞬态，引入一个翻转态
                    Item->load = 0;             //允许重装载
                }
                else if ((Temp_Time.time_num - Item->Set_Time.time_num) >= 0) // us超时s
                {
                    Item->Flag = 'p';
                    Item->Flip = !(Item->Flip);
                    Item->load = 0;
                }
                else
                {
                    Item->Flag = 0; //为 0 是常态，为 'p' 是瞬态
                }
            }
            else if ((temp_num[0] - temp_num[1]) < 0)
            {
                return Item->Flag;
            }
            else //暂时没有超时，这个很重要
            {
                Item->Flag = 0;
            }
        }
    }
    return Item->Flag;
}
