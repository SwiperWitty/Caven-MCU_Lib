#ifndef _INFO_FRAME_HANDLE__H_
#define _INFO_FRAME_HANDLE__H_
/*
    this is info_frame_handle.h feil    
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

int send_info_frame_handle (const void * data, int size, char channel);
int receive_info_frame_handle (const void * data, int size, char channel);


#endif
