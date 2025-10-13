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

int TIM1_arr = 0x00;
int TIM2_arr = 0x00;
int TIM3_arr = 0x00;
int TIM4_arr = 0x00;
int TIM5_arr = 0x00;

uint8_t TIM1_mode = 0x00;
uint8_t TIM2_mode = 0x00;
uint8_t TIM3_mode = 0x00;
uint8_t TIM4_mode = 0x00;
uint8_t TIM5_mode = 0x00;

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
			currentCount = tmr_counter_value_get(TMR1);
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
			currentCount = tmr_counter_value_get(TMR2);
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
			currentCount = tmr_counter_value_get(TMR3);
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
			currentCount = tmr_counter_value_get(TMR4);
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
			currentCount = tmr_counter_value_get(TMR5);
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

void TIM1_Capture_GPIO_Init(int Set)
{
#ifdef Exist_CAPTURE
    if (Set)
    {

    }
    else
    {
		
    }
#endif
}
void TIM2_Capture_GPIO_Init(int Set)
{
#ifdef Exist_CAPTURE

#endif
}
void TIM3_Capture_GPIO_Init(int Set)
{
#ifdef Exist_CAPTURE
	if (Set)
	{
		User_GPIO_config(1,6,0);
		User_GPIO_config(1,7,0);
		User_GPIO_config(2,0,0);
		User_GPIO_config(2,1,0);
	}
	else
	{
		
	}
#endif
}

/*
	溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
	Channel = 1-4
	mode 0-输入捕获	1-编码捕获
	set	0/1
*/
void TIM1_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#ifdef Exist_CAPTURE

#endif
}

void TIM2_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#ifdef Exist_CAPTURE

#endif
}

/*
	溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
	Channel = (1)-1Channel,(1<<1)-2Channel,(1<<2)-3Channel,(1<<3)-4Channel,
	mode 0-输入捕获	1-编码捕获
	set	0/1
*/
void TIM3_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#ifdef Exist_CAPTURE
    confirm_state state = FALSE;
    if (Set)
        state = TRUE;
    if(arr < 0 || psc < 0)
        return ;
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, state);
    tmr_type *Temp_TIM = TMR3;
    tmr_reset(Temp_TIM);
	/* nvic configuration */
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);

    TIM3_Capture_GPIO_Init(Set);
	TIM3_arr = arr;
    TIM3_mode = mode;
    tmr_base_init(Temp_TIM, arr, psc);
    tmr_cnt_dir_set(Temp_TIM, TMR_COUNT_UP);

	if (mode)
	{
        tmr_encoder_mode_config(Temp_TIM, TMR_ENCODER_MODE_C, TMR_INPUT_RISING_EDGE, TMR_INPUT_RISING_EDGE);
	}
	else
	{
        tmr_input_config_type tmr_input_struct;

        tmr_input_struct.input_filter_value = 0;
        tmr_input_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
        tmr_input_struct.input_polarity_select = TMR_INPUT_BOTH_EDGE;
        if (Channel & 0x01)
        {
            tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_1;
            tmr_input_channel_init(Temp_TIM,&tmr_input_struct,TMR_CHANNEL_INPUT_DIV_1);
            tmr_interrupt_enable(Temp_TIM, TMR_C1_INT, TRUE);
        }
        if (Channel & (0x01 << 1))
        {
            tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
            tmr_input_channel_init(Temp_TIM,&tmr_input_struct,TMR_CHANNEL_INPUT_DIV_1);
            tmr_interrupt_enable(Temp_TIM, TMR_C2_INT, TRUE);
        }
        if (Channel & (0x01 << 2))
        {
            tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_3;
            tmr_input_channel_init(Temp_TIM,&tmr_input_struct,TMR_CHANNEL_INPUT_DIV_1);
            tmr_interrupt_enable(Temp_TIM, TMR_C3_INT, TRUE);
        }
        if (Channel & (0x01 << 3))
        {
            tmr_input_struct.input_channel_select = TMR_SELECT_CHANNEL_4;
            tmr_input_channel_init(Temp_TIM,&tmr_input_struct,TMR_CHANNEL_INPUT_DIV_1);
            tmr_interrupt_enable(Temp_TIM, TMR_C4_INT, TRUE);
        }
	}
	tmr_counter_enable(Temp_TIM, state);
#endif
}

void TIM5_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set)
{
#ifdef Exist_CAPTURE

#endif
}

