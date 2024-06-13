#ifndef _MODE_RTC8564_H_
#define _MODE_RTC8564_H_

#include "Base.h"
#include "Caven_Type.h"

#define RTC8564_ADDR    0x51

int MODE_RTC8564_Init (int Set);
int MODE_RTC8564_Write_time (U64 sec);
int MODE_RTC8564_Read_time (U64 *sec);

#endif
