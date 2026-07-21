#include "Base_SPI.h"
#include "Base_GPIO.h"

char SPI1_Width = 0;
char SPI2_Width = 0;
char SPI1_Init = 0;
char SPI2_Init = 0;
#define SPI_CS_MAX_NUM  5

typedef struct
{
    D_pFun SPI_MOSI_Ctl;
    D_pFun SPI_MISO_Gain;
    D_pFun SPI_SCLK_Ctl;
    D_pFun SPI_CS_Ctl[SPI_CS_MAX_NUM];
    char SPI_CS_State[SPI_CS_MAX_NUM];
} SPI_GPIOCtl_Fun_Type;

#if SPI_DMA
    #if Exist_SPI & OPEN_0010
    uint8_t DMA_SPI1_Buff[SPI_DMA_SIZE];
    #endif
    #if Exist_SPI & OPEN_0100
    uint8_t DMA_SPI2_Buff[SPI_DMA_SIZE];
    #endif
#endif

void SPI_Delay (int time)
{
    int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 12;            // SET
        do{
            NOP();
        }while((temp--) > 0);
    }
}

#if Exist_SPI & OPEN_0010
SPI_GPIOCtl_Fun_Type SPI1_GPIOCtl;
static void SPI1_MOSI_Ctl(void *data)
{
    u8 temp_data;
    if (data != NULL)
    {
        temp_data = *(u8 *)data;
        if (temp_data != 0)
        {
            SPI1_MOSI_H();
        }
        else
        {
            SPI1_MOSI_L();
        }
    }
}

static void SPI1_SCLK_Ctl(void *data)
{
    u8 temp_data;
    if (data != NULL)
    {
        temp_data = *(u8 *)data;
        if (temp_data != 0)
        {
            SPI1_SCK_H();
        }
        else
        {
            SPI1_SCK_L();
        }
    }
}

static void SPI1_MISO_Gain(void *data)
{
    if (data != NULL)
    {
        u8 temp_data;
        temp_data = SPI1_MISO_R() ? 1 : 0;
        *(u8 *)data = temp_data;
    }
}

static void SPI1_GPIO_Init(int Set)
{
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

        gpio_init_struct.GPIO_Pin = SPI1_SCK | SPI1_MOSI;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI1_MISO;
        gpio_init_struct.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.GPIO_Pin = SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);
    }
}

/*
    SPI初始化
*/
static void Base_SPI1_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    SPI_TypeDef *spi_Temp = NULL;
    spi_Temp = SPI1;
    if (Set)
        set = ENABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, set);

    #if HOST_MODE
    spi_init_struct.SPI_Mode = SPI_Mode_Master;
    #else
    spi_init_struct.SPI_Mode = SPI_Mode_Slave;
    #endif
    if(Width > 8)
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_8b;
    }
    spi_init_struct.SPI_FirstBit = SPI_FirstBit_MSB;
    #if (SPI_MODE_CFG == SPI_MODE_CFG_0)
    spi_init_struct.SPI_CPOL = SPI_CPOL_Low;
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;
    #elif (SPI_MODE_CFG == SPI_MODE_CFG_3)
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;
    #endif
    spi_init_struct.SPI_CRCPolynomial = 7;              // 
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                   //
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    SPI_Init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    #endif
    SPI_Cmd(spi_Temp, set);
}

#endif

#if Exist_SPI & OPEN_0100
SPI_GPIOCtl_Fun_Type SPI2_GPIOCtl;
static void SPI2_MOSI_Ctl(void *data)
{
    u8 temp_data;
    if (data != NULL)
    {
        temp_data = *(u8 *)data;
        if (temp_data != 0)
        {
            SPI2_MOSI_H();
        }
        else
        {
            SPI2_MOSI_L();
        }
    }
}

static void SPI2_SCLK_Ctl(void *data)
{
    u8 temp_data;
    if (data != NULL)
    {
        temp_data = *(u8 *)data;
        if (temp_data != 0)
        {
            SPI2_SCK_H();
        }
        else
        {
            SPI2_SCK_L();
        }
    }
}

static void SPI2_MISO_Gain(void *data)
{
    if (data != NULL)
    {
        u8 temp_data;
        temp_data = SPI2_MISO_R() ? 1 : 0;
        *(u8 *)data = temp_data;
    }
}

static void SPI2_GPIO_Init(int Set)
{
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

        gpio_init_struct.GPIO_Pin = SPI2_SCK | SPI2_MOSI;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI2_MISO;
        gpio_init_struct.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.GPIO_Pin = SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);
    }
}

