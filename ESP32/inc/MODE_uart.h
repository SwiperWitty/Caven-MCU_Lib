#ifndef _MODE_UART__H_
#define _MODE_UART__H_
/*
    this is MODE_UART.h file    
    

    2023.9.13   
*/

#include "stdint.h"
#include "Items.h"

#include "esp_event.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#ifdef Exist_UART
    #if CONFIG_IDF_TARGET_ESP32
        
        #define CUSTOM_UART1
        #define ECHO_TXD1 (15)
        #define ECHO_RXD1 (34)
        #define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
        #define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

        #define ECHO_TXD2 (4)
        #define ECHO_RXD2 (35)
    #endif

#endif

typedef void (*D_Callback_pFun) (void *data);   // 数据回调类型

int custom_uart1_init(int baud_rate, int set);
int custom_uart2_init(int baud_rate, int set);
int custom_uart1_send_data(uint8_t *data, int size);
int custom_uart2_send_data(uint8_t *data, int size);
void custom_uart1_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun);
void custom_uart2_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun);

void custom_uart_task_Fun(void);

#endif
