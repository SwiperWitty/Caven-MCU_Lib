#include "MODE_BZZ.h"

void MODE_BZZ_Init(int Set)
{
#if Exist_BZZ
    
    User_GPIO_config(2,BZZ_IO,1);
    MODE_BZZ_Set_Fun(0);
#endif
}

/*
 * 蜂鸣器必须用NPN管不然给爷死
 */
void MODE_BZZ_Set_Fun(int Set)
{
#if Exist_BZZ
    if(Set)
        User_GPIO_set(2,BZZ_IO,1);
    else
        User_GPIO_set(2,BZZ_IO,0);
#endif
}


