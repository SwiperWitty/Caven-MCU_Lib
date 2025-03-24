#include "Base_adc.h"

float VDDA_num = 3.30; 							//其实这个是动态的，ADC内部基准源
static int Channel_NUMs;
static u16 ADC1_DMA_list[20];
int adc_enable_flag = 0;

void ADC_GPIO_Init(int Set)
{
#ifdef Exist_ADC
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    adc_enable_flag = Set;
    if (Set == 0)
    {
        return ;
    }
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB,ENABLE);

    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Pin = 0;                 //清除

    #ifdef ADC_IO_PA0
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_0;
    #endif
    #ifdef ADC_IO_PA1
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_1;
    #endif
    #ifdef ADC_IO_PA2
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_2;
    #endif
    #ifdef ADC_IO_PA3
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_3;
    #endif
    #ifdef ADC_IO_PA4
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_4;
    #endif
    #ifdef ADC_IO_PA5
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_5;
    #endif
    #ifdef ADC_IO_PA6
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_6;
    #endif
    #ifdef ADC_IO_PA7
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_7;
    #endif
    GPIO_Init(GPIOA, &gpio_init_struct);

    gpio_init_struct.GPIO_Pin = 0;                 //清除
    #ifdef ADC_IO_PB0
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_0;
    #endif
    #ifdef ADC_IO_PB1
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_1;
    #endif
    GPIO_Init(GPIOB, &gpio_init_struct);

#endif
}

void ADC1_DMA_Config (const void *DMA_Buffer,int BufferSize)
{
#ifdef Exist_ADC
    DMA_InitTypeDef dma_init_struct;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 配置DMA中断（传输完成）
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_DeInit(DMA1_Channel1);
    dma_init_struct.DMA_BufferSize  = BufferSize;
    dma_init_struct.DMA_PeripheralBaseAddr = ADC1->DR;          // 外设地址（ADC数据寄存器）
    dma_init_struct.DMA_MemoryBaseAddr = (uint32_t)DMA_Buffer;  // 内存地址（目标数组）
    dma_init_struct.DMA_DIR = DMA_DIR_PeripheralSRC;            // 外设→内存
    dma_init_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定
    dma_init_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;       // 内存地址自增
    dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位数据
    dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init_struct.DMA_Mode = DMA_Mode_Circular;               // 循环模式
    dma_init_struct.DMA_Priority = DMA_Priority_High;
    dma_init_struct.DMA_M2M = DMA_M2M_Disable;                  // 禁用内存到内存
    DMA_Init(DMA1_Channel1, &dma_init_struct);

    // 启用DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

#endif
}

int Base_ADC_Init(int Set)
{
#ifdef Exist_ADC
    FunctionalState set = DISABLE;
    if (Set)
    {
        set = ENABLE;
    }
    ADC_InitTypeDef adc_base_struct;
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, set);
//    crm_adc_clock_div_set(CRM_ADC_DIV_6);                         //ADC是时钟的6分频
    ADC_GPIO_Init(Set);                                             //GPIO

    int list = 1;
    #ifdef ADC_IO_PA0
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA0, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA1
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA1, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA2
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA2, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA3
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA3, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA4
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA4, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA5
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA5, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA6
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA6, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PA7
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA7, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PB0
    ADC_RegularChannelConfig(ADC1, ADC_IO_PB0, list++, ADC_Speed);
    #endif
    #ifdef ADC_IO_PB1
    ADC_RegularChannelConfig(ADC1, ADC_IO_PB1, list++, ADC_Speed);
    #endif
    #ifdef ADC_Temp
    ADC_RegularChannelConfig(ADC1, ADC_Temp, list++, ADC_Speed);
    ADC_TempSensorVrefintCmd(set);                           //开温传
    #endif
    Channel_NUMs = list - 1;
    
    ADC1_DMA_Config (ADC1_DMA_list, Channel_NUMs);

    ADC_StructInit(&adc_base_struct);
    adc_base_struct.ADC_Mode = ADC_Mode_Independent;          // 独立模式
    adc_base_struct.ADC_ScanConvMode = set;
    adc_base_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 软件触发
    adc_base_struct.ADC_ContinuousConvMode = DISABLE;                   // 连续转换
    adc_base_struct.ADC_DataAlign = ADC_DataAlign_Right;
    adc_base_struct.ADC_NbrOfChannel  = Channel_NUMs;            //  Channel_NUM
    ADC_Init(ADC1, &adc_base_struct);
    
    ADC_DMACmd(ADC1, ENABLE);                       //ADC  DMA
    
    // 启动ADC
    ADC_Cmd(ADC1, ENABLE);
    
    // 校准ADC（推荐）
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
#endif
    return 0;
}

float ADC_Conversion_Vol_Fun(int num)
{
    float Temp = num;
#ifdef Exist_ADC
    Temp /= ADC_MAX;
    Temp *= ADC_VREF;
#endif
    return Temp;
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
    if (DMA_GetITStatus(DMA1_IT_TC1))       // ADC数据全部完成
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        if (ADC_Quick_Get_pFun != NULL && adc_enable_flag > 0)
        {
            ADC_Quick_Get_pFun(ADC1_DMA_list);
        }
    }
}
#endif
