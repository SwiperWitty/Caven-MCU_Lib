#include "Mode.h"

#ifdef STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "stdarg.h"
#endif

struct _Mode_Init Mode_Init;
struct _Mode_Use Mode_Use;          // 结构体实体

static void Mode_Use_index(void)    // 索引 功能函数 本体
{
    Mode_Use.Debug_Out_Way = Debug_Out;
#if Exist_LCD
    Mode_Use.LCD.Fill_pFun = LCD_Fill_Fun;

    Mode_Use.LCD.Draw_Point_pFun = LCD_Draw_Point;
    Mode_Use.LCD.Draw_Line_pFun = LCD_Draw_Line;
    Mode_Use.LCD.Draw_Circle_pFun = LCD_Draw_Circle;
    Mode_Use.LCD.Draw_Rectangle_pFun = LCD_Draw_Rectangle;
    
    Mode_Use.LCD.Show_String_pFun = LCD_Show_String;
    Mode_Use.LCD.Show_Picture_pFun = LCD_Show_Picture;

    Mode_Use.LCD.Set_Direction_pFun = LCD_Set_Horizontal;
    Mode_Use.LCD.Set_TargetModel_pFun = LCD_Set_TargetModel;
#endif

#if Exist_OLED
    Mode_Use.OLED.Fill_pFun = OLED_Fill_Fun;
	
    Mode_Use.OLED.Draw_Point_pFun = OLED_Draw_Point;
    Mode_Use.OLED.Draw_Line_pFun = OLED_Draw_Line;
    Mode_Use.OLED.Draw_Circle_pFun = OLED_Draw_Circle;
    Mode_Use.OLED.Draw_Rectangle_pFun = OLED_Draw_Rectangle;

    Mode_Use.OLED.Show_String_pFun = OLED_Show_String;

    Mode_Use.OLED.Set_Direction_pFun = OLED_Set_Horizontal_addr;
    Mode_Use.OLED.Refresh = OLED_Refresh;
#endif

#if Exist_SYS_TIME
    Mode_Use.TIME.Delay_Us = MODE_Delay_Us;
    Mode_Use.TIME.Delay_Ms = MODE_Delay_Ms;
    Mode_Use.TIME.Delay_S = MODE_Delay_S;
	
    Mode_Use.TIME.Set_BaseTIME_pFun = MODE_TIME_Set_BaseTIME;
    Mode_Use.TIME.Get_BaseTIME_pFun = MODE_TIME_Get_BaseTIME;
    Mode_Use.TIME.Get_Date_pFun = MODE_TIME_Get_Date;
#endif

#if Exist_LED
    Mode_Use.LED.RGB_pFun = MODE_LED_RGB_Fun;
    Mode_Use.LED.Set_pFun = MODE_LED_Set_Fun;

#endif
#if Exist_BZZ
    Mode_Use.BZZ.Set_pFun = MODE_BZZ_Set_Fun;
#endif
#if Exist_HC595
    Mode_Use.HC595.Set_DATA = HC595_Set_DATA_Fun;
#endif
#if Exist_HC138
    Mode_Use.HC138.Set_DATA = HC138_Set_Data_Fun;
#endif
#if Exist_DS18B20
    Mode_Use.DS18B20.Get_Temp = MODE_DS18B20_Get_Temp_Fun;
#endif
#if Exist_DHT11
    Mode_Use.DHT11.Get_data = MODE_DHT11_Get_data_Fun;
#endif
#if Exist_HX711
	Mode_Use.HX711.Calibrate = MODE_HX711_Calibrate;
	Mode_Use.HX711.Get_Weight = MODE_HX711_Get_Weight;
#endif

#if Exist_UART
	Mode_Use.UART.Send_Data_pFun = MODE_UART_DMA_Send_Data_Fun;
    Mode_Use.UART.Send_String_pFun = MODE_UART_Send_String_Fun;
    Mode_Use.UART.Receive_Bind_pFun = MODE_UART_Receive_Bind_Fun;
#endif

#if Exist_USB
    Mode_Use.USB_HID.Send_Data = USB_Send_Data;
    Mode_Use.USB_HID.RX_Callback_Bind = USB_Callback_Bind;
#endif

#if Exist_BUTTON
    Mode_Use.Button.State_pFun = MODE_Get_Button_State_Fun;
#endif
#if Exist_Ultrasonic
    Mode_Use.Ultrasonic.Distance = MODE_Ultrasonic_Distance;
#endif

#if Exist_Voice
    Mode_Use.MP3.Voice = Voice;
#endif
#if Exist_Steering_Engine
    Mode_Use.Steering_Engine.Set_Angle = MODE_Steering_Engine_Angle_Fun;
    Mode_Use.Steering_Engine.Set_360_Sport = MODE_Steering_Engine_360_Sport_Fun;
    Mode_Use.Steering_Engine.Bind_PWM_Time = MODE_Steering_Engine_Bind_PWM_Time;
#endif  
#if Exist_Motor_BYJ
    Mode_Use.Motor_BYJ.Drive = Motor_BYJ_Drive;
#endif 
#if Exist_Motor_STEP
    Mode_Use.Motor_Step.Drive = Motor_Step_Drive;
#endif 

#if Exist_ADC
    Mode_Use.Base_ADC.Get_MCU_Temperature_pFun = ADC_Get_MCU_Temperature_Fun;
    Mode_Use.Base_ADC.Conversion_Vol_pFun = ADC_Conversion_Vol_Fun; 
    Mode_Use.Base_ADC.Receive_Bind_pFun = ADC_Quick_Get_Bind_Fun;
#endif

}   //  Mode_Use 

