#include "MODE_BZZ.h"

void MODE_BZZ_Init(int Set)
{
#if Exist_BZZ
    
    User_GPIO_config(2,BZZ_IO,1);
    MODE_BZZ_SET_Fun(DISABLE);
#endif
}

/*
 * 蜂鸣器必须用NPN管不然给爷死
 */
void MODE_BZZ_SET_Fun(int Set)
{
#if Exist_BZZ
    if(SET)
        User_GPIO_set(2,BZZ_IO,1);
    else
        User_GPIO_set(2,BZZ_IO,0);
#endif
}


