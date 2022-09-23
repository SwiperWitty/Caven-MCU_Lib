#include "HC595.h"

void HC595_Init (int Set)
{
#ifdef Exist_HC595
    HC595_GPIO_Init(Set);
	HC595_Data_Clr();
    SHIFT_CLOCK_Clr();
    LATCH_CLOCK_Clr();
#endif
}

#ifdef Exist_HC595
static void HC595_Delay (int time)
{
    #if MCU_SYS_Freq >= 72000000 
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 10;            //SET
        while((temp--) > 0);
    }
    #else
    while((time--) > 0);
    #endif
}
#endif

static void Send_HC595 (const unsigned char Dat)
{
#ifdef Exist_HC595
	char n;
    SHIFT_CLOCK_Clr();
    LATCH_CLOCK_Clr();
    for(n = 0;n < 8;n++)								//ÉÏÉýÑØÓÐÐ§
    {
        if((Dat << n) & 0x80) HC595_Data_Set();
        else       			  HC595_Data_Clr();
        SHIFT_CLOCK_Set();
        HC595_Delay (10);
		SHIFT_CLOCK_Clr();
        HC595_Delay (10);
    }
#endif
}

static void HC595_END (void)
{
#ifdef Exist_HC595
    LATCH_CLOCK_Clr();
    HC595_Delay (10);
	LATCH_CLOCK_Set();
    HC595_Delay (10);
	LATCH_CLOCK_Clr();
#endif
}

void Set_DATA (const unsigned char *Dat,char num)
{
	char m;
	for(m = 0; m < num;m++)
	{
		Send_HC595 (*(Dat+m));
	}
    HC595_END ();
}
