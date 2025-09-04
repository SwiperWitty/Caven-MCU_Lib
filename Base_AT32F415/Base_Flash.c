#include "Base_Flash.h"
#include "string.h"

int Base_addr_check (int addr,int len)
{
	int retval = 0;
#if Exist_FLASH 
	if(len == 0)
    {
		retval = -2;
		return retval;
    }
    if((addr >= FLASH_END_ADDR) || (addr < FLASH_START_ADDR))
    {
        retval = (-1);
        return retval;
    }
    if((addr + len) >= FLASH_END_ADDR)
    {
        retval = (-1);
        return retval;
    }
#endif
	return retval;
}

/*
    符合地址大小 ≥ 0,且返回扇区
    不符合 (-1)
*/
int Base_Addr_Get_Area(int addr)
{
    int retval = 0;
#if Exist_FLASH 
    if((addr >= FLASH_END_ADDR) || (addr < FLASH_START_ADDR))
    { 
//        printf("S-ERROR\r\n"); 
        retval = (-1);
        return retval;
    }
    retval = (addr - FLASH_START_ADDR) / FLASH_PAGE_SIZE;
	retval = FLASH_START_ADDR + retval * FLASH_PAGE_SIZE;
#endif
    return retval;
}

int Base_Flash_Erase (int addr,int len)
{
	int retval = 0,flashStatus = 0;
#if Exist_FLASH 
	uint32_t start_addr,end_addr;
	int temp_num = 0,temp_val;
	if(len == 0)
    {
		retval = -2;
		return retval;
    }
    retval = Base_addr_check (addr,len);
	if (retval < 0)
	{
		return retval;
	}
    start_addr = Base_Addr_Get_Area(addr);
    end_addr = addr + len;
	temp_num = end_addr - start_addr;
	if ((temp_num % FLASH_PAGE_SIZE) > 0)
	{
		temp_num = (temp_num / FLASH_PAGE_SIZE) + 1;
	}
	else
	{
		temp_num = (temp_num / FLASH_PAGE_SIZE);
	}
	__disable_irq();
    flash_unlock();
	for (int i = 0; i < temp_num; i++)
    {
		temp_val = start_addr + (i * FLASH_PAGE_SIZE);
        retval = flash_operation_wait_for(ERASE_TIMEOUT);
        if((retval == FLASH_PROGRAM_ERROR) || (retval == FLASH_EPP_ERROR))
        {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
        else if(retval == FLASH_OPERATE_TIMEOUT)
        {return ERROR;}
        flashStatus |= flash_sector_erase(temp_val);
    }
	__enable_irq();
	flash_lock();
#endif
    if(flashStatus != FLASH_OPERATE_DONE)
    {
		retval = 1;
    }
    else
    {
        retval = 0;
    }
	return retval;
}

/*
retval = 0,COMPLETE
retval = x,error
*/
int Base_Flash_Read (void *data,int addr,int len)
{
    int retval = 0;
#if Exist_FLASH 
	if(data == NULL || len == 0)
    {
		retval = -2;
		return retval;
    }
    retval = Base_addr_check (addr,len);
	if (retval < 0)
	{
		return retval;
	}
    // 直接内存拷贝，按32位读取
    for (uint32_t i = 0; i < len; i++) {
       *((uint8_t *)data + i) = *(__IO uint8_t*)(addr + i);
    }
#endif
    return retval;
}

/*
retval = 0,COMPLETE
retval = x,error
*/
int Base_Flash_Write (void *data,int addr,int len)
{
    int retval = 0,flashStatus = FLASH_OPERATE_DONE;
#if Exist_FLASH 
    u32 start_addr = 0,temp_data;
    int temp_num = 0;
	if(data == NULL || len == 0)
    {
		retval = -2;
		return retval;
    }
    retval = Base_addr_check (addr,len);
	if (retval < 0)
	{
		return retval;
	}
    start_addr = addr;

	__disable_irq();
    flash_unlock();

    if (flashStatus == FLASH_OPERATE_DONE)
    {
		retval = 0;
        for (int i = 0; i < len;)
        {
            temp_data = 0;
            temp_num = addr + len;
            temp_num = temp_num - (start_addr + i);
            if (temp_num > 4)
            {
                temp_num = 4;
            }
            for (int j = temp_num; j > 0; j--)
            {
                temp_data |= *((uint8_t *)data + i + (j - 1));
				if (j > 1)
				{
					temp_data <<= 8;
				}
            }
            retval = flash_operation_wait_for(ERASE_TIMEOUT);
            if((retval == FLASH_PROGRAM_ERROR) || (retval == FLASH_EPP_ERROR))
            {flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);}
            else if(retval == FLASH_OPERATE_TIMEOUT)
            {return ERROR;}
            flashStatus = flash_word_program(start_addr + i, temp_data);
            i += 4;
            if(flashStatus != FLASH_OPERATE_DONE)
            {
                break;
            }
        }
    }
	__enable_irq();
    flash_lock();
#endif
    if(flashStatus != FLASH_OPERATE_DONE)
    {
		retval = 1;
    }
    else
    {
        retval = 0;
    }
	return retval;
}
