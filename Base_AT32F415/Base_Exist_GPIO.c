#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int Set)
{
#ifdef Exist_LCD
    gpio_init_type  gpio_init_struct;
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);       //时钟

        gpio_init_struct.gpio_pins = GPIO_PINS_10;            //LCD_DC
        gpio_init_struct.GPIO_Mode = GPIO_Mode_OUT_PP;        //推挽输出
        gpio_init_struct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //速度50MHz
        gpio_init(GPIOA, &gpio_init_struct);    //初始化GPIOA
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_10;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init(GPIOA, &gpio_init_struct);
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
        
        gpio_init_struct.gpio_pins = GPIO_PINS_4;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOA, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = GPIO_PINS_2;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_4;
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
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        
        gpio_init_struct.gpio_pins = GPIO_PINS_5;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_OUT_PP;
        gpio_init_struct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = GPIO_PINS_5;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init(GPIOB, &gpio_init_struct);
    }
#endif
}

void KEY_GPIO_Init(int Set)
{
#ifdef Exist_KEY
    gpio_init_type gpio_init_struct;
    if (Set) {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);

        gpio_init_struct.gpio_pins = GPIO_PINS_13;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_PU;
//        gpio_init_struct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else {
        gpio_init_struct.gpio_pins = GPIO_PINS_13;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

void HC595_GPIO_Init(int Set)
{
#ifdef Exist_HC595
    gpio_init_type gpio_init_struct;
    if (Set) 
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);       //时钟
        
        gpio_init_struct.gpio_pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_OUT_PP;
        gpio_init_struct.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else                                                    //标志取消GPIO
    {
        gpio_init_struct.gpio_pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}

