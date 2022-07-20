#ifndef __LCD_H_
#define __LCD_H_

#include "Base.h"
#include "Caven.h"

/*
        【Mode】      ————C语言环境即可运行
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[LCD]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

#define LCD_Speed   1       //写数据

#define USE_HORIZONTAL 0	//设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
#define LCD_W 240			//X MAX 240
#define LCD_H 240			//Y MAX 240


#define LCD_CMD  0	//写命令
#define LCD_DATA 1	//写数据

extern  u16 BACK_COLOR;   //背景色

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

struct LCD_
{
    void (*Fill)(u16 x_sta,u16 y_sta,u16 x_end,u16 y_end,u16 color);//指定区域填充颜色

    void (*Draw_Point)(u16 x,u16 y,u16 color);//在指定位置画一个点
    void (*Draw_Line)(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线
    void (*Draw_Circle)(u16 x0,u16 y0,char r,u16 color);//在指定位置画一个圆
    void (*Draw_Rectangle)(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形
    
    void (*Show_String)(u16 x,u16 y,const char *p,u16 coloer,u16 b_coloer,char Size);//显示字符串
    void (*Show_Chinese)(u16 x,u16 y,char *s,u16 coloer,u16 b_coloer,char Size,char mode);//显示汉字串
    void (*Show_Picture)(u16 x,u16 y,u16 length,u16 width,const unsigned char pic[]);//显示图片
};

void LCD_Fill(u16 x_sta,u16 y_sta,u16 x_end,u16 y_end,u16 color);//指定区域填充颜色
void LCD_Draw_Point(u16 x,u16 y,u16 color);//在指定位置画一个点
void LCD_Draw_Line(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线
void LCD_Draw_Circle(u16 x0,u16 y0,char r,u16 color);//在指定位置画一个圆
void LCD_Draw_Rectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形

void LCD_Show_String(u16 x,u16 y,const char *p,u16 coloer,u16 b_coloer,char Size);//显示字符串
void LCD_Show_Chinese(u16 x,u16 y,char *s,u16 coloer,u16 b_coloer,char Size,char mode);//显示汉字串
void LCD_Show_Picture(u16 x,u16 y,u16 length,u16 width,const unsigned char pic[]);//显示图片

void LCD_Init(int SET);//LCD初始化

#endif





