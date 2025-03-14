#include "Base_GPIO.h"


/*
User_GPIO_config 设置GPIO-PIN的工作模式
gpiox [1-x]
pin [0-15]  一次一个
set [0-1] 0-INPUT,1-OUTPUT 
*/
void User_GPIO_config(int gpiox,int pin,int set)
{
    gpio_init_type gpio_init_struct;
    crm_periph_clock_type value;        // 时钟
    gpio_type *gpio_x;                  // 寄存器偏移地址
    uint16_t temp_num = 0x01;
    gpio_default_para_init(&gpio_init_struct);

    if (set)
    {
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    }
    else
    {
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;                       // 输入
    }
    if (pin >= 16)
    {
        return;
    }
    temp_num <<= pin;
    gpio_init_struct.gpio_pins = temp_num;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

	switch (gpiox)
    {
    case 1:
		if (pin == 15)		// swd 不要用
		{
//			crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
//			gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
		}
        value = CRM_GPIOA_PERIPH_CLOCK;
        gpio_x = GPIOA;
        break;
    case 2:
		if (pin == 3 || pin == 4)
		{
//			crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
//			gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
		}
        value = CRM_GPIOB_PERIPH_CLOCK;
        gpio_x = GPIOB;
        break;
    case 3:
        value = CRM_GPIOC_PERIPH_CLOCK;
        gpio_x = GPIOC;
        break;
    case 4:
        value = CRM_GPIOD_PERIPH_CLOCK;
        gpio_x = GPIOD;
        break;
    default:
        return;
//        break;
    }
    crm_periph_clock_enable(value,TRUE);
    gpio_init(gpio_x, &gpio_init_struct);
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
    gpio_type *gpio_x;
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
        gpio_bits_set(gpio_x, temp_num);
    }
    else
    {
        gpio_bits_reset(gpio_x, temp_num);
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
    gpio_type *gpio_x;
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

	retval = gpio_input_data_bit_read(gpio_x,temp_num);
	return retval;
}
