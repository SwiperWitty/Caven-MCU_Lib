#include "Base_GPIO.h"


/*
User_GPIO_config 设置GPIO-PIN的工作模式
gpiox [1-x]
pin [0-15]  一次一个
set [0-1] 0-INPUT,1-OUTPUT 
*/
void User_GPIO_config(int gpiox,int pin,int set)
{
    rcu_periph_enum value;          // 时钟
    uint32_t gpio_x;                //
    uint32_t mode, pull_up_down;
    uint32_t temp_num = 0x01;

    if (pin >= 16)
    {
        return;
    }
    temp_num <<= pin;
	switch (gpiox)
    {
    case 1:
		if (pin == 15)		// swd 不要用
		{
		}
        value = RCU_GPIOA;
        gpio_x = GPIOA;
    break;
    case 2:
		if (pin == 3 || pin == 4)
		{
		}
        value = RCU_GPIOB;
        gpio_x = GPIOB;
        break;
    case 3:
        value = RCU_GPIOC;
        gpio_x = GPIOC;
        break;
    case 4:
        value = RCU_GPIOD;
        gpio_x = GPIOD;
        break;
    case 5:
        value = RCU_GPIOE;
        gpio_x = GPIOE;
    break;
    case 6:
        value = RCU_GPIOF;
        gpio_x = GPIOF;
    break;
    case 7:
        value = RCU_GPIOG;
        gpio_x = GPIOG;
        break;
    case 8:
        value = RCU_GPIOH;
        gpio_x = GPIOH;
    break;
    case 9:
        value = RCU_GPIOI;
        gpio_x = GPIOI;
    break;
    default:
        return;
    }
    rcu_periph_clock_enable(value);
    if (set)
    {
        mode = GPIO_MODE_OUTPUT;
        pull_up_down = GPIO_PUPD_NONE;
    }
    else
    {
        mode = GPIO_MODE_INPUT;                       // 输入
        pull_up_down = GPIO_PUPD_PULLUP;
    }
    gpio_mode_set(gpio_x, mode, pull_up_down,temp_num);
    gpio_output_options_set(gpio_x, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,temp_num);
}
/*
User_GPIO_set 设置GPIO-PIN的输出电平(输出模式)
gpiox [1-x]
pin [0-15]
set [0-1]
*/
void User_GPIO_set(int gpiox,int pin,int set)
{
    uint32_t gpio_x;                //
    uint32_t temp_num = 0x01;

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
        case 5:
            gpio_x = GPIOE;
            break;
        case 6:
            gpio_x = GPIOF;
            break;
        case 7:
            gpio_x = GPIOG;
            break;
        case 8:
            gpio_x = GPIOH;
            break;
        case 9:
            gpio_x = GPIOI;
            break;
        default:
            return;
    }

    if (pin >= 16)
    {
        return;
    }
    temp_num <<= pin;
    if (set)
    {
        gpio_bit_set(gpio_x, temp_num);
    }
    else
    {
        gpio_bit_reset(gpio_x, temp_num);
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
    char retval = -1;
    uint32_t gpio_x;                //
    uint32_t temp_num = 0x01;

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
        case 5:
            gpio_x = GPIOE;
            break;
        case 6:
            gpio_x = GPIOF;
            break;
        case 7:
            gpio_x = GPIOG;
            break;
        case 8:
            gpio_x = GPIOH;
            break;
        case 9:
            gpio_x = GPIOI;
            break;
        default:
            return retval;
    }

    if (pin >= 16)
    {
        return retval;
    }
    temp_num <<= pin;

	retval = gpio_input_bit_get(gpio_x,temp_num);
	return retval;
}

