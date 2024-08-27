#ifndef _MODE_RTC8564_H_
#define _MODE_RTC8564_H_

#include "Base.h"
#include "Caven_Type.h"

#define RTC8564_ADDR    0x51

int MODE_RTC8564_Init (int Set);
int MODE_RTC8564_Read_time (uint32_t *sec);
int MODE_RTC8564_Write_time (uint32_t sec);

#endif
