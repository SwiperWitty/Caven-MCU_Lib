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
    switch (Channel) {
        case 1:
            if(SET)
                LED_Clr();
            else
                LED_Set();
            break;
        case 2:

            break;
        default:
            break;
    }

#endif
}

void LED_REG(void)
{
}

