#include "Steering_engine.h"

#define Angle_coefficient	1.11


int MODE_Steering_Engine_Init (int Set)
{
	int retval = 1;
#ifdef Exist_Steering_Engine
    TIM4_PWM_Start_Init(Arr_T4,Por_T4,Set);
    MODE_Steering_Engine_Angle_Fun(1,0);
    MODE_Steering_Engine_Angle_Fun(2,0);
    MODE_Steering_Engine_Angle_Fun(3,0);
    MODE_Steering_Engine_Angle_Fun(4,0);    // 默认0°
	retval = 0;
#endif
	return retval;
}

/*
舵机角度计算
x = Angle_coefficient
90° ->  (-90 * x) + 150 = 50  
0°  ->  (0 * x) + 150   = 150 <set angle calculator>    
90° ->  (90 * x) + 150  = 250  
解得 Angle_coefficient = 1.11
*/
#ifdef Exist_Steering_Engine
static int Set_Angle_Cal_180(float Angle)	
{
    int Time_PWM = 150;
	if(Angle <= 90 || Angle >= -90) 
    {
        Time_PWM = (Angle * Angle_coefficient) + 150;       //符合范围
    }
    return Time_PWM;
}
#endif

/*  通道、设置角度  */
void MODE_Steering_Engine_Angle_Fun (char Channel,float Angle)
{
#ifdef Exist_Steering_Engine

    TIM4_PWMx_SetValue(Channel,Set_Angle_Cal_180(Angle));

#endif
}

/*  通道、旋转方向、旋转的速度、保持的时间  */
void MODE_Steering_Engine_360_Sport_Fun (char Channel,char Rotation,char Speed,int Time)
{
#ifdef Exist_Steering_Engine
	int Time_PWM = 150;
	if(Speed > 100)	Speed = 100;
	if(Rotation == 'r' || Rotation == 'R')
	{
		Time_PWM = 150 - (Speed * 0.5);
	}
	else if(Rotation == 'l' || Rotation == 'L')
	{
		Time_PWM = 150 + (Speed * 0.5);
	}
    TIM4_PWMx_SetValue(Channel,Time_PWM);
#endif
}
