#include "Base_PWM.h"

void TIM3_PWM_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		//gpio时钟
//        gpio_pin_remap_config(TMR3_GMUX_0011,TRUE);                                 //重映射Tim3

		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                                         //复用模式
        gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &gpio_init_struct);
		gpio_init_struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &gpio_init_struct);
		gpio_init_struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
#endif
}
void TIM4_PWM_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
        
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                                         //复用模式
        gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
    else
    {
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
#endif
}
void TIM8_PWM_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                                         //复用模式
        gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        GPIO_Init(GPIOC, &gpio_init_struct);
    }
#endif

}


//溢出时间T = (arr+1)*(psc+1)
void TIM3_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM
    FunctionalState state = DISABLE;
    if (Set)
        state = ENABLE;
    if(arr < 0 || psc < 0)
        return ;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, state);
    TIM_TypeDef *Temp_TIM = TIM3;

    TIM4_PWM_GPIO_Init(Set);

    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Temp_TIM, &TIM_TimeBaseStructure);	                    //向上计数

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(Temp_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC2Init(Temp_TIM, &TIM_OCInitStructure); 
	TIM_OC2PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC3Init(Temp_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC4Init(Temp_TIM, &TIM_OCInitStructure); 
	TIM_OC4PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);
    
	TIM_CtrlPWMOutputs(Temp_TIM, state); 												//OCPolarity 
	TIM_ARRPreloadConfig(Temp_TIM, state);
	
	TIM_ITConfig(Temp_TIM,TIM_IT_Update,DISABLE);
	TIM_Cmd(Temp_TIM, state);
#endif
}

// 溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
void TIM4_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM
    FunctionalState state = DISABLE;
    if (Set)
        state = ENABLE;
    if(arr < 0 || psc < 0)
        return ;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, state);
    TIM_TypeDef *Temp_TIM = TIM4;

    TIM4_PWM_GPIO_Init(Set);

    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Temp_TIM, &TIM_TimeBaseStructure);	                    // 向上计数

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(Temp_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC2Init(Temp_TIM, &TIM_OCInitStructure); 
	TIM_OC2PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC3Init(Temp_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);

	TIM_OC4Init(Temp_TIM, &TIM_OCInitStructure); 
	TIM_OC4PreloadConfig(Temp_TIM, TIM_OCPreload_Enable);
    
	TIM_CtrlPWMOutputs(Temp_TIM, state); 												//OCPolarity 
	TIM_ARRPreloadConfig(Temp_TIM, state);
	
	TIM_ITConfig(Temp_TIM,TIM_IT_Update,DISABLE);
	TIM_Cmd(Temp_TIM, state);
#endif
}

void TIM8_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM
    // AT32f415没有定时器8，但是IO与定时器3的复用是一样的

#endif
}

void TIM3_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM
    TIM_TypeDef *Temp_TIM = TIM3;

    switch (PWMx)
    {
    case (1):
        TIM_SetCompare1(Temp_TIM, value);
        break;
    case (2):
        TIM_SetCompare2(Temp_TIM, value);
        break;
    case (3):
        TIM_SetCompare3(Temp_TIM, value);
        break;
    case (4):
        TIM_SetCompare4(Temp_TIM, value);
        break;
    default:
        break;
    }
#endif
}

void TIM4_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM
    TIM_TypeDef *Temp_TIM = TIM4;

    switch (PWMx)
    {
    case (1):
        TIM_SetCompare1(Temp_TIM, value);
        break;
    case (2):
        TIM_SetCompare2(Temp_TIM, value);
        break;
    case (3):
        TIM_SetCompare3(Temp_TIM, value);
        break;
    case (4):
        TIM_SetCompare4(Temp_TIM, value);
        break;
    default:
        break;
    }
#endif
}

void TIM8_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM
    TIM_TypeDef *Temp_TIM = TIM8;

    switch (PWMx)
    {
    case (1):
        TIM_SetCompare1(Temp_TIM, value);
        break;
    case (2):
        TIM_SetCompare2(Temp_TIM, value);
        break;
    case (3):
        TIM_SetCompare3(Temp_TIM, value);
        break;
    case (4):
        TIM_SetCompare4(Temp_TIM, value);
        break;
    default:
        break;
    }
#endif
}

