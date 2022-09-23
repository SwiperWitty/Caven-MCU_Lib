#include "mode.h"

struct _Mode_Init Mode_Init;
struct _Mode_User Mode_User;    //结构体实体


static void Mode_User_index(void)		//索引 功能函数 本体
{
    Mode_User.Debug_Out = Debug_Out;
#ifdef Exist_LCD
    Mode_User.LCD.Fill = LCD_Fill;

    Mode_User.LCD.Draw_Point = LCD_Draw_Point;
    Mode_User.LCD.Draw_Line = LCD_Draw_Line;
    Mode_User.LCD.Draw_Circle = LCD_Draw_Circle;
    Mode_User.LCD.Draw_Rectangle = LCD_Draw_Rectangle;
    
    Mode_User.LCD.Show_String = LCD_Show_String;
    Mode_User.LCD.Show_Chinese = LCD_Show_Chinese;
    Mode_User.LCD.Show_Picture = LCD_Show_Picture;
#endif

#ifdef Exist_SYS_TIME
    Mode_User.Delay.Delay_us = SYS_Delay_us;
    Mode_User.Delay.Delay_ms = SYS_Delay_ms;
    Mode_User.Delay.Delay_S = SYS_Delay_S;
	
	Mode_User.TIME.Get_TIME = Get_TIME;
	Mode_User.TIME.Set_TIME = Set_TIME;
	Mode_User.TIME.Get_Lose_Tiem = Get_Lose_Tiem;
#endif

#ifdef Exist_LED
    Mode_User.LED.LED_SET = LED_SET;
    Mode_User.LED.LED_REG = LED_REG;

#endif
#ifdef Exist_BZZ
    Mode_User.BZZ.BZZ_SET = BZZ_SET;
#endif
#ifdef Exist_HC595
    Mode_User.HC595.Set_DATA = Set_DATA;
#endif

#ifdef Exist_UART
    Mode_User.UART.Send_Data = UART_Send_Data;
    Mode_User.UART.Send_String = UART_Send_String;
#endif

#ifdef Exist_KEY
    Mode_User.KEY.KEY_State = KEY_State;
#endif
#ifdef Exist_Ultrasonic
    Mode_User.Ultrasonic.Distance = Distance;
#endif

}   //  Mode_User

void Mode_Index(void)
{
    Mode_User_index();

#ifdef Exist_LCD
    Mode_Init.LCD = LCD_Init;
#endif
#ifdef Exist_SYS_TIME
    Mode_Init.TIME = Time_Init;
#endif
#ifdef Exist_UART
    Mode_Init.UART = Uart_Init;
#endif
#ifdef Exist_LED
    Mode_Init.LED = LED_Init;
#endif
#ifdef Exist_BZZ
    Mode_Init.BZZ = BZZ_Init;
#endif
#ifdef Exist_HC595
    Mode_Init.HC_595 = HC595_Init;
#endif

#ifdef Exist_KEY
    Mode_Init.KEY = KEY_Init;
#endif
#ifdef Exist_Ultrasonic
    Mode_Init.Ultrasonic = Ultrasonic_Init;
#endif
}   //  Mode_Index

//--------------------------------//

void Debug_Out(const char *String)              //选一个通信接口为Debug
{
#ifdef Exist_UART
    UART_Send_String(DEBUG_OUT,String);
#endif
}
