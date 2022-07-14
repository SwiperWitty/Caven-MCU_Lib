#ifndef _LED__H_
#define _LED__H_

#include "Base.h"


struct LED_
{
    void (*LED_REG)();
    void (*LED_SET)(char Channel,int SET);
};

void LED_Init(int SET);

void LED_REG();
void LED_SET(char Channel,int SET);

#endif
