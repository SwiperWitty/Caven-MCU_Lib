#include <MODE_BZZ.h>

void MODE_BZZ_Init(int SET)
{
    #ifdef Exist_BZZ
    BZZ_GPIO_Init(SET);
    MODE_BZZ_SET_Fun(DISABLE);
    #endif
}

/*
 * 蜂鸣器必须用NPN管不然给爷死
 */
void MODE_BZZ_SET_Fun(int SET)
{
    #ifdef Exist_BZZ
    if(SET)
        BZZ_H();
    else
        BZZ_L();
    #endif
}


