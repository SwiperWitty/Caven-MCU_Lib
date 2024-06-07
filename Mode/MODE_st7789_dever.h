#ifndef _MODE_ST7789_DEVER_H_
#define _MODE_ST7789_DEVER_H_

#include "Base.h"

/*

*/

int MODE_st7789_dever_Init(int set);

int MODE_st7789_dever_Set_TargetModel(char set);
int MODE_st7789_dever_Set_Horizontal(char set);

void MODE_st7789_dever_Set_Address (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void MODE_st7789_dever_WR_Data (uint16_t data);
void MODE_st7789_dever_Send_Data (uint8_t *data,int num);

#endif
