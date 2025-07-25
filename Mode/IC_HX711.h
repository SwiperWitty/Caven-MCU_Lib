#ifndef _IC__HX711_H_
#define _IC__HX711_H_

#include "stdint.h"
#include "Items.h"
#if Exist_HX711
	#include "Base.h"
#endif
#include "Caven_Type.h"

/*
*/


typedef struct
{
    void (*Set_DATA) (int Data);
}MODE_HC138_Way;

int MODE_IC_HX711_Init(int gpiox,int SCK_pin,int DOUT_pin,int Set); 

#endif
