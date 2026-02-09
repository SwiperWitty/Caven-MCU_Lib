#include "Base_GPIO.h"



/*
User_GPIO_config 设置GPIO-PIN的工作模式
gpiox [1-x]
pin [0-15]
set [0-1] 0-INPUT,1-OUTPUT
*/
void User_GPIO_config(int gpiox,int pin,int set)
{
    GPIO_InitTypeDef gpio_init_struct;
    uint32_t value;
    GPIO_TypeDef *gpio_x;
    uint16_t temp_num = 0x01;
//    gpio_default_para_init(&gpio_init_struct);

    if (set)
    {
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    else
    {
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
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
        if (pin == 15)      // swd 不要用
        {
            // WCH 没有Jtag
        }
        value = RCC_APB2Periph_GPIOA;
        gpio_x = GPIOA;
        break;
    case 2:
        if (pin == 3 || pin == 4)
        {
            // WCH 没有Jtag
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
     case 5:
        value = RCC_APB2Periph_GPIOE;
        gpio_x = GPIOE;
        break;
    default:
        return;
//        break;
    }
    RCC_APB2PeriphClockCmd(value,ENABLE);
    GPIO_Init(gpio_x, &gpio_init_struct);
}
/*
User_GPIO_config 设置GPIO-PIN的输出电平(输出模式)
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
        // value = CRM_GPIOA_PERIPH_CLOCK;
        gpio_x = GPIOA;
        break;
    case 2:
        // value = CRM_GPIOB_PERIPH_CLOCK;
        gpio_x = GPIOB;
        break;
    case 3:
        // value = CRM_GPIOC_PERIPH_CLOCK;
        gpio_x = GPIOC;
        break;
    case 4:
        // value = CRM_GPIOD_PERIPH_CLOCK;
        gpio_x = GPIOD;
        break;
    case 5:
        // value = CRM_GPIOD_PERIPH_CLOCK;
        gpio_x = GPIOE;
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
User_GPIO_config 设置GPIO-PIN的输出电平(输出模式)
gpiox [1-x]
pin [0-15]
set [0-1]
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
        // value = CRM_GPIOA_PERIPH_CLOCK;
        gpio_x = GPIOA;
        break;
    case 2:
        // value = CRM_GPIOB_PERIPH_CLOCK;
        gpio_x = GPIOB;
        break;
    case 3:
        // value = CRM_GPIOC_PERIPH_CLOCK;
        gpio_x = GPIOC;
        break;
    case 4:
        // value = CRM_GPIOD_PERIPH_CLOCK;
        gpio_x = GPIOD;
        break;
    case 5:
        // value = CRM_GPIOD_PERIPH_CLOCK;
        gpio_x = GPIOE;
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
