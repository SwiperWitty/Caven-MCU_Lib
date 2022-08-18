#ifndef _UART_X__H_
#define _UART_X__H_

#include "Items.h"
#include "stdio.h"

#define RXD_Falg    USART_IT_RXNE     //  接收标志
#define TXD_Falg    USART_FLAG_TC       //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

/* 【宏函数群】   */

#define UART_Channel_MAX  2     //最高通道数

#ifdef Exist_UART
/*  中断   */
    #define UART1_Interrupt() USART1_IRQHandler()
    #define UART2_Interrupt() USART2_IRQHandler()

#endif

/*  end */

void Uart1_Init(int Baud,int SET);
void Uart2_Init(int Baud,int SET);

uint16_t UART_RXD_Receive(char Channel);
void UART_TXD_Send(char Channel,uint16_t DATA);

char UART_RXD_Flag(char Channel);
void UART_RXD_Flag_Clear(char Channel);

#endif
