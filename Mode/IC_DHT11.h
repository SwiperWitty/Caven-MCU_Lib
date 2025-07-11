#ifndef _IC_DHT11__H_
#define _IC_DHT11__H_

#include "stdint.h"
#include "Items.h"
#if Exist_DHT11
	#include "Base.h"
#endif

/*

*/


typedef struct
{
    int (*Get_data) (int *temp,int *hum);
}MODE_DHT11_Way;

int MODE_DHT11_Init (int gpiox,int pin,int Set);

int DHT11_Get_data_Fun (int *temp,int *hum);

#endif
