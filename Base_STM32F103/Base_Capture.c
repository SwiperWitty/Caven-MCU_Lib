#include "Base_Capture.h"

D_pFun TIM1_Capture_Fun = NULL;
D_pFun TIM2_Capture_Fun = NULL;
D_pFun TIM3_Capture_Fun = NULL;
D_pFun TIM4_Capture_Fun = NULL;
D_pFun TIM5_Capture_Fun = NULL;

int TIM1_lastCount = 0;
int TIM2_lastCount = 0;
int TIM3_lastCount = 0;
int TIM4_lastCount = 0;
int TIM5_lastCount = 0;

void TIMx_Capture_Callback_pFunBind(char TIMx,D_pFun pFun)
{
	if(TIMx > 3 || pFun == NULL)
	{
		return;
	}
	switch (TIMx)
	{
		case 1:
		{
			TIM1_Capture_Fun = pFun;
		}
		break;
		case 2:
		{
			TIM2_Capture_Fun = pFun;
		}
		break;
		case 3:
		{
			TIM3_Capture_Fun = pFun;
		}
		break;
		case 4:
		{
			TIM4_Capture_Fun = pFun;
		}
		break;
		case 5:
		{
			TIM5_Capture_Fun = pFun;
		}
		break;
		default:
			break;
	}
}

int TIMx_Encoder_Capture(char TIMx)
{
    int currentCount = 0;
	int lastCount = 0;
	switch (TIMx)
	{
		case 1:
		{
			lastCount = TIM1_lastCount;
			currentCount = TIM_GetCounter(TIM1);
			TIM1_lastCount = currentCount;
		}
		break;
		case 2:
		{
			lastCount = TIM2_lastCount;
			currentCount = TIM_GetCounter(TIM2);
			TIM2_lastCount = currentCount;
		}
		break;
		case 3:
		{
			lastCount = TIM3_lastCount;
			currentCount = TIM_GetCounter(TIM3);
			TIM3_lastCount = currentCount;
		}
		break;
		case 4:
		{
			lastCount = TIM4_lastCount;
			currentCount = TIM_GetCounter(TIM4);
			TIM4_lastCount = currentCount;
		}
		break;
		case 5:
		{
			lastCount = TIM5_lastCount;
			currentCount = TIM_GetCounter(TIM5);
			TIM5_lastCount = currentCount;
		}
		break;
		default:
			break;
	}
    int diff = (currentCount - lastCount);
    return diff;
}

void TIM1_Capture_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                                         //复用模式
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
    else
    {
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
#endif
}
void TIM2_Capture_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
    else
    {
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
        GPIO_Init(GPIOA, &gpio_init_struct);
    }
#endif
}
void TIM3_Capture_GPIO_Init(int Set)
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
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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

/*
	溢出时间T = (arr+1)*(psc+1)
	Channel = 1-4
	mode 0-输入捕获	1-编码捕获
	set	0/1
*/
void TIM1_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#ifdef Exist_PWM
    FunctionalState state = DISABLE;
    if (Set)
        state = ENABLE;
    if(arr < 0 || psc < 0)
        return ;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, state);
    TIM_TypeDef *Temp_TIM = TIM1;

    TIM1_Capture_GPIO_Init(Set);

    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Temp_TIM, &TIM_TimeBaseStructure);                    //向上计数
	if (mode)
	{
		TIM_EncoderInterfaceConfig(Temp_TIM, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		
		TIM_ICInitStructure.TIM_ICFilter = 5;		// 适当滤波
		TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
	}
	else
	{
		if (Channel & 0x01)
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		// 上升沿捕获
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
		}
		if (Channel & (0x01 << 1))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		// 上升沿捕获
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
		}
		if (Channel & (0x01 << 2))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		// 上升沿捕获
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
		}
		if (Channel & (0x01 << 3))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		// 上升沿捕获
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			TIM_ICInitStructure.TIM_ICFilter = 0x0;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
		}
		// 中断配置
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ITConfig(Temp_TIM, TIM_IT_CC1, ENABLE); // 使能捕获中断
	}
    TIM_Cmd(Temp_TIM, ENABLE); // 使能定时器
#endif
}

void TIM2_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
}

void TIM3_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
}

void TIM1_HANDLERIT ()
{
	TIM_TypeDef *Temp_TIM = TIM1;
	TIM_Capture_Type temp_Capture = {0};
    if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC1) != RESET)
    {
        if(TIM1_Capture_Fun != NULL)
		{
			temp_Capture.Channel = 1;
			temp_Capture.val = TIM_GetCapture1(Temp_TIM);
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC1);
    }
    if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC2) != RESET)
    {
        if(TIM1_Capture_Fun != NULL)
		{
			temp_Capture.Channel = 2;
			temp_Capture.val = TIM_GetCapture1(Temp_TIM);
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC2);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC3) != RESET)
    {
        if(TIM1_Capture_Fun != NULL)
		{
			temp_Capture.Channel = 3;
			temp_Capture.val = TIM_GetCapture1(Temp_TIM);
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC2);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC4) != RESET)
    {
        if(TIM1_Capture_Fun != NULL)
		{
			temp_Capture.Channel = 4;
			temp_Capture.val = TIM_GetCapture1(Temp_TIM);
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC2);
    }
}


