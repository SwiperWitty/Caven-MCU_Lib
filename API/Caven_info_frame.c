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
** -0x80 < retval < 0 协议消息解析错误
** retval = -0x80 包里存在协议消息没处理
** retval = -0x8F 目标包的指针没有索引
** retval & 0x80 >= 0 获取到协议消息
** retval 其他 获取中（包含没开始retval = 0）
*/
int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, unsigned char data)
{
    int retval = 0;
    int temp = 0, i = 0;
    static int s_status = 0;
    static int s_getnum = 0;
    static uint8_t s_array_buff[300];               /* 可以不清除 */
    static Caven_info_packet_Type s_temp_packet = {0};

    if (target->Result & 0x80) /* 目标有数据没处理 */
    {
        return (-0x80);       
    }
    if (target == NULL || target->p_Data == NULL)
    {
        return (-0x8F);
    }

    switch (s_status)
    {
    case 0: /* Head */
        s_temp_packet.Head = (s_temp_packet.Head << 8) + data;
        if (s_temp_packet.Head == standard.Head)
        {
            s_getnum = 0;
            s_array_buff[s_getnum++] = (s_temp_packet.Head >> 8) & 0x00ff;
            s_array_buff[s_getnum++] = (s_temp_packet.Head) & 0x00ff;
            s_status++;
        }
        break;
    case 1: /* agreement Versions */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Versions = data;
        if (s_temp_packet.Versions <= standard.Versions)
        {
            s_status++;
        }
        else
        {
            s_status = -s_status;
        }
        break;
    case 2: /* Type */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Type = data;
        if (s_temp_packet.Type == standard.Type || s_temp_packet.Type == 0)
        {
            s_status++;
        }
        else
        {
            s_status = -s_status;
        }
        break;
    case 3: /* Addr */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Addr = data;
        if (s_temp_packet.Addr == standard.Addr || s_temp_packet.Addr == 0 || s_temp_packet.Addr == 0xff)
        {
            s_status++;
        }
        else
        {
            s_status = -s_status;
        }
        break;
    case 4: /* Cmd */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Cmd = data;
        s_status++;

        break;
    case 5: /* Cmd_sub */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Cmd_sub = data;
        s_status++;

        break;
    case 5: /* Size */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Size = (s_temp_packet.Size << 8) + data;
        temp =  7 + 2;
        if (s_getnum >= temp)
        {
            if (s_temp_packet.Size > standard.Size)
            {
                s_status = -s_status;
            }
            else if (s_temp_packet.Size == 0)
            {
                s_status+= 2;         /* 0个 p_Data ，直接去 End_crc */
            }
            else
            {
                s_status++;
            }
        }
        break;
    case 6: /* p_Data */
        s_array_buff[s_getnum++] = data;
        temp =  7 + 2 + s_temp_packet.Size;
        if (s_getnum >= temp)
        {
            s_temp_packet.p_Data = s_array_buff[9];     //
            s_status++;
        }
        break;
    case 7: /* Result */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.Result = data;
        s_temp_packet.Result &= (~0x80);
        s_status++;
        break;
    case 8: /* End_crc */
        s_array_buff[s_getnum++] = data;
        s_temp_packet.End_crc = (s_temp_packet.End_crc << 8) + data;
        temp =  7 + 2 + s_temp_packet.Size + 3;
        if (s_getnum >= temp)
        {
            temp = CRC16_CCITT_CalculateBuf(&s_array_buff[2], (s_getnum - 2));
            if(s_temp_packet.End_crc == temp)       
            {
                s_temp_packet.Result |= 0x80;       // crc secc 
            }
        }
        break;
    default:
        break;
    }
    /*  结果    */
    if (s_status < 0) // error
    {
        memset(&s_temp_packet, 0, sizeof(Caven_info_packet_Type));
        retval = s_status;
        s_status = 0;
        s_getnum = 0;
    }
    else if (s_temp_packet.Result & 0x80 != 0) // secc 
    {
        temp =  (7 + 2) - 1;
        s_temp_packet.p_Data = target->p_Data;  /* 提取目标的数据指针 */
        memcpy(s_temp_packet.p_Data, &s_array_buff[temp], s_temp_packet.Size);
        *target = s_temp_packet;

        memset(&s_temp_packet, 0, sizeof(Caven_info_packet_Type));
        retval = 0x80;
        s_status = 0;
        s_getnum = 0;
    }
    else // doing
    {
        target->Result = 0;
        retval = s_status;
    }
    return retval;
}

/*
Caven_info_Split_packet_Fun 
** split packet 
** 将[packet]数据转化成[data] 
传参 
** source ：数据来源包(指针，这个包内的指针必须要有索引) 
** data     ：数据目标
return   : retval 
** retval < 0 转化错误 
** retval = 0 转化成功 
** retval = (-0x8F) 指针未索引  
*/
int Caven_info_Split_packet_Fun(Caven_info_packet_Type const soure, unsigned char *data)
{
    int retval = 0;
    int temp = 0;
    int getnum = 0;
    uint8_t array_buff[300];
    if (data == NULL || soure.p_Data == NULL)
    {
        retval = (-0x8F);
    }
    else
    {
        array_buff[getnum ++] = soure.Head & 0xff;
        array_buff[getnum ++] = (soure.Head >> 8) & 0xff;

        array_buff[getnum ++] = soure.Versions;
        array_buff[getnum ++] = soure.Type;
        array_buff[getnum ++] = soure.Addr;
        array_buff[getnum ++] = soure.Cmd;
        array_buff[getnum ++] = soure.Cmd_sub;

        array_buff[getnum ++] = soure.Size & 0xff;
        array_buff[getnum ++] = (soure.Size >> 8) & 0xff;

        memcpy(&array_buff[(getnum - 1)],soure.p_Data,soure.Size);
        getnum += soure.Size;

        array_buff[getnum ++] = soure.Result;
        
        array_buff[getnum ++] = soure.End_crc & 0xff;
        array_buff[getnum ++] = (soure.End_crc >> 8) & 0xff;

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
