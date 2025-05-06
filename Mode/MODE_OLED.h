#ifndef _MODE_OLED_H_
#define _MODE_OLED_H_

#include "Base.h"

/*
    
    
*/

#define OLED_ADDR 0x78

extern uint16_t OLED_W_Max;
extern uint16_t OLED_H_Max;
extern int OLED_PicSize;

typedef struct
{
    void (*Fill_pFun)(uint16_t x_sta, uint16_t y_sta, uint16_t x_end, uint16_t y_end, uint16_t color);      // 指定区域填充颜色

    void (*Draw_Point_pFun)(uint16_t x, uint16_t y, uint16_t color);                                        // 在指定位置画一个点
    void (*Draw_Line_pFun)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);             // 在指定位置画一条线
    void (*Draw_Circle_pFun)(uint16_t x0, uint16_t y0, char r, uint16_t color);                             // 在指定位置画一个圆
    void (*Draw_Rectangle_pFun)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);        // 在指定位置画一个矩形

    void (*Show_String_pFun)(uint16_t x, uint16_t y, const char *p, uint16_t coloer, uint16_t b_coloer, char sizey);    // 显示字符串
    void (*Show_Picture_pFun)(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]);            // 显示图片

    int (*Set_Direction_pFun)(char set);
    void (*Refresh) (void);
}MODE_OLED_Way;

void OLED_Fill_Fun (uint16_t x_sta, uint16_t y_sta, uint16_t x_end, uint16_t y_end, uint16_t color);     // 指定区域填充颜色
void OLED_Draw_Point(uint16_t x, uint16_t y, uint16_t color);                                            // 在指定位置画一个点
void OLED_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                 // 在指定位置画一条线
void OLED_Draw_Circle(uint16_t x0, uint16_t y0, char r, uint16_t color);                                 // 在指定位置画一个圆
void OLED_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);            // 在指定位置画一个矩形

void OLED_Show_String(uint16_t x, uint16_t y, const char *p, uint16_t coloer, uint16_t b_coloer, char sizey);    // 显示字符串
//void OLED_Show_Picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]);            // 显示图片

int OLED_Set_Horizontal(char set);
void OLED_Refresh (void);

int MODE_OLED_Init(int set);


#endif
