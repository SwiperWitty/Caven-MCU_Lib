#include "Base_PWM.h"

uint8_t tim3_enable = 0;
uint8_t tim4_enable = 0;
uint8_t tim8_enable = 0;

void TIM3_PWM_GPIO_Init(int Set)
{
#if Exist_PWM
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);                       // gpio时钟
        crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
        gpio_pin_remap_config(TMR3_GMUX_0011,TRUE);                                 // 重映射Tim3

        gpio_init_struct.gpio_pins = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;                                         // 复用模式
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOC, &gpio_init_struct);
    }
#endif
}
void TIM4_PWM_GPIO_Init(int Set)
{
#if Exist_PWM
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);       //gpio时钟
        crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

        gpio_init_struct.gpio_pins = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;                                         //复用模式
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
#endif
}


/*
溢出 T = (arr+1)*(psc+1)
arr是run_cnt溢出值，psc是对时钟源的分频
*/
void TIM3_PWM_Start_Init(int arr,int psc,int Set)
{
#if Exist_PWM
    confirm_state state = FALSE;
    if (Set)
        state = TRUE;
    if(arr < 0 || psc < 0)
        return ;
    tim3_enable = Set;
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, state);
    tmr_type *Temp_TIM = TMR3;
    tmr_reset(Temp_TIM);

    TIM3_PWM_GPIO_Init(Set);

    tmr_base_init(Temp_TIM,arr,psc);                            //前面是周期，后面是分频
    tmr_cnt_dir_set(Temp_TIM, TMR_COUNT_UP);                    //向上计数

    tmr_output_config_type tmr_output_struct;                   //时钟输出配置（PWM）
    tmr_output_default_para_init(&tmr_output_struct);
    tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
    tmr_output_struct.oc_output_state = TRUE;
    tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
    tmr_output_struct.oc_idle_state = TRUE;
    tmr_output_struct.occ_output_state = FALSE;
    tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
    tmr_output_struct.occ_idle_state = FALSE;

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_1, 0);                       //TIMx 通道1 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_2, 0);                       //TIMx 通道2 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_3, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_3, 0);                       //TIMx 通道3 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_4, 0);                       //TIMx 通道4 启动时占峰比为0

    tmr_output_enable(Temp_TIM, state);          //使能定时器输出
    tmr_counter_enable(Temp_TIM, state);         //启动定时器 TIMx
#endif
}

//溢出时间T = (arr+1)*(psc+1)
void TIM4_PWM_Start_Init(int arr,int psc,int Set)
{
#if Exist_PWM
    confirm_state state = FALSE;
    if (Set)
        state = TRUE;
    if(arr < 0 || psc < 0)
        return ;
    tim4_enable = Set;
    crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, state);
    tmr_type *Temp_TIM = TMR4;
    tmr_reset(Temp_TIM);
    crm_clocks_freq_type crm_clocks_freq_struct = {0};
    crm_clocks_freq_get(&crm_clocks_freq_struct);               //get system clock 

    TIM4_PWM_GPIO_Init(Set);

    tmr_base_init(Temp_TIM,arr,psc);                            //前面是周期，后面是分频
    tmr_cnt_dir_set(Temp_TIM, TMR_COUNT_UP);                    //向上计数

    tmr_output_config_type tmr_output_struct;                   //时钟输出配置（PWM）
    tmr_output_default_para_init(&tmr_output_struct);
    tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
    tmr_output_struct.oc_output_state = TRUE;
    tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
    tmr_output_struct.oc_idle_state = TRUE;
    tmr_output_struct.occ_output_state = FALSE;
    tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
    tmr_output_struct.occ_idle_state = FALSE;

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_1, 0);                       //TIMx 通道1 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_2, 0);                       //TIMx 通道2 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_3, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_3, 0);                       //TIMx 通道3 启动时占峰比为0

    tmr_output_channel_config(Temp_TIM, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
    tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_4, 0);                       //TIMx 通道4 启动时占峰比为0

    tmr_output_enable(Temp_TIM, state);          //使能定时器输出
    tmr_counter_enable(Temp_TIM, state);         //启动定时器 TIMx
#endif
}

//溢出时间T = (arr+1)*(psc+1)
void TIM8_PWM_Start_Init(int arr,int psc,int Set)
{
#if TIM8_PWM
    //AT32f415没有定时器8，但是IO与定时器3的复用是一样的
    tim8_enable = Set;
#endif
}

void TIM3_PWMx_SetValue(int PWMx,void *data)
{
#if Exist_PWM
    tmr_type *Temp_TIM = TMR3;
    int value = 0;
    if(data == NULL)
    {
        return;
    }
    if (tim3_enable == 0)
    {
        TIM3_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 100000)-1,1);
    }
    
    value = *(int *)data;
    switch (PWMx)
    {
    case (1):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_1, value);
        break;
    case (2):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_2, value);
        break;
    case (3):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_3, value);
        break;
    case (4):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_4, value);
        break;
    default:
        break;
    }
#endif
}

void TIM4_PWMx_SetValue(int PWMx,void *data)
{
#if Exist_PWM
    tmr_type *Temp_TIM = TMR4;
    int value = 0;
    if(data == NULL)
    {
        return;
    }
    if (tim4_enable == 0)
    {
        TIM4_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 100000)-1,1);
    }
    
    value = *(int *)data;
    switch (PWMx)
    {
    case (1):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_1, value);
        break;
    case (2):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_2, value);
        break;
    case (3):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_3, value);
        break;
    case (4):
        tmr_channel_value_set(Temp_TIM, TMR_SELECT_CHANNEL_4, value);
        break;
    default:
        break;
    }
#endif
}

