#include "caven_at_info_frame.h"
#include "Items.h"

/*
 *  1) standard 标准格式
 *  2) target 储存数据的地址
 *  3) data 数据
 *  4) retval 是当前步骤，如果为[-0x40]数据没处理不能装填数据，[-0x4F]数据缓冲地址无效，[-0x50]数据量超过内存长度，[0x4x]接收完成
*/
int caven_at_info_Make_packet_Fun(Caven_at_info_packet_Type const standard, Caven_at_info_packet_Type *target, uint8_t data)
{
    int retval = 0;
    Caven_at_info_packet_Type temp_packet = *target;
    if (temp_packet.Result == standard.Result)
    {
        retval = (-0x40);
    }
    else if (target != NULL && temp_packet.p_Data != NULL)
    {
        switch (temp_packet.Run_status)
        {
        case 0x00:      // 接收头
        {
            temp_packet.Head <<= 8;
            temp_packet.Head |= data;
            if (standard.Head == temp_packet.Head)
            {
                temp_packet.Run_status++;
                temp_packet.Get_num = 2;
                temp_packet.dSize = 0;
                temp_packet.Result = 0;
                temp_packet.End = 0;
            }
        }
        break;
        case 0x01:      // 接收尾，或者接收'{'
        {
            if (temp_packet.dSize > standard.dSize)
            {
                temp_packet.Result = -(0x40 + 0x10);
            }
            else
            {
                temp_packet.Get_num++;
                temp_packet.p_Data[temp_packet.dSize++] = data;
            }
            if (data == '{')
            {
                temp_packet.Run_status++;
            }
            temp_packet.End <<= 8;
            temp_packet.End |= data;
            if (standard.End == temp_packet.End)
            {
                temp_packet.Run_status++;
                temp_packet.Result = standard.Result;
            }
        }
        break;
        case 0x02:      // 接收'}'
        {
            if (temp_packet.dSize > standard.dSize)
            {
                temp_packet.Result = -(0x40 + 0x10);
            }
            else
            {
                temp_packet.Get_num++;
                temp_packet.p_Data[temp_packet.dSize++] = data;
            }
            if (data == '}')
            {
                temp_packet.Run_status++;
            }
        }
        break;
        case 0x03:      // 接收尾
        {
            if (temp_packet.dSize > standard.dSize)
            {
                temp_packet.Result = -(0x40 + 0x10);
            }
            else
            {
                temp_packet.Get_num++;
                temp_packet.p_Data[temp_packet.dSize++] = data;
            }
            temp_packet.End <<= 8;
            temp_packet.End |= data;
            if (standard.End == temp_packet.End)
            {
                temp_packet.Run_status++;
                temp_packet.Result = standard.Result;
            }
        }
        break;
        default:
            break;
        }
    }
    else
    {
        retval = (-0x4F);
    }
    if (temp_packet.Result < 0)     // 异常
    {
        retval = temp_packet.Run_status;
        caven_at_info_packet_fast_clean_Fun(target);
    }
    else if ((temp_packet.Result & 0xF0) == standard.Result)    // 收到完整帧
    {
        temp_packet.Run_status = 0xFF;
        retval = temp_packet.Run_status;
        memcpy(target,&temp_packet,sizeof(temp_packet));
    }
    else    // 接收过程
    {
        retval = 0;
        memcpy(target,&temp_packet,sizeof(temp_packet));
    }
    return retval;
}

int caven_at_info_Split_packet_Fun(Caven_at_info_packet_Type const target, void *data)
{
    int retval = 0;
    int temp_len = 0;
    char * temp_str = data;
    if (data != NULL)
    {
        temp_len = strlen(data);
        memset(data,0,temp_len);
        strcat(data,"AT");
        retval += 2;
        if (target.dSize)
        {
            memcpy(&temp_str[retval],target.p_Data,target.dSize);
            retval += target.dSize;
        }
        if (memcmp(&temp_str[retval-2],"\r\n",2) != 0)
        {
            memcpy(&temp_str[retval],"\r\n",2);
            retval += 2;
        }
    }
    return retval;
}

int caven_at_info_packet_index_Fun(Caven_at_info_packet_Type *target, uint8_t *data)
{
    int retval = 0;
    if (target != NULL)
    {
        target->p_Data = data;
    }
    return retval;
}

int caven_at_info_packet_clean_Fun(Caven_at_info_packet_Type *target)
{
    int retval = 0;
    if (target != NULL)
    {
        target->Head = 0;
        target->End = 0;
        if (target->p_Data != NULL)
        {
            memset(target->p_Data,0,target->dSize);
        }
        target->dSize = 0;
        target->Result = 0;
        target->Run_status = 0;
        target->Get_num = 0;
        target->Comm_way = 0;
    }
    return retval;
}

int caven_at_info_packet_fast_clean_Fun(Caven_at_info_packet_Type *target)
{
    int retval = 0;
    if (target != NULL)
    {
        target->Head = 0;
        target->End = 0;

        target->dSize = 0;
        target->Result = 0;
        target->Run_status = 0;
        target->Get_num = 0;
        target->Comm_way = 0;
    }
    return retval;
}
