#include "Mode.h"

struct _Mode_Init Mode_Init;
struct _Mode_Use Mode_Use;    //结构体实体


static void Mode_Use_index(void)		//索引 功能函数 本体
{
    Mode_Use.Debug_Out = NULL;
#ifdef Exist_LCD
    Mode_Use.LCD.Fill = LCD_Fill;

    Mode_Use.LCD.Draw_Point = LCD_Draw_Point;
    Mode_Use.LCD.Draw_Line = LCD_Draw_Line;
    Mode_Use.LCD.Draw_Circle = LCD_Draw_Circle;
    Mode_Use.LCD.Draw_Rectangle = LCD_Draw_Rectangle;
    
    Mode_Use.LCD.Show_String = LCD_Show_String;
    Mode_Use.LCD.Show_Chinese = LCD_Show_Chinese;
    Mode_Use.LCD.Show_Picture = LCD_Show_Picture;

    Mode_Use.LCD.Set_HORIZONTAL = LCD_Set_HORIZONTAL;
#endif

#ifdef Exist_SYS_TIME
    Mode_Use.TIME.Delay_Us = MODE_Delay_Us;
    Mode_Use.TIME.Delay_Ms = MODE_Delay_Ms;
    Mode_Use.TIME.Delay_S = MODE_Delay_S;
	
    Mode_Use.TIME.Get_Watch_Fun = MODE_TIME_Get_Watch_Fun;
    Mode_Use.TIME.Get_Date_Fun = MODE_TIME_Get_Date_Fun;
    Mode_Use.TIME.Get_differ_Fun = MODE_TIME_Get_differ_Fun;
    Mode_Use.TIME.SYNC_TIME_Fun = SYNC_TIME_Fun;

    Mode_Use.TIME.Set_Date_Fun = MODE_TIME_Set_Date_Fun;
    Mode_Use.TIME.Set_Watch_Fun = MODE_TIME_Set_Watch_Fun;

#endif

#ifdef Exist_LED
    Mode_Use.LED.LED_SET = MODE_LED_SET;
    Mode_Use.LED.LED_REG = MODE_LED_REG;

#endif
#ifdef Exist_BZZ
    Mode_Use.BZZ.BZZ_SET = BZZ_SET;
#endif
#ifdef Exist_HC595
    Mode_Use.HC595.Set_DATA = HC595_Set_DATA;
#endif
#ifdef Exist_HC138
    Mode_Use.HC138.Set_DATA = HC138_Set_Data;
#endif
#ifdef Exist_DS18B20
    Mode_Use.DS18B20.Get_Temp = DS18B20_Get_Temp;
#endif

#ifdef Exist_UART
    Mode_Use.UART.Send_Data_Fun = MODE_UART_Send_Data_Fun;
    Mode_Use.UART.Send_String_Fun = MODE_UART_Send_String_Fun;
    Mode_Use.UART.Receive_Bind_Fun = MODE_UART_Receive_Bind_Fun;
#endif

#ifdef Exist_USB
    Mode_Use.USB_HID.Keyboard_Send_Data = USB_Keyboard_Send_Data;
    Mode_Use.USB_HID.Keyboard_Send_String = USB_Keyboard_Send_String;
    Mode_Use.USB_HID.Custom_Send = USB_Buffer_send;
    Mode_Use.USB_HID.Custom_Receive = USB_Buffer_Receive;
#endif

#ifdef Exist_KEY
    Mode_Use.KEY.K_State = KEY_State;
#endif
#ifdef Exist_Ultrasonic
    Mode_Use.Ultrasonic.Distance = Distance;
#endif

#ifdef Exist_Voice
    Mode_Use.MP3.Voice = Voice;
#endif
#ifdef Exist_Steering_Engine
    Mode_Use.Steering_Engine.Set_Angle = Steering_Engine_Angle;
    Mode_Use.Steering_Engine.Set_360_Sport = Steering_Engine_360_Sport;
#endif  

#ifdef Exist_ADC
    Mode_Use.USER_ADC.WAY_Get_Temperature = ADC_Get_Temperature;   //方法
    Mode_Use.USER_ADC.WAY_Conversion_Vol = ADC_Conversion_Vol; 
    Mode_Use.USER_ADC.WAY_Get_List = ADC_Get_List;
    
    Mode_Use.USER_ADC.DATA_List = ADC1_valuetab_list;      //数据
#endif

}   //  Mode_Use 

void Mode_Index(void) 
{
    Mode_Use_index();
    
#ifdef Exist_SYS_TIME
    Mode_Init.TIME = MODE_TIME_Init;
#endif
    
#ifdef Exist_UART
    Mode_Init.UART = MODE_UART_Init;
#endif
#ifdef Exist_USB
    Mode_Init.USB = USB_User_init;
#endif
    
#ifdef Exist_LED
    Mode_Init.LED = MODE_LED_Init;
#endif
#ifdef Exist_LCD
    Mode_Init.LCD = LCD_Init;
#endif
#ifdef Exist_BZZ
    Mode_Init.BZZ = BZZ_Init;
#endif

#ifdef Exist_HC595
    Mode_Init.HC_595 = HC595_Init;
#endif
#ifdef Exist_HC138
    Mode_Init.HC_138 = HC138_Init;
#endif
#ifdef Exist_DS18B20
    Mode_Init.DS18B20 = DS18B20_Init;
#endif

#ifdef Exist_KEY
    Mode_Init.KEY = KEY_Init;
#endif
#ifdef Exist_Ultrasonic
    Mode_Init.Ultrasonic = Ultrasonic_Init;
#endif

#ifdef Exist_Voice
    Mode_Init.Voice = Voice_Init;
#endif
#ifdef Exist_Steering_Engine
    Mode_Init.Steering_Engine = Steering_Engine_Init;
#endif  

#ifdef Exist_ADC
    Mode_Init.User_ADC = ADC_Start_Init;
#endif

}   //  Mode_Index

//--------------------------------//

void Debug_Out(const char *String)              //选一个通信接口为Debug
{
#ifdef Exist_UART
    if (DEBUG_OUT == m_UART_CH1)
    {
        MODE_UART_Send_String_Fun(m_UART_CH1,String);
    }

#endif
}
