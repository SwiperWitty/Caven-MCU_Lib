#include <MODE_LED.h>


void MODE_LED_Init(int SET)
{
#ifdef Exist_LED
    LED_GPIO_Init(SET);
    
#endif
}

void MODE_LED_SET_Fun(char Channel,int SET)
{
#ifdef Exist_LED
    switch (Channel) {
        case 1:
            if(SET)
                LED_L();
            else
                LED_H();
            break;

        default:
            break;
    }

#endif
}

void WS2812_Delay(int time)
{
    SYS_Base_Delay(time, 10);
}

void WS2812_Reset (void)
{
#ifdef Exist_LED
	LED_L();
	WS2812_Delay(60);
	LED_H();
#endif
}

void WS2812_write_byte(char data)
{
#ifdef Exist_LED
	char temp = data;
	for(char i = 0; i < 8; i++)
	{
	    if((temp << i) & 0x80)
	    {
	        LED_H();
	        WS2812_Delay(7);
	        LED_L();
	        WS2812_Delay(3);
	    }
	    else
	    {
            LED_H();
            WS2812_Delay(3);
            LED_L();
	        WS2812_Delay(6);
        }
	}
	LED_L();
#endif
}

void MODE_LED_RGB_Fun(Caven_Color_Type Color,int SET)
{
//    NVIC_SETPRIMASK();
	if(SET)
	{
		WS2812_write_byte(0xaa);
		WS2812_write_byte(0x01);
		WS2812_write_byte(0x00);
	}
	else
	{
		WS2812_write_byte(0);
		WS2812_write_byte(0);
		WS2812_write_byte(0);
	}
//	NVIC_RESETPRIMASK();
}

