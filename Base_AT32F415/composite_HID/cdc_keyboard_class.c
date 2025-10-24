/**
  **************************************************************************
  * @file     cdc_keyboard_class.c
  * @version  v2.0.7
  * @date     2022-08-16
  * @brief    usb cdc and keyboard class type
  **************************************************************************  */

#include "usbd_core.h"
#include "cdc_keyboard_class.h"
#include "cdc_keyboard_desc.h"

/** @addtogroup AT32F415_middlewares_usbd_class
  * @{
  */

/** @defgroup USB_cdc_keyboard_class
  * @brief usb device class composite cdc and keyboard
  * @{
  */

/** @defgroup USB_cdc_keyboard_class_private_functions
  * @{
  */

static usb_sts_type class_init_handler(void *udev);
static usb_sts_type class_clear_handler(void *udev);
static usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup);
static usb_sts_type class_ept0_tx_handler(void *udev);
static usb_sts_type class_ept0_rx_handler(void *udev);
static usb_sts_type class_in_handler(void *udev, uint8_t ept_num);
static usb_sts_type class_out_handler(void *udev, uint8_t ept_num);
static usb_sts_type class_sof_handler(void *udev);
static usb_sts_type class_event_handler(void *udev, usbd_event_type event);
static usb_sts_type cdc_class_setup_handler(void *udev, usb_setup_type *setup);
static usb_sts_type keyboard_class_setup_handler(void *udev, usb_setup_type *setup);


HID_compilation_type HID_compilation;

#define SHIFT 0x80
const static unsigned char _asciimap[128] =
{
  0x00,// NUL
  0x00,// SOH
  0x00,// STX
  0x00,// ETX
  0x00,// EOT
  0x00,// ENQ
  0x00,// ACK
  0x00,// BEL
  0x2A,// BS Backspace
  0x2B,// TAB	Tab
  0x28,// LF	Enter
  0x00,// VT
  0x00,// FF
  0x00,// CR
  0x00,// SO
  0x00,// SI
  0x00,// DEL
  0x00,// DC1
  0x00,// DC2
  0x00,// DC3
  0x00,// DC4
  0x00,// NAK
  0x00,// SYN
  0x00,// ETB
  0x00,// CAN
  0x00,// EM
  0x00,// SUB
  0x00,// ESC
  0x00,// FS
  0x00,// GS
  0x00,// RS
  0x00,// US

  0x2C,//  ' '
  0x1E|SHIFT,// !
  0x34|SHIFT,// "
  0x20|SHIFT,// #
  0x21|SHIFT,// $
  0x22|SHIFT,// %
  0x24|SHIFT,// &
  0x34,      // '
  0x26|SHIFT,// (
  0x27|SHIFT,// )
  0x25|SHIFT,// *
  0x2E|SHIFT,// +
  0x36,// ,
  0x2D,// -
  0x37,// .
  0x38,// /
  0x27,// 0
  0x1E,// 1
  0x1F,// 2
  0x20,// 3
  0x21,// 4
  0x22,// 5
  0x23,// 6
  0x24,// 7
  0x25,// 8
  0x26,// 9
  0x33|SHIFT,// :
  0x33,      // ;
  0x36|SHIFT,// <
  0x2E,      // =
  0x37|SHIFT,// >
  0x38|SHIFT,// ?
  0x1F|SHIFT,// @
  0x04|SHIFT,// A
  0x05|SHIFT,// B
  0x06|SHIFT,// C
  0x07|SHIFT,// D
  0x08|SHIFT,// E
  0x09|SHIFT,// F
  0x0A|SHIFT,// G
  0x0B|SHIFT,// H
  0x0C|SHIFT,// I
  0x0D|SHIFT,// J
  0x0E|SHIFT,// K
  0x0F|SHIFT,// L
  0x10|SHIFT,// M
  0x11|SHIFT,// N
  0x12|SHIFT,// O
  0x13|SHIFT,// P
  0x14|SHIFT,// Q
  0x15|SHIFT,// R
  0x16|SHIFT,// S
  0x17|SHIFT,// T
  0x18|SHIFT,// U
  0x19|SHIFT,// V
  0x1A|SHIFT,// W
  0x1B|SHIFT,// X
  0x1C|SHIFT,// Y
  0x1D|SHIFT,// Z
  0x2F,      // [
  0x31,      // bslash
  0x30,      // ]
  0x23|SHIFT,// ^
  0x2D|SHIFT,// _
  0x35,      // `
  0x04,      // a
  0x05,      // b
  0x06,      // c
  0x07,      // d
  0x08,      // e
  0x09,      // f
  0x0A,      // g
  0x0B,      // h
  0x0C,      // i
  0x0D,      // j
  0x0E,      // k
  0x0F,      // l
  0x10,      // m
  0x11,      // n
  0x12,      // o
  0x13,      // p
  0x14,      // q
  0x15,      // r
  0x16,      // s
  0x17,      // t
  0x18,      // u
  0x19,      // v
  0x1A,      // w
  0x1B,      // x
  0x1C,      // y
  0x1D,      // z
  0x2f|SHIFT,//
  0x31|SHIFT,// |
  0x30|SHIFT,// }
  0x35|SHIFT,// ~
  0	// DEL
};


