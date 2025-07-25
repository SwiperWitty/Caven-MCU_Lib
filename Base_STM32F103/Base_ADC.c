#include "Base_ADC.h"

#if Exist_ADC
float VDDA_num = 3.30; 							//其实这个是动态的，ADC内部基准源
static int Channel_NUMs;
static u16 ADC1_DMA_list[20];
int adc_enable_flag = 0;

static void ADC_GPIO_Init(int Set)
{
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    adc_enable_flag = Set;
    if (Set == 0)
    {
        return ;
    }
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    gpio_init_struct.GPIO_Mode = GPIO_Mode_AIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Pin = 0;                 // 清除

    Channel_NUMs = 0;
    #if (Exist_ADC & OPEN_0001)
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_0 | GPIO_Pin_1;
    #endif
    #if (Exist_ADC & OPEN_0010)
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_2 | GPIO_Pin_3;
    #endif
    #if (Exist_ADC & OPEN_0100)
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_4 | GPIO_Pin_5;
    #endif
    #if (Exist_ADC & OPEN_1000)
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_6 | GPIO_Pin_7;
    #endif
    GPIO_Init(GPIOA, &gpio_init_struct);

    gpio_init_struct.GPIO_Pin = 0;                 // 清除
    #if (Exist_ADC & OPEN_10000)
    gpio_init_struct.GPIO_Pin |= GPIO_Pin_0 | GPIO_Pin_1;
    #endif
    GPIO_Init(GPIOB, &gpio_init_struct);
}
#endif

/*
启动后会自动开始dma，
规则通道（ADC_RegularChannelConfig）
按照规定的顺序获取adc，放在指定的顺序数组里
注入通道（ADC_InjectedChannelConfig）
由特定事件触发（GPI、TIME）高响应速度，有专门的寄存器存放adc数据

*/
void ADC1_DMA_Config (void *DMA_Buffer,int BufferSize)
{
#if Exist_ADC
    DMA_InitTypeDef dma_init_struct;
    NVIC_InitTypeDef NVIC_InitStructure;
	memset(DMA_Buffer,0,BufferSize);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1);
    dma_init_struct.DMA_BufferSize  = BufferSize;
    dma_init_struct.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);          // 外设地址（ADC数据寄存器）
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
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
    // 配置DMA中断（传输完成）
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
#endif
}

int Base_ADC_Init(int Set)
{
#if Exist_ADC
    FunctionalState set = DISABLE;
    if (Set)
    {
        set = ENABLE;
    }
    ADC_InitTypeDef adc_base_struct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, set);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);				// 复用时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
//    crm_adc_clock_div_set(CRM_ADC_DIV_6);                         // ADC是时钟的6分频
    ADC_GPIO_Init(Set);                                             // GPIO

	Channel_NUMs = 1;
    #if (Exist_ADC & OPEN_0001)
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA0, Channel_NUMs++, ADC_Speed);
	ADC_RegularChannelConfig(ADC1, ADC_IO_PA1, Channel_NUMs++, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_0010)
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA2, Channel_NUMs++, ADC_Speed);
	ADC_RegularChannelConfig(ADC1, ADC_IO_PA3, Channel_NUMs++, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_0100)
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA4, Channel_NUMs++, ADC_Speed);
	ADC_RegularChannelConfig(ADC1, ADC_IO_PA5, Channel_NUMs++, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_1000)
    ADC_RegularChannelConfig(ADC1, ADC_IO_PA6, Channel_NUMs++, ADC_Speed);
	ADC_RegularChannelConfig(ADC1, ADC_IO_PA7, Channel_NUMs++, ADC_Speed);
    #endif
    #if (Exist_ADC & OPEN_10000)
    ADC_RegularChannelConfig(ADC1, ADC_IO_PB0, Channel_NUMs++, ADC_Speed);
    ADC_RegularChannelConfig(ADC1, ADC_IO_PB1, Channel_NUMs++, ADC_Speed);
    #endif
    #ifdef ADC_Temp
    ADC_RegularChannelConfig(ADC1, ADC_Temp, Channel_NUMs++, ADC_Speed);
    ADC_TempSensorVrefintCmd(set);                           // 开温传
    #endif
    Channel_NUMs = Channel_NUMs - 1;
    
    ADC1_DMA_Config ((u16 *)ADC1_DMA_list, Channel_NUMs);

    ADC_StructInit(&adc_base_struct);
    adc_base_struct.ADC_Mode = ADC_Mode_Independent;          	// 独立模式
    adc_base_struct.ADC_ScanConvMode = ENABLE;        			// 扫描模式
    adc_base_struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // 软件触发
    adc_base_struct.ADC_ContinuousConvMode = ENABLE;                   	// 连续转换（DMA需要）
    adc_base_struct.ADC_DataAlign = ADC_DataAlign_Right;
    adc_base_struct.ADC_NbrOfChannel  = Channel_NUMs;			//  Channel_NUM
    ADC_Init(ADC1, &adc_base_struct);
    
    ADC_DMACmd(ADC1, ENABLE);		// ADC  DMA
    ADC_Cmd(ADC1, ENABLE);			// 启动ADC
    
    // 校准ADC（推荐）
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
	
	if (Set && Channel_NUMs)
    {
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}
#endif
    return 0;
}

float ADC_Conversion_Vol_Fun(int num)
{
    float Temp = num;
#if Exist_ADC
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
void * data 以u16数组解析
*/
D_pFun ADC_Quick_Get_pFun = NULL;
void ADC_Quick_Get_Bind_Fun(D_pFun ADC_pFun)
{
	ADC_Quick_Get_pFun = ADC_pFun;
}

#if Exist_ADC
void ADC_FINISH_HANDLERIT()
{
//    int *temp_p = ADC1_DMA_list;
    if (DMA_GetITStatus(DMA1_IT_TC1))       // ADC数据全部完成
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        if (ADC_Quick_Get_pFun != NULL && adc_enable_flag > 0)
        {
            ADC_Quick_Get_pFun((u16 *)ADC1_DMA_list);
        }
    }
}
#endif
