#ifndef _MOTOR_ENGINE_H__
#define _MOTOR_ENGINE_H__

#include "Items.h"
#if Exist_Motor_Engine
    #include "Base.h"
#endif
/*



*/

#define OUT1 1
#define OUT2 2
#define OUT_ON 1000
#define OUT_OF 0

void Control_Mode_Init (void);
void Control_Mode_OUT(char OUTX,int PWM);

#endif
