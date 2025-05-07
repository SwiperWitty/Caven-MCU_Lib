#include "Base_PWM.h"

uint8_t tim0_enable = 0;
uint8_t tim1_enable = 0;
uint8_t tim2_enable = 0;
uint8_t tim3_enable = 0;
uint8_t tim4_enable = 0;
uint8_t tim5_enable = 0;

void TIM3_PWM_GPIO_Init(int Set)
{
#ifdef Exist_PWM
    rcu_periph_clock_enable(RCU_GPIOB);
    if (Set)
    {
        gpio_af_set(GPIOB, GPIO_AF_2, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
    }
    else
    {
		gpio_mode_set(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
    }
#endif
}
void TIM8_PWM_GPIO_Init(int Set)
{
#ifdef Exist_PWM

#endif

}


// 溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
void TIM3_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM
    uint32_t timx = TIMER3;
    rcu_periph_clock_enable(RCU_TIMER3);
    TIM3_PWM_GPIO_Init(Set);
    tim3_enable = Set;
    
    timer_parameter_struct timer_initpara;
    timer_initpara.prescaler         = psc;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(timx, &timer_initpara);
    
    // 
    timer_oc_parameter_struct timer_ocintpara;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    
    timer_channel_output_config(timx, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_config(timx, TIMER_CH_1, &timer_ocintpara);
    timer_channel_output_config(timx, TIMER_CH_2, &timer_ocintpara);
    timer_channel_output_config(timx, TIMER_CH_3, &timer_ocintpara);
    
    timer_channel_output_mode_config(timx, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(timx, TIMER_CH_0, 0);
    timer_channel_output_mode_config(timx, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(timx, TIMER_CH_1, 0);
    timer_channel_output_mode_config(timx, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(timx, TIMER_CH_2, 0);
    timer_channel_output_mode_config(timx, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_pulse_value_config(timx, TIMER_CH_3, 0);
    // 使能定时器
    timer_auto_reload_shadow_enable(timx);
    if(Set)
    {
        timer_enable(timx);
    }
    else
    {
        timer_disable(timx);
    }
#endif
}

// 溢出时间 T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
void TIM4_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM

#endif
}

void TIM8_PWM_Start_Init(int arr,int psc,int Set)
{
#ifdef Exist_PWM
    //

#endif
}

void TIM3_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM
    uint32_t timx = TIMER3;
    if(tim3_enable == 0)
    {
        return;
    }
    switch (PWMx)
    {
    case (1):
        timer_channel_output_pulse_value_config(timx, TIMER_CH_0, value);
        break;
    case (2):
        timer_channel_output_pulse_value_config(timx, TIMER_CH_1, value);
        break;
    case (3):
        timer_channel_output_pulse_value_config(timx, TIMER_CH_2, value);
        break;
    case (4):
        timer_channel_output_pulse_value_config(timx, TIMER_CH_3, value);
        break;
    default:
        break;
    }
#endif
}

void TIM4_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM

#endif
}

void TIM8_PWMx_SetValue(char PWMx,int value)
{
#ifdef Exist_PWM

#endif
}

