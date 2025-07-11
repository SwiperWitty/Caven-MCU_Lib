#include "MODE_LED.h"


void MODE_LED_Init(int Set)
{
#if Exist_LED
    User_GPIO_config(1,LED_IO,1);
    User_GPIO_set(1,LED_IO,1);
#endif
}

void MODE_LED_Set_Fun(char Channel,int Set)
{
#if Exist_LED
    switch (Channel) {
        case 1:
            if(Set)
                User_GPIO_set(1,LED_IO,0);
            else
                User_GPIO_set(1,LED_IO,1);
            break;

        default:
            break;
    }

#endif
}

void WS2812_Delay(int time)
{
#if Exist_LED
    SYS_Base_Delay(time, 10);
#endif
}

void WS2812_Reset (void)
{
#if Exist_LED
	User_GPIO_set(1,LED_IO,0);
	WS2812_Delay(60);
	User_GPIO_set(1,LED_IO,1);
#endif
}

void WS2812_write_byte(char data)
{
#if Exist_LED
	char temp = data;
	for(char i = 0; i < 8; i++)
	{
	    if((temp << i) & 0x80)
	    {
	        User_GPIO_set(1,LED_IO,1);
	        WS2812_Delay(7);
	        User_GPIO_set(1,LED_IO,0);
	        WS2812_Delay(3);
	    }
	    else
	    {
            User_GPIO_set(1,LED_IO,1);
            WS2812_Delay(3);
            User_GPIO_set(1,LED_IO,0);
	        WS2812_Delay(6);
        }
	}
	User_GPIO_set(1,LED_IO,0);
#endif
}

void MODE_LED_RGB_Fun(Caven_Color_Type Color,int Set)
{
//    NVIC_SETPRIMASK();
	if(Set)
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

