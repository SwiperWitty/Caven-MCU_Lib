#ifndef _MODE_BZZ__H_
#define _MODE_BZZ__H_

#include "Base.h"
#include "Caven_Type.h"

/*
        【MODE】      ————C语言环境即可运行



*/

typedef struct
{
    void (*SET_pFun)(int SET);
}MODE_BZZ_Way;

void MODE_BZZ_Init(int SET);
void MODE_BZZ_SET_Fun(int SET);

#endif
