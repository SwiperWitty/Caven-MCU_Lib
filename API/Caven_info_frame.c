#include "Caven_info_frame.h"

#include "Encrypt_crc.h"

/*
Caven_info_Make_packet_Fun
** make packet
** 将[data]数据转化成[packet]
传参
** standard ：标准要求(不需要指针索引)
** target   : 目标包(指针，这个包内的指针必须要有索引)
** data     ：数据来源（填入）
return   : retval
** 1.(-0x80 < retval < 0) 协议消息解析错误
** 2.retval = -0x80 包里存在协议消息没处理
** 3.retval = -0x8F 目标包的指针没有索引
** 4.retval & 0x50 >= 0 获取到协议消息,可以开始解析，同时(Result & 0x50 > 1)
** 5.retval = 其他   获取中(包含没开始retval = 0)
** 示例[FA 55 01 00 01 00 00 00 04 12 34 56 78 00 A1 CE]
*/
int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, uint8_t data)
{
    int retval = 0;
    int temp = 0;

    Caven_info_packet_Type temp_packet = *target;
    uint8_t *tepm_pData = temp_packet.p_AllData;

    if (temp_packet.Result & 0x50) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL || tepm_pData == NULL)
    {
        return (-0x8F);
    }
    if (temp_packet.Run_status > 0 && temp_packet.Run_status < 9)   // 跳过头帧和crc校验帧
    {
        temp_packet.end_crc = CRC16_XMODEM_Table_Byte(data, temp_packet.end_crc);
    }
    switch (temp_packet.Run_status)
    {
    case 0: /* Head */
        temp_packet.Head = (temp_packet.Head << 8) + data;
        if (temp_packet.Head == standard.Head)
        {
            temp_packet.Get_num = 0;
            temp_packet.dSize = 0;
            temp_packet.get_crc = 0;
            temp_packet.end_crc = 0;
            temp_packet.p_Data = NULL;
            tepm_pData[temp_packet.Get_num++] = (temp_packet.Head >> 8) & 0x00ff;
            tepm_pData[temp_packet.Get_num++] = (temp_packet.Head) & 0x00ff;
            temp_packet.Run_status++;
        }
        break;
    case 1: /* agreement Versions */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Versions = data;
        if (temp_packet.Versions <= standard.Versions)
        {
            temp_packet.Run_status++;
        }
        else
        {
            temp_packet.Run_status = -temp_packet.Run_status;
        }
        break;
    case 2: /* Type */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Type = data;
        if (temp_packet.Type == standard.Type || temp_packet.Type == 0)
        {
            temp_packet.Run_status++;
        }
        else
        {
            temp_packet.Run_status = -temp_packet.Run_status;
        }
        break;
    case 3: /* Addr */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Addr = data;
        if (temp_packet.Addr == standard.Addr || temp_packet.Addr == 0 || temp_packet.Addr == 0xff)
        {
            temp_packet.Run_status++;
        }
        else
        {
            temp_packet.Run_status = -temp_packet.Run_status;
        }
        break;
    case 4: /* Cmd */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Cmd = data;
        temp_packet.Run_status++;

        break;
    case 5: /* Cmd_sub */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Cmd_sub = data;
        temp_packet.Run_status++;

        break;
    case 6: /* Size */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.dSize = (temp_packet.dSize << 8) + data;
        temp = 2 + 5 + 2;
        if (temp_packet.Get_num >= temp)
        {
            if (temp_packet.dSize > standard.dSize)
            {
                temp_packet.Run_status = -temp_packet.Run_status;
            }
            else if (temp_packet.dSize == 0)
            {
                temp_packet.Run_status += 2; /* 0个 p_Data ，直接去 crc */
            }
            else
            {
                temp_packet.Run_status++;
            }
        }
        break;
    case 7: /* p_Data */
        tepm_pData[temp_packet.Get_num++] = data;
        temp = 2 + 5 + 2 + temp_packet.dSize;
        if (temp_packet.Get_num >= temp)
        {
            temp_packet.Run_status++;
        }
        break;
    case 8: /* Result */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.Result = data;
        temp_packet.Result &= 0x0F;     // 高4位置0
        temp_packet.Run_status++;
        break;
    case 9: /* crc */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.get_crc = (temp_packet.get_crc << 8) + data;
        temp = 2 + 5 + 2 + temp_packet.dSize + 3;
        if (temp_packet.Get_num >= temp)
        {
            if (temp_packet.end_crc == temp_packet.get_crc)
            {
                temp_packet.Result |= 0x50; // crc successful
                temp_packet.Run_status = 0xff;
            }
            else
            {
                temp_packet.Run_status = -temp_packet.Run_status;
            }
        }
        break;
    default:
        break;
    }
    /*  结果    */
    if (temp_packet.Run_status < 0) // error
    {
        retval = temp_packet.Run_status;
        Caven_info_packet_fast_clean_Fun(target);
        // printf("error code [%d] \n",retval);
    }
    else if (temp_packet.Run_status == 0xff) // Successful
    {
        temp_packet.p_Data = tepm_pData + 2 + 5 + 2;
        *target = temp_packet;
        retval = 0xFF;
    }
    else // doing
    {
        *target = temp_packet;
        retval = temp_packet.Run_status;
    }
    return retval;
}

