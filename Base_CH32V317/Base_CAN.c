#include "Base_CAN.h"

uint8_t CAN1_enable = 0;
int CAN1_Id = 1;
int CAN_rx_Id = 0;
int CAN_Filter[5] = {0};
int CAN_Range[2] = {0,0x0f};

iD_pFun CAN_Receive_pFun = NULL;
int CAN_Receive_Bind_Fun(iD_pFun Data_pFun)
{
    int retval = 0;
    CAN_Receive_pFun = Data_pFun;
    return retval;
}

#if Exist_CAN

int CAN2FilterStartBank = CANSOFTFILTER_MAX_GROUP_NUM;

/*
This is the structure of the software filtering table. It can be configured through the CAN_SoftFilterInit function,
or you can directly set the configuration values. The configured values can be modified directly during runtime.
However, when using the interrupt mode for reception, you need to be aware that if the modification is interrupted,
it may affect the filtering results during this period.
*/
struct CANFilterStruct_t
{
    union
    {
        union
        {
            struct
            {
                uint32_t : 1;
                uint32_t RTR : 1;
                uint32_t IDE : 1;
                uint32_t ExID : 29;
            } Access_Ex;
            struct
            {
                uint32_t : 1;
                uint32_t RTR : 1;
                uint32_t IDE : 1;
                uint32_t : 18;
                uint32_t StID : 11;
            } Access_St;
        };
        union
        {
            struct
            {
                uint16_t FR_16_L;
                uint16_t FR_16_H;
            };
            uint32_t FR_32;
        };
    } FR[2];
    union
    {
        struct
        {
            uint16_t en : 1;
            uint16_t mode : 4;
            uint16_t scale : 3;
        };
        uint16_t ctrl_byte;
    };
} CANFilterStruct[CANSOFTFILTER_MAX_GROUP_NUM];

/*********************************************************************
 * @fn      CAN_SoftSlaveStartBank
 *
 * @brief   This function applies only to CH32 Connectivity line devices.
 *
 * @param   CAN_BankNumber - Select the start slave bank filter from 1...size of CANFilterStruct
 *
 * @return  none
 */
void CAN_SoftSlaveStartBank(uint8_t CAN_BankNumber)
{
    CAN2FilterStartBank = CAN_BankNumber;
}

/*********************************************************************
 * @fn      CAN_SoftFilterInit
 *
 * @brief   Initializes the CAN peripheral according to the specified
 *        parameters in the CAN_FilterInitStruct.
 *
 * @param   CAN_FilterInitStruct - pointer to a CAN_FilterInitTypeDef
 *        structure that contains the configuration information.
 *
 * @return  none
 */
void CAN_SoftFilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct)
{
    if (CAN_FilterInitStruct->CAN_FilterNumber > CANSOFTFILTER_MAX_GROUP_NUM)
    {
        return;
    }
    if (CAN_FilterInitStruct->CAN_FilterActivation)
    {
        CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 1;
    }
    else
    {
        CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 0;
    }
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_H = CAN_FilterInitStruct->CAN_FilterIdHigh;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_L = CAN_FilterInitStruct->CAN_FilterIdLow;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_H = CAN_FilterInitStruct->CAN_FilterMaskIdHigh;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_L = CAN_FilterInitStruct->CAN_FilterMaskIdLow;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].mode = CAN_FilterInitStruct->CAN_FilterMode;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].scale = CAN_FilterInitStruct->CAN_FilterScale;
}

/*********************************************************************
 * @fn      CAN_ReceiveViaSoftFilter
 *
 * @brief   Receives a message via soft filter.
 *
 * @param   CANx - where x can be 1 to select the CAN peripheral.
 *          FIFONumber - Receive FIFO number.
 *            CAN_FIFO0.
 *          RxMessage -  pointer to a structure receive message which contains
 *        CAN Id, CAN DLC, CAN datas and FMI number.
 *
 * @return  none
 */
void CAN_ReceiveViaSoftFilter(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage)
{
    int group, start_bank, end_bank;
    if (CANx == CAN1)
    {
        start_bank = 0;
        end_bank = CAN2FilterStartBank;
    }
    else
    {
        start_bank = CAN2FilterStartBank;
        end_bank = CANSOFTFILTER_MAX_GROUP_NUM;
    }

    for (group = start_bank; group < end_bank; group++)
    {
        if (CANFilterStruct[group].en)
        {
            uint32_t temp = CANx->sFIFOMailBox[0].RXMIR & (~0x1);
            switch ((uint8_t)CANFilterStruct[group].ctrl_byte & ~0x1)
            {

            case CANSOFTFILER_PREDEF_CTRLBYTE_ID32:
                if ((CANFilterStruct[group].FR[0].FR_32 != temp) && (CANFilterStruct[group].FR[1].FR_32 != temp))
                {
                    continue;
                }
                else
                {
                    CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                    return;
                }
                break;

            case CANSOFTFILER_PREDEF_CTRLBYTE_MASK32:
                if ((CANFilterStruct[group].FR[0].FR_32 & CANFilterStruct[group].FR[1].FR_32) ^ (temp & CANFilterStruct[group].FR[1].FR_32))
                {
                    continue;
                }
                else
                {
                    CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                    return;
                }
                break;

            default:
                return;
                break;
            }
        }
    }
    CAN_FIFORelease(CANx, CAN_FIFO0);
}

/*********************************************************************
 * @fn      CAN_Send_Msg
 *
 * @brief   CAN Transmit function.
 *
 * @param   msg - Transmit data buffer.
 *          len - Data length.
 *
 * @return  0 - Send successful.
 *          1 - Send failed.
 */
