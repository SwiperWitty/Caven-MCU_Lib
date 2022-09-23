#include "base_Exist_GPIO.h"

void LCD_GPIO_Init(int SET)
{
#ifdef Exist_LCD
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);       //时钟

        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;            //LCD_DC
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;        //推挽输出
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //速度50MHz
        GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
    }
    else
    {
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
#endif
}

void LED_GPIO_Init(int SET)
{
#ifdef Exist_LED
    GPIO_InitType GPIO_InitStructure;
    if (SET) 
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);       //时钟
        
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else                                                    //标志取消GPIO
    {
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
#endif
}

void BZZ_GPIO_Init(int SET)
{
#ifdef Exist_BZZ
    GPIO_InitType GPIO_InitStructure;
    if (SET) 
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
        
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else                                                    //标志取消GPIO
    {
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
#endif
}

void KEY_GPIO_Init(int SET)
{
#ifdef Exist_KEY
    GPIO_InitType GPIO_InitStructure;
    if (SET) {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
//        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    else {
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
#endif
}

void HC595_GPIO_Init(int SET)
{
#ifdef Exist_HC595
    GPIO_InitType GPIO_InitStructure;
    if (SET) 
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);       //时钟
        
        GPIO_InitStructure.GPIO_Pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else                                                    //标志取消GPIO
    {
        GPIO_InitStructure.GPIO_Pins = LATCH_CLOCK | SHIFT_CLOCK | HC595_Data;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
#endif
}

