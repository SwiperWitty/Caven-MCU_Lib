#ifndef __USB_USER_H
#define __USB_USER_H

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#ifdef Exist_USB
#include "Caven_Type.h"

typedef struct
{
    int (*Keyboard_Send_String) (char *string);
    int (*Keyboard_Send_Data) (char *data, int size);
    //custom
    int (*Send_Data_pFun) (const uint8_t *data,int size);
    int (*Receive_Bind_pFun) (D_pFun USB_Callback_pFun);
}MODE_USB_Way;

#endif

int USB_User_init (int Set);

//keyboard
int USB_Keyboard_Send_String (char *string);
int USB_Keyboard_Send_Data (char *data, int size);
//custom
int USB_Send_Data (const uint8_t *data,int size);        // 有缓冲的发送
int USB_Callback_Bind (D_pFun USB_Callback_pFun);
int USB_Buffer_Receive (uint8_t *data);

int USB_User_State_Get (void);
#endif

//