/*
    SPI初始化
*/
static void Base_SPI2_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    SPI_TypeDef *spi_Temp = NULL;
    spi_Temp = SPI2;
    if (Set)
        set = ENABLE;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, set);

    #if HOST_MODE
    spi_init_struct.SPI_Mode = SPI_Mode_Master;
    #else
    spi_init_struct.SPI_Mode = SPI_Mode_Slave;
    #endif
    spi_init_struct.SPI_DataSize = SPI_DataSize_8b;
    if(Width > 8)
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_16b;
    }
    spi_init_struct.SPI_FirstBit = SPI_FirstBit_MSB;
    #if (SPI_MODE_CFG == SPI_MODE_CFG_0)
    spi_init_struct.SPI_CPOL = SPI_CPOL_Low;
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;
    #elif (SPI_MODE_CFG == SPI_MODE_CFG_3)
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;
    #endif
    spi_init_struct.SPI_CRCPolynomial = 7;              // 
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                   //
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    SPI_Init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    #endif
    SPI_Cmd(spi_Temp, set);
}
#endif

/*
 retval < 0;-1失败，-2申请cs失败
 retval = 0;初始化超过
 retval > 0;申请的cs Serial
*/
int Base_SPI_Init(SPI_mType Channel, uint8_t Width, int Set, D_pFun cs_pFun)
{
    int retval = -1;
    char *spi_pCS_State = NULL;
    D_pFun *spi_CS_pFun = NULL;
#if Exist_SPI
    if(Width == 8 || Width == 16)
    {}
    else
    {
        return retval;
    }
    switch (Channel)
    {
    case 1:
#if Exist_SPI & OPEN_0010
        SPI1_GPIO_Init(Set);
        Base_SPI1_Init(Width, Set);
        SPI1_Width = Width;
        SPI1_Init = Set;

        SPI1_GPIOCtl.SPI_MOSI_Ctl = SPI1_MOSI_Ctl;
        SPI1_GPIOCtl.SPI_MISO_Gain = SPI1_MISO_Gain;
        SPI1_GPIOCtl.SPI_SCLK_Ctl = SPI1_SCLK_Ctl;
        spi_pCS_State = SPI1_GPIOCtl.SPI_CS_State;
        spi_CS_pFun = SPI1_GPIOCtl.SPI_CS_Ctl;
        retval = 0;
#endif
        break;

    case 2:
#if Exist_SPI & OPEN_0100
        SPI2_GPIO_Init(Set);
        Base_SPI2_Init(Width, Set);
        SPI2_Width = Width;
        SPI2_Init = Set;

        SPI2_GPIOCtl.SPI_MOSI_Ctl = SPI2_MOSI_Ctl;
        SPI2_GPIOCtl.SPI_MISO_Gain = SPI2_MISO_Gain;
        SPI2_GPIOCtl.SPI_SCLK_Ctl = SPI2_SCLK_Ctl;
        spi_pCS_State = SPI2_GPIOCtl.SPI_CS_State;
        spi_CS_pFun = SPI2_GPIOCtl.SPI_CS_Ctl;
        retval = 0;
#endif
        break;

    default:
        break;
    }
    if ((retval != 0) || (spi_CS_pFun == NULL) || (spi_pCS_State == NULL))
    {
        return retval;
    }
    if (cs_pFun == NULL || Set == 0)
    {
        return retval;
    }
    retval = -2;
    for (int i = 0; i < SPI_CS_MAX_NUM; i++)
    {
        if (spi_CS_pFun[i] == NULL)
        {
            spi_CS_pFun[i] = cs_pFun;
            spi_pCS_State[i] = 0;
            cs_pFun(&spi_pCS_State[i]);

            retval = i + 1;
            break;
        }
    }
#endif
    (void)spi_CS_pFun;
    (void)spi_pCS_State;
    return retval;
}

/*
 * State = 1:
 *   重新选择 Serial 对应设备。
 *   会先关闭同通道所有 CS，再打开目标 CS。
 *   如果目标设备已被选中，也会产生一次 CS 复位脉冲。
 * State = 0:
 *   关闭 Serial 对应设备。
 * Serial = 0:关闭所有cs
 */
