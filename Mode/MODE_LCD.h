#ifndef _MODE_LCD_H_
#define _MODE_LCD_H_

#include "Base.h"
#include "Caven_Type.h"

/*
        【Mode】      ————C语言环境即可运行
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[LCD]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

#define USE_HORIZONTAL 0 // 设置默认横屏或者竖屏显示 0或1为竖屏 2或3为横屏
#define LCD_W 240        // X MAX 240
#define LCD_H 240        // Y MAX 240

#define LCD_CMD 0	// 写命令
#define LCD_DATA 1	// 写数据

extern U16  LCD_Back_Color; // 背景色
extern U16  LCD_Word_Color; // 字体色
extern U8   LCD_HORIZONTAL; // 设置横屏或者竖屏

//画笔颜色
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
#define LCD_BROWN            0XBC40 //棕色
#define LCD_BRRED            0XFC07 //棕红色
#define LCD_GRAY             0X8430 //灰色
#define LCD_DARKBLUE         0X01CF //深蓝色
#define LCD_LIGHTBLUE        0X7D7C //浅蓝色
#define LCD_GRAYBLUE         0X5458 //灰蓝色
#define LCD_LIGHTGREEN       0X841F //浅绿色
#define LCD_LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define LCD_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LCD_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

typedef struct
{
    void (*Fill_pFun)(U16 x_sta, U16 y_sta, U16 x_end, U16 y_end, U16 color); //指定区域填充颜色

    void (*Draw_Point_pFun)(U16 x, U16 y, U16 color);                       //在指定位置画一个点
    void (*Draw_Line_pFun)(U16 x1, U16 y1, U16 x2, U16 y2, U16 color);      //在指定位置画一条线
    void (*Draw_Circle_pFun)(U16 x0, U16 y0, char r, U16 color);            //在指定位置画一个圆
    void (*Draw_Rectangle_pFun)(U16 x1, U16 y1, U16 x2, U16 y2, U16 color); //在指定位置画一个矩形

    void (*Show_String_pFun)(U16 x, U16 y, const char *p, U16 coloer, U16 b_coloer, char Size);       //显示字符串
    void (*Show_Chinese_pFun)(U16 x, U16 y, char *s, U16 coloer, U16 b_coloer, char Size, char mode); //显示汉字串
    void (*Show_Picture_pFun)(U16 x, U16 y, U16 length, U16 width, const unsigned char pic[]);        //显示图片

    int (*Set_Direction_pFun)(char set);
}MODE_LCD_Way;

void LCD_Fill_Fun (U16 x_sta, U16 y_sta, U16 x_end, U16 y_end, U16 color); //指定区域填充颜色
void LCD_Draw_Point(U16 x, U16 y, U16 color);                         //在指定位置画一个点
void LCD_Draw_Line(U16 x1, U16 y1, U16 x2, U16 y2, U16 color);        //在指定位置画一条线
void LCD_Draw_Circle(U16 x0, U16 y0, char r, U16 color);              //在指定位置画一个圆
void LCD_Draw_Rectangle(U16 x1, U16 y1, U16 x2, U16 y2, U16 color);   //在指定位置画一个矩形

void LCD_Show_String(U16 x, U16 y, const char *p, U16 coloer, U16 b_coloer, char Size);       //显示字符串
void LCD_Show_Chinese(U16 x, U16 y, char *s, U16 coloer, U16 b_coloer, char Size, char mode); //显示汉字串
void LCD_Show_Picture(U16 x, U16 y, U16 length, U16 width, const unsigned char pic[]);        //显示图片

int LCD_Set_HORIZONTAL(char set);

int MODE_LCD_Init(int SET); // LCD初始化


#endif
