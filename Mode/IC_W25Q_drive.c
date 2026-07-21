#include "IC_W25Q_drive.h"
#include "Base.h"

#if Exist_W25Q_DRV

#ifndef W25Q_BUSY_WAIT_MAX
#define W25Q_BUSY_WAIT_MAX     5000
#endif

static u32 g_W25Q_MaxAddr = 0;
static u8 g_W25Q_Enable = 0;
static char g_W25Q_CS_Serial = 0;

static void W25Q_CS_Ctl(void *data)
{
    u8 state = 0;

    if (data != NULL)
    {
        state = *(u8 *)data;

        if (state)
        {
            User_GPIO_set(2, 1, 0);
        }
        else
        {
            User_GPIO_set(2, 1, 1);
        }
    }
}

static void W25Q_CS_EN(void)
{
    Base_SPI_CS_Set(W25Q_BIND_SPI_CH, g_W25Q_CS_Serial, 1);
}

static void W25Q_CS_DIS(void)
{
    Base_SPI_CS_Set(W25Q_BIND_SPI_CH, g_W25Q_CS_Serial, 0);
}

static void W25Q_Write_Enable(void)
{
    W25Q_CS_EN();
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_WREN);
    // W25Q_CS_DIS();
}

static u8 MODE_W25Q_Read_Status_Fun(void)
{
    u8 sta = 0;

    W25Q_CS_EN();
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_RDSR);
    Base_SPI_ASK_Receive(W25Q_BIND_SPI_CH, 0xFF, &sta, 1);
    // W25Q_CS_DIS();

    return sta;
}

static int MODE_W25Q_Wait_Busy_Fun(void)
{
    uint32_t retry = 0;

    while (MODE_W25Q_Read_Status_Fun() & W25Q_STA_BUSY)
    {
        if (++retry >= W25Q_BUSY_WAIT_MAX)
        {
            return -4;
        }

        SYS_Base_Delay(100, 1000);
    }

    return 0;
}

static int W25Q_Check_Addr_Range(u32 StartAddr, int Len)
{
    uint64_t endAddr;

    if ((g_W25Q_Enable == 0) || (g_W25Q_MaxAddr == 0) || (Len <= 0))
    {
        return -3;
    }

    endAddr = (uint64_t)StartAddr + Len - 1;

    if (endAddr > g_W25Q_MaxAddr)
    {
        return -3;
    }

    return 0;
}

#endif

u32 MODE_W25Q_Get_Max_Addr_Fun(void)
{
#if Exist_W25Q_DRV
    if (g_W25Q_Enable == 0)
    {
        return 0;
    }

    return g_W25Q_MaxAddr;
#endif
    return 0;
}

int MODE_W25Q_Erase_Sector_Fun(u32 FlashAddr)
{
    int retval = -1;

#if Exist_W25Q_DRV
    int Flash_num;
    if (g_W25Q_Enable == 0)
    {
        return -1;
    }

    retval = W25Q_Check_Addr_Range(FlashAddr, 4096);
    if (retval < 0)
    {
        return retval;
    }
    Flash_num = FlashAddr & 0xfffff000;
    W25Q_Write_Enable();

    W25Q_CS_EN();
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_SECTOR_ER);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (Flash_num >> 16) & 0xFF);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (Flash_num >> 8) & 0xFF);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, Flash_num & 0xFF);
    // W25Q_CS_DIS();

    retval = MODE_W25Q_Wait_Busy_Fun();

#endif

    return retval;
}

int MODE_W25Q_Write_Data_Fun(u32 FlashAddr, const u8 *Data, int Length)
{
    int retval = -1;

#if Exist_W25Q_DRV
    int send_len = 0;
    int remain = 0;
    u32 addr = 0;

    if ((Data == NULL) || (Length <= 0))
    {
        return -2;
    }

    if (g_W25Q_Enable == 0)
    {
        return -1;
    }

    retval = W25Q_Check_Addr_Range(FlashAddr, Length);
    if (retval < 0)
    {
        return retval;
    }

    addr = FlashAddr;
    remain = Length;

    while (remain > 0)
    {
        send_len = W25Q_PAGE_SIZE - (addr & (W25Q_PAGE_SIZE - 1));

        if (send_len > remain)
        {
            send_len = remain;
        }

        W25Q_Write_Enable();

        W25Q_CS_EN();
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_PAGE_WR);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (addr >> 16) & 0xFF);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (addr >> 8) & 0xFF);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, addr & 0xFF);

        for (int i = 0; i < send_len; i++)
        {
            Base_SPI_Send_Data(W25Q_BIND_SPI_CH, Data[i]);
        }

        // W25Q_CS_DIS();

        retval = MODE_W25Q_Wait_Busy_Fun();
        if (retval < 0)
        {
            return retval;
        }

        Data += send_len;
        addr += send_len;
        remain -= send_len;
    }

    retval = 0;

