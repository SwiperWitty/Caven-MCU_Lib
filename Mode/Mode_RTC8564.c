#include "Mode_RTC8564.h"
#include "time.h"

struct tm timeinfo;

/*
 *
 */
int MODE_RTC8564_Init (int set)
{
    int retval = 0;
#ifdef Exist_RTC8564
    U64 temp_time;
    Base_IIC_Init(set);
    SYS_Base_Delay(100,1000);
    if(set)
    {
        retval = MODE_RTC8564_Read_time (&temp_time);
    }
#endif
    return retval;
}

#ifdef Exist_RTC8564
static uint8_t bcdTOdec(uint8_t val)
{
    uint8_t retval = (val >> 4) * 10 + (val & 0x0f);
    return retval;
}

static uint8_t decTObcd(uint8_t val)
{
    uint8_t retval = ((val / 10) << 4) + (val % 10);
    return retval;
}
#endif

int MODE_RTC8564_Read_time (U64 *sec)
{
    int retval = 0;
#ifdef Exist_RTC8564
    uint8_t temp_array[10];
    temp_array[0] = 0x02;
    retval = Base_IIC_Send_DATA(RTC8564_ADDR,temp_array,1,1,1,1);
    memset(temp_array,0,sizeof(temp_array));
    if (retval) {
        if(Base_IIC_Receive_DATA(RTC8564_ADDR,temp_array,1,7,1) != 1)
        {
            retval = 0;
        }
    }
    if (retval) {
        timeinfo.tm_sec = bcdTOdec(temp_array[0] & 0x7F);
        timeinfo.tm_min = bcdTOdec(temp_array[1] & 0x7F);
        timeinfo.tm_hour = bcdTOdec(temp_array[2] & 0x3F);
        timeinfo.tm_mday = bcdTOdec(temp_array[3] & 0x3F);
        timeinfo.tm_wday = bcdTOdec(temp_array[4] & 0x07);    // tm_wday is 0 to 6
        timeinfo.tm_mon = bcdTOdec(temp_array[5] & 0x1F) - 1; // tm_mon is 0 to 11
        timeinfo.tm_year = bcdTOdec(temp_array[6]) + 2000;
        timeinfo.tm_isdst = 0;
        *sec = mktime(&timeinfo);
    }

#endif
    return retval;
}



int MODE_RTC8564_Write_time (U64 sec)
{
    int retval = 0;
#ifdef Exist_RTC8564
    int temp_run = 0;
    time_t temp_time = sec;
    uint8_t temp_array[10];
    localtime_r(&temp_time, &timeinfo);
    if (timeinfo.tm_year >= 100)
    {
        timeinfo.tm_year = timeinfo.tm_year - 100;
    }
    temp_array[temp_run++] = 2;
    temp_array[temp_run++] = decTObcd(timeinfo.tm_sec);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_min);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_hour + 8);

    temp_array[temp_run++] = decTObcd(timeinfo.tm_mday);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_wday);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_mon + 1);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_year);
    if(Base_IIC_Send_DATA(RTC8564_ADDR,temp_array,1,temp_run,1,0) != 0)
    {
        retval = 0;
    }
#endif
    return retval;
}

