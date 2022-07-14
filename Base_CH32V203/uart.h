#ifndef _UART_X__H_
#define _UART_X__H_

#include "Items.h"

#define RXD_Falg    USART_FLAG_RXNE     //  接收标志
#define TXD_Falg    USART_FLAG_TC       //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

/* 【宏函数群】   */

#define UART_Channel_MAX  5     //最高通道数


#ifdef Exist_UART
/*  中断   */
    #define UART1_Interrupt() USART1_IRQHandler()
    #define UART2_Interrupt() USART2_IRQHandler()
    #define UART3_Interrupt() USART3_IRQHandler()
    #define UART4_Interrupt() UART4_IRQHandler()
    #define UART5_Interrupt() UART5_IRQHandler()

/*  Falg   */       //这个是为了给 mode 相关函数提供灵活度
    #define UART_Interrupt_RXDFalg(Channel) UART_RXD_Flag(Channel)
    #define UART_Interrupt_RXDFalgClear(Channel)    UART_RXD_Flag_Clear(Channel)

/*  数据    */
    #define UART_RXD_DATA(Channel)    UART_RXD_Receive(Channel)
    #define UART_TXD_DATA(Channel,DATA)    UART_TXD_Send(Channel,DATA)

#endif

/*  end */

void Uart1_Init(int Baud,int SET);
void Uart2_Init(int Baud,int SET);
void Uart3_Init(int Baud,int SET);
void Uart4_Init(int Baud,int SET);
void Uart5_Init(int Baud,int SET);

uint16_t UART_RXD_Receive(char Channel);
void UART_TXD_Send(char Channel,uint16_t DATA);

char UART_RXD_Flag(char Channel);
void UART_RXD_Flag_Clear(char Channel);

#endif
