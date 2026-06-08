#include "Base_CAN.h"

uint8_t CAN1_enable = 0;
int CAN1_Id = 1;
int CAN_rx_Id = 0;
int CAN_Filter[5] = {0};
int CAN_Range[2] = {0,0x0f};

D_pFun CAN_Receive_pFun = NULL;
int CAN_Receive_Bind_Fun(D_pFun Data_pFun)
{
    int retval = 0;
    CAN_Receive_pFun = Data_pFun;
    return retval;
}

#if Exist_CAN


#endif

/*
tsjw - CAN synchronisation jump width.
Bps = Fpclk1 / ((tbs1+tbs2+1)*brp)
brp - Specifies the length of a time quantum.

*/
int Base_CAN_Init(u8 tsjw, u8 mode, int Bps, int Id, int en)
{
    int retval = 0;
#if Exist_CAN
    int brp = 0;

    if (Bps > 0 && brp > 0)
    {
        brp = brp / (CAN_BS1_NUM + CAN_BS2_NUM + 3); // n/12
        brp = brp / Bps;
        brp &= 0xFF;
    }
    //
    if (en > 0)
    {
        CAN1_enable = 1;
        CAN1_Id = Id;

    #if CAN_MODE == OPEN_0000

    #else
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

        GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_8;
        GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitSturcture);

        GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitSturcture);
    #endif

// 
        uint32_t id = CAN_Range[1];
        id <<= 21;
        uint32_t mask = CAN_Range[1];
        mask = (0x7FF & ~mask) << 21;

        id = CAN_Filter[0];
        id <<= 21;
        mask = CAN_Filter[1];
        mask <<= 21;

    }
    else
    {
        CAN1_enable = 0;
    }
#endif
    return retval;
}

/*
    retval = 0,succ
*/
int Base_CAN_Filter (int *Filter,int len,int Range_a,int Range_b)
{
    int retval = -1;
    if(Filter == NULL || len >= sizeof(CAN_Filter)/sizeof(int))
    {
        // 
    }
    else
    {
        if(len > 0)
        {
            memcpy(CAN_Filter,Filter,len * sizeof(int));
            retval = 0;
        }
    }
    if(Range_a >= 0 && Range_b > Range_a)
    {
        CAN_Range[0] = Range_a;
        CAN_Range[1] = Range_b;
        retval = 0;
    }
    return retval;
}

int Base_CAN_Get_RXId (void)
{
    return CAN_rx_Id;
}

/*
    retval = 0,succ
*/
int Base_CAN_Send_Data(u8 *Data, u16 len)
{
    int retval = -1;
#if Exist_CAN
    if (CAN1_enable)
    {
        uint8_t *temp_buff = (uint8_t *)Data;
        int temp_num = 0, temp_run = len;
        while (temp_run > 0)
        {
            temp_num = temp_run;
            if (temp_num > 8)
            {
                temp_num = 8;
            }
//            retval = CAN1_Send_Msg(temp_buff, temp_num);
            temp_run -= temp_num;
        }
    }
#endif
    return retval;
}
