/*
 * Base_UART.h
 * 需要规定什么使用宏函数，什么时候直接用函数。
 * 但是他们调用的时候是一样的，所以单纯在Base里面纠结。
 * 串口1是没有RX DMA的，SPI2 TX已经占用。
 * 
为了方便上层统一调用，本文件需要提供以下函数

int Base_UART_Init(UART_mType Channel,int Baud,int Set);
void Base_UART_Send_Data(UART_mType Channel, uint16_t Data)

int State_Machine_Bind (UART_mType Channel,iD_pFun UART_pFun);
void Base_UART_Recv_Poll_Task(void);
-测试案例
dma缓存512，主机发送600字节，并需要解析机处理（隐性边界 bug）
主机发送一包数据，查看响应时间
连续500k有效数据测试（缓存、吞吐测试）
 */
#ifndef _Base_UART__H_
#define _Base_UART__H_

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

typedef enum
{
   m_UART_CH0 = 0,
    m_UART_CH1 = 1,
    m_UART_CH2,
    m_UART_CH3,
    m_UART_CH4,
    m_UART_CH5,
}UART_mType;

#ifdef Exist_UART
    #define DMA_UART        1
    #define UART_BUFF_MAX   360
#endif

#if (Exist_UART & OPEN_0001)

#endif

#if (Exist_UART & OPEN_0010)
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define UART1_HANDLERIT() USART1_IRQHandler()
#define DMA_UART1_RX    0
#if DMA_UART1_RX & DMA_UART
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define DMA1_CH5_HANDLERIT() DMA1_Channel5_IRQHandler()
#endif
#endif

#if (Exist_UART & OPEN_0100)
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define UART2_HANDLERIT() USART2_IRQHandler()
#define DMA_UART2_RX    1
#if DMA_UART2_RX & DMA_UART
void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define DMA1_CH6_HANDLERIT() DMA1_Channel6_IRQHandler()
#endif
#endif

#if (Exist_UART & OPEN_1000)
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define UART3_HANDLERIT() USART3_IRQHandler()
#define DMA_UART3_RX    1
#if DMA_UART3_RX & DMA_UART
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define DMA1_CH3_HANDLERIT() DMA1_Channel3_IRQHandler()
#endif
#endif

#if (Exist_UART & OPEN_10000)
void UART4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#define UART4_HANDLERIT() UART4_IRQHandler()
#define DMA_UART4_RX    0
#if DMA_UART4_RX & DMA_UART

#endif
#endif

// fun
int Base_UART_Init(UART_mType Channel,int Baud,int Set);
void Base_UART_Send_Data(UART_mType Channel,uint16_t Data);
void Base_UART_DMA_Send_Buff(UART_mType Channel,const uint8_t *Data,int Length);

/*
 * 接收入口的状态机，这个很重要
 * 在初始化之后执行一次
 * 不可用阻塞
 */
int State_Machine_Bind(UART_mType Channel,iD_pFun UART_pFun);
/*
 * 接收任务
 */
void Base_UART_Recv_Poll_Task(void);

#endif
