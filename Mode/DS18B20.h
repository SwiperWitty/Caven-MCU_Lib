#ifndef _DS18B20__H_
#define _DS18B20__H_

#include "Base.h"

/*
	// 30min  set 60max
	// 1-L  15min read (60min next)

*/

typedef struct
{
    float (*Get_Temp) (void);
}MODE_DS18B20_Way;

int MODE_DS18B20_Init (int Set);

float DS18B20_Get_Temp_Fun (void);


#endif
