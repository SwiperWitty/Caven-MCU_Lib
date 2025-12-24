#include "IC_AS608.h"


// 计算校验和
static uint16_t calculate_checksum(uint8_t *data, uint16_t length) 
{
    uint16_t sum = 0;
    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

/*
 * fast 主要给中断，这样就不会循环套娃
 */
int AS608_info_packet_fast_clean_Fun(AS608_Packet *target)
{
    int retval = 0;
	if (target != NULL)
	{
		target->header = 0;
		target->address = 0;

		target->checksum = 0;
		target->get_crc = 0;
		target->get_num = 0;
		target->dSize = 0;
		target->Run_status = 0;
	}
    return retval;
}

int AS608_info_Make_packet_Fun(AS608_Packet *target, uint8_t data)
{
    int retval = 0;
    uint16_t temp_sum = 0;
    AS608_Packet temp_packet = *target;
    uint8_t *tepm_pData = temp_packet.data;
    if (temp_packet.Run_status & 0x40) /* 目标有数据没处理 */
    {
        return (-0x80);
    }
    if (target == NULL || tepm_pData == NULL)
    {
        return (-0x8F);
    }
    switch (temp_packet.Run_status)
    {
    case 0:
        temp_packet.header = (temp_packet.header << 8) + data;
        if (temp_packet.header == AS608_CMD_HEADER)
        {
            temp_packet.checksum = 0;
            temp_packet.get_crc = 0;
            temp_packet.length = 0;
            temp_packet.dSize = 0;
            temp_packet.get_num = 2;
            temp_packet.Run_status++;
        }
        break;
    case 1:
        temp_packet.address = (temp_packet.address << 8) + data;
        temp_packet.get_num ++;
        if (temp_packet.get_num >= 6)
        {
            temp_packet.Run_status++;
        }
        break;
    case 2:
        temp_packet.type = data;
        temp_packet.get_num ++;

        temp_packet.Run_status++;
        break;
    case 3:
        temp_packet.length = (temp_packet.length << 8) + data;
        temp_packet.get_num ++;
        if (temp_packet.get_num >= 9)
        {
            temp_packet.Run_status++;
        }
        break;
    case 4:
        tepm_pData[temp_packet.dSize++] = data;
        temp_packet.get_num ++;
        if (temp_packet.dSize >= (temp_packet.length - 2))
        {
            temp_packet.Run_status++;
        }
        break;
    case 5:
        temp_packet.get_crc = (temp_packet.get_crc << 8) + data;
        temp_packet.get_num ++;
        if (temp_packet.get_num >= (9 + temp_packet.length))
        {
            temp_sum = temp_packet.type + temp_packet.length;
			temp_sum += calculate_checksum (temp_packet.data,temp_packet.dSize);
            if (temp_packet.get_crc == temp_sum)
            {
                temp_packet.Run_status = 0x40;
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
    if (temp_packet.Run_status < 0)
    {
        retval = temp_packet.Run_status;
        AS608_info_packet_fast_clean_Fun(target);
    }
    else if (temp_packet.Run_status == 0x40)
    {
        *target = temp_packet;
        retval = temp_packet.Run_status;
    }
    else
    {
        *target = temp_packet;
        retval = temp_packet.Run_status;
    }
    return retval;
}

int AS608_info_Split_packet_Fun(AS608_Packet const source, uint8_t *data)
{
    int retval = 0;
    int temp_run = 0,temp_val = 0;
    uint16_t temp_sum = 0;
    if (data != NULL)
    {
        data[temp_run ++] = source.header & 0xff;
        data[temp_run ++] = (source.header << 8) & 0xff;

        data[temp_run ++] = source.address & 0xff;
        data[temp_run ++] = (source.address << 8) & 0xff;
        data[temp_run ++] = (source.address << 16) & 0xff;
        data[temp_run ++] = (source.address << 24) & 0xff;

        data[temp_run ++] = source.type & 0xff;
        
        data[temp_run ++] = (source.length << 8) & 0xff;
        data[temp_run ++] = source.length & 0xff;

        if (source.dSize == 0)
        {
            temp_val = source.length - 2;
        }
        if (source.dSize > 0)
        {
            memcpy(&data[temp_run],source.data,temp_val);
            temp_run += temp_val;
        }
        temp_sum = calculate_checksum(&data[7], temp_run - 6);
        data[temp_run ++] = temp_sum & 0xff;
        data[temp_run ++] = (temp_sum << 8) & 0xff;
        retval = temp_run;
    }
    return retval;
}

