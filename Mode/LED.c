#include "LED.h"

void LED_Init(int SET)
{
#ifdef Exist_LED
    LED_GPIO_Init(SET);
#endif
}

void LED_SET(char Channel,int SET)
{
#ifdef Exist_LED
    if (Channel == 1)
    {
        if(SET)
            LED_Clr();
        else
            LED_Set();
    }
#endif
}


