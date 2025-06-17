#include "Base_Capture.h"
#include "Base_GPIO.h"

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

int TIM1_arr = 0;
int TIM2_arr = 0;
int TIM3_arr = 0;
int TIM4_arr = 0;
int TIM5_arr = 0;

uint8_t TIM1_mode = 0x00;
uint8_t TIM2_mode = 0x00;
uint8_t TIM3_mode = 0x00;
uint8_t TIM4_mode = 0x00;
uint8_t TIM5_mode = 0x00;

uint8_t TIM1_enable = 0x00;
uint8_t TIM2_enable = 0x00;
uint8_t TIM3_enable = 0x00;
uint8_t TIM4_enable = 0x00;
uint8_t TIM5_enable = 0x00;


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
            if(TIM1_mode == 0)
            {
                return 0;
            }
			lastCount = TIM1_lastCount;
			currentCount = TIM_GetCounter(TIM1);
			TIM1_lastCount = currentCount;
		}
		break;
		case 2:
		{
            if(TIM2_mode == 0)
            {
                return 0;
            }
			lastCount = TIM2_lastCount;
			currentCount = TIM_GetCounter(TIM2);
			TIM2_lastCount = currentCount;
		}
		break;
		case 3:
		{
            if(TIM3_mode == 0)
            {
                return 0;
            }
			lastCount = TIM3_lastCount;
			currentCount = TIM_GetCounter(TIM3);
			TIM3_lastCount = currentCount;
		}
		break;
		case 4:
		{
            if(TIM4_mode == 0)
            {
                return 0;
            }
			lastCount = TIM4_lastCount;
			currentCount = TIM_GetCounter(TIM4);
			TIM4_lastCount = currentCount;
		}
		break;
		case 5:
		{
            if(TIM5_mode == 0)
            {
                return 0;
            }
			lastCount = TIM5_lastCount;
			currentCount = TIM_GetCounter(TIM5);
			TIM5_lastCount = currentCount;
		}
		break;
		default:
        {
            return 0;
        }
	}
    int diff = (currentCount - lastCount);
    return diff;
}

// PA8 PA9 PA10 PA11 
void TIM1_Capture_GPIO_Init(int Set)
{
#if Exist_CAPTURE
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                                         //复用模式
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IPD;
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

// PB3 PA15
void TIM2_Capture_GPIO_Init(int Set)
{
#if Exist_CAPTURE
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
    #if TIM2_REMAP == OPEN_0000
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
    
    #elif TIM2_REMAP == OPEN_0001
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		gpio_init_struct.GPIO_Pin = GPIO_Pin_15;
        GPIO_Init(GPIOA, &gpio_init_struct);
        gpio_init_struct.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOB, &gpio_init_struct);
    }
    else
    {
		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpio_init_struct.GPIO_Pin = GPIO_Pin_15;
        GPIO_Init(GPIOA, &gpio_init_struct);
        gpio_init_struct.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOB, &gpio_init_struct);
    #endif
    }
#endif
}
void TIM3_Capture_GPIO_Init(int Set)
{
#if Exist_CAPTURE
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		//gpio时钟
//        gpio_pin_remap_config(TMR3_GMUX_0011,TRUE);           // 重映射Tim3

		gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
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
	溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
	Channel = (1)-1Channel,(1<<1)-2Channel,(1<<2)-3Channel,(1<<3)-4Channel,
	mode 0-输入捕获	1-编码捕获
	set	0/1
*/
void TIM1_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#if Exist_CAPTURE
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
	TIM1_arr = arr;
    TIM1_mode = mode;
    TIM1_enable = Channel;
    
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Temp_TIM, &TIM_TimeBaseStructure);         // 向上计数
	if (mode)
	{
		TIM_EncoderInterfaceConfig(Temp_TIM, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		
		TIM_ICInitStructure.TIM_ICFilter = 5;                   // 适当滤波
		TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
	}
	else
	{
		// 中断配置
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;		// 上升沿捕获
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICFilter = 0x0;
		if (Channel & 0x01)
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC1, ENABLE);
		}
		if (Channel & (0x01 << 1))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC2, ENABLE);
		}
		if (Channel & (0x01 << 2))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC3, ENABLE);
		}
		if (Channel & (0x01 << 3))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC4, ENABLE);
		}
	}
    TIM_Cmd(Temp_TIM, ENABLE); // 使能定时器