/* usb device class handler */
usbd_class_handler cdc_keyboard_class_handler =
{
  class_init_handler,
  class_clear_handler,
  class_setup_handler,
  class_ept0_tx_handler,
  class_ept0_rx_handler,
  class_in_handler,
  class_out_handler,
  class_sof_handler,
  class_event_handler,
  &HID_compilation,
};

/**
  * @brief  initialize usb endpoint
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type
  */
static usb_sts_type class_init_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
//    custom_hid_type *pcshid = (custom_hid_type *)pudev->class_handler->pdata;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata; 

  /* open in endpoint */
//  usbd_ept_open(pudev, USBD_VCPKYBRD_CDC_INT_EPT, EPT_INT_TYPE, USBD_VCPKYBRD_CMD_MAXPACKET_SIZE);    //!!!!

  /* open in endpoint */
  usbd_ept_open(pudev, USBD_VCPKYBRD_CDC_BULK_IN_EPT, EPT_BULK_TYPE, USBD_VCPKYBRD_IN_MAXPACKET_SIZE);

  /* open out endpoint */
  usbd_ept_open(pudev, USBD_VCPKYBRD_CDC_BULK_OUT_EPT, EPT_BULK_TYPE, USBD_VCPKYBRD_OUT_MAXPACKET_SIZE);

  /* open hid in endpoint */
  usbd_ept_open(pudev, USBD_VCPKYBRD_HID_IN_EPT, EPT_INT_TYPE, USBD_VCPKYBRD_IN_MAXPACKET_SIZE);

  /* set out endpoint to receive status */
  usbd_ept_recv(pudev, USBD_VCPKYBRD_CDC_BULK_OUT_EPT, HID->g_rx_buff, USBD_VCPKYBRD_OUT_MAXPACKET_SIZE);

  HID->g_custom_tx_completed = 0;
  HID->g_keyboard_tx_completed = 0;

  return status;
}

/**
  * @brief  clear endpoint or other state
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type
  */
static usb_sts_type class_clear_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;

  /* close in endpoint */
//  usbd_ept_close(pudev, USBD_VCPKYBRD_CDC_INT_EPT);       //!!!!

  /* close in endpoint */
  usbd_ept_close(pudev, USBD_VCPKYBRD_CDC_BULK_IN_EPT);

  /* close out endpoint */
  usbd_ept_close(pudev, USBD_VCPKYBRD_CDC_BULK_OUT_EPT);

  /* close in endpoint */
  usbd_ept_close(pudev, USBD_VCPKYBRD_HID_IN_EPT);

  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: to the structure of usbd_core_type
  * @param  setup: setup packet
  * @retval status of usb_sts_type
  */
