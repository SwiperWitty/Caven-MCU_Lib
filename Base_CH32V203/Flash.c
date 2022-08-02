#include "Flash.h"

static uint32_t WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

const struct _Flash_DATA Flash_DATA =
{
    .information = 0,
    .version = 2,
    .baud = 4,
    .IP = 6,
    .GPIO = 8,
};

int Read_Flash(int Address)
{
#ifdef Exist_FLASH
    int temp;
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP|FLASH_FLAG_PGERR |FLASH_FLAG_WRPRTERR);

    temp = *(__IO int*) Address;

    FLASH_Lock();
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();
    return temp;
#endif
}

char Clear_Flash_Area(int Area)
{
    char Status = 0;
#ifdef Exist_FLASH
    int addr = 0x08000000 + (Area * FLASH_AREA_SIZE);
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV2;
    __disable_irq();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

    if (Area < 16)
    {
        if (addr >= FLASH_DATA &&  addr <= FLASH_END)   //·ÀÖ¹¹ýÍ·
        {
            FLASHStatus = FLASH_ErasePage(addr);        //²Á4K
        }
    }
    else
    {
        FLASHStatus = FLASH_EraseAllPages();
    }

    FLASH_Lock();
    RCC->CFGR0 &= ~(uint32_t)RCC_HPRE_DIV2;
    __enable_irq();

    printf("addr : %x \n",addr);        //debug
    if(FLASHStatus != FLASH_COMPLETE)
    {
        Status = 1;
        printf("FLASH Erase Fail\r\n");
    }
    else {
        printf("FLASH Erase Suc\r\n");
    }
#endif
    return Status;
}




