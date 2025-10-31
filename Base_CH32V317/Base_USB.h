#ifndef __USB_USER_H
#define __USB_USER_H

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif
#include "Caven_Type.h"

#if Exist_USB
#include "usb_core.h"
extern otg_core_type otg_core_struct;

typedef void (*D_Callback_pFun) (void *data);
typedef struct
{
    int (*Keyboard_Send_Data) (char *data, int size);
    //custom
    void (*Send_Data) (uint8_t *data,int size);
    void (*RX_Callback_Bind) (D_Callback_pFun USB_Callback_pFun);
}MODE_USB_Way;

void USB_Callback_Bind (D_Callback_pFun USB_Callback_pFun);
#endif

int USB_User_init (int Set);

void USB_Send_Data (uint8_t *data,int size);        // 有缓冲的发送
void USB_RX_Callback_Bind (D_pFun pFun);

#endif

//
