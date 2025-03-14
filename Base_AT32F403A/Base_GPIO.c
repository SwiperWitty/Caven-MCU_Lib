#include "Base_GPIO.h"

void User_GPIO_Init(int Set)
{
	
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK,TRUE);
    if (Set) 
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_2;	// PD_CGF_A
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOD, &gpio_init_struct);
		
		gpio_init_struct.gpio_pins = GPIO_Pin_12;	// PD_CGF_B
		gpio_init(GPIOC, &gpio_init_struct);
		
		PD_CGF_A_L();
		PD_CGF_B_L();
		
		gpio_init_struct.gpio_pins = GPIO_Pin_0;	// DC_5V
		gpio_init(GPIOC, &gpio_init_struct);
		
		gpio_init_struct.gpio_pins = GPIO_Pin_1;	// DC_OUT
		gpio_init(GPIOC, &gpio_init_struct);
		
		DC_5V_ON();
		DC_OUT_ON();
    }
    else                                                    //标志取消GPIO
    {

    }
}

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
