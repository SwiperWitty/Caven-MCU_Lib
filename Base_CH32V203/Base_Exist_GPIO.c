#include "Base_Exist_GPIO.h"

void LCD_GPIO_Init(int SET)
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

void LED_GPIO_Init(int SET)
{
#ifdef Exist_LED
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        LED_H();
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

    }
#endif
}

void BZZ_GPIO_Init(int SET)
{
#ifdef Exist_BZZ
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        BZZ_L();
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
#endif
}

void HC595_GPIO_Init(int SET)
{
#ifdef Exist_HC595
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    if (SET) {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //HC595
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        HC595_SCK_L();
        HC595_RCK_L();          // 上升沿有效
    }
    else {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
#endif
}

void Button_GPIO_Init(int SET)
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

void Custom_GPIO_Init(int SET)
{
#ifdef Exist_CustomIO
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if (SET) {
        // GPIO_Mode_Out_PP
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

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

        RSTIC_L();
        WIG0_L();
        WIG1_L();
        GPO1_L();
        GPO2_L();

        // GPIO_Mode_IN_FLOATING
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;  // GPIO_IN
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

    }
    else {
    }
#endif
}

