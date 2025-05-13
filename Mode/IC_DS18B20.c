#include "IC_DS18B20.h"

#if Exist_DS18B20
static int DS18B20_Time = 0;
static int DS18B20_gpiox = 0,DS18B20_pin = 0;
static void Write_Byte (char Data);

#endif

static int DS18B20_Exist_Flag = 0;

static void DS18B20_Delay (int Num)
{
#if Exist_DS18B20
    SYS_Base_Delay(Num,DS18B20_Time);
#endif
}

char DS18B20_Start (void)
{
	char temp = 0;
#if Exist_DS18B20
	User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
	User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);DS18B20_Delay (50);
	User_GPIO_set(DS18B20_gpiox,DS18B20_pin,0);DS18B20_Delay (500);
	
	User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);DS18B20_Delay (5);
    User_GPIO_config(DS18B20_gpiox,DS18B20_pin,0);
	DS18B20_Delay (50);
    int time = 0;
    do{
        DS18B20_Delay (50);
        time++;
        if(time > 6)
        {
            DS18B20_Exist_Flag = 0;
            return temp;                    //启动失败了
        }
    }while(User_GPIO_get(DS18B20_gpiox,DS18B20_pin) == 1);
	User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
	User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);DS18B20_Delay (5);
	temp = 1;
#endif
	return temp;
}

int MODE_DS18B20_Init (int gpiox,int pin,int Set)
{
	int temp = 0;
    
#if Exist_DS18B20
    DS18B20_gpiox = gpiox;
    DS18B20_pin = pin;
    User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
    #ifdef MCU_SYS_FREQ
    DS18B20_Time = (MCU_SYS_FREQ/6000000);
    #else
    DS18B20_Time = 100;
    #endif
	DS18B20_Delay (500);
	if(DS18B20_Start () == 1)
	{
		DS18B20_Exist_Flag = 1;             //成功
		temp = 1;
        DS18B20_Delay (50);
        DS18B20_Get_Temp_Fun();
	}
#endif
	return temp; 
}

static void Write_Byte (char Data)
{
#if Exist_DS18B20
    char Temp = Data;
    User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
    User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);
    DS18B20_Delay (10);
	
    for (char i = 8; i > 0; i--)
    {
        User_GPIO_set(DS18B20_gpiox,DS18B20_pin,0);
        DS18B20_Delay (8);

        if(Temp & 0x01)             //与1按位与运算，Data最低位为1时DQ总线为1，Data最低位为0时DQ总线为0
        {
            User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);
            DS18B20_Delay (30);
        }
        else
        {
            DS18B20_Delay (40);
        }
        Temp >>= 1;
        User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);
        DS18B20_Delay (10);

    }
    DS18B20_Delay (50);
#endif
}

static char Read_Byte (void)
{
    char Data = 0;
#if Exist_DS18B20
    for (char i = 0; i < 8; i++)
    {
        User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
        User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);
        DS18B20_Delay (8);
        User_GPIO_set(DS18B20_gpiox,DS18B20_pin,0);         // 低脉冲
		DS18B20_Delay (1);
        User_GPIO_config(DS18B20_gpiox,DS18B20_pin,0);  // 弱拉高

        DS18B20_Delay (20);
		Data >>= 1;
		if(User_GPIO_get(DS18B20_gpiox,DS18B20_pin))
		{ Data |= 0x80; }
		//else {0}
		DS18B20_Delay (40);
    }

    User_GPIO_config(DS18B20_gpiox,DS18B20_pin,1);
	User_GPIO_set(DS18B20_gpiox,DS18B20_pin,1);
#endif
    return Data;
}

float DS18B20_Get_Temp_Fun (void)
{
    float Temp = 0;
	if(DS18B20_Start () == 0)
	{ 
//        return 0;
        DS18B20_Exist_Flag = 1;
    } 
	
    if(DS18B20_Exist_Flag)      //  在DB18B20存在的情况下
    {
        int Data;
        unsigned char Temp_H,Temp_L;
		char t = 0;

        Write_Byte(0xcc);	// skip rom		1100 1100
        Write_Byte(0x44);	// convert		0100 0100

		DS18B20_Start();
        
        Write_Byte(0xcc);	// skip rom
        Write_Byte(0xbe);	// convert		10111110

        Temp_L = Read_Byte ();
        Temp_H = Read_Byte ();
		
		if(Temp_H > 7)
		{
			Temp_H = ~Temp_H;
			Temp_L = ~Temp_L;
			t = 1;
		}
        Data = Temp_H;
        Data <<= 8;
        Data += Temp_L;

		Temp = Data;
		Temp *= (float)0.0625;	
		if(t)
		{Temp = -Temp;}
    }
    return Temp;
}


