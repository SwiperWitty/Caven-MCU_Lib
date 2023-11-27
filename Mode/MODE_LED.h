#ifndef _MODE_LED__H_
#define _MODE_LED__H_

#include "Base.h"
#include "Caven_Type.h"

/*
 * WS2812
 * LED
*/
typedef struct
{
    void (*SET_pFun)(char Channel,int SET);
    void (*RGB_pFun)(Caven_Color_Type Color,int SET);
}MODE_LED_Way;

void MODE_LED_Init(int SET);

void MODE_LED_SET_Fun(char Channel,int SET);
void MODE_LED_RGB_Fun(Caven_Color_Type Color,int SET);

#endif
