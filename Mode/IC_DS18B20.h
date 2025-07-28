#ifndef _IC__DS18B20__H_
#define _IC__DS18B20__H_

#include "stdint.h"
#include "Items.h"
#if Exist_DS18B20
	#include "Base.h"
#endif

/*
*/


typedef struct
{
    float (*Get_Temp) (void);
}MODE_DS18B20_Way;

int MODE_DS18B20_Init (int gpiox,int pin,int Set);

float MODE_DS18B20_Get_Temp_Fun (void);


#endif
