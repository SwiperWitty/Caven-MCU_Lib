#ifndef __USB_USER_H
#define __USB_USER_H

#include "BASE.h"

#include "usb_core.h"

typedef struct
{
    //keyboard
    int (*Keyboard_Send_String) (char *string);
    int (*Keyboard_Send_Data) (char *data, int Sendlen);
    //custom
    int (*Custom_Send) (const void *Data,int bufflen);        //有缓冲的发送
    int (*Custom_Receive) (char *Data);
}MODE_USB_HID_Way;


extern otg_core_type otg_core_struct;


int MODE_USB_Init (int SET);
//keyboard
int USB_Keyboard_Send_String (char *string);
int USB_Keyboard_Send_Data (char *data, int Sendlen);
//custom
int USB_Buffer_send (const void *Data,int bufflen);        //有缓冲的发送
int USB_Buffer_Receive (char *Data);

#endif

//