/*
Caven_info_Split_packet_Fun
** split packet
** 将[packet]数据转化成[data]
传参
** source ：数据来源包(指针必须要有索引,如果里面的dSize == 0,p_Data可以为NULL)
** data     ：数据目标
return   : retval
** retval = 返回数据目标split出的长度
*/
int Caven_info_Split_packet_Fun(Caven_info_packet_Type const source, unsigned char *data)
{
    int retval;
    int temp;
    int getnum;
    unsigned char *array;

    if (data == NULL || ((source.p_Data == NULL) && (source.dSize != 0)))
    {
        retval = (-1);
    }
    else
    {
        array = data;
        getnum = 0;
        array[getnum++] = (source.Head >> 8) & 0xff;
        array[getnum++] = source.Head & 0xff;

        array[getnum++] = source.Versions;
        array[getnum++] = source.Type;
        array[getnum++] = source.Addr;
        array[getnum++] = source.Cmd;
        array[getnum++] = source.Cmd_sub;

        array[getnum++] = (source.dSize >> 8) & 0xff;
        array[getnum++] = source.dSize & 0xff;

        if (source.dSize > 0)
        {
            memcpy(&array[getnum], source.p_Data, source.dSize);
            getnum += source.dSize;
        }
        array[getnum++] = (source.Result & 0x0F);
        
        temp = getnum - 2;
        temp = Encrypt_XMODEM_CRC16_Fun(&array[sizeof(source.Head)], temp);

        array[getnum++] = (temp >> 8) & 0xff;
        array[getnum++] = temp & 0xff;

        retval = getnum;
    }
    return retval;
}

/*
 * retval = (-1):队列满载，拒绝载入
 * 这个函数需要快速响应
 */
int Caven_Circular_queue_input (Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num)
{
    int retval = 0;
    if (data == NULL || Buff_data == NULL || Buff_Num <= 0)
    {
        retval = -2;
        return retval;
    }

    for (int i = 0;i < Buff_Num;i++)
    {
        if (Buff_data[i].Result & 0x50)
        {
            retval = (-1);
        }
        else
        {
            Caven_packet_data_copy_Fun(&Buff_data[i],data); // 载入数据到队列
//            Caven_info_packet_clean_Fun(data);
            retval = i;
            break;
        }
    }
    return retval;
}

/*
 * retval = (-1):没有要处理的数据
 * retval = other:有
 *
 */
int Caven_Circular_queue_output (Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num)
{
    int retval = 0;
    Caven_info_packet_Type temp_packet;
    if (data == NULL || Buff_data == NULL || Buff_Num <= 0)
    {
        retval = -2;
        return retval;
    }
    
    for (int i = 0;i < Buff_Num;i++)
    {
        if (Buff_data[i].Result & 0x50)
        {
            Caven_packet_data_copy_Fun(data,&Buff_data[i]);    // 从队列提取数据
            Caven_info_packet_fast_clean_Fun(&Buff_data[i]);
            retval = i;

            break;
        }
        else
        {
            retval = (-1);
        }
    }
    return retval;
}

/*
Caven_info_packet_index_Fun
** index 索引
** 将数据[data]绑定到[packet]的指针变量
传参
** target ：数据源包(这个包内有指针变量)
** data     ：要绑定的数据目标
return   : retval
** retval < 0 索引错误
** retval = 0 索引成功
*/
int Caven_info_packet_index_Fun(Caven_info_packet_Type *target, uint8_t *data)
{
    int retval = 0;
    if (target != NULL)
    {
        target->p_AllData = data;
    }
    else
    {
        retval = -1;
    }
    return retval;
}

/*
 * Caven_packet_data_copy_Fun
 * 把[target]内容给[source]
*/
int Caven_packet_data_copy_Fun(Caven_info_packet_Type *source,Caven_info_packet_Type *target)
{
    int retval = 0;
    Caven_info_packet_Type temp_packet;

    if ((source != NULL) && (target != NULL))
    {
        if ((source->p_AllData != NULL) && (target->p_AllData != NULL))
        {
            temp_packet = *target;      // 先拿框架
            temp_packet.p_AllData = source->p_AllData;      // 还原指针
            memcpy(temp_packet.p_AllData,target->p_AllData,target->Get_num);
            if (temp_packet.dSize > 0)
            {
                temp_packet.p_Data = temp_packet.p_AllData + 2 + 5 + 2;
            }
            else
            {
                temp_packet.p_Data = NULL;
            }
            *source = temp_packet;
        }
    }
    return retval;
}

/*
Caven_info_packet_clean_Fun
** clean function
** 将数据[target]清除，但不解除指针绑定的数据
传参
** target ：数据源包(这个包内有指针变量)
return   : retval
** retval = 0 索引成功
*/
int Caven_info_packet_clean_Fun(Caven_info_packet_Type *target)
{
    int retval = 0;
    unsigned char *p_data;
    p_data = target->p_AllData;
#ifdef BUFF_MAX
    if (p_data != NULL && (target->Get_num > 0 && target->Get_num < BUFF_MAX))
    {
        memset(p_data, 0, target->Get_num);    // 清除指针内容,内容的长度依据是[Get_num]
    }
#endif
    Caven_info_packet_fast_clean_Fun(target);
    return retval;
}

/*
 * fast 主要给中断，这样就不会循环套娃
 */
int Caven_info_packet_fast_clean_Fun(Caven_info_packet_Type *target)
{
    int retval = 0;
    target->Result = 0;
    target->Run_status = 0;
    target->Get_num = 0;

    target->Comm_way = 0;
    return retval;
}