static usb_sts_type class_setup_handler(void *udev, usb_setup_type *setup)          //启动
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
    custom_hid_type *pcshid = (custom_hid_type *)pudev->class_handler->pdata;         //!!!!    custom_hid_type
  switch(setup->bmRequestType & USB_REQ_RECIPIENT_MASK)
  {
    case USB_REQ_RECIPIENT_INTERFACE:
      if(setup->wIndex == VCPKYBRD_KEYBOARD_INTERFACE)      //接口1
      {
        keyboard_class_setup_handler(udev, setup);
      }
      else
      {
        cdc_class_setup_handler(udev, setup);                    //接口0
      }
      break;
    case USB_REQ_RECIPIENT_ENDPOINT:
      break;
    
    default:
      break;
  }
  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)      //!!!!
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case HID_REQ_SET_PROTOCOL:
          pcshid->hid_protocol = (uint8_t)setup->wValue;
          break;
        case HID_REQ_GET_PROTOCOL:
          usbd_ctrl_send(pudev, (uint8_t *)&pcshid->hid_protocol, 1);
          break;
        case HID_REQ_SET_IDLE:
          pcshid->hid_set_idle = (uint8_t)(setup->wValue >> 8);
          break;
        case HID_REQ_GET_IDLE:
          usbd_ctrl_send(pudev, (uint8_t *)&pcshid->hid_set_idle, 1);
          break;
        case HID_REQ_SET_REPORT:
          pcshid->hid_state = HID_REQ_SET_REPORT;
          usbd_ctrl_recv(pudev, pcshid->hid_set_report, setup->wLength);
          break;
		case HID_REQ_GET_REPORT:
//          usbd_ctrl_send(pudev, pcshid->hid_get_report, setup->wLength);
          break;
        default:
          usbd_ctrl_unsupport(pudev);
          break;
      }
      break;
  }
  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: to the structure of usbd_core_type
  * @param  setup: setup packet
  * @retval status of usb_sts_type
  */
static usb_sts_type cdc_class_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata;
  uint16_t len;
  uint8_t *buf;
    
  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */  
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case HID_REQ_SET_PROTOCOL:
          HID->hid_protocol = (uint8_t)setup->wValue;
          break;
        case HID_REQ_GET_PROTOCOL:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->hid_protocol, 1);
          break;
        case HID_REQ_SET_IDLE:
          HID->hid_set_idle = (uint8_t)(setup->wValue >> 8);
          break;
        case HID_REQ_GET_IDLE:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->hid_set_idle, 1);
          break;
        case HID_REQ_SET_REPORT:
//          vcpkybrd->hid_state = HID_REQ_SET_REPORT;
//          usbd_ctrl_recv(pudev, vcpkybrd->hid_set_report, setup->wLength);
          break;
        default:
          usbd_ctrl_unsupport(pudev);
          break;
      }
      break;
    /* standard request */
    case USB_REQ_TYPE_STANDARD:
      switch(setup->bRequest)
      {
        case USB_STD_REQ_GET_DESCRIPTOR:
          if(setup->wValue >> 8 == HID_REPORT_DESC)
          {
            len = MIN(USBD_SIZE_Report_HID_DESC, setup->wLength);
            buf = (uint8_t *)USBD_HIDRepDesc;                    //报告描述符-数据
          }
          else if(setup->wValue >> 8 == HID_DESCRIPTOR_TYPE)
          {
            len = MIN(9, setup->wLength);
            buf = (uint8_t *)g_vcpkybrd_hid_usb_desc;               //why?
          }
          usbd_ctrl_send(pudev, (uint8_t *)buf, len);
          break;
        case USB_STD_REQ_GET_INTERFACE:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->alt_setting, 1);
          break;
        case USB_STD_REQ_SET_INTERFACE:
          HID->alt_setting = setup->wValue;
          break;
        default:
          break;
      }
      break;
    default:
      usbd_ctrl_unsupport(pudev);
      break;
  }
  return status;
}

/**
  * @brief  usb device class setup request handler
  * @param  udev: to the structure of usbd_core_type
  * @param  setup: setup packet
  * @retval status of usb_sts_type
  */
