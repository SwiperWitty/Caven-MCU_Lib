#include "MODE_Ultrasonic.h"

#define GPIOx		2
#define TRIG_PIN	4
#define ECHO_PIN	5

void Ultrasonic_Delay (int Num)
{
#if Exist_Ultrasonic
//    SYS_Base_Delay(Num,DS18B20_Time);
	SYS_Delay_us(Num);
#endif
}

int MODE_Ultrasonic_Init(int Set)
{
    int retval = -1;
#if Exist_Ultrasonic
	if(Set)
	{
		User_GPIO_config(GPIOx,TRIG_PIN,1);
		User_GPIO_config(GPIOx,ECHO_PIN,0);
		User_GPIO_set(GPIOx,TRIG_PIN,0);
	}
	else
	{
		User_GPIO_config(GPIOx,TRIG_PIN,0);
		User_GPIO_config(GPIOx,ECHO_PIN,0);
	}
    retval = 0;
#endif
    return retval;
}

/*
	检测超时自动退出,最多等待100ms
	没有超声波器件直接退出
*/
float MODE_Ultrasonic_Distance (void)
{
    float distance = 0;
#if Exist_Ultrasonic
	int count = 0,temp_num;
	SYS_BaseTIME_Type temp_time,diff_time;
	if (User_GPIO_get(GPIOx,ECHO_PIN) == 1)
	{
		return 0;
	}
	User_GPIO_set(GPIOx,TRIG_PIN,0);
	Ultrasonic_Delay(1);
	User_GPIO_set(GPIOx,TRIG_PIN,1);
	Ultrasonic_Delay(10);
	User_GPIO_set(GPIOx,TRIG_PIN,0);
	SYS_Time_Get(&temp_time);
	do{
		temp_num = User_GPIO_get(GPIOx,ECHO_PIN);
		SYS_Time_Get(&diff_time);
		count = diff_time.SYS_Sec - temp_time.SYS_Sec;
		if(count > 0)
		{
			count = 1000000;
		}
		else
		{
			count = 0;
		}
		count += diff_time.SYS_Us;
		count -= temp_time.SYS_Us;
		if(count > 100000)
		{
			return 0;
		}
	}
	while(temp_num == 0);	//等待信号发出，信号发出，计数器置0,信号发出，变为高电平，引脚置1
	
	do{
		temp_num = User_GPIO_get(GPIOx,ECHO_PIN);
		SYS_Time_Get(&temp_time);
		count = temp_time.SYS_Sec - diff_time.SYS_Sec;
		if(count > 0)
		{
			count = 1000000;
		}
		else
		{
			count = 0;
		}
		count += temp_time.SYS_Us;
		count -= diff_time.SYS_Us;
		if(count > 100000)
		{
			return 0;
		}
	}
	while(temp_num == 1);	//等待信号发出，信号发出，计数器置0,信号发出，变为高电平，引脚置1
	
	//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us ;distance = run_distance / 2
	distance = (float)count*0.017;
#endif
	return distance;
}

