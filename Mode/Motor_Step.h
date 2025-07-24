#ifndef _MODE_STEP_H__
#define _MODE_STEP_H__

#include "Items.h"
#if Exist_Motor_STEP
	#include "Base.h"
#endif
#include "Caven_Type.h"

/*
    
    
*/

typedef struct
{
    char (*Drive) (char Rotation,char Mode,int Code);
}MODE_Motor_Step_Way;

void Motor_Step_Init(int gpiox,int PUL_pin,int DIR_pin,int Set);
char Motor_Step_Drive(char Rotation,char Mode,int Code);


#endif
