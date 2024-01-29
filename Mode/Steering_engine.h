#ifndef	__STEERING_ENGINE_H_
#define	__STEERING_ENGINE_H_

#include "Base.h"
#include "Caven_Type.h"

#define Steering_PWM 4
#define	Arr_T4	2000-1
#define Por_T4	(SystemCoreClock / 100000)-1      //144 000 000 / 100000 = 1440

//	T = ((arr + 1) * (por + 1)) / 144MHz = 20MS

typedef struct
{
    void (*Set_Angle)(char Channel,float Angle);
    void (*Set_360_Sport)(char Channel,char Rotation,char Speed,int Time);
}MODE_Steering_Engine_Way;

int MODE_Steering_Engine_Init (int Set);

void MODE_Steering_Engine_Angle_Fun (char Channel,float Angle);
void MODE_Steering_Engine_360_Sport_Fun (char Channel,char Rotation,char Speed,int Time);

#endif
