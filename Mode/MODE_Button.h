#ifndef _MODE_BUTTON_H_
#define _MODE_BUTTON_H_

#include "Base.h"
#include "Caven_Type.h"

typedef struct
{
    u32 (*State_pFun) (char Channel);
}MODE_Button_Way;

void MODE_Button_Init (char Channel,int Set);

u32 MODE_Get_Button_State_Fun (char Channel);


#endif