void Base_SPI_CS_Set(SPI_mType Channel, char Serial, char State)
{
#if Exist_SPI
    D_pFun *cs_fun = NULL;
    char *cs_state = NULL;
    SPI_TypeDef *spi_Temp = NULL;
    int index = Serial - 1;

    if (Serial > SPI_CS_MAX_NUM)
    {
        return;
    }

    switch (Channel)
    {
    case 1:
    #if Exist_SPI & OPEN_0010
        if(SPI1_Init == 0) return;
        cs_fun = SPI1_GPIOCtl.SPI_CS_Ctl;
        cs_state = SPI1_GPIOCtl.SPI_CS_State;
        spi_Temp = SPI1;
    #endif
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        if(SPI2_Init == 0) return;
        cs_fun = SPI2_GPIOCtl.SPI_CS_Ctl;
        cs_state = SPI2_GPIOCtl.SPI_CS_State;
        spi_Temp = SPI2;
    #endif
        break;
    default:
        return;
    }

    if ((cs_fun == NULL) || (cs_state == NULL) || spi_Temp == NULL)
    {
        return;
    }
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_BSY) == SET);
    if (State)
    {
        for (int i = 0; i < SPI_CS_MAX_NUM; i++)
        {
            cs_state[i] = 0;

            if (cs_fun[i] != NULL)
            {
                cs_fun[i](&cs_state[i]);
            }
        }
    }
    if(Serial == 0)
    {
        return;
    }
    cs_state[index] = State ? 1 : 0;
    if (cs_fun[index] != NULL)
    {
        cs_fun[index](&cs_state[index]);
    }
#endif
}

void Base_SPI_Send_Data(SPI_mType Channel,uint16_t Data)
{
#if Exist_SPI
    SPI_TypeDef *spi_Temp = NULL;
    SPI_GPIOCtl_Fun_Type *temp_gpioctl_pfun = NULL;
    u8 SPI_Width = 0;

    switch (Channel)
    {
    case 0:
    {
    }
        break;
    case 1:
    {
        #if Exist_SPI & OPEN_0010
        if(SPI1_Init == 0) return;
        temp_gpioctl_pfun = &SPI1_GPIOCtl;
        SPI_Width = SPI1_Width;
        spi_Temp = SPI1;
        #endif
    }
        break;
    case 2:
    {
        #if Exist_SPI & OPEN_0100
        if(SPI2_Init == 0) return;
        temp_gpioctl_pfun = &SPI2_GPIOCtl;
        SPI_Width = SPI2_Width;
        spi_Temp = SPI2;
        #endif
    }
        break;
    default:
        break;
    }
    if(spi_Temp == NULL || temp_gpioctl_pfun == NULL)
    {
        return;
    }
    #if SPI_SOFTWARE
    int temp_num = 0;
    u8 temp_data = 0,temp_val = 0;
    if ((temp_gpioctl_pfun->SPI_SCLK_Ctl == NULL) ||
        (temp_gpioctl_pfun->SPI_MOSI_Ctl == NULL))
    {
        return;
    }
    for (int i = 0; i < SPI_Width; i++)
    {
        temp_data = SPI_SCLK_IDLE;
        temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);  // sclk_IDLE
        #if SPI_MODE_CFG == SPI_MODE_CFG_3
        temp_data = !SPI_SCLK_IDLE;
        temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);
        #endif
        temp_num = (Data << i) & ((u16)0x01 << (SPI_Width - 1));
        if (temp_num)
        {
            temp_val = 1;
            temp_gpioctl_pfun->SPI_MOSI_Ctl(&temp_val);  // mosi_h
        } 
        else
        {
            temp_val = 0;
            temp_gpioctl_pfun->SPI_MOSI_Ctl(&temp_val);  // mosi_l
        }
        temp_data = !temp_data;
        temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);  // sclk_ation
    }
    temp_data = 1;
    temp_gpioctl_pfun->SPI_MOSI_Ctl(&temp_data);  // mosi_h
    temp_data = SPI_SCLK_IDLE;
    temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);  // sclk_IDLE
    #else

    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(spi_Temp, Data);
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_BSY) == SET);
    // 短用这个，长用dma
    #endif
    (void)SPI_Width;
#endif
}

void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data,int Length)
{
#if SPI_DMA
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_Channel_TypeDef *Temp_DMA_Channel = NULL;
    SPI_TypeDef *spi_Temp = NULL;
    uint32_t DMAy_FLAG;

    uint8_t *p_DMA_BUFF = NULL;
    static u8 dma_send_First = 0;
	
    switch (Channel)
    {
    case 1:
    #if Exist_SPI & OPEN_0010
        if(SPI1_Init == 0) return;
        p_DMA_BUFF = DMA_SPI1_Buff;
        spi_Temp = SPI1;
        DMAy_FLAG = DMA1_FLAG_TC3;
        Temp_DMA_Channel = DMA1_Channel3;
    #endif 
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        if(SPI2_Init == 0) return;
        p_DMA_BUFF = DMA_SPI2_Buff;
        spi_Temp = SPI2;
        DMAy_FLAG = DMA1_FLAG_TC5;
        Temp_DMA_Channel = DMA1_Channel5;
    #endif 
        break;
    default:
        spi_Temp = NULL;
        break;
    }
    // 开始DMA
    if (Data == NULL || p_DMA_BUFF == NULL || spi_Temp == NULL || (Length <= 0)) {
        return;
    }

    if ((dma_send_First & (1U << Channel)) == 0)
    {
        dma_send_First |= (1U << Channel);
        DMA_ClearFlag(DMAy_FLAG);

        DMA_DeInit(Temp_DMA_Channel);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&spi_Temp->DATAR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)p_DMA_BUFF;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = 0;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(Temp_DMA_Channel, &DMA_InitStructure);
    }
    else
    {
        while(DMA_GetFlagStatus(DMAy_FLAG) == RESET);   /* Wait DMA1 Transfer Complete */
        DMA_ClearFlag(DMAy_FLAG);
    }

    DMA_Cmd(Temp_DMA_Channel, DISABLE);
    SPI_I2S_DMACmd(spi_Temp, SPI_I2S_DMAReq_Tx, DISABLE);
    memcpy(p_DMA_BUFF,Data,Length);                     //
    DMA_SetCurrDataCounter(Temp_DMA_Channel,Length);
    
    SPI_I2S_DMACmd(spi_Temp,SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(Temp_DMA_Channel, ENABLE);
#endif 
}

