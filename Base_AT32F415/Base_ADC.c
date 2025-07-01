#include "Base_ADC.h"


#ifdef Exist_ADC

float VDDA_num = 3.30; 							//其实这个是动态的，ADC内部基准源

static int Channel_NUMs;
static u16 ADC1_DMA_list[20];
int adc_enable_flag = 0;
static void ADC_GPIO_Init(int Set)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set == 0)
    {
        return;
    }
    
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_pins = 0;                 //清除
    
    Channel_NUMs = 0;
    #if (Exist_ADC & OPEN_0001)
    Channel_NUMs += 2;
    gpio_init_struct.gpio_pins |= GPIO_PINS_0 | ADC_CHANNEL_1;  // tim5
    #endif
    #if (Exist_ADC & OPEN_0010)
    Channel_NUMs += 2;
    gpio_init_struct.gpio_pins |= GPIO_PINS_2 | ADC_CHANNEL_3;  // uart2
    #endif
    #if (Exist_ADC & OPEN_0100)
    Channel_NUMs += 2;
    gpio_init_struct.gpio_pins |= GPIO_PINS_4 | ADC_CHANNEL_5;  // adc
    #endif
    #if (Exist_ADC & OPEN_1000)
    Channel_NUMs += 2;
    gpio_init_struct.gpio_pins |= GPIO_PINS_6 | ADC_CHANNEL_7;  // tim3
    #endif
    
    Channel_NUMs += 1;  // ADC_Temp
    gpio_init(GPIOA, &gpio_init_struct);
}

static void ADC1_DMA_Config (void)
{
    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
    dma_reset(DMA1_CHANNEL1);
    if(Channel_NUMs)
    {
        dma_init_struct.buffer_size = Channel_NUMs;
        dma_init_struct.memory_base_addr = (uint32_t)ADC1_DMA_list;
        dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
        dma_init_struct.memory_inc_enable = TRUE;
        dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
        dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
        dma_init_struct.peripheral_inc_enable = FALSE;
        dma_init_struct.priority = DMA_PRIORITY_HIGH;
        dma_init_struct.loop_mode_enable = TRUE;
        dma_init(DMA1_CHANNEL1, &dma_init_struct);

        dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
        dma_channel_enable(DMA1_CHANNEL1, TRUE);
    }
}

static int ADC_Start_Init(int Set)
{
	int retval = -1;

    adc_base_config_type adc_base_struct;
    crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
    crm_adc_clock_div_set(CRM_ADC_DIV_6);                   // 时钟配置在最前 

    Channel_NUMs = 0;
    #if (Exist_ADC & OPEN_0001)
    adc_ordinary_channel_set(ADC1, ADC_IO_PA0, ++Channel_NUMs, ADC_Speed);
    adc_ordinary_channel_set(ADC1, ADC_IO_PA1, ++Channel_NUMs, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_0010)
    adc_ordinary_channel_set(ADC1, ADC_IO_PA2, ++Channel_NUMs, ADC_Speed);
    adc_ordinary_channel_set(ADC1, ADC_IO_PA3, ++Channel_NUMs, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_0100)
    adc_ordinary_channel_set(ADC1, ADC_IO_PA4, ++Channel_NUMs, ADC_Speed);
    adc_ordinary_channel_set(ADC1, ADC_IO_PA5, ++Channel_NUMs, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_1000)
    adc_ordinary_channel_set(ADC1, ADC_IO_PA6, ++Channel_NUMs, ADC_Speed);
    adc_ordinary_channel_set(ADC1, ADC_IO_PA7, ++Channel_NUMs, ADC_Speed);
    #endif
    adc_ordinary_channel_set(ADC1, ADC_Temp, ++Channel_NUMs, ADC_Speed);

    adc_base_default_para_init(&adc_base_struct);
    adc_base_struct.sequence_mode = TRUE;
    adc_base_struct.repeat_mode = TRUE;
    adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
    adc_base_struct.ordinary_channel_length = Channel_NUMs;
    adc_base_config(ADC1, &adc_base_struct);

    if (Set)
    {
        adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);  // 普通通道组转换的触发模式使能及触发事件选择为软件
        adc_dma_mode_enable(ADC1, TRUE);                    // ADC  DMA
        adc_tempersensor_vintrv_enable(TRUE);               // 温传
        
        adc_enable(ADC1, TRUE);
        adc_calibration_init(ADC1);                         // 校准
        while(adc_calibration_init_status_get(ADC1));       // 等待复位校准结束
        adc_calibration_start(ADC1);                        // 开启AD校准
        while(adc_calibration_status_get(ADC1));
        retval = 0;
    }
    else
    {
        adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, FALSE); // 普通通道组转换的触发模式使能及触发事件选择为软件
        adc_dma_mode_enable(ADC1, FALSE);                   // ADC  DMA
        adc_enable(ADC1, FALSE);
        retval = 1;
    }

	return retval;
}
#endif

int Base_ADC_Init(int Set)
{
    int retval = 0;
    adc_enable_flag = Set;
#ifdef Exist_ADC
    ADC_GPIO_Init(Set);
    if (Set)
    {
        ADC1_DMA_Config ();
    }
    retval = ADC_Start_Init(Set);
    if(retval == 0)
    {
        adc_ordinary_software_trigger_enable(ADC1, TRUE);       // 启动
    }
#endif
    return retval;
}

float ADC_Get_MCU_Temperature_Fun(void)
{
    float retval;
#ifdef ADC_Temp
    double Temp = 0;
	// adc_ordinary_software_trigger_enable(ADC1, TRUE);
    // while(dma_trans_complete_flag == 0);
    // dma_trans_complete_flag = 0;
	
    Temp = ADC_Conversion_Vol_Fun(ADC1_DMA_list[Channel_NUMs - 1]);     // 温传是最后一个 
    Temp = (ADC_TEMP_BASE - Temp) / ADC_TEMP_SLOPE + 25;
	retval = (float)Temp;
#endif
    return retval;
}

float ADC_Conversion_Vol_Fun(int NUM)
{
    float Temp = NUM;
#ifdef Exist_ADC
    Temp /= ADC_MAX;
    Temp *= ADC_VREF;
#endif
    return Temp;
}

/*
回调函数
*/
D_pFun ADC_Quick_Get_pFun = NULL;
void ADC_Quick_Get_Bind_Fun(D_pFun ADC_pFun)
{
    if (ADC_pFun != NULL)
    {
        ADC_Quick_Get_pFun = ADC_pFun;
    }
}

#ifdef Exist_ADC
void ADC_FINISH_HANDLERIT()
{
//    int *temp_p = ADC1_DMA_list;
    if(dma_flag_get(DMA1_FDT1_FLAG) != RESET)       // ADC数据全部完成
    {
        dma_flag_clear(DMA1_FDT1_FLAG);
        if (ADC_Quick_Get_pFun != NULL && adc_enable_flag > 0)
        {
            ADC_Quick_Get_pFun(ADC1_DMA_list);
        }
    }
}
#endif