u8 CAN1_Send_Msg(u8 *msg, u8 len)
{
    u8 mbox;
    int i = 0;

    CanTxMsg CanTxStructure;

    CanTxStructure.StdId = CAN1_Id;
    CanTxStructure.IDE = CAN_Id_Standard;
    CanTxStructure.RTR = CAN_RTR_Data;
    CanTxStructure.DLC = len;

    for (i = 0; i < len; i++)
    {
        CanTxStructure.Data[i] = msg[i];
    }

    mbox = CAN_Transmit(CAN1, &CanTxStructure);

    i = 0;

    while ((CAN_TransmitStatus(CAN1, mbox) != CAN_TxStatus_Ok) && (i <= CAN_SEND_OVERTIME))
    {
        i++;
    }

    if (i >= CAN_SEND_OVERTIME)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

u8 CAN1_Receive_Msg(u8 *buf)
{
    u8 i,j = 0;

    CanRxMsg CanRxStructure;

    if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
    {
        return 0;
    }
#if USE_SOFT_FILTER
    CAN_ReceiveViaSoftFilter(CAN1, CAN_FIFO0, &CanRxStructure);
#else
    CAN_Receive(CAN1, CAN_FIFO0, &CanRxStructure);
#endif

    buf[j++] = (CanRxStructure.StdId >> (8 * 3)) & 0xff;
    buf[j++] = (CanRxStructure.StdId >> (8 * 2)) & 0xff;
    buf[j++] = (CanRxStructure.StdId >> (8 * 1)) & 0xff;
    buf[j++] = (CanRxStructure.StdId >> (8 * 0)) & 0xff;
    for (i = 0; i < 8; i++)
    {
        buf[j++] = CanRxStructure.Data[i];
    }

    return CanRxStructure.DLC;
}

void USB_LP_CAN1_RX0_IRQHandler()
{
    int i = 0;
    uint8_t px, pbuf[16];
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0))
    {
        px = CAN1_Receive_Msg(pbuf);
        if (CAN_Receive_pFun != NULL && CAN1_enable > 0)
        {
            px += 4;
            CAN_rx_Id = pbuf[i++];
            CAN_rx_Id <<= 8;
            CAN_rx_Id |= pbuf[i++];
            CAN_rx_Id <<= 8;
            CAN_rx_Id |= pbuf[i++];
            CAN_rx_Id <<= 8;
            CAN_rx_Id |= pbuf[i++];

            for (i = 4; i < px; i++)
            {
                CAN_Receive_pFun(&pbuf[i]);
            }
        }
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}
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
    RCC_ClocksTypeDef RCC_ClocksStatus;
    GPIO_InitTypeDef GPIO_InitSturcture = {0};
    CAN_InitTypeDef CAN_InitSturcture = {0};
    CAN_FilterInitTypeDef CAN_FilterInitSturcture = {0};
    //
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    brp = RCC_ClocksStatus.PCLK1_Frequency;
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
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    #if CAN_MODE == OPEN_0000
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitSturcture);

        GPIO_InitSturcture.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitSturcture);
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
        CAN_InitSturcture.CAN_TTCM = DISABLE;
        CAN_InitSturcture.CAN_ABOM = ENABLE;        // 自动can重置
        CAN_InitSturcture.CAN_AWUM = DISABLE;
        CAN_InitSturcture.CAN_NART = DISABLE;       // 自动重传
        CAN_InitSturcture.CAN_RFLM = DISABLE;
        CAN_InitSturcture.CAN_TXFP = DISABLE;
        CAN_InitSturcture.CAN_Mode = mode;
        CAN_InitSturcture.CAN_SJW = tsjw;
        CAN_InitSturcture.CAN_BS1 = CAN_BS1_NUM;
        CAN_InitSturcture.CAN_BS2 = CAN_BS2_NUM;
        CAN_InitSturcture.CAN_Prescaler = brp;
        CAN_Init(CAN1, &CAN_InitSturcture);
// 
        uint32_t id = CAN_Range[1];
        id <<= 21;
        uint32_t mask = CAN_Range[1];
        mask = (0x7FF & ~mask) << 21;
        CAN_FilterInitSturcture.CAN_FilterNumber = 1;
        CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
        CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
        CAN_FilterInitSturcture.CAN_FilterIdHigh = (id >> 16) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterIdLow = (id >> 0) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = (mask >> 16) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterMaskIdLow = (mask >> 0) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;
#if USE_SOFT_FILTER
        CAN_SoftFilterInit(&CAN_FilterInitSturcture);
#else
        CAN_FilterInit(&CAN_FilterInitSturcture);
#endif
        id = CAN_Filter[0];
        id <<= 21;
        mask = CAN_Filter[1];
        mask <<= 21;
        CAN_FilterInitSturcture.CAN_FilterNumber = 2;
        CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdList;
        CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
        CAN_FilterInitSturcture.CAN_FilterIdHigh = (id >> 16) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterIdLow = (id >> 0) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = (mask >> 16) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterMaskIdLow = (mask >> 0) & 0xffff;
        CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
        CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;
#if USE_SOFT_FILTER
        CAN_SoftFilterInit(&CAN_FilterInitSturcture);
#else
        CAN_FilterInit(&CAN_FilterInitSturcture);
#endif
        CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
        NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    }
    else
    {
        CAN1_enable = 0;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, DISABLE);
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
            retval = CAN1_Send_Msg(temp_buff, temp_num);
            temp_run -= temp_num;
        }
    }
#endif
    return retval;
}
