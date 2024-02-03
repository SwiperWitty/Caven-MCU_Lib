#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int SET)
{
#ifdef Exist_LCD
    gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
    if (SET)
    {
		crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_10;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_0;		// RES
        gpio_init(GPIOB, &gpio_init_struct);
		LCD_RES_H();
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_10;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_0;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    
#endif
}

void LED_GPIO_Init(int SET)
{
#ifdef Exist_LED
    gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
    if (SET) 
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_4;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_4;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
#endif
}

void BZZ_GPIO_Init(int SET)
{
#ifdef Exist_BZZ
    gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
    if (SET) {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_5;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
    else {
        gpio_init_struct.gpio_pins = GPIO_Pin_5;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
    }
#endif
}

void KEY_GPIO_Init(int SET)
{
#ifdef Exist_KEY
    gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
    if (SET) {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_Pin_13;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else {
        gpio_init_struct.gpio_pins = GPIO_Pin_13;
		gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }

#endif
}

