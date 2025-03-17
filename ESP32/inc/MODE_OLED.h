#ifndef _OLED__H_
#define _OLED__H_

#include "Items.h"
#include "Caven_Type.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define OLED_ADDR 0x78

void OLED_ClearPoint(U8 x,U8 y);
void OLED_ColorTurn(U8 i);
void OLED_DisplayTurn(U8 i);

void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);

void OLED_DrawPoint(U8 x,U8 y,U8 t);
void OLED_DrawLine(U8 x1,U8 y1,U8 x2,U8 y2,U8 mode);
void OLED_DrawCircle(U8 x,U8 y,U8 r);

void OLED_ShowChar6x8(U8 x,U8 y,U8 chr,U8 mode);
void OLED_ShowString(U8 x,U8 y,U8 *chr,U8 size1,U8 mode);
void OLED_ShowNum(U8 x,U8 y,int num,U8 len,U8 size1,U8 mode);
void OLED_ShowChinese(U8 x,U8 y,U8 num,U8 size1,U8 mode);
void OLED_ScrollDisplay(U8 num,U8 space,U8 mode);
void OLED_ShowPicture(U8 x,U8 y,U8 sizex,U8 sizey,U8 BMP[],U8 mode);
void OLED_Init (int Set);

#endif
