#include "Base_GPIO.h"


/*
User_GPIO_config 设置GPIO-PIN的工作模式
gpiox [1-x]
pin [0-15]  一次一个
set [0-1] 0-INPUT,1-OUTPUT 
*/
void User_GPIO_config(int gpiox,int pin,int set)
{
    GPIO_InitTypeDef gpio_init_struct;
    uint32_t value;        // 时钟
    GPIO_TypeDef *gpio_x;                  // 寄存器偏移地址
    uint16_t temp_num = 0x01;
    GPIO_StructInit(&gpio_init_struct);

    if (set)
    {
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    else
    {
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;                       // 输入
    }
    if (pin >= 16)
    {
        return;
    }
    temp_num <<= pin;
    gpio_init_struct.GPIO_Pin = temp_num;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;

	switch (gpiox)
    {
    case 1:
		if (pin == 15)		// swd 不要用
		{
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //支持 SWD，禁用 JTAG，PA15/PB3/PB4 可作GPIO
		}
        value = RCC_APB2Periph_GPIOA;
        gpio_x = GPIOA;
        break;
    case 2:
		if (pin == 3 || pin == 4)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //支持 SWD，禁用 JTAG，PA15/PB3/PB4 可作GPIO
		}
        value = RCC_APB2Periph_GPIOB;
        gpio_x = GPIOB;
        break;
    case 3:
        value = RCC_APB2Periph_GPIOC;
        gpio_x = GPIOC;
        break;
    case 4:
        value = RCC_APB2Periph_GPIOD;
        gpio_x = GPIOD;
        break;
    default:
        return;
//        break;
    }
    RCC_APB2PeriphResetCmd(value,ENABLE);
    GPIO_Init(gpio_x, &gpio_init_struct);
}
/*
User_GPIO_set 设置GPIO-PIN的输出电平(输出模式)
gpiox [1-x]
pin [0-15]
set [0-1]
*/
void User_GPIO_set(int gpiox,int pin,int set)
{
    // crm_periph_clock_type value;
    GPIO_TypeDef *gpio_x;
    uint16_t temp_num = 0x01;

	switch (gpiox)
    {
    case 1:
        gpio_x = GPIOA;
        break;
    case 2:
        gpio_x = GPIOB;
        break;
    case 3:
        gpio_x = GPIOC;
        break;
    case 4:
        gpio_x = GPIOD;
        break;
    default:
        return;
//        break;
    }

    if (pin >= 16)
    {
        return;
    }
    temp_num <<= pin;
    if (set)
    {
        GPIO_SetBits(gpio_x, temp_num);
    }
    else
    {
        GPIO_ResetBits(gpio_x, temp_num);
    }
}

/*
User_GPIO_get 获取GPIO-PIN的输入电平
gpiox [1-x]
pin [0-15]

返回 0/1
*/
char User_GPIO_get(int gpiox,int pin)
{
    // crm_periph_clock_type value;
    GPIO_TypeDef *gpio_x;
    uint16_t temp_num = 0x01;
	char retval = 0;
	switch (gpiox)
    {
    case 1:
        gpio_x = GPIOA;
        break;
    case 2:
        gpio_x = GPIOB;
        break;
    case 3:
        gpio_x = GPIOC;
        break;
    case 4:
        gpio_x = GPIOD;
        break;
    default:
        return retval;
//        break;
    }

    if (pin >= 16)
    {
        return retval;
    }
    temp_num <<= pin;

	retval = GPIO_ReadInputDataBit(gpio_x,temp_num);
	return retval;
}

