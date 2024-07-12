#ifndef _MODE_LCD_H_
#define _MODE_LCD_H_

#include "MODE_st7789_dever.h"
#include "Caven_Type.h"

/*

*/

extern uint16_t  LCD_Back_Color; // 背景色
extern uint16_t  LCD_Word_Color; // 字体色

extern uint16_t LCD_W_Max;
extern uint16_t LCD_H_Max;
extern int LCD_PicSize;

#ifndef LVGL_LIB
    #define LCD_LIB_ASCII
#endif

// 画笔颜色
#define LCD_WHITE            0xFFFF
#define LCD_BLACK            0x0000
#define LCD_BLUE             0x001F
#define LCD_BRED             0XF81F
#define LCD_GRED             0XFFE0
#define LCD_GBLUE            0X07FF
#define LCD_RED              0xF800
#define LCD_MAGENTA          0xF81F
#define LCD_GREEN            0x07E0
#define LCD_CYAN             0x7FFF
#define LCD_YELLOW           0xFFE0
#define LCD_BROWN            0XBC40 // 棕色
#define LCD_BRRED            0XFC07 // 棕红色
#define LCD_GRAY             0X8430 // 灰色
#define LCD_DARKBLUE         0X01CF // 深蓝色
#define LCD_LIGHTBLUE        0X7D7C // 浅蓝色
#define LCD_GRAYBLUE         0X5458 // 灰蓝色
#define LCD_LIGHTGREEN       0X841F // 浅绿色
#define LCD_LGRAY            0XC618 // 浅灰色(PANNEL),窗体背景色
#define LCD_LGRAYBLUE        0XA651 // 浅灰蓝色(中间层颜色)
#define LCD_LBBLUE           0X2B12 // 浅棕蓝色(选择条目的反色)

typedef enum
{
    //
    m_LCD_TYPE_1_14 = 11,   // 
    m_LCD_TYPE_1_28,        // 圆屏
    m_LCD_TYPE_1_30,        // 方屏
    m_LCD_TYPE_1_69,        // 曲屏
    m_LCD_TYPE_1_90,        // 
    m_LCD_TYPE_2_40,        //
}LCD_TYPE_mType;


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
    int (*Set_TargetModel_pFun)(char set);
}MODE_LCD_Way;

void LCD_Fill_Fun (uint16_t x_sta, uint16_t y_sta, uint16_t x_end, uint16_t y_end, uint16_t color);     // 指定区域填充颜色
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);                                            // 在指定位置画一个点
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                 // 在指定位置画一条线
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, char r, uint16_t color);                                 // 在指定位置画一个圆
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);            // 在指定位置画一个矩形

void LCD_Show_String(uint16_t x, uint16_t y, const char *p, uint16_t coloer, uint16_t b_coloer, char sizey);    // 显示字符串
void LCD_Show_Picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]);            // 显示图片

int LCD_Set_TargetModel(char set);
int LCD_Set_Horizontal(char set);

int MODE_LCD_Init(int set);


#endif
