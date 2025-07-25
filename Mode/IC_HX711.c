#include "IC_HX711.h"

int HX711_gpiox = 0,HX711_SCK_pin = 0,HX711_DOUT_pin = 0;

#if Exist_HX711
static void HX711_Delay (int Num)
{
    SYS_Delay_us(Num);
}
#endif

int MODE_IC_HX711_Init(int gpiox,int SCK_pin,int DOUT_pin,int Set)
{
	int retval = 0;
#if Exist_HX711
	int over_time = 0;
	if (Set)
	{
		HX711_gpiox = gpiox;
		HX711_SCK_pin = SCK_pin;
		HX711_DOUT_pin = DOUT_pin;
		User_GPIO_config (HX711_gpiox,HX711_SCK_pin,1);
		User_GPIO_config (HX711_gpiox,HX711_DOUT_pin,0);
		
		User_GPIO_set(HX711_gpiox,HX711_SCK_pin,0);
		do{
			retval = User_GPIO_get(HX711_gpiox,HX711_DOUT_pin);
			HX711_Delay (100);
			over_time ++;
			if(over_time > 10)
			{
				return retval;
			}
		}while(retval == 1);
	}
#endif
	return retval;
}

int32_t HX711_Read(void) {
	int retval = 0,over_time = 0;
    int32_t data = 0;
    
    // 等待DOUT变为低电平
	do{
		retval = User_GPIO_get(HX711_gpiox,HX711_DOUT_pin);
		HX711_Delay (100);
		over_time ++;
		if(over_time > 10)
		{
			return retval;
		}
	}while(retval == 1);
    
    // 读取24位数据
    for(uint8_t i = 0; i < 24; i++) 
	{
        User_GPIO_set(HX711_gpiox,HX711_SCK_pin,1);
        data <<= 1;
        if(User_GPIO_get(HX711_gpiox,HX711_DOUT_pin) == 1) 
		{
			data++;
        }
        User_GPIO_set(HX711_gpiox,HX711_SCK_pin,0);
    }
    
    // 设置增益为128 (发送25个脉冲)
    User_GPIO_set(HX711_gpiox,HX711_SCK_pin,1);
	HX711_Delay (1);
	User_GPIO_set(HX711_gpiox,HX711_SCK_pin,0);
    
    // 扩展符号位到32位
    if(data & 0x800000) {
        data |= 0xFF000000;
    }
    
    return data;
}

int32_t HX711_Read_Average(uint8_t times) {
    int32_t sum = 0;
    for(uint8_t i = 0; i < times; i++) {
        sum += HX711_Read();
        HX711_Delay (10);
    }
    return sum / times;
}

// 校准参数
float HX711_SCALE = 1.0f;    // 比例系数
int32_t HX711_OFFSET = 0;    // 零点偏移

void HX711_Calibrate(uint16_t known_weight) {
    // 1. 获取空载读数
    int32_t offset = HX711_Read_Average(10);
    HX711_OFFSET = offset;
    
    // 2. 放置已知重量，获取读数
    // 这里需要用户放置已知重量后继续执行
    HX711_Delay (1000); // 
    
    int32_t with_weight = HX711_Read_Average(10);
    int32_t net_value = with_weight - offset;
    
    // 3. 计算比例系数
    HX711_SCALE = (float)net_value / known_weight;
}

float HX711_Get_Weight(uint8_t times) {
    int32_t raw = HX711_Read_Average(times) - HX711_OFFSET;
    return (float)raw / HX711_SCALE;
}

