/*
 * Base_USB.c
 *
 *  Created on: 2024年6月3日
 */
#include "Base_USB.h"

#include "usbd_int.h"


#if	USB_MODE == OPEN_0000
#include "custom_hid_desc.h"
#include "custom_hid_class.h"
#elif USB_MODE == OPEN_0001
#include "cdc_class.h"
#include "cdc_desc.h"
#elif USB_MODE == OPEN_0010
#include "cdc_keyboard_class.h"
#include "cdc_keyboard_desc.h"
#endif

#if Exist_USB
otg_core_type otg_core_struct;      // USB全局控制量
static uint16_t USB_Buff_MAX = 64;
static void usb_low_power_wakeup_config(void);
static void usb_clock48m_select(usb_clk48_s clk_s);
static void usb_gpio_config(void);
static void usb_low_power_wakeup_config(void);

extern void SYS_Base_Delay(int time, int speed);
void usb_delay (int times)
{
    SYS_Base_Delay (times, 2000);
}
#endif

int USB_User_init (int Set)
{
    int retval = 0;
#if Exist_USB
    if(SET)
    {
        usb_gpio_config();
        usb_low_power_wakeup_config();

        /* enable otgfs clock */
        crm_periph_clock_enable(OTG_CLOCK, TRUE);
        /* select usb 48m clcok source */
        usb_clock48m_select(USB_CLK_HEXT);

        /* enable otgfs irq */
        nvic_irq_enable(OTG_IRQ, 0, 0);
        otg_core_struct.cfg.vbusig = 1;       // 关掉PA9
        /* init usb */

#if USB_MODE	== OPEN_0000
		usbd_init(&otg_core_struct,
			USB_FULL_SPEED_CORE_ID,
			USB_ID,
			&custom_hid_class_handler,
			&custom_hid_desc_handler
		);
		USB_Buff_MAX = 64;
#elif USB_MODE	== OPEN_0001
		usbd_init(&otg_core_struct,
			USB_FULL_SPEED_CORE_ID,
			USB_ID,
			&cdc_class_handler,
			&cdc_desc_handler
		);
		USB_Buff_MAX = 60;
#elif USB_MODE	== OPEN_0010
		usbd_init(&otg_core_struct,
			USB_FULL_SPEED_CORE_ID,
			USB_ID,
			&cdc_keyboard_class_handler,
			&cdc_keyboard_desc_handler
		);
		USB_Buff_MAX = 64;
#endif
        usb_delay_ms(500);
    }
    else
    {
        crm_periph_clock_enable(OTG_CLOCK, FALSE);
    }
    retval = 1;
#endif
    return retval;
}

#if Exist_USB 
#include "usbd_core.h"
extern D_Callback_pFun USB_HID_Callback_Fun;
void USB_Callback_Bind (D_Callback_pFun USB_Callback_pFun)
{
    USB_HID_Callback_Fun = USB_Callback_pFun;
}
#endif

void USB_Send_Data (uint8_t *data,int size)
{
#if Exist_USB
    
    uint8_t Buffer[64];
    int temp = 0,temp2 = 0,temp3;
	temp = size;
	memset(Buffer,0,USB_Buff_MAX);
    usbd_core_type *pudev = &otg_core_struct.dev; 
    if(usbd_connect_state_get(pudev) == USB_CONN_STATE_CONFIGURED)
	{
		do{
			if((temp - USB_Buff_MAX) > 0)
			{
				memcpy(Buffer,(uint8_t*)data + temp2,USB_Buff_MAX);
				temp2 += USB_Buff_MAX;
				temp3 = USB_Buff_MAX;
				temp -= USB_Buff_MAX;
			}
			else
			{
				memcpy(Buffer,(uint8_t*)data + temp2,temp);
				temp3 = temp;
				temp = 0;
			}
	#if USB_MODE	== OPEN_0000
			custom_hid_type *pcshid = (custom_hid_type *)pudev->class_handler->pdata;
			while(pcshid->send_state == 1)
			{
				usb_delay_ms(1);
			};
			temp3 = USB_Buff_MAX;
			custom_hid_class_send_report(pudev,Buffer, temp3);
	#elif USB_MODE	== OPEN_0001
			cdc_struct_type *pcdc = (cdc_struct_type *)pudev->class_handler->pdata;
			while(pcdc->g_tx_completed == 0)
			{
				usb_delay_ms(1);
			};
			usb_vcp_send_data(pudev,Buffer, temp3);
	#elif USB_MODE	== OPEN_0010
			HID_compilation_type *pcshid = (HID_compilation_type *)pudev->class_handler->pdata;
			while(pcshid->g_custom_tx_completed == 1)
			{
				usb_delay_ms(1);
			};
			temp3 = USB_Buff_MAX;
			rec_hid_class_send_report(pudev,Buffer, temp3);
	#endif
			memset(Buffer,0,USB_Buff_MAX);
		}while(temp > 0);
	}
#endif
}

void USB_RX_Callback_Bind (D_Callback_pFun pFun)
{
	if (pFun != NULL)
	{
		USB_HID_Callback_Fun = pFun;
	}
}

void OTG_IRQ_HANDLER(void)
{
#if Exist_USB
	usbd_irq_handler(&otg_core_struct);
#endif
}

void OTG_WKUP_HANDLER(void)
{
#if Exist_USB
    exint_flag_clear(OTG_WKUP_EXINT_LINE);
#endif
}

/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
#if Exist_USB
	#ifdef USB_LOW_POWER_WAKUP
    exint_init_type exint_init_struct;

    exint_default_para_init(&exint_init_struct);    

    exint_init_struct.line_enable = TRUE;
    exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
    exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
    exint_init(&exint_init_struct);

    nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
	#endif
#endif
}

extern unsigned int system_core_clock;
/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  switch(system_core_clock)
  {
    /* 48MHz */
    case 48000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1);
      break;

    /* 72MHz */
    case 72000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1_5);
      break;

    /* 96MHz */
    case 96000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2);
      break;

    /* 120MHz */
    case 120000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2_5);
      break;

    /* 144MHz */
    case 144000000:
      crm_usb_clock_div_set(CRM_USB_DIV_3);
      break;

    default:
      break;
  }
}

void usb_gpio_config(void)
{
	gpio_init_type gpio_init_struct;

	crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
	gpio_default_para_init(&gpio_init_struct);

	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

#if USB_SOF_OUTPUT_ENABLE
	crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
	gpio_init_struct.gpio_pins = OTG_PIN_SOF;
	gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
#endif

  /* otgfs use vbus pin */

	gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;      //!!!!
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
}