#endif
}

void TIM2_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#if Exist_CAPTURE
    FunctionalState state = DISABLE;
    if (Set)
        state = ENABLE;
    if(arr < 0 || psc < 0)
        return ;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, state);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// AFIO复用功能模块时钟
    
    TIM_TypeDef *Temp_TIM = TIM2;

    TIM2_Capture_GPIO_Init(Set);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);       // 全重映射
	TIM2_arr = arr;
    TIM2_mode = mode;
    TIM2_enable = Channel;
    
    TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Temp_TIM, &TIM_TimeBaseStructure);     // 向上计数
	if (mode)
	{
		TIM_EncoderInterfaceConfig(Temp_TIM, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
		
		TIM_ICInitStructure.TIM_ICFilter = 5;		        // 适当滤波
		TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
	}
	else
	{
		// 中断配置
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICFilter = 0x0;
		if (Channel & 0x01)
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC1, ENABLE);
		}
		if (Channel & (0x01 << 1))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC2, ENABLE);
		}
		if (Channel & (0x01 << 2))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC3, ENABLE);
		}
		if (Channel & (0x01 << 3))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC4, ENABLE);
		}
	}
    TIM_Cmd(Temp_TIM, ENABLE); // 使能定时器
#endif
}

void TIM3_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#if Exist_CAPTURE
    FunctionalState state = DISABLE;
    if (Set)
        state = ENABLE;
    if(arr < 0 || psc < 0)
        return ;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, state);
    TIM_TypeDef *Temp_TIM = TIM2;

    TIM3_Capture_GPIO_Init(Set);
	TIM3_arr = arr;
    TIM3_mode = mode;
    TIM3_enable = Channel;
    
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
		// 中断配置
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;		// 上升沿捕获
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICFilter = 0x0;
		if (Channel & 0x01)
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC1, ENABLE);
		}
		if (Channel & (0x01 << 1))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC2, ENABLE);
		}
		if (Channel & (0x01 << 2))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC3, ENABLE);
		}
		if (Channel & (0x01 << 3))
		{
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
			TIM_ICInit(Temp_TIM, &TIM_ICInitStructure);
			TIM_ITConfig(Temp_TIM, TIM_IT_CC4, ENABLE);
		}
	}
    TIM_Cmd(Temp_TIM, ENABLE); // 使能定时器
#endif
}

void TIM5_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#if Exist_CAPTURE

#endif
}

