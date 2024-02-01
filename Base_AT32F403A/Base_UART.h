/*
 * Base_UART.h
 * 需要规定什么使用宏函数，什么时候直接用函数。
 * 但是他们调用的时候是一样的，所以单纯在Base里面纠结。
 * 
为了方便上层统一调用，本文件需要提供以下函数

int Base_UART_Init(UART_mType Channel,int Baud,int SET);
void Base_UART_Send_Byte(UART_mType Channel,uint16_t DATA);

int State_Machine_Bind (UART_mType Channel,D_pFun UART_pFun);

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
//    m_UART_CH0 = 0,
    m_UART_CH1 = 1,
    m_UART_CH2,
    m_UART_CH3,
    m_UART_CH4,
//    m_UART_CH5,
}UART_mType;

#define UART_pf

#if (Exist_UART & OPEN_0001)

#define UART0_HANDLERIT() USART0_IRQHandler()
#endif

#if (Exist_UART & OPEN_0010)

#define UART1_HANDLERIT() USART1_IRQHandler()
#endif

#if (Exist_UART & OPEN_0100)

#define UART2_HANDLERIT() USART2_IRQHandler()
#endif

#if (Exist_UART & OPEN_1000)

#define UART3_HANDLERIT() USART3_IRQHandler()
#endif

#if (Exist_UART & OPEN_10000)

#define UART4_HANDLERIT() UART4_IRQHandler()
#endif

#ifdef Exist_UART
    #define DMA_UART
	#define DEBUG_OUT	m_UART_CH3   // Debug 通道(Caved 3.14是串口3)
    #define UART_BUFF_MAX   500
#endif

// fun
int Base_UART_Init(UART_mType Channel,int Baud,int SET);
void Base_UART_Send_Byte(UART_mType Channel,uint16_t Data);
void Base_UART_DMA_Send_Data(UART_mType Channel,const uint8_t *Data,int Length);

/*
 * 接收入口的状态机，这个很重要
 * 在初始化之后执行一次
 * 不可用阻塞
 */
int State_Machine_Bind(UART_mType Channel,D_pFun UART_pFun);

#endif
