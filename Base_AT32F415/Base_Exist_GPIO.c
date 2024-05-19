#include "Base_Exist_GPIO.h"


/*
User_GPIO_config 设置GPIO-PIN的工作模式
gpiox [1-x]
pin [0-15]
set [0-1] 0-INPUT,1-OUTPUT 
*/
void User_GPIO_config(int gpiox,int pin,int set)
{
    gpio_init_type gpio_init_struct;
    crm_periph_clock_type value;
    gpio_type *gpio_x;
    int temp_num = 0x01;
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
			crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
			gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
		}
        value = CRM_GPIOA_PERIPH_CLOCK;
        gpio_x = GPIOA;
        break;
    case 2:
		if (pin == 3 || pin == 4)
		{
			crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
			gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
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
User_GPIO_config 设置GPIO-PIN的输出电平(输出模式)
gpiox [1-x]
pin [0-15]
set [0-1]
*/
void User_GPIO_set(int gpiox,int pin,int set)
{
    // crm_periph_clock_type value;
    gpio_type *gpio_x;
    int temp_num = 0x01;

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
User_GPIO_config 设置GPIO-PIN的输出电平(输出模式)
gpiox [1-x]
pin [0-15]
set [0-1]
*/
char User_GPIO_get(int gpiox,int pin)
{
    // crm_periph_clock_type value;
    gpio_type *gpio_x;
    int temp_num = 0x01;
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

//
void STEP_Motor_GPIO_Init (int Set)
{
    //BYJ_48 4线
#ifdef Exist_STEP_Motor
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
		crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = STEP_OUT1 | STEP_OUT2 | STEP_OUT3 | STEP_OUT4;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(STEP_Clock, &gpio_init_struct);

    }
    else
    {
        gpio_init_struct.gpio_pins = STEP_OUT1 | STEP_OUT2 | STEP_OUT3 | STEP_OUT4;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(STEP_Clock, &gpio_init_struct);
    }
#endif
}

void LCD_GPIO_Init(int Set)
{
#ifdef Exist_LCD
    gpio_init_type  gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        gpio_init_struct.gpio_pins = GPIO_Pin_10;            //LCD_DC
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        #ifdef LCD_RES_H
        gpio_init_struct.gpio_pins = GPIO_Pin_0;
        gpio_init(GPIOB, &gpio_init_struct);
        #endif 
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_10;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        #ifdef LCD_RES_H
        gpio_init_struct.gpio_pins = GPIO_Pin_0;
        gpio_init(GPIOB, &gpio_init_struct);
        #endif 
    }
#endif
}

void LED_GPIO_Init(int Set)
{
#ifdef Exist_LED
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_LED;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_LEDR;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_LED;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
#endif
}

void BZZ_GPIO_Init(int Set)
{
#ifdef Exist_BZZ
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);

        gpio_init_struct.gpio_pins = GPIO_Pin_5;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_5;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
#endif
}

void Button_GPIO_Init(int Set)
{
#ifdef Exist_BUTTON
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);

        gpio_init_struct.gpio_pins = GPIO_Pin_13;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;                       // 输入
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else 
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_13;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC138_GPIO_Init(int Set)
{
#ifdef Exist_HC138
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       // 时钟

        gpio_init_struct.gpio_pins = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = HC595_D1 | HC595_D2 | HC595_D3;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC595_GPIO_Init(int Set)
{
#ifdef Exist_HC595
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       // 时钟

        gpio_init_struct.gpio_pins = HC595_RCK | HC595_SCK | HC595_Data;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = HC595_RCK | HC595_SCK | HC595_Data;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void DS18B20_IO_Config(int Set)
{
#ifdef Exist_DS18B20
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);          // 时钟(切换GPIO记得看一眼)

    gpio_init_struct.gpio_pins = DS18B20_IO;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
    if (Set)                                                        // 输出
    {
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(DS18B20_Clock, &gpio_init_struct);
    }
    else                                                            // 输入
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(DS18B20_Clock, &gpio_init_struct);
    }
#endif
}

void Ultrasonic_GPIO_Init(int Set)
{
#ifdef Exist_Ultrasonic
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);          // 时钟(切换GPIO记得看一眼)

    if (Set)                                                        // 输出
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_0;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_1;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else                                                            // 输入
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_0 | GPIO_Pin_1;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
#endif
}
