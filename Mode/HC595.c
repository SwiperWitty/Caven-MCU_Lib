#include "HC595.h"

void MODE_HC595_Init (int Set)
{
#if Exist_HC595
    unsigned char temp_array[8];
    memset(temp_array,0,sizeof(temp_array));
    User_GPIO_config(HC595_GPIO,HC595_SCK_IO,1);
    User_GPIO_config(HC595_GPIO,HC595_Data_IO,1);
    User_GPIO_config(HC595_GPIO,HC595_RCK_IO,1);
    HC595_Set_DATA_Fun (temp_array,5);
#endif
}

#if Exist_HC595
static void HC595_Delay (int time)
{
//    SYS_Base_Delay (time,5);
}
#endif

static void Send_HC595 (const unsigned char Dat)
{
#if Exist_HC595
	char n;
    User_GPIO_set(HC595_GPIO,HC595_SCK_IO,0);
    User_GPIO_set(HC595_GPIO,HC595_Data_IO,0);
    User_GPIO_set(HC595_GPIO,HC595_RCK_IO,0);

    for(n = 0;n < 8;n++)
    {
        if((Dat << n) & 0x80)   {User_GPIO_set(HC595_GPIO,HC595_Data_IO,1);}
        else    {User_GPIO_set(HC595_GPIO,HC595_Data_IO,0);}
        User_GPIO_set(HC595_GPIO,HC595_SCK_IO,1);
        HC595_Delay (10);
		User_GPIO_set(HC595_GPIO,HC595_SCK_IO,0);
        HC595_Delay (10);
    }
#endif
}

static void HC595_END (void)
{
#if Exist_HC595
    User_GPIO_set(HC595_GPIO,HC595_RCK_IO,0);
    HC595_Delay (10);
	User_GPIO_set(HC595_GPIO,HC595_RCK_IO,1);
    HC595_Delay (10);
	User_GPIO_set(HC595_GPIO,HC595_RCK_IO,0);
#endif
}

/*
 * 先入先出
 */
void HC595_Set_DATA_Fun (const uint8_t *Dat,char num)
{
    int temp_num = num;
    unsigned char temp_dat;
	for(char i = 0; i < temp_num;i++)
	{
	    temp_dat = Dat[i];
		Send_HC595 (temp_dat);
	}
    HC595_END ();
}