static usb_sts_type keyboard_class_setup_handler(void *udev, usb_setup_type *setup)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata;
  uint16_t len;
  uint8_t *buf;

  switch(setup->bmRequestType & USB_REQ_TYPE_RESERVED)
  {
    /* class request */
    case USB_REQ_TYPE_CLASS:
      switch(setup->bRequest)
      {
        case HID_REQ_SET_PROTOCOL:
          HID->hid_protocol = (uint8_t)setup->wValue;
          break;
        case HID_REQ_GET_PROTOCOL:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->hid_protocol, 1);
          break;
        case HID_REQ_SET_IDLE:
          HID->hid_set_idle = (uint8_t)(setup->wValue >> 8);
          break;
        case HID_REQ_GET_IDLE:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->hid_set_idle, 1);
          break;
        case HID_REQ_SET_REPORT:
//          vcpkybrd->hid_state = HID_REQ_SET_REPORT;
//          usbd_ctrl_recv(pudev, vcpkybrd->hid_set_report, setup->wLength);
          break;
        default:
          usbd_ctrl_unsupport(pudev);
          break;
      }
      break;
    /* standard request */
    case USB_REQ_TYPE_STANDARD:
      switch(setup->bRequest)
      {
        case USB_STD_REQ_GET_DESCRIPTOR:
          if(setup->wValue >> 8 == HID_REPORT_DESC)
          {
            len = MIN(USBD_SIZE_KEYBOARD_DESC, setup->wLength);
            buf = (uint8_t *)USBD_KeyRepDesc;                    //报告描述符-键盘
          }
          else if(setup->wValue >> 8 == HID_DESCRIPTOR_TYPE)
          {
            len = MIN(9, setup->wLength);
            buf = (uint8_t *)g_vcpkybrd_hid_usb_desc;
          }
          usbd_ctrl_send(pudev, (uint8_t *)buf, len);
          break;
        case USB_STD_REQ_GET_INTERFACE:
          usbd_ctrl_send(pudev, (uint8_t *)&HID->alt_setting, 1);
          break;
        case USB_STD_REQ_SET_INTERFACE:
          HID->alt_setting = setup->wValue;
          break;
        default:
          break;
      }
      break;
    default:
      usbd_ctrl_unsupport(pudev);
      break;
  }
  return status;
}

/**
  * @brief  usb device class endpoint 0 in status stage complete
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type
  */
static usb_sts_type class_ept0_tx_handler(void *udev)
{
  usb_sts_type status = USB_OK;

  /* ...user code... */

  return status;
}

/**
  * @brief  usb device class endpoint 0 out status stage complete
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type
  */
static usb_sts_type class_ept0_rx_handler(void *udev)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata; 
  uint32_t recv_len = usbd_get_recv_len(pudev, 0);
  /* ...user code... */

  if( HID->hid_state == HID_REQ_SET_REPORT)     //使用端点0接收
  {
    /* hid buffer process */
//    usb_hid_buf_process(udev, pcshid->hid_set_report, recv_len);
    HID->hid_state = 0;
  }
  return status;
}

/**
  * @brief  usb device class transmision complete handler
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type
  */
static usb_sts_type class_in_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata; 

  /* ...user code...
    trans next packet data
  */
  if((ept_num & 0x7F) == (USBD_VCPKYBRD_CDC_BULK_IN_EPT & 0x7F))
  {
    HID->g_custom_tx_completed = 0;                           
  }
  if((ept_num & 0x7F) == (USBD_VCPKYBRD_HID_IN_EPT & 0x7F))
  {
    HID->g_keyboard_tx_completed = 0;
  }

  return status;
}

/**
  * @brief  usb device class endpoint receive data
  * @param  udev: to the structure of usbd_core_type
  * @param  ept_num: endpoint number
  * @retval status of usb_sts_type
  */
static usb_sts_type class_out_handler(void *udev, uint8_t ept_num)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *HID = (HID_compilation_type *)pudev->class_handler->pdata;

  /* get endpoint receive data length  */
  HID->g_rxlen = usbd_get_recv_len(pudev, ept_num);
	for(int i = 0;i < HID->g_rxlen;i++)
	{
		USB_HID_Callback_Fun (&HID->g_rx_buff[i]);
	}
  usbd_ept_recv(pudev, USBD_VCPKYBRD_CDC_BULK_OUT_EPT, HID->g_rx_buff, USBD_VCPKYBRD_OUT_MAXPACKET_SIZE);
  return status;
}