#ifdef Exist_CAPTURE
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
	tmr_type *Temp_TIM = TMR2;
	TIM_Capture_Type temp_Capture = {0};
    
    if(tmr_flag_get(Temp_TIM, TMR_C1_FLAG) != RESET)
    {
        temp_level = User_GPIO_get(1,15);
        temp_num = tmr_channel_value_get(Temp_TIM,TMR_SELECT_CHANNEL_1);
        tmr_flag_clear(Temp_TIM, TMR_C1_FLAG);
        temp_Capture.finish_flag = 0;
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
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag == 1)
		{
			temp_Capture.Channel = 1;
			TIM2_Capture_Fun(&temp_Capture);
		}
    }
    if(tmr_flag_get(Temp_TIM, TMR_C2_FLAG) != RESET)
    {
        temp_level = User_GPIO_get(2,3);
        temp_num = tmr_channel_value_get(Temp_TIM,TMR_SELECT_CHANNEL_2);
        tmr_flag_clear(Temp_TIM, TMR_C2_FLAG);
        temp_Capture.finish_flag = 0;
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
        if(TIM2_Capture_Fun != NULL && temp_Capture.finish_flag == 1)
		{
			temp_Capture.Channel = 2;
			TIM2_Capture_Fun(&temp_Capture);
		}
    }
}

int time3_ch1_mode = 0;
int time3_ch1_up = 0;
int time3_ch1_down = 0;
int time3_ch2_mode = 0;
int time3_ch2_up = 0;
int time3_ch2_down = 0;
int time3_ch3_mode = 0;
int time3_ch3_up = 0;
int time3_ch3_down = 0;
int time3_ch4_mode = 0;
int time3_ch4_up = 0;
int time3_ch4_down = 0;
void TIM3_HANDLERIT ()
{
	int temp_num = 0,temp_level = 0;
	tmr_type *Temp_TIM = TMR3;
	TIM_Capture_Type temp_Capture = {0};
    
    if(tmr_flag_get(Temp_TIM, TMR_C1_FLAG) != RESET)
    {
        temp_level = User_GPIO_get(1,6);
        temp_num = tmr_channel_value_get(Temp_TIM,TMR_SELECT_CHANNEL_1);
        tmr_flag_clear(Temp_TIM, TMR_C1_FLAG);
        temp_Capture.finish_flag = 0;
		if (time3_ch1_mode == 0 && temp_level == 1)
		{
			time3_ch1_up = temp_num;
			time3_ch1_mode ++;
		}
		else if (time3_ch1_mode == 1 && temp_level == 0)
		{
			time3_ch1_down = temp_num;
			time3_ch1_mode ++;
		}
		else if(time3_ch1_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time3_ch1_up)
			{
				temp_num += (TIM3_arr + 1);
			}
			temp_Capture.period_val = temp_num - time3_ch1_up;
			if (time3_ch1_down < time3_ch1_up)
			{
				time3_ch1_down += (TIM3_arr + 1);
			}
			temp_Capture.high_val = time3_ch1_down - time3_ch1_up;
			time3_ch1_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM3_Capture_Fun != NULL && temp_Capture.finish_flag == 1)
		{
			temp_Capture.Channel = 1;
			TIM3_Capture_Fun(&temp_Capture);
		}
    }
    if(tmr_flag_get(Temp_TIM, TMR_C2_FLAG) != RESET)
    {
        temp_level = User_GPIO_get(1,7);
        temp_num = tmr_channel_value_get(Temp_TIM,TMR_SELECT_CHANNEL_2);
        tmr_flag_clear(Temp_TIM, TMR_C2_FLAG);
        temp_Capture.finish_flag = 0;
		if (time3_ch2_mode == 0 && temp_level == 1)
		{
			time3_ch2_up = temp_num;
			time3_ch2_mode ++;
		}
		else if (time3_ch2_mode == 1 && temp_level == 0)
		{
			time3_ch2_down = temp_num;
			time3_ch2_mode ++;
		}
		else if(time3_ch2_mode >= 2 && temp_level == 1)
		{
			if (temp_num <= time3_ch2_up)
			{
				temp_num += (TIM3_arr + 1);
			}
			temp_Capture.period_val = temp_num - time3_ch2_up;
			if (time3_ch2_down < time3_ch2_up)
			{
				time3_ch2_down += (TIM3_arr + 1);
			}
			temp_Capture.high_val = time3_ch2_down - time3_ch2_up;
			time3_ch2_mode = 0;
			temp_Capture.finish_flag = 1;
		}
        if(TIM3_Capture_Fun != NULL && temp_Capture.finish_flag == 1)
		{
			temp_Capture.Channel = 2;
			TIM3_Capture_Fun(&temp_Capture);
		}
    }

}
#endif
