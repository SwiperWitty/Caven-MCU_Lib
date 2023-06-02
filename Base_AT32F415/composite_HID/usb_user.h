#ifndef __USB_USER_H
#define __USB_USER_H

#include "BASE.h"

#include "usb_core.h"

struct USB_         //功能（不包括初始化）
{
    //keyboard
    int (*Keyboard_Send_String) (char *string);
    int (*Keyboard_Send_Data) (char *data, int Sendlen);
    //custom
    int (*Custom_Send) (const void *Data,int bufflen);        //有缓冲的发送
    int (*Custom_Receive) (char *Data);
};


extern otg_core_type otg_core_struct;


void USB_User_init (int SET);
//keyboard
int USB_Keyboard_Send_String (char *string);
int USB_Keyboard_Send_Data (char *data, int Sendlen);
//custom
int USB_Buffer_send (const void *Data,int bufflen);        //有缓冲的发送
int USB_Buffer_Receive (char *Data);

#endif

//