#endif

    return retval;
}

/*
    retval < 0;error
    retval = 0;succ
*/
int MODE_W25Q_Read_Data_Fun(u32 FlashAddr, u8 *RecvBuf, int Length)
{
    int retval = -1;

#if Exist_W25Q_DRV
    if ((RecvBuf == NULL) || (Length <= 0))
    {
        return -2;
    }

    if (g_W25Q_Enable == 0)
    {
        return -1;
    }

    retval = W25Q_Check_Addr_Range(FlashAddr, Length);
    if (retval < 0)
    {
        return retval;
    }

    W25Q_CS_EN();
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_READ);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (FlashAddr >> 16) & 0xFF);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, (FlashAddr >> 8) & 0xFF);
    Base_SPI_Send_Data(W25Q_BIND_SPI_CH, FlashAddr & 0xFF);
    Base_SPI_ASK_Receive(W25Q_BIND_SPI_CH, 0xFF, RecvBuf, Length);
    // W25Q_CS_DIS();

    retval = 0;

#endif

    return retval;
}

/*
    retval < 0:error
    retval = 0:succ
*/
int MODE_W25Q_drive_Init(int set)
{
    int retval = -1;

#if Exist_W25Q_DRV
    u8 retry = 0;
    u8 id_buf[2] = {0};
    u8 cap_code = 0;

    if (set == 0)
    {
        g_W25Q_Enable = 0;
        g_W25Q_MaxAddr = 0;
        g_W25Q_CS_Serial = 0;
        Base_SPI_Init(W25Q_BIND_SPI_CH, 8, set, NULL);
        return 0;
    }

    User_GPIO_config(2, 1, 1);
    User_GPIO_set(2, 1, 1);

    retval = Base_SPI_Init(W25Q_BIND_SPI_CH, 8, set, W25Q_CS_Ctl);
    if (retval <= 0)
    {
        g_W25Q_Enable = 0;
        g_W25Q_MaxAddr = 0;
        g_W25Q_CS_Serial = 0;
        return -1;
    }

    g_W25Q_CS_Serial = retval;

    for (retry = 0; retry < 3; retry++)
    {
        id_buf[0] = 0;
        id_buf[1] = 0;

        W25Q_CS_EN();
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, W25Q_CMD_READ_ID);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, 0x00);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, 0x00);
        Base_SPI_Send_Data(W25Q_BIND_SPI_CH, 0x00);
        Base_SPI_ASK_Receive(W25Q_BIND_SPI_CH, 0xFF, id_buf, 2);
        W25Q_CS_DIS();

        if (id_buf[0] == W25Q_MFG_WINBOND)
        {
            break;
        }

        SYS_Base_Delay(100, 1000);
    }

    if (id_buf[0] != W25Q_MFG_WINBOND)
    {
        g_W25Q_Enable = 0;
        g_W25Q_MaxAddr = 0;
        return -1;
    }

    cap_code = id_buf[1];

    if ((cap_code < 0x11) || (cap_code > 0x19))
    {
        g_W25Q_Enable = 0;
        g_W25Q_MaxAddr = 0;
        return -3;
    }
    if (cap_code == 0x13)          // W25Q80  (8M-bit = 1M-byte)
    {
        g_W25Q_MaxAddr = 1 * 1024 * 1024 - 1;  // 0x0FFFFF
    }
    else if (cap_code == 0x14)     // W25Q16  (16M-bit = 2M-byte)
    {
        g_W25Q_MaxAddr = 2 * 1024 * 1024 - 1;  // 0x1FFFFF
    }
    else if (cap_code == 0x15)     // W25Q32  (32M-bit = 4M-byte)
    {
        g_W25Q_MaxAddr = 4 * 1024 * 1024 - 1;  // 0x3FFFFF
    }
    else if (cap_code == 0x16)     // W25Q64  (64M-bit = 8M-byte)
    {
        g_W25Q_MaxAddr = 8 * 1024 * 1024 - 1;  // 0x7FFFFF
    }
    else if (cap_code == 0x17)     // W25Q128 (128M-bit = 16M-byte)
    {
        g_W25Q_MaxAddr = 16 * 1024 * 1024 - 1; // 0xFFFFFF (需要4字节地址)
    }
    else if (cap_code == 0x18)     // W25Q256 (256M-bit = 32M-byte)
    {
        g_W25Q_MaxAddr = 32 * 1024 * 1024 - 1; // 0x1FFFFFF
    }
    else if (cap_code == 0x19)     // W25Q512 (512M-bit = 64M-byte)
    {
        g_W25Q_MaxAddr = 64 * 1024 * 1024 - 1; // 0x3FFFFFF
    }
    else    // 未知ID，可做错误处理
    {
        g_W25Q_MaxAddr = 0;
    }
    g_W25Q_Enable = 1;
    retval = 0;
#endif
    return retval;
}
