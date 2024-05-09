/*
 * caven_gui.h
 *
 *
 *
 */

#ifndef CAVEN_GUI_H_
#define CAVEN_GUI_H_

#include "Mode.h"
#include "math.h"

typedef void (*draw_pixel_pFun)(U16 x, U16 y, U16 color);

void Caven_GUI_draw_pixel_bind(draw_pixel_pFun Fun);
int Caven_GUI_Draw_Circle(U16 x0, U16 y0, U16 radius, char wide, char percent, U16 color);

#endif
