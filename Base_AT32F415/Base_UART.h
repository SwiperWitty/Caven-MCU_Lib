#ifndef _Base_UART_H__
#define _Base_UART_H__

#include "items.h"      //功能使能、指定文件（因MCU而异）
#include "stdio.h"


/* 【宏函数群】   */
#ifdef Exist_UART
	#define UART1_EXIST
	#define UART2_EXIST
	#define UART3_EXIST
//    #define UART4_EXIST
//    #define UART5_EXIST
	#define UART_End 6

/*	标志位	*/
	#define RXD_Falg    USART_FLAG_RDNE     //  接收标志
	#define TXD_Falg    USART_FLAG_TRAC		//  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

/*  中断   */
	#ifdef UART1_EXIST
    	#define UART1_Interrupt() USART1_IRQHandler()
	#endif
    #ifdef UART2_EXIST
    	#define UART2_Interrupt() USART2_IRQHandler()
	#endif
    #ifdef UART3_EXIST
    	#define UART3_Interrupt() USART3_IRQHandler()
	#endif
	#ifdef UART4_EXIST
    	#define UART4_Interrupt() USART3_IRQHandler()
	#endif
	#ifdef UART5_EXIST
    	#define UART5_Interrupt() USART3_IRQHandler()
	#endif
#endif
/*  end */

//可以有多个UART,建议把应用层也这样写一遍
#ifdef UART1_EXIST
void Uart1_Init(int Baud,int SET);
#endif
#ifdef UART2_EXIST
void Uart2_Init(int Baud,int SET);
#endif
#ifdef UART3_EXIST
void Uart3_Init(int Baud,int SET);
#endif
#ifdef UART4_EXIST
void Uart4_Init(int Baud,int SET);
#endif
#ifdef UART5_EXIST
void Uart5_Init(int Baud,int SET);
#endif

//发送、接收
uint16_t UART_RXD_Receive(char Channel);            //接收
void UART_TXD_Send(char Channel,uint16_t DATA);     //发送

//接收的标志位，给应用层更大权限、灵活度
char UART_RXD_Flag(char Channel);
void UART_RXD_Flag_Clear(char Channel);

#endif
