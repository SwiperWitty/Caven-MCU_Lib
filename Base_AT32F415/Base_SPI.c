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

#if Exist_SPI
extern void SYS_Base_Delay(int time, int speed);

/*
retval:1 succ
retval:0 fail
*/ 
static uint8_t SPI_Wait_TX_Flag(SPI_mType Channel)
{
    spi_type *spi_Temp = NULL;

    switch (Channel)
    {
    case 0: return 0;
    case 1: spi_Temp = SPI1; break;
    case 2: spi_Temp = SPI2; break;
    default: return 0;
    }

    uint32_t retry = 0;
    do
    {
        if (spi_i2s_flag_get(spi_Temp, SPI_I2S_TDBE_FLAG) == SET)   // 1为空
        {
            return 1;
        }
        SYS_Base_Delay(1, 1000);
    } while (++retry < SPI_WAIT_MAX);

    return 0;
}

static uint8_t SPI_Wait_RX_Flag(SPI_mType Channel)
{
    spi_type *spi_Temp = NULL;

    switch (Channel)
    {
    case 0: return 0;
    case 1: spi_Temp = SPI1; break;
    case 2: spi_Temp = SPI2; break;
    default: return 0;
    }

    uint32_t retry = 0;
    do
    {
        if (spi_i2s_flag_get(spi_Temp, SPI_I2S_RDBF_FLAG) == SET)   // 1为有数据了
        {
            return 1;
        }
        SYS_Base_Delay(1, 1000);
    } while (++retry < SPI_WAIT_MAX);

    return 0;
}

static uint8_t SPI_Wait_Busy_Flag(SPI_mType Channel)
{
    spi_type *spi_Temp = NULL;

    switch (Channel)
    {
    case 0: return 0;
    case 1: spi_Temp = SPI1; break;
    case 2: spi_Temp = SPI2; break;
    default: return 0;
    }

    uint32_t retry = 0;
    do
    {
        if (spi_i2s_flag_get(spi_Temp, SPI_I2S_BF_FLAG) != SET)   // 1忙碌
        {
            return 1;
        }
        SYS_Base_Delay(1, 1000);
    } while (++retry < SPI_WAIT_MAX);

    return 0;
}

static uint8_t SPI_Wait_DMA_Flag(uint32_t flag)
{
    uint32_t retry = 0;
    do
    {
        if (dma_flag_get(flag) != RESET)
        {
            dma_flag_clear(flag);
            return 1;
        }
        SYS_Base_Delay(1, 1000);
    } while (++retry < SPI_WAIT_MAX);
    return 0;
}
#endif

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
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
		
        gpio_init_struct.gpio_pins = SPI1_SCK | SPI1_MOSI;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = SPI_MODE_OUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI1, &gpio_init_struct);

        gpio_init_struct.gpio_pins = SPI1_MISO;
        gpio_init_struct.gpio_mode = SPI_MODE_IN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIO_SPI1, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
		gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI1, &gpio_init_struct);
    }
}

/*
    SPI初始化
*/
static void Base_SPI1_Init (uint8_t Width,int Set)
{
    confirm_state set = FALSE;
    spi_init_type spi_init_struct;
    spi_type *spi_Temp = NULL;
    spi_Temp = SPI1;
    if (Set)
        set = ENABLE;
    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, set);
	spi_default_para_init(&spi_init_struct);
    #if HOST_MODE
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    #else
    spi_init_struct.master_slave_mode = SPI_MODE_SLAVE;
    #endif
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    if(Width > 8)
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
    }
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    #if (SPI_MODE_CFG == SPI_MODE_CFG_0)
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
    #elif (SPI_MODE_CFG == SPI_MODE_CFG_3)
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    #endif

    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.mclk_freq_division = SPI_SPEED;                   //
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, set);
	
    #endif
    spi_enable(spi_Temp, set);
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
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
		
        gpio_init_struct.gpio_pins = SPI2_SCK | SPI2_MOSI;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = SPI_MODE_OUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI2, &gpio_init_struct);

        gpio_init_struct.gpio_pins = SPI2_MISO;
        gpio_init_struct.gpio_mode = SPI_MODE_IN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIO_SPI2, &gpio_init_struct);
    }
    else
    {
        gpio_init_struct.gpio_pins = SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
		gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI2, &gpio_init_struct);
    }
}

/*
    SPI初始化
*/
static void Base_SPI2_Init (uint8_t Width,int Set)
{
    confirm_state set = FALSE;
    spi_init_type spi_init_struct;
    spi_type *spi_Temp = NULL;
    spi_Temp = SPI2;
    if (Set)
        set = TRUE;
    crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, set);
	spi_default_para_init(&spi_init_struct);
    #if HOST_MODE
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    #else
    spi_init_struct.master_slave_mode = SPI_MODE_SLAVE;
    #endif
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    if(Width > 8)
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
    }
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    #if (SPI_MODE_CFG == SPI_MODE_CFG_0)
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
    #elif (SPI_MODE_CFG == SPI_MODE_CFG_3)
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    #endif

    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.mclk_freq_division = SPI_SPEED;                   //
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, set);
	
    #endif
    spi_enable(spi_Temp, set);
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
    #endif
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        if(SPI2_Init == 0) return;
        cs_fun = SPI2_GPIOCtl.SPI_CS_Ctl;
        cs_state = SPI2_GPIOCtl.SPI_CS_State;
    #endif
        break;
    default:
        return;
    }

    if ((cs_fun == NULL) || (cs_state == NULL))
    {
        return;
    }
    #if SPI_SOFTWARE == 0
    SPI_Wait_Busy_Flag(Channel);
    #endif
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
    spi_type *spi_Temp = NULL;
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
    uint8_t tx_flag = 0;
    tx_flag = SPI_Wait_TX_Flag(Channel);
    if(tx_flag)
    {
        spi_i2s_data_transmit(spi_Temp, Data);
        SPI_Wait_Busy_Flag(Channel);
    }
    // 短用这个，长用dma
    #endif
    (void)SPI_Width;
