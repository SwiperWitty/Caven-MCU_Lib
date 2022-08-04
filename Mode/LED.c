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
//            if(SET)
//                LEDR_Clr();
//            else
//                LEDR_Set();
            break;
        default:
            break;
    }

#endif
}

void WS2812_Delay(int time)
{
	int temp = 0;
	do
	{
		temp = 11;
		while(temp--);
	}while(time--);
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
			WS2812_Delay(8);
			LED_Clr();
			WS2812_Delay(4);
		}
		else
		{
			LED_Set();
			WS2812_Delay(4);
			LED_Clr();
			WS2812_Delay(8);
		}
	}
	LED_Set();
}

void LED_REG(struct Caven_Color Color,int SET)
{
	WS2812_Reset ();
	if(SET)
	{
		WS2812_write_byte(0xf0);
		WS2812_write_byte(0xf0);
		WS2812_write_byte(0xf0);
	}
	else
	{
		WS2812_write_byte(0);
		WS2812_write_byte(0);
		WS2812_write_byte(0);
	}
}

