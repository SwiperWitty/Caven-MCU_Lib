#include "Caven_info_frame.h"

#include "Check_CRC.h"

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
*/

static int Caven_info_packet_fast_clean_Fun(Caven_info_packet_Type *target);

int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, unsigned char data)
{
    int retval = 0;
    int temp = 0;
#ifdef BUFF_MAX
    unsigned char array[BUFF_MAX];
#else
    unsigned char array[300];
#endif

    Caven_info_packet_Type temp_packet = *target;
    unsigned char *tepm_pData = temp_packet.p_Data;

    if (temp_packet.Result & 0x50) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL || temp_packet.p_Data == NULL)
    {
        return (-0x8F);
    }

    switch (temp_packet.Run_status)
    {
    case 0: /* Head */
        temp_packet.Head = (temp_packet.Head << 8) + data;
        if (temp_packet.Head == standard.Head)
        {
            temp_packet.Get_num = 0;
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
                temp_packet.Run_status += 2; /* 0个 p_Data ，直接去 End_crc */
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
        temp_packet.Result &= 0x0F;     // 高位置0
        temp_packet.Run_status++;
        break;
    case 9: /* End_crc */
        tepm_pData[temp_packet.Get_num++] = data;
        temp_packet.End_crc = (temp_packet.End_crc << 8) + data;
        temp = 7 + 2 + temp_packet.dSize + 3;
        if (temp_packet.Get_num >= temp)
        {
            temp = temp_packet.Get_num - sizeof(temp_packet.Head) - sizeof(temp_packet.End_crc); /* 减尾 减头 */
            temp = ModBusCRC16((tepm_pData + sizeof(temp_packet.Head)), temp);
            if (temp_packet.End_crc == temp)
            {
                temp_packet.Result |= 0x50; // crc successful
                temp_packet.Run_status = 0xff;
            }
            else
            {
                //                 printf("crc is %04x \n",temp);
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
        //        printf("error %x \n",retval);
    }
    else if (temp_packet.Run_status == 0xff) // Successful
    {
#if 1
        // 切割数据
        temp = 7 + sizeof(temp_packet.Head); //
        if (temp_packet.dSize > 0)
        {
            memcpy(array, temp_packet.p_Data + temp, temp_packet.dSize);
            memcpy(temp_packet.p_Data, array, temp_packet.dSize);
        }
        else
        {
            memset(temp_packet.p_Data, 0, 10);
        }
#else
        // 原始数据
#endif
        *target = temp_packet;
        retval = 0xFF;
        //        printf("succ %x \n",retval);
    }
    else // doing
    {
        *target = temp_packet;
        target->Result &= 0x0F;
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
#ifdef BUFF_MAX
    unsigned char array[BUFF_MAX];
#else
    unsigned char array[300];
#endif

    if (data == NULL || ((source.p_Data == NULL) && (source.dSize != 0)))
    {
        retval = (-1);
    }
    else
    {
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
        temp = ModBusCRC16(&array[2], temp);
        //        source.End_crc = temp;
        array[getnum++] = (temp >> 8) & 0xff;
        array[getnum++] = temp & 0xff;

        memcpy(data, array, getnum);
        retval = getnum;
    }
    return retval;
}

/*
 * 这个函数需要快速响应
 */
int Caven_Circular_queue_input (Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num)
{
    int retval = 0;
    Caven_info_packet_Type temp_packet;
    for (int i = 0;i < Buff_Num;i++)
    {
        temp_packet = Buff_data[i];
        if (temp_packet.Result & 0x50)
        {
            retval = (-1);                                  // 队列满载，拒绝载入
        }
        else
        {
            Caven_packet_data_copy_Fun(&Buff_data[i],data); // 载入数据到队列
//            Caven_info_packet_clean_Fun(data);
            retval = i;
            break;
        }
    }
    Caven_info_packet_fast_clean_Fun(data);      // 无论是否能载入，都要清，否则影响下一帧接收
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
    for (int i = 0;i < Buff_Num;i++)
    {
        temp_packet = Buff_data[i];

        if (temp_packet.Result & 0x50)
        {
            Caven_packet_data_copy_Fun(data,&temp_packet);    // 从队列提取数据
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
int Caven_info_packet_index_Fun(Caven_info_packet_Type *target, unsigned char *data)
{
    int retval = 0;
    if (target != NULL)
    {
        target->p_Data = data;
    }
    return retval;
}

int Caven_packet_data_copy_Fun(Caven_info_packet_Type *source,Caven_info_packet_Type *target)
{
    int retval = 0;
    Caven_info_packet_Type temp_packet;

    if ((source != NULL) && (target != NULL))
    {
        temp_packet = *target;                  // 抽离数据
        temp_packet.p_Data = source->p_Data;    // 保留指针
        memcpy(temp_packet.p_Data,target->p_Data,target->dSize);    // 复制指针内容,内容的长度依据是[dSize]
        *source = temp_packet;                  // copy
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
    p_data = target->p_Data;
    if (p_data != NULL)
    {
#ifdef BUFF_MAX
        if(target->dSize > 0 && target->dSize <= BUFF_MAX)
        {
            memset(p_data, 0, target->dSize);
        }
#else
#endif
    }
    memset(target, 0, sizeof(Caven_info_packet_Type));
    target->p_Data = p_data;
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
    return retval;
}