#if Exist_CAPTURE
int time1_ch1_mode = 0;
int time1_ch1_up = 0;
int time1_ch1_down = 0;
int time1_ch2_mode = 0;
int time1_ch2_up = 0;
int time1_ch2_down = 0;
int time1_ch3_mode = 0;
int time1_ch3_up = 0;
int time1_ch3_down = 0;
int time1_ch4_mode = 0;
int time1_ch4_up = 0;
int time1_ch4_down = 0;
void TIM1_HANDLERIT ()
{
	int temp_num = 0,temp_level = 0;
	TIM_TypeDef *Temp_TIM = TIM1;
	TIM_Capture_Type temp_Capture = {0};
    if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC1) != RESET && TIM1_enable & 0x01)
    {
        temp_level = User_GPIO_get(1,8);
        temp_num = TIM_GetCapture1(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time1_ch1_mode == 0 && temp_level == 1)
		{
			time1_ch1_up = temp_num;
			time1_ch1_mode ++;
		}
		else if (time1_ch1_mode == 1 && temp_level == 0)
		{
			time1_ch1_down = temp_num;
			time1_ch1_mode ++;
		}
		else if(time1_ch1_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time1_ch1_up)
			{
				temp_num += (TIM1_arr + 1);
			}
			temp_Capture.period_val = temp_num - time1_ch1_up;
			if (time1_ch1_down < time1_ch1_up)
			{
				time1_ch1_down += (TIM1_arr + 1);
			}
			temp_Capture.high_val = time1_ch1_down - time1_ch1_up;
			time1_ch1_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM1_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 1;
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC1);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC2) != RESET && TIM1_enable & 0x02)
    {
        temp_level = User_GPIO_get(1,9);
        temp_num = TIM_GetCapture2(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time1_ch2_mode == 0 && temp_level == 1)
		{
			time1_ch2_up = temp_num;
			time1_ch2_mode ++;
		}
		else if (time1_ch2_mode == 1 && temp_level == 0)
		{
			time1_ch2_down = temp_num;
			time1_ch2_mode ++;
		}
		else if(time1_ch2_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time1_ch2_up)
			{
				temp_num += (TIM1_arr + 1);
			}
			temp_Capture.period_val = temp_num - time1_ch2_up;
			if (time1_ch2_down < time1_ch2_up)
			{
				time1_ch2_down += (TIM1_arr + 1);
			}
			temp_Capture.high_val = time1_ch2_down - time1_ch2_up;
			time1_ch2_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM1_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 2;
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC2);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC3) != RESET && TIM1_enable & 0x04)
    {
        temp_level = User_GPIO_get(1,10);
        temp_num = TIM_GetCapture3(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time1_ch3_mode == 0 && temp_level == 1)
		{
			time1_ch3_up = temp_num;
			time1_ch3_mode ++;
		}
		else if (time1_ch3_mode == 1 && temp_level == 0)
		{
			time1_ch3_down = temp_num;
			time1_ch3_mode ++;
		}
		else if(time1_ch3_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time1_ch3_up)
			{
				temp_num += (TIM1_arr + 1);
			}
			temp_Capture.period_val = temp_num - time1_ch3_up;
			if (time1_ch3_down < time1_ch3_up)
			{
				time1_ch3_down += (TIM1_arr + 1);
			}
			temp_Capture.high_val = time1_ch3_down - time1_ch3_up;
			time1_ch3_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM1_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 3;
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC3);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC4) != RESET && TIM1_enable & 0x08)
    {
        temp_level = User_GPIO_get(1,11);
        temp_num = TIM_GetCapture4(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time1_ch4_mode == 0 && temp_level == 1)
		{
			time1_ch4_up = temp_num;
			time1_ch4_mode ++;
		}
		else if (time1_ch4_mode == 1 && temp_level == 0)
		{
			time1_ch4_down = temp_num;
			time1_ch4_mode ++;
		}
		else if(time1_ch4_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time1_ch4_up)
			{
				temp_num += (TIM1_arr + 1);
			}
			temp_Capture.period_val = temp_num - time1_ch4_up;
			if (time1_ch4_down < time1_ch4_up)
			{
				time1_ch4_down += (TIM1_arr + 1);
			}
			temp_Capture.high_val = time1_ch4_down - time1_ch4_up;
			time1_ch4_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM1_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 4;
			TIM1_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC4);
    }
}

