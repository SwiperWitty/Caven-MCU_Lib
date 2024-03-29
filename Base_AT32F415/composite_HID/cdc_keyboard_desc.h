/**
  **************************************************************************
  * @file     cdc_keyboard_desc.h
  * @version  v2.0.7
  * @date     2022-08-16
  * @brief    usb cdc and keyboard descriptor header file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __CDC_KEYBOARD_DESC_H
#define __CDC_KEYBOARD_DESC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cdc_keyboard_class.h"
#include "usbd_core.h"

/** @addtogroup AT32F415_middlewares_usbd_class
  * @{
  */

/** @addtogroup USB_cdc_keyboard_desc
  * @{
  */

/** @defgroup USB_cdc_keyboard_desc_definition
  * @{
  */

/**
  * @brief usb bcd number define
  */
#define VCPKYBRD_BCD_NUM                 0x0110

/**
  * @brief usb vendor id and product id define
  */
#define USBD_VCPKYBRD_VENDOR_ID          0x03eb     //VID
#define USBD_VCPKYBRD_PRODUCT_ID         0x2421     //PID


/**
  * @brief usb descriptor size define
  */
#define USBD_VCPKYBRD_CONFIG_DESC_SIZE  82//0X42
#define USBD_SIZE_KEYBOARD_DESC         0x3E
#define USBD_SIZE_Report_HID_DESC       0x22

#define USBD_VCPKYBRD_SIZ_STRING_LANGID           4
#define USBD_VCPKYBRD_SIZ_STRING_SERIAL           0x1A

/**
  * @brief usb string define(vendor, product configuration, interface)
  */
#define USBD_VCPKYBRD_DESC_MANUFACTURER_STRING    "Cavendish"
#define USBD_VCPKYBRD_DESC_PRODUCT_STRING         "Cavendish-3.14"
#define USBD_VCPKYBRD_DESC_CONFIGURATION_STRING   "Composite HID and Keyboard Config"
#define USBD_VCPKYBRD_DESC_INTERFACE_STRING       "HID Interface"

/**
  * @brief usb endpoint interval define
  */
#define VCPKYBRD_HID_BINTERVAL_TIME                0x06

/**
  * @brief usb interface define
  */
//#define VCPKYBRD_CDC_INTERFACE                    0x00
#define VCPKYBRD_CDC_DATA_INTERFACE               0x00      //接口
#define VCPKYBRD_KEYBOARD_INTERFACE               0x01

/**
  * @brief usb mcu id address deine
  */
#define         MCU_ID1                   (0x1FFFF7E8)
#define         MCU_ID2                   (0x1FFFF7EC)
#define         MCU_ID3                   (0x1FFFF7F0)

/**
  * @}
  */
                                    //报告描述符
extern uint8_t USBD_HIDRepDesc[];
extern uint8_t USBD_KeyRepDesc[];

extern uint8_t g_vcpkybrd_hid_usb_desc[9];
extern usbd_desc_handler cdc_keyboard_desc_handler;     //结构体


/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif
