#include "key.h"

void KEY_Init (char Channel,int SET)
{
    switch (Channel) {
        case 0:
            break;
        case 1:
        #ifdef Exist_KEY
            KEY_GPIO_Init(SET);
        #endif
            break;
        case 2:
        #ifdef Exist_KEY

        #endif
            break;
        default:
            break;
    }
}

U8 KEY_State (char Channel)
{
    U8 temp = 1;
	#ifdef Exist_KEY
    switch (Channel) {
        case 0:
            break;
        case 1:
            temp = KEY_IN();
            break;
        default:
            break;
    }
	#endif
    return temp;
}
