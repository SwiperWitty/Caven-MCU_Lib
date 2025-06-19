#ifndef _MODE_BZZ__H_
#define _MODE_BZZ__H_

#include "stdint.h"
#include "Items.h"
#if Exist_BZZ
	#include "Base.h"
#endif
#include "Caven_Type.h"

/*
        【MODE】      ————C语言环境即可运行



*/
#ifdef Exist_BZZ

#define BZZ_IO  5

typedef struct
{
    void (*Set_pFun)(int Set);
}MODE_BZZ_Way;

#endif
void MODE_BZZ_Init(int Set);
void MODE_BZZ_Set_Fun(int Set);

#endif
