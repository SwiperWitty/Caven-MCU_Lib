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
	void (*Calibrate)(u16 known_weight);
    float (*Get_Weight) (u8 times);
}MODE_HX711_Way;

int MODE_IC_HX711_Init(int gpiox,int SCK_pin,int DOUT_pin,int Set);

void MODE_HX711_Calibrate(u16 known_weight);
float MODE_HX711_Get_Weight(u8 times);

#endif
