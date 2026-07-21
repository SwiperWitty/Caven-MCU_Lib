#ifndef _IC__W25Q_DRV_H_
#define _IC__W25Q_DRV_H_

#include "Items.h"
#if Exist_W25Q_DRV
	#include "Base.h"
#endif
#include "API.h"

// W25Q硬件固定挂载SPI2通道，内部使用，业务层无需关心
#define W25Q_BIND_SPI_CH    2

// W25Q全系列通用标准指令集
#define W25Q_CMD_WREN       0x06U  // 写使能
#define W25Q_CMD_RDSR       0x05U  // 读状态寄存器
#define W25Q_CMD_SECTOR_ER  0x20U  // 4K扇区擦除
#define W25Q_CMD_PAGE_WR    0x02U  // 页写入
#define W25Q_CMD_READ       0x03U  // 连续读取数据
#define W25Q_CMD_READ_ID    0x90U  // 读取厂商+容量ID

// W25Q全系列统一厂商ID（华邦）
#define W25Q_MFG_WINBOND    0xEFU

// 状态寄存器位（全系列通用）
#define W25Q_STA_BUSY       (1U << 0)
#define W25Q_PAGE_SIZE      256 
#define W25Q_SECTOR_SIZE    4096 

// W25Q操作函数集结构体
typedef struct
{
    int (*Write_Data_pFun)(u32 FlashAddr, const u8 *Data, int Length);
    int (*Read_Data_pFun)(u32 FlashAddr, u8 *RecvBuf, int Length);
    int (*Erase_Sector_pFun)(u32 FlashAddr);
} MODE_W25Q_Way;

// 初始化：自动识别芯片容量
int MODE_W25Q_drive_Init(int set);

// 对外业务接口
int MODE_W25Q_Write_Data_Fun(u32 FlashAddr, const u8 *Data, int Length);
int MODE_W25Q_Read_Data_Fun(u32 FlashAddr, u8 *RecvBuf, int Length);
int MODE_W25Q_Erase_Sector_Fun(u32 FlashAddr);

// 获取Flash最大合法地址（动态识别后返回，未初始化返回0）
u32 MODE_W25Q_Get_Max_Addr_Fun(void);

#endif
