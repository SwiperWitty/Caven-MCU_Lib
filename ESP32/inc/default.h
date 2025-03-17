#ifndef _DEFAULT__H_
#define _DEFAULT__H_
/*
    this is default.h feil    
    这是个模板文件

    2023.9.13   
*/
#include "Items.h"
#include "Caven_Type.h"


#ifdef Exist_UART
    #if (BOARD_NAME == ESP32_CAVEND)
        
    #elif (BOARD_NAME == EY1001)
        
    #endif
#endif

int default_init (int set);    // 示例 


#endif
