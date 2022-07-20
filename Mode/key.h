#ifndef _KEY__H_
#define _KEY__H_

#include "Base.h"

struct KEY_
{
    char (*KEY_State) (char Channel);
};

void KEY_Init (char Channel,int SET);
char KEY_State (char Channel);


#endif
