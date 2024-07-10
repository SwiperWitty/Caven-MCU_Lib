#include "MODE_Button.h"

void MODE_Button_Init (char Channel,int Set)
{
#ifdef Exist_BUTTON
    switch (Channel) {
        case 0:
            break;
        case 1:
            User_GPIO_config(3,BUTTON_IO,0);
            break;
        case 2:

            break;
        default:
            break;
    }
#endif
}

u32 MODE_Get_Button_State_Fun (char Channel)
{
    u32 temp = 0;
	#ifdef Exist_BUTTON
    switch (Channel) {
        case 0:
            break;
        case 1:
            if (User_GPIO_get(3,BUTTON_IO)) {
                temp |= (0x01 << Channel);
            }
            break;
        default:
            break;
    }
	#endif
    return temp;
}