int time2_ch1_mode = 0;
int time2_ch1_up = 0;
int time2_ch1_down = 0;
int time2_ch2_mode = 0;
int time2_ch2_up = 0;
int time2_ch2_down = 0;
int time2_ch3_mode = 0;
int time2_ch3_up = 0;
int time2_ch3_down = 0;
int time2_ch4_mode = 0;
int time2_ch4_up = 0;
int time2_ch4_down = 0;
void TIM2_HANDLERIT ()
{
	int temp_num = 0,temp_level = 0;
	TIM_TypeDef *Temp_TIM = TIM2;
	TIM_Capture_Type temp_Capture = {0};
    if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC1) != RESET && TIM2_enable & 0x01)
    {
        temp_level = User_GPIO_get(1,15);
        temp_num = TIM_GetCapture1(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time2_ch1_mode == 0 && temp_level == 1)
		{
			time2_ch1_up = temp_num;
			time2_ch1_mode ++;
		}
		else if (time2_ch1_mode == 1 && temp_level == 0)
		{
			time2_ch1_down = temp_num;
			time2_ch1_mode ++;
		}
		else if(time2_ch1_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time2_ch1_up)
			{
				temp_num += (TIM2_arr + 1);
			}
			temp_Capture.period_val = temp_num - time2_ch1_up;
			if (time2_ch1_down < time2_ch1_up)
			{
				time2_ch1_down += (TIM2_arr + 1);
			}
			temp_Capture.high_val = time2_ch1_down - time2_ch1_up;
			time2_ch1_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 1;
			TIM2_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC1);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC2) != RESET && TIM2_enable & 0x02)
    {
        temp_level = User_GPIO_get(2,3);
        temp_num = TIM_GetCapture2(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time2_ch2_mode == 0 && temp_level == 1)
		{
			time2_ch2_up = temp_num;
			time2_ch2_mode ++;
		}
		else if (time2_ch2_mode == 1 && temp_level == 0)
		{
			time2_ch2_down = temp_num;
			time2_ch2_mode ++;
		}
		else if(time2_ch2_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time2_ch2_up)
			{
				temp_num += (TIM2_arr + 1);
			}
			temp_Capture.period_val = temp_num - time2_ch2_up;
			if (time2_ch2_down < time2_ch2_up)
			{
				time2_ch2_down += (TIM2_arr + 1);
			}
			temp_Capture.high_val = time2_ch2_down - time2_ch2_up;
			time2_ch2_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 2;
			TIM2_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC2);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC3) != RESET && TIM2_enable & 0x04)
    {
        temp_level = User_GPIO_get(2,10);
        temp_num = TIM_GetCapture3(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time2_ch3_mode == 0 && temp_level == 1)
		{
			time2_ch3_up = temp_num;
			time2_ch3_mode ++;
		}
		else if (time2_ch3_mode == 1 && temp_level == 0)
		{
			time2_ch3_down = temp_num;
			time2_ch3_mode ++;
		}
		else if(time2_ch3_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time2_ch3_up)
			{
				temp_num += (TIM2_arr + 1);
			}
			temp_Capture.period_val = temp_num - time2_ch3_up;
			if (time2_ch3_down < time2_ch3_up)
			{
				time2_ch3_down += (TIM2_arr + 1);
			}
			temp_Capture.high_val = time2_ch3_down - time2_ch3_up;
			time2_ch3_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 3;
			TIM2_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC3);
    }
	if(TIM_GetITStatus(Temp_TIM, TIM_IT_CC4) != RESET && TIM2_enable & 0x08)
    {
        temp_level = User_GPIO_get(2,11);
        temp_num = TIM_GetCapture4(Temp_TIM);
		memset(&temp_Capture,0,sizeof(TIM_Capture_Type));
		if (time2_ch4_mode == 0 && temp_level == 1)
		{
			time2_ch4_up = temp_num;
			time2_ch4_mode ++;
		}
		else if (time2_ch4_mode == 1 && temp_level == 0)
		{
			time2_ch4_down = temp_num;
			time2_ch4_mode ++;
		}
		else if(time2_ch4_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time2_ch4_up)
			{
				temp_num += (TIM2_arr + 1);
			}
			temp_Capture.period_val = temp_num - time2_ch4_up;
			if (time2_ch4_down < time2_ch4_up)
			{
				time2_ch4_down += (TIM2_arr + 1);
			}
			temp_Capture.high_val = time2_ch4_down - time2_ch4_up;
			time2_ch4_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag)
		{
			temp_Capture.Channel = 4;
			TIM2_Capture_Fun(&temp_Capture);
		}
        TIM_ClearITPendingBit(Temp_TIM, TIM_IT_CC4);
    }
}

#endif
