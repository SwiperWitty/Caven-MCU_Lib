/*
 * Base_USB.c
 *
 *  Created on: 2024年6月3日
 */
#include "Base_USB.h"

#ifdef Exist_USB
extern uint8_t HIDTxBuffer[];
extern uint8_t HID_Data_Buff[];

extern uint8_t HIDKey[];

extern void SYS_Base_Delay(int time, int speed);
void usb_delay (int times)
{
    SYS_Base_Delay (times, 2000);
}
#endif

int USB_User_init (int SET)
{
    int retval = 0;
#ifdef Exist_USB
    HID_Data_Buff[0] = 0;
    HID_Data_Buff[1] = 0;

    Set_USBConfig();
    USB_Init();
    USB_Port_Set(DISABLE, DISABLE);
    usb_delay(700);
    USB_Port_Set(ENABLE, ENABLE);
    USB_Interrupts_Config();
    usb_delay(700);          //一定要
#endif
    return retval;
}

//keyboard
int USB_Keyboard_Send_Data (char *data, int size)
{
    int retval = 0;
#ifdef Exist_USB
    int temp = size;
    int num = 0;
    do {
        for(int i = 0;i< ENDP1_IN_SIZE;i++) HIDKey[i] = 0;
        if(temp >= ENDP1_IN_SIZE)
        {
            for(int i = 0;i< ENDP1_IN_SIZE;i++) HIDKey[i] = data[num + i];
            temp -= ENDP1_IN_SIZE;
            num += ENDP1_IN_SIZE;
        }
        else
        {
            for(int i = 0;i< temp;i++) HIDKey[i] = data[num + i];
            temp = 0;
        }
        USBD_HID_Keyboard_Updata();
        retval = 0;
    } while (temp);
#endif
    return retval;
}

int USB_Keyboard_Send_String (char *string)
{
    int retval = 0;
#ifdef Exist_USB
    if (string != NULL) {
        USB_Keyboard_Send_Data (string, strlen(string));
    }

#endif
    return retval;
}


//custom
int USB_Buffer_Send (const uint8_t *data,int size)
{
    int retval = 1;
#ifdef Exist_USB
    int temp = size;
    int num = 0;
    do {
        for(int i = 0;i< ENDP2_IN_SIZE;i++) HIDTxBuffer[i] = 0;
        if(temp >= ENDP2_IN_SIZE)
        {
            for(int i = 0;i< ENDP2_IN_SIZE;i++) HIDTxBuffer[i] = data[num + i];
            temp -= ENDP2_IN_SIZE;
            num += ENDP2_IN_SIZE;
        }
        else
        {
            for(int i = 0;i< temp;i++) HIDTxBuffer[i] = data[num + i];
            temp = 0;
        }
        USBD_HID_Data_Updata();                         //发usb
        retval = 0;
    } while (temp);
#endif
    return retval;
}

int USB_Buffer_Receive (uint8_t *data)
{
    int retval = 0;
#ifdef Exist_USB
    if (data == NULL) {
        return retval;
    }
    retval = HID_Data_Buff[0];
    retval <<= 8;
    retval += HID_Data_Buff[1];
    memcpy(data,&HID_Data_Buff[2],retval);
    HID_Data_Buff[0] = 0;
    HID_Data_Buff[1] = 0;
#endif
    return retval;
}

