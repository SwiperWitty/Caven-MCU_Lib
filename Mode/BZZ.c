#include "BZZ.h"

void BZZ_Init(int SET)
{
    #ifdef Exist_BZZ
    BZZ_GPIO_Init(SET);
    #endif
}

void BZZ_SET(int SET)
{
    #ifdef Exist_BZZ
    if(SET)
        BZZ_Clr();
    else
        BZZ_Set();
    #endif
}


