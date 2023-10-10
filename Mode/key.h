#ifndef _KEY__H_
#define _KEY__H_

#include "Base.h"
#include "Caven_Type.h"

struct KEY_
{
    U8 (*K_State) (char Channel);
};

void KEY_Init (char Channel,int Set);
U8 KEY_State (char Channel);


#endif
