#ifndef	_MODE_STEERING_ENGINE_H_
#define	_MODE_STEERING_ENGINE_H_

#include "Base.h"
#include "Caven_Type.h"


//	T = ((arr + 1) * (por + 1)) / MCU_SYS_FREQ = 20MS
/*
    MODE_Steering_Engine_Bind_PWM_Time 绑定某个TIMX来作为PWM生成器，对应的绑定函数在Base_PWM.h文件
    需要注意，如果这个定时器没有启动过直接调用，会自动启动(Base_PWM不初始化的情况下，MODE_Steering_Engine_Angle_Fun控制函数会自己调对应TIMX初始化满足舵机频率)
    如果被启动过，那么需要Base_PWM的手动重新初始化参数 例如 TIM3_PWM_Start_Init(2000-1,(MCU_SYS_FREQ / 100000)-1,1);
*/

typedef struct
{
    void (*Set_Angle)(char Channel,float Angle);
    void (*Bind_PWM_Time) (ND_pFun Fun);
    void (*Set_360_Sport)(char Channel,char Rotation,char Speed,int Time);
}MODE_Steering_Engine_Way;

int MODE_Steering_Engine_Init (int Set);

void MODE_Steering_Engine_Bind_PWM_Time (ND_pFun Fun);
void MODE_Steering_Engine_Angle_Fun (char Channel,float Angle);
void MODE_Steering_Engine_360_Sport_Fun (char Channel,char Rotation,char Speed,int Time);

#endif