void Base_SPI_DMA_Send_Buff(SPI_mType Channel,const void *Data,int Length)
{
#if SPI_DMA
    uint8_t *temp_buff = (uint8_t *)Data;
    int temp_num = 0,temp_run = Length;
    while(temp_run > 0)
    {
        temp_num = temp_run;
        if(temp_num > SPI_DMA_SIZE)
        {
            temp_num = SPI_DMA_SIZE;
        }
        Base_SPI_DMA_Send_Data(Channel,temp_buff,temp_num);
        temp_buff += temp_num;
        temp_run -= temp_num;
    }
#endif
}

void Base_SPI_ASK_Receive(SPI_mType Channel, uint16_t Data, uint8_t *Receive, int Length)
{
#if Exist_SPI
    SPI_TypeDef *spi_Temp = NULL;
    SPI_GPIOCtl_Fun_Type *temp_gpioctl_pfun = NULL;
    uint16_t temp_val = 0;

    switch (Channel)
    {
    case 1:
#if Exist_SPI & OPEN_0010
        if (SPI1_Init == 0) return;
        spi_Temp = SPI1;
        temp_gpioctl_pfun = &SPI1_GPIOCtl;
#endif
        break;

    case 2:
#if Exist_SPI & OPEN_0100
        if (SPI2_Init == 0) return;
        spi_Temp = SPI2;
        temp_gpioctl_pfun = &SPI2_GPIOCtl;
#endif
        break;

    default:
        return;
    }
    if ((Receive == NULL) || (Length <= 0))
    {
        return;
    }
    #if SPI_SOFTWARE
    uint8_t temp_data = 0,rx_data = 0;
    if ((temp_gpioctl_pfun == NULL) ||
        (temp_gpioctl_pfun->SPI_SCLK_Ctl == NULL) ||
        (temp_gpioctl_pfun->SPI_MOSI_Ctl == NULL) ||
        (temp_gpioctl_pfun->SPI_MISO_Gain == NULL))
    {
        return;
    }
    temp_data = 1;
    temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);   // back idle high
    for (int i = 0; i < Length; i++)
    {
        rx_data = 0;

        for (int bit = 0; bit < 8; bit++)
        {
            temp_data = 0;
            temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);

            temp_data = ((Data << bit) & 0x80) ? 1 : 0;
            temp_gpioctl_pfun->SPI_MOSI_Ctl(&temp_data);

            temp_data = 1;
            temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);

            temp_data = 0;
            temp_gpioctl_pfun->SPI_MISO_Gain(&temp_data);

            rx_data <<= 1;
            if (temp_data != 0)
            {
                rx_data |= 0x01;
            }
        }
        Receive[i] = rx_data;
    }
    temp_data = 1;
    temp_gpioctl_pfun->SPI_SCLK_Ctl(&temp_data);   // back idle high
    temp_data = 1;
    temp_gpioctl_pfun->SPI_MOSI_Ctl(&temp_data);

    #else
    if (spi_Temp == NULL)
    {
        return;
    }

    /*
     * 清掉前面发送命令/地址时产生的无效接收数据。
     */
    while (SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_RXNE) == SET)
    {
        temp_val = SPI_I2S_ReceiveData(spi_Temp);
    }

    for (int i = 0; i < Length; i++)
    {
        while (SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(spi_Temp, Data);

        while (SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_RXNE) == RESET);
        temp_val = SPI_I2S_ReceiveData(spi_Temp);

        Receive[i] = temp_val & 0xff;
    }

    while (SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_BSY) == SET);
    #endif
    (void)temp_val;
    (void)spi_Temp;
    (void)temp_gpioctl_pfun;
#endif
}
