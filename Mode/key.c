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

char KEY_State (char Channel)
{
    char temp = 1;
    switch (Channel) {
        case 0:
            break;
        case 1:
        #ifdef Exist_KEY
            temp = KEY_Read();
        #endif
            break;
        default:
            break;
    }
    return temp;
}
