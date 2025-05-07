#ifndef _MODE_LED__H_
#define _MODE_LED__H_

#include "Base.h"
#include "Caven_Type.h"

/*
 * WS2812
 * LED
*/
#ifdef Exist_LED

#define LED_IO  4

typedef struct
{
    void (*SET_pFun)(char Channel,int Set);
    void (*RGB_pFun)(Caven_Color_Type Color,int Set);
}MODE_LED_Way;
#endif

void MODE_LED_Init(int Set);

void MODE_LED_SET_Fun(char Channel,int Set);
void MODE_LED_RGB_Fun(Caven_Color_Type Color,int Set);

#endif