void Mode_Index(void) 
{
    Mode_Use_index();
    memset(&Mode_Init,0,sizeof(Mode_Init));
#if Exist_SYS_TIME
    Mode_Init.TIME = MODE_TIME_Init;
#endif

#if Exist_UART
    Mode_Init.UART = MODE_UART_Init;
#endif
#if Exist_USB
    Mode_Init.USB = USB_User_init;      // 来自Base
#endif
    
#if Exist_LED
    Mode_Init.LED = MODE_LED_Init;
#endif
#if Exist_LCD
    Mode_Init.LCD = MODE_LCD_Init;
#endif
#if Exist_OLED
    Mode_Init.OLED = MODE_OLED_Init;
#endif
#if Exist_BZZ
    Mode_Init.BZZ = MODE_BZZ_Init;
#endif

#if Exist_HC595
    Mode_Init.HC_595 = MODE_HC595_Init;
#endif
#if Exist_HC138
    Mode_Init.HC_138 = MODE_HC138_Init;
#endif
#if Exist_DS18B20
    Mode_Init.DS18B20 = MODE_DS18B20_Init;
#endif
#if Exist_DHT11
    Mode_Init.DHT11 = MODE_DHT11_Init;
#endif
#if Exist_HX711
	Mode_Init.HX711 = MODE_IC_HX711_Init;
#endif

#if Exist_BUTTON
    Mode_Init.Button = MODE_Button_Init;
#endif
#if Exist_Ultrasonic
    Mode_Init.Ultrasonic = MODE_Ultrasonic_Init;
#endif

#if Exist_Voice
    Mode_Init.Voice = MODE_Voice_Init;
#endif
#if Exist_Steering_Engine
    Mode_Init.Steering_Engine = MODE_Steering_Engine_Init;
#endif  
#if Exist_Motor_BYJ
    Mode_Init.Motor_BYJ = Motor_BYJ_Init;
#endif 
#if Exist_Motor_STEP
    Mode_Init.Motor_STEP = Motor_Step_Init;
#endif 

#if Exist_ADC
    Mode_Init.Base_ADC = Base_ADC_Init;
#endif

}   // Mode_Index

//--------------------------------//

void Debug_Out(uint8_t *data,int Length)    // 选一个通信接口为Debug
{
#if Exist_UART
    #if DEBUG_CH
    MODE_UART_DMA_Send_Data_Fun(DEBUG_CH, data, Length);
    #endif

#endif
}

// 定义回调函数
char *stb_callback(const char *buf, void *user, int len)
{
#ifdef STB_SPRINTF_IMPLEMENTATION
    stbsp__context *c = (stbsp__context *)user;
    //    (void) sizeof(buf);
	Debug_Out((U8 *)buf,len);
    c->length += len;
    return c->tmp;  // go direct into buffer if you can
#else
	return 0;
#endif
}

// 自定义 ptf 函数
int stb_printf(const char *fmt, ...)
{
#if defined (STB_SPRINTF_IMPLEMENTATION)
    stbsp__context c = {0};
    va_list args;
    va_start(args, fmt);                                  // 启动 va_list
    stbsp_vsprintfcb(stb_callback, &c, c.tmp, fmt, args);  // 调用 stbsp_vsprintfcb
    va_end(args);                                         // 结束 va_list

    return c.length;  // 返回生成的字符长度
#elif defined (NO_SPRINTF)
    return 0;
#else
	return printf(fmt);
#endif
}
