#ifndef _MODE_BZZ__H_
#define _MODE_BZZ__H_

#include "Base.h"
#include "Caven_Type.h"

/*
        【MODE】      ————C语言环境即可运行



*/
#ifdef Exist_BZZ

#define BZZ_IO  5

typedef struct
{
    void (*SET_pFun)(int Set);
}MODE_BZZ_Way;

#endif
void MODE_BZZ_Init(int Set);
void MODE_BZZ_SET_Fun(int Set);

#endif
