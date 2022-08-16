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
            if(SET)
                LEDR_Clr();
            else
                LEDR_Set();
            break;
        default:
            break;
    }

#endif
}

void WS2812_Delay(int time)
{
    volatile int var;
    for (var = time;var > 0;var--)
    {

    }
}

void WS2812_Reset (void)
{
	LED_Clr();
	WS2812_Delay(60);
	LED_Set();
}

void WS2812_write_byte(char data)
{
	char temp = data;
	for(char i = 0; i < 8; i++)
	{
	    if((temp << i) & 0x80)
	    {
	        LED_Set();
	        WS2812_Delay(6);
	        LED_Clr();
	        WS2812_Delay(3);
	    }
	    else
	    {
            LED_Set();
            WS2812_Delay(3);
            LED_Clr();
	        WS2812_Delay(6);
        }
	}
	LED_Clr();
}

void LED_REG(struct Caven_Color Color,int SET)
{
	WS2812_Reset ();
	if(SET)
	{
		WS2812_write_byte(0x01);
		WS2812_write_byte(0x01);
		WS2812_write_byte(0x00);
	}
	else
	{
		WS2812_write_byte(0);
		WS2812_write_byte(0);
		WS2812_write_byte(0);
	}
}

