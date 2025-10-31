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
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//            gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
        }
        value = RCC_APB2Periph_GPIOA;
        gpio_x = GPIOA;
        break;
    case 2:
        if (pin == 3 || pin == 4)
        {
//            crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
//            gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //禁用Jtag，但是保留SWD
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

void LCD_GPIO_Init(int Set)
{
#ifdef Exist_LCD
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   // 使能A端口时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // 使能B端口时钟

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;          // LCD_DC
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 速度50MHz
        GPIO_Init(GPIOA, &GPIO_InitStructure);              // 初始化GPIOA

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;           // LCD_RES
        GPIO_Init(GPIOB, &GPIO_InitStructure);              // 初始化GPIOB
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

    }
#endif
}

void Button_GPIO_Init(int Set)
{
#ifdef Exist_BUTTON
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
//        RCC_LSEConfig(RCC_LSE_OFF);
        BKP_TamperPinCmd(DISABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    PWR_BackupAccessCmd(DISABLE);/* 禁止修改RTC和后备寄存器*/

#endif
}

void Custom_GPIO_Init(int Set)
{
#ifdef Exist_CustomIO
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

        GPIO_PinRemapConfig(GPIO_Remap_PD01,ENABLE);
        //
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15; //RSTIC & WIG
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;               //WIG
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;  //GPIO_OUT
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;  //RFID_LED
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11;  //GPIO_OUT
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        RSTIC_L();
        WIG0_L();
        WIG1_L();
        GPO1_L();
        GPO2_L();
        GPO3_L();
        GPO4_L();

        // GPIO_Mode_IN_FLOATING
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;  // GPIO_IN
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // GPIO_IN
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  // GPIO_IN
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(GPIOD, &GPIO_InitStructure);
    }
    else {
    }
#endif
}

