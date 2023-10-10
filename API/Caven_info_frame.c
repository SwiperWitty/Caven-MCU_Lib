#include "Caven_info_frame.h"

#include "Check_CRC.h"

/*
** Caven_info_frame_Fun 处理 Caven_info_packet
** standard ：标准要求
** target   : 目标包
** data     ：数据来源（填入）
** channel  ：数据来源（方式）
*/
int Caven_info_frame_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, uint16_t data, char channel)
{
    int retval = 0;
    int temp = 0, i = 0;
    static int s_status = 0;
    static int s_getnum = 0;
    static uint8_t s_array_buff[300];
    static Caven_info_packet_Type s_temp_packet = {0};

    if ((target->Result & 0x80) || target == NULL) /* 目标有数据没处理 & 标准不存在 (如果不想进入处理,end_flag = 0x80) */
    {
        return -1;
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
        
        if (s_getnum >= 9)
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
        if (temp >= s_getnum)
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
        target->end_flag = status;
        memset(&temp_data, 0, sizeof(temp_data));

        retval = status;
        status = 0;
        getnum = 0;
    }
    else if (s_temp_packet.Result & 0x80 != 0)  // secc 
    {
        i = 5;
        *target = s_temp_packet;
        memset(target->buff, 0, sizeof(target->buff)); // ou
        memcpy(target->buff, &temp_data.buff[i], target->buff_length);
        target->pack_length = getnum;
        target->end_flag = status;

        memset(&temp_data, 0, sizeof(temp_data));

        retval = 0x80;
        s_status = 0;
        getnum = 0;
    }
    else // doing
    {
        target->end_flag = 0;
        retval = s_status;
    }
}

int data_handle_XA(packet_data const standard, packet_data *target, u16 data, char mchannel)
{
    int retval = 0;
    int temp = 0, i = 0;
    static int status = 0;
    static int getnum = 0;
    static packet_data temp_data = {0};

    if ((target->end_flag & 0x80) || target == NULL) // 目标没处理 & 标准不存在 (如果不想进入处理,end_flag = 0x80)
    {
        return -1;
    }
    switch (status)
    {
    case 0: // 头
        temp_data.packet_head = (temp_data.packet_head << 8) + data;
        if (temp_data.packet_head == standard.packet_head)
        {
            getnum = 0;
            temp_data.buff[getnum++] = (temp_data.packet_head >> 8) & 0x00ff;
            temp_data.buff[getnum++] = (temp_data.packet_head) & 0x00ff;
            status++;
        }
        break;
    case 1: // RS485地址
        temp_data.buff[getnum++] = data;
        temp_data.flag_485 = data;
        if (temp_data.flag_485 == standard.flag_485)
        {
            status++;
        }
        else
        {
            status = (-1);
        }
        break;
    case 2: // cmd
        temp_data.buff[getnum++] = data;
        temp_data.cmd = data;
        status++;
        break;
    case 3: // length
        temp_data.buff[getnum++] = data;
        temp_data.buff_length = data & 0xFF;
        status++;
        if (temp_data.buff_length == 0)
        {
            status++; // 0个 buff，直接去crc
        }
        break;
    case 4: //  buff
        temp_data.buff[getnum++] = data;
        temp = 5 + temp_data.buff_length;
        if (getnum >= temp)
        {
            status++;
        }
        break;

    case 5: //  crc
        temp_data.buff[getnum++] = data;
        temp_data.crc = data & 0xff;
        temp = CRC_8BIT_XA(&temp_data.buff[2], 3 + temp_data.buff_length);

        if (temp == temp_data.crc)
        {
            status |= 0x80; // secc
        }
        else
        {
            status = (-3);
        }

        break;
    default:
        break;
    }
    /*  结果      */
    if (status < 0) // error
    {
        target->end_flag = status;
        memset(&temp_data, 0, sizeof(temp_data));

        retval = status;
        status = 0;
        getnum = 0;
    }
    else if (status > 0x80) // secc
    {
        i = 5;
        *target = temp_data;
        memset(target->buff, 0, sizeof(target->buff)); // ou
        memcpy(target->buff, &temp_data.buff[i], target->buff_length);
        target->pack_length = getnum;
        target->end_flag = status;

        memset(&temp_data, 0, sizeof(temp_data));

        retval = 0x80;
        status = 0;
        getnum = 0;
    }
    else // doing
    {
        target->end_flag = 0;
        retval = status;
    }
    return retval;
}
