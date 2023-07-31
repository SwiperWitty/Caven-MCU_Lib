#include "Mode_RTC8564.h"

int RTC8564_Init (int Set)
{
    int retval = 0;
#ifdef Exist_IIC
    char temp = 0xff;
    IIC_Start_Init(Set);
    Base_Delay (100,10);
    if(Set)
    {
        retval = IIC_Send_DATA(RTC8564_ADDR, &temp,1,1,10);
        IIC_Receive_DATA(RTC8564_ADDR, &temp,1,1,10);
    }

#endif
    return retval;
}






