/*
 * Base_USB.c
 *
 *  Created on: 2024年6月3日
 */
#include "Base_USB.h"
#include "ch32v30x_usbfs_device.h"
#include "ch32v30x_usb.h"
// #include "usbd_compatibility_hid.h"

#ifdef Exist_USB
uint8_t usb_init_flag = 0;

extern void SYS_Base_Delay(int time, int speed);
void usb_delay (int times)
{
    SYS_Base_Delay(times, 10000);
}
#endif

int USB_User_init (int Set)
{
    int retval = 0;
#ifdef Exist_USB
    if(usb_init_flag)
    {
        return retval;
    }
    // usb buff
    RingBuffer_Comm.LoadPtr = 0;
    RingBuffer_Comm.StopFlag = 0;
    RingBuffer_Comm.DealPtr = 0;
    RingBuffer_Comm.RemainPack = 0;
    for(uint16_t i=0; i < DEF_Ring_Buffer_Max_Blks; i++)
    {
        RingBuffer_Comm.PackLen[i] = 0;
    }
    if (SET) {
        USBFS_RCC_Init();
        USBFS_Device_Init(ENABLE);
        NVIC_EnableIRQ(USBFS_IRQn);
    }
    else {
        USBFS_Device_Init(DISABLE);
        NVIC_DisableIRQ(USBFS_IRQn);
    }

    usb_init_flag = SET;
#endif
    return retval;
}

int USB_User_State_Get  (void)
{
    int retval = 0;
#ifdef Exist_USB
    if (usb_init_flag) {
        retval = USBFS_DevEnumStatus;
    }
#endif
    return retval;
}

//keyboard
int USB_Keyboard_Send_Data (char *data, int size)
{
    int retval = 1;
#ifdef Exist_USB
    int temp = size;
    int num = 0;
    if (USB_User_State_Get() == 0) {
        return retval;
    }
    do {
        // for(int i = 0;i< ENDP1_IN_SIZE;i++) HIDKey[i] = 0;
        // if(temp >= ENDP1_IN_SIZE)
        // {
        //     for(int i = 0;i< ENDP1_IN_SIZE;i++) HIDKey[i] = data[num + i];
        //     temp -= ENDP1_IN_SIZE;
        //     num += ENDP1_IN_SIZE;
        // }
        // else
        // {
        //     for(int i = 0;i< temp;i++) HIDKey[i] = data[num + i];
        //     temp = 0;
        // }
        // USBD_HID_Keyboard_Updata();
        // retval = 0;
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


int USB_Send_Data (const uint8_t *data,int size)
{
    int retval = 1;
#ifdef Exist_USB
    int temp = size;
    int num = 0,temp_run = 0,temp_time = 0;
    if (USB_User_State_Get() == 0) {
        return retval;
    }
    do {
        while(USBFS_Endp_Busy[DEF_UEP2])
        {
            usb_delay(5);
            temp_time ++;
            if(temp_time > 10)
            {
                return retval = temp_time;
            }
        }

        /* Calculate the length of this packet upload via USB. */
        num = temp;
        if (num >= (DEF_USBD_FS_PACK_SIZE - 1))
        {
            num = (DEF_USBD_FS_PACK_SIZE - 1);
        }
        /* Upload packet via USB. */
        if (num)
        {
            USBFSD->UEP2_DMA = (uint16_t)(uint32_t)USBFS_EP2_Buf;
            USBFS_EP2_Buf[0] = num;
            memset(USBFS_EP2_Buf,0,DEF_USB_EP2_FS_SIZE);
            memcpy(USBFS_EP2_Buf + 1,data + temp_run,num);
            USBFSD->UEP2_TX_LEN = num + 1;
            USBFS_Endp_Busy[DEF_UEP2] = 1;
            temp_run += num;
            USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;    // Start Upload
            temp -= num;
        }
        temp_time = 0;
        retval = 0;
    } while (temp);
#endif
    return retval;
}

int USB_Callback_Bind (D_pFun USB_Callback_pFun)
{
    int retval = 0;
#ifdef Exist_USB
    usb_callbk_pFun = USB_Callback_pFun;
#endif
    return retval;
}