/**
  * @brief  usb device class sof handler
  * @param  udev: to the structure of usbd_core_type
  * @retval status of usb_sts_type
  */
static usb_sts_type class_sof_handler(void *udev)
{
  usb_sts_type status = USB_OK;

  /* ...user code... */

  return status;
}

/**
  * @brief  usb device class event handler
  * @param  udev: to the structure of usbd_core_type
  * @param  event: usb device event
  * @retval status of usb_sts_type
  */
static usb_sts_type class_event_handler(void *udev, usbd_event_type event)
{
  usb_sts_type status = USB_OK;
  switch(event)
  {
    case USBD_RESET_EVENT:

      /* ...user code... */

      break;
    case USBD_SUSPEND_EVENT:

      /* ...user code... */

      break;
    case USBD_WAKEUP_EVENT:
      /* ...user code... */

      break;
    default:
      break;
  }
  return status;
}

/**
  * @brief  usb device class send report
  * @param  udev: to the structure of usbd_core_type
  * @param  report: report buffer
  * @param  len: report length
  * @retval status of usb_sts_type
  */
usb_sts_type rec_hid_class_send_report(void *udev, uint8_t *report, uint16_t len)
{
  usb_sts_type status = USB_FAIL;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *pcshid = (HID_compilation_type *)pudev->class_handler->pdata;

  if(usbd_connect_state_get(pudev) == USB_CONN_STATE_CONFIGURED && pcshid->g_custom_tx_completed == 0)
  {
    pcshid->g_custom_tx_completed = 1;
    usbd_ept_send(pudev, USBD_VCPKYBRD_CDC_BULK_IN_EPT, report, len);
    status = USB_OK;
  }
  return status;
}

usb_sts_type usb_vcpkybrd_class_send_report(void *udev, uint8_t *report, uint16_t len)
{
  usb_sts_type status = USB_OK;
  usbd_core_type *pudev = (usbd_core_type *)udev;
  HID_compilation_type *pcshid = (HID_compilation_type *)pudev->class_handler->pdata;
  if(usbd_connect_state_get(pudev) == USB_CONN_STATE_CONFIGURED && pcshid->g_keyboard_tx_completed == 0)
  {
	  usbd_ept_send(pudev, USBD_VCPKYBRD_HID_IN_EPT, report, len);        // 键盘
		pcshid->g_keyboard_tx_completed = 1;
  }
  return status;
}


/**
  * @brief  usb keyoard send char
  * @param  udev: to the structure of usbd_core_type
  * @param  op: operation
  * @retval none
  */
void usb_vcpkybrd_keyboard_send_char(void *udev, uint8_t ascii_code)
{
  uint8_t key_data = 0;
  static uint8_t temp = 0;
  static uint8_t keyboard_buf[8] = {0, 0, 6, 0, 0, 0, 0, 0};
	usbd_core_type *pudev = (usbd_core_type *)udev;
  
  if(ascii_code > 128)
  {
    ascii_code = 0;
  }
  else
  {
    ascii_code = _asciimap[ascii_code];
    if(!ascii_code)
    {
      ascii_code = 0;
    }

    if(ascii_code & SHIFT)
    {
      key_data = 0x2;
      ascii_code &= 0x7F;
    }
  }
	HID_compilation_type *pcshid = (HID_compilation_type *)pudev->class_handler->pdata;
	if(usbd_connect_state_get(pudev) == USB_CONN_STATE_CONFIGURED)
	{
		while(pcshid->g_keyboard_tx_completed == 1)
		{
			usb_delay_ms(100);
		};
		if((temp == ascii_code) && (ascii_code != 0))
		{
			keyboard_buf[0] = 0;
			keyboard_buf[2] = 0;
			usb_vcpkybrd_class_send_report(udev, keyboard_buf, 8);
		}
		else
		{
			keyboard_buf[0] = key_data;
			keyboard_buf[2] = ascii_code;
			usb_vcpkybrd_class_send_report(udev, keyboard_buf, 8);
		}
	}
  // delay
}


