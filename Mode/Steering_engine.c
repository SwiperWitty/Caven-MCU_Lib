#include "Steering_engine.h"

#define Angle_coefficient	1.11f

ND_pFun PWM_Time_Control_Fun = NULL;

void MODE_Steering_Engine_Bind_PWM_Time (ND_pFun Fun)
{
#if Exist_Steering_Engine
    PWM_Time_Control_Fun = Fun;
    
#endif
}

/*
    舵机角度计算
    x = Angle_coefficient
    90° ->  (-90 * x) + 150 = 50  
    0°  ->  (0 * x) + 150   = 150 <set angle calculator>    
    90° ->  (90 * x) + 150  = 250  
    解得 Angle_coefficient = 1.11
*/
static int Set_Angle_Cal_180(float Angle)	
{
    int Time_PWM = 150;
	if(Angle <= 90 || Angle >= -90) 
    {
        Time_PWM = (Angle * Angle_coefficient); // 符合范围
        Time_PWM += 150;
    }
    return Time_PWM;
}

int MODE_Steering_Engine_Init (int Set)
{
	int retval = 1;
    
    if(PWM_Time_Control_Fun != NULL)
    {
        int temp_val = Set_Angle_Cal_180(0.0);
        PWM_Time_Control_Fun (1,&temp_val);
        PWM_Time_Control_Fun (2,&temp_val);
        PWM_Time_Control_Fun (3,&temp_val);
        PWM_Time_Control_Fun (4,&temp_val);
    }   
    // 默认0°
	retval = 0;
	return retval;
}

/*  通道、设置角度  */
void MODE_Steering_Engine_Angle_Fun (char Channel,float Angle)
{
#if Exist_Steering_Engine
    int temp_val = 0;
    if(PWM_Time_Control_Fun != NULL)
    {
        temp_val = Set_Angle_Cal_180(Angle);
        PWM_Time_Control_Fun (Channel,&temp_val);
    }
#endif
}

/*  通道、旋转方向、旋转的速度、保持的时间  */
void MODE_Steering_Engine_360_Sport_Fun (char Channel,char Rotation,char Speed,int Time)
{
#if Exist_Steering_Engine
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
    PWM_Time_Control_Fun(Channel,&Time_PWM);
#endif
}
