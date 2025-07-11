#include "IC_DHT11.h"

#if Exist_DHT11

static int DHT11_gpiox = 0,DHT11_pin = 0;
static void Write_Byte (char Data);

#endif

static int DS18B20_Exist_Flag = 0;

void DHT11_Delay (int Num)
{
#if Exist_DHT11
//    SYS_Base_Delay(Num,DS18B20_Time);
	SYS_Delay_us(Num);
#endif                                                                                                                   
}

int MODE_DHT11_Init (int gpiox,int pin,int Set)
{
	int temp = 0;
    
#if Exist_DHT11
    DHT11_gpiox = gpiox;
    DHT11_pin = pin;
    User_GPIO_config(DHT11_gpiox,DHT11_pin,1);
	User_GPIO_set(DHT11_gpiox,DHT11_pin,1);
	SYS_Delay_ms (2000);

#endif
	return temp; 
}

/*
0: null
1: Exist
*/
char DHT11_Start (void)
{
    uint8_t retry = 0;
    
    User_GPIO_config(DHT11_gpiox,DHT11_pin,1);
    User_GPIO_set(DHT11_gpiox,DHT11_pin,1);
    
	User_GPIO_set(DHT11_gpiox,DHT11_pin,0);	// 主机拉低
	SYS_Delay_ms (20);                  	// 至少18ms
    
    User_GPIO_set(DHT11_gpiox,DHT11_pin,1);	// 主机拉高
    DHT11_Delay (30);                  		// 20-40us
    
    User_GPIO_config(DHT11_gpiox,DHT11_pin,0);	// 切换为输入模式
    
    // 等待DHT11 low响应
	retry = 0;
    while(User_GPIO_get(DHT11_gpiox,DHT11_pin) == 1 && retry < 10) {
        DHT11_Delay (10);
        retry++;
    }
    if(retry >= 10)
	{
		return 0;
	}
    // 等待DHT11 high响应
    retry = 0;
    while(User_GPIO_get(DHT11_gpiox,DHT11_pin) == 0 && retry < 10) {
        DHT11_Delay (10);
        retry++;
    }
	
    if(retry >= 10) 
	{
		return 0;
    }
    return 1;
}

static char Read_Byte (void)
{
	unsigned char retry = 0;
	unsigned char data = 0;
#if Exist_DHT11
	User_GPIO_config(DHT11_gpiox,DHT11_pin,0);
    for (char i = 0; i < 8; i++)
    {
		while(User_GPIO_get(DHT11_gpiox,DHT11_pin) == 0 && retry < 10) {
			DHT11_Delay (10);
			retry++;
		}
		if(retry >= 10)
		{
			data = 0;
			break;
		}
		data <<= 1;
		DHT11_Delay (30);
		if(User_GPIO_get(DHT11_gpiox,DHT11_pin) == 1)	// 如果过了30us还是高电平的话就是数据1
		{
			data |= 1;		// 数据+1
		}
		while(User_GPIO_get(DHT11_gpiox,DHT11_pin) == 1 && retry < 10) {
			DHT11_Delay (10);
			retry++;
		}
		if(retry >= 10)
		{
			data = 0;
			break;
		}
    }
#endif
    return data;
}

/*
0~50℃，误差±2℃
20%~90%RH，误差±5%RH

*/
int DHT11_Get_data_Fun (int *temp,int *hum)
{
	int retval = 0;
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH = 0,RL = 0,TH = 0,TL = 0,CHECK = 0;
	
	if(DHT11_Start() == 1)	// 主机发送信号
	{
		R_H = Read_Byte();
		R_L = Read_Byte();
		T_H = Read_Byte();
		T_L = Read_Byte();
		CHECK = Read_Byte(); //接收5个数据
		
		User_GPIO_config(DHT11_gpiox,DHT11_pin,1);
		User_GPIO_set(DHT11_gpiox,DHT11_pin,1);
		DHT11_Delay (1);
		User_GPIO_set(DHT11_gpiox,DHT11_pin,0);		// 当最后一bit数据传送完毕后，DHT11拉低总线 50us
		DHT11_Delay (55);
		User_GPIO_set(DHT11_gpiox,DHT11_pin,1);
		
		if(R_H + R_L + T_H + T_L == CHECK) 			// 和检验位对比，判断校验接收到的数据是否正确
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
		if(temp != NULL)
		{
			*temp = RH;
		}
		if(hum != NULL)
		{
			*hum = TH;
		}
		retval = 1;
	}
	return retval;
}