#endif
}

void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data,int Length)
{
#if Exist_SPI
    dma_init_type DMA_InitStructure = {0};
    dma_channel_type *Temp_DMA_Channel = NULL;
    spi_type *spi_Temp = NULL;
    uint32_t DMAy_FLAG = 0;
    u8 dma_flag = 0;
    uint8_t *p_DMA_BUFF = NULL;
	dma_default_para_init(&DMA_InitStructure);
    static u8 dma_send_First = 0;
	if (Data == NULL || (Length <= 0)) {
        return;
    }
    switch (Channel)
    {
    case 1:
    #if (Exist_SPI & OPEN_0010) && SPI_DMA
        if(SPI1_Init == 0) return;
        p_DMA_BUFF = DMA_SPI1_Buff;
        spi_Temp = SPI1;
        DMAy_FLAG = DMA1_FDT3_FLAG;
        Temp_DMA_Channel = DMA1_CHANNEL3;
    #endif 
        break;
    case 2:
    #if (Exist_SPI & OPEN_0100) && SPI_DMA
        if(SPI2_Init == 0) return;
        p_DMA_BUFF = DMA_SPI2_Buff;
        spi_Temp = SPI2;
        DMAy_FLAG = DMA1_FDT5_FLAG;
        Temp_DMA_Channel = DMA1_CHANNEL5;
    #endif 
        break;
    default:
        break;
    }
    if(p_DMA_BUFF == NULL)
    {
        uint16_t temp_data;
        for(int i = 0; i < Length; i++)
        {
            temp_data = *((uint8_t *)Data + i);
            Base_SPI_Send_Data(Channel,temp_data);
        }
        return;
    }
    // 开始DMA
    if (p_DMA_BUFF == NULL || spi_Temp == NULL) {
        return;
    }

    if ((dma_send_First & (1U << Channel)) == 0)
    {
        dma_send_First |= (1U << Channel);
		dma_flag_clear(DMAy_FLAG);
		dma_reset(Temp_DMA_Channel);
        DMA_InitStructure.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;       //
		DMA_InitStructure.memory_base_addr = (uint32_t)p_DMA_BUFF;        //
		DMA_InitStructure.memory_inc_enable = TRUE;
		DMA_InitStructure.peripheral_base_addr = (uint32_t)&(spi_Temp->dt);   //
		DMA_InitStructure.peripheral_inc_enable = FALSE;                      /*外设地址自增：关闭（一直是 SPI 数据寄存器，不变）*/
		DMA_InitStructure.priority = DMA_PRIORITY_MEDIUM;
		DMA_InitStructure.loop_mode_enable = FALSE;                           // 自动循环关
		dma_init(Temp_DMA_Channel, &DMA_InitStructure);
        dma_flag = 1;
    }
    else
    {
        dma_flag = SPI_Wait_DMA_Flag(DMAy_FLAG);   /* Wait DMA1 Transfer Complete */
    }
    if(dma_flag == 1)
    {
        if(Length > SPI_DMA_SIZE)
        {
            Length = SPI_DMA_SIZE;
        }
		dma_channel_enable(Temp_DMA_Channel, FALSE);
		spi_i2s_dma_transmitter_enable(spi_Temp, FALSE);
        memcpy(p_DMA_BUFF,Data,Length);                     //
        dma_data_number_set(Temp_DMA_Channel, Length);
        spi_i2s_dma_transmitter_enable(spi_Temp, TRUE);
        dma_channel_enable(Temp_DMA_Channel, TRUE);
    }

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
    spi_type *spi_Temp = NULL;
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
    u8 flag = 0;
    if (spi_Temp == NULL)
    {
        return;
    }

    /*
     * 清掉前面发送命令/地址时产生的无效接收数据。
     */
    SPI_Wait_RX_Flag(Channel);
    temp_val = spi_i2s_data_receive(spi_Temp);

    for (int i = 0; i < Length; i++)
    {
        flag = SPI_Wait_TX_Flag(Channel);
        if(flag == 1)
        {
            spi_i2s_data_transmit(spi_Temp, Data);
            SPI_Wait_RX_Flag(Channel);
            temp_val = spi_i2s_data_receive(spi_Temp);
            Receive[i] = temp_val & 0xff;
        }
        else
        {
            return;
        }
    }
    if(flag)
    {
        SPI_Wait_Busy_Flag(Channel);
    }
    #endif
    (void)temp_val;
    (void)spi_Temp;
    (void)temp_gpioctl_pfun;
#endif
}
