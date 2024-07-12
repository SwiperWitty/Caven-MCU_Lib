#include "MODE_LCD.h"
#include "lcdfont.h" // 字库

uint16_t LCD_Back_Color = 0xFFFB; // 背景色
uint16_t LCD_Word_Color = LCD_GRAY;

uint16_t LCD_W_Max = 0;
uint16_t LCD_H_Max = 0;
int LCD_PicSize = 0;

static char LCD_Target_Model = 0;
static uint8_t LCD_Horizontal = 0;

// Very important 
static void (*s_LCD_Set_Address_pFun)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void (*s_LCD_WR_Data_pFun)(uint16_t data);
static void (*s_LCD_Send_Data_pFun)(uint8_t *data, int num);


/*
	函数说明：在指定位置画点
	入口数据：
			x,y 画点坐标
			color 点的颜色
	返回值：  无
*/
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color)
{
#ifdef Exist_LCD
	s_LCD_Set_Address_pFun(x, y, x, y); // 设置光标位置
	s_LCD_WR_Data_pFun(color);
#endif
}

/*
	函数说明：画线
	入口数据：
			x1,y1   起始坐标
			x2,y2   终止坐标
			color   线的颜色
	返回值：  无
*/
void LCD_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
#ifdef Exist_LCD
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; // 画线起点坐标
	uCol = y1;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_Draw_Point(uRow, uCol, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
#endif
}

/*
	函数说明：画矩形
	入口数据：
			x1,y1   起始坐标
			x2,y2   终止坐标
			color   矩形的颜色
	返回值：  无
*/
void LCD_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	LCD_Draw_Line(x1, y1, x2, y1, color);
	LCD_Draw_Line(x1, y1, x1, y2, color);
	LCD_Draw_Line(x1, y2, x2, y2, color);
	LCD_Draw_Line(x2, y1, x2, y2, color);
}

/*
	函数说明：画圆
	入口数据：
			x0,y0   圆心坐标
			r       半径
			color   圆的颜色
	返回值：  无
*/
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, char r, uint16_t color)
{
	int a, b;
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_Draw_Point(x0 - b, y0 - a, color); // 3
		LCD_Draw_Point(x0 + b, y0 - a, color); // 0
		LCD_Draw_Point(x0 - a, y0 + b, color); // 1
		LCD_Draw_Point(x0 - a, y0 - b, color); // 2
		LCD_Draw_Point(x0 + b, y0 + a, color); // 4
		LCD_Draw_Point(x0 + a, y0 - b, color); // 5
		LCD_Draw_Point(x0 + a, y0 + b, color); // 6
		LCD_Draw_Point(x0 - b, y0 + a, color); // 7
		a++;
		if ((a * a + b * b) > (r * r)) // 判断要画的点是否过远
		{
			b--;
		}
	}
}

/*
	函数说明：显示单个字符
	入口数据：
			x,y显示坐标
			num 要显示的字符
			fc 字的颜色
			bc 字的背景色
			sizey 字号
			mode:  0非叠加模式  1叠加模式
	返回值：  无
*/
void LCD_Show_Char(uint16_t x, uint16_t y, char num, uint16_t fc, uint16_t bc, char sizey, char mode)
{
#ifdef LCD_LIB_ASCII
	char temp, sizex, t, m = 0;
	uint16_t i, TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	int temp_num = num - ' ';											// 得到偏移后的值
	s_LCD_Set_Address_pFun(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 16)
		{
			temp = ascii_1608[temp_num][i];
		} // 调用8x16字体
		else if (sizey == 24)
		{
			temp = ascii_2412[temp_num][i];
		} // 调用12x24字体
		else if (sizey == 32)
		{
			temp = ascii_3216[temp_num][i];
		} // 调用16x32字体
		else
		{
			return;
		}
		for (t = 0; t < 8; t++)
		{
			if (!mode) // 非叠加模式
			{
				if (temp & (0x01 << t))
				{
					s_LCD_WR_Data_pFun(fc);
				}
				else
				{
					s_LCD_WR_Data_pFun(bc);
				}
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else // 叠加模式
			{
				if (temp & (0x01 << t))
				{
					LCD_Draw_Point(x, y, fc);
				} // 画一个点
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
#endif
}

void LCD_Show_String(uint16_t x, uint16_t y, const char *p, uint16_t fc, uint16_t bc, char sizey)
{
	if (sizey == 16 || sizey == 24 || sizey == 32)
	{
		x *= (sizey / 2);
		y *= (sizey);
		if (x > LCD_W_Max)
		{
			x = LCD_W_Max;
		}
		if (y > LCD_H_Max)
		{
			y = LCD_W_Max;
		}
	}
	else
		return;

	while (*p != '\0')
	{
		LCD_Show_Char(x, y, *p, fc, bc, sizey, 0);
		x += sizey / 2;
		p++;
	}
}

/*
	函数说明：在指定区域填充颜色
	入口数据：
			xsta,ysta   起始坐标
			xend,yend   终止坐标
			color       要填充的颜色
	返回值：  无
*/
void LCD_Fill_Fun(uint16_t x_sta, uint16_t y_sta, uint16_t x_end, uint16_t y_end, uint16_t color)
{
#ifdef Exist_LCD
	uint8_t pic_buff[650]; /* 一个y 320 * 2	*/
	uint16_t x_len = (x_end - x_sta);
	uint16_t y_len = (y_end - y_sta);
	int temp_num = 0;
	uint8_t color_l = 0xff & color;
	uint8_t color_h = (color >> 8) & 0xff;
	s_LCD_Set_Address_pFun(x_sta, y_sta, x_end - 1, y_end - 1); // 设置显示范围

	for (int i = 0; i < y_len; i++)
	{
		pic_buff[temp_num++] = color_h;
		pic_buff[temp_num++] = color_l;
	}
	for (; x_len > 0; x_len--)
	{
		s_LCD_Send_Data_pFun(pic_buff, y_len * 2);
	}

#endif
}

/*
	函数说明：显示图片
	入口数据：
			x,y起点坐标
			length 图片长度
			width  图片宽度
			pic[]  图片数组
	返回值：  无
*/
void LCD_Show_Picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
#ifdef Exist_LCD
	int temp_num = 0;
	int temp_run = 0;
	uint8_t pic_buff[650]; /* 一个y 320 * 2	*/
	uint16_t temp_data;

	s_LCD_Set_Address_pFun(x, y, (x + length - 1), (y + width - 1));

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < width; j++)
		{
			temp_data = pic[temp_run++];
			temp_data <<= 8;
			temp_data |= pic[temp_run++];
			s_LCD_WR_Data_pFun(temp_data);
			// pic_buff[temp_num++] = pic[temp_run++];
			// pic_buff[temp_num++] = pic[temp_run++];
		}
		// s_LCD_Send_Data_pFun(pic_buff, width * 2);
		temp_num = 0;
	}
#endif
}

/*
	先行
	此函数实现在其他文件
*/
int LCD_Set_TargetModel(char set)
{
	int retval = 0;
#ifdef Exist_LCD
	if ((set > 0 && set < 10)) // MODE_
	{
		s_LCD_Set_Address_pFun = NULL;
		s_LCD_WR_Data_pFun = NULL;
		s_LCD_Send_Data_pFun = NULL;

		retval = LCD_Target_Model;
	}
	else if (set > 10 && set < 20) // MODE_st7789
	{
		s_LCD_Set_Address_pFun = MODE_st7789_dever_Set_Address;
		s_LCD_WR_Data_pFun = MODE_st7789_dever_WR_Data;
		s_LCD_Send_Data_pFun = MODE_st7789_dever_Send_Data;
		LCD_Target_Model = MODE_st7789_dever_Set_TargetModel(set);
		retval = LCD_Target_Model;
	}
	else
	{
		retval = 0;
	}
#endif
	return retval;
}

/*
	符合则设置方向
	不符合则理解为查询方向
	此函数实现在其他文件
*/
int LCD_Set_Horizontal(char set)
{
	int retval = 0;
#ifdef Exist_LCD
	if ((LCD_Target_Model > 0 && LCD_Target_Model < 10))
	{

	}
	else if (LCD_Target_Model > 10 && LCD_Target_Model < 20)
	{
		LCD_Horizontal = MODE_st7789_dever_Set_Horizontal(set);
	}
	else
	{
	}
// set max 
	switch (LCD_Target_Model)
	{
	case m_LCD_TYPE_1_14:
	{
		if (LCD_Horizontal < 2)
		{
		}
		else
		{
		}
	}
	break;
	case m_LCD_TYPE_1_28:
	{
		LCD_W_Max = 240;
		LCD_H_Max = 240;
	}
	break;
	case m_LCD_TYPE_1_30:
	{
		LCD_W_Max = 240;
		LCD_H_Max = 240;
	}
	break;
	case m_LCD_TYPE_1_69:
	{
		if (LCD_Horizontal < 2)
		{
			LCD_W_Max = 240;
			LCD_H_Max = 280;
		}
		else
		{
			LCD_W_Max = 280;
			LCD_H_Max = 240;
		}
	}
	break;
	case m_LCD_TYPE_1_90:
	{
		if (LCD_Horizontal < 2)
		{
			LCD_W_Max = 170;
			LCD_H_Max = 320;
		}
		else
		{
			LCD_W_Max = 320;
			LCD_H_Max = 170;
		}
	}
	break;
	default:
		break;
	}
	LCD_PicSize = LCD_W_Max * LCD_H_Max;
#endif
	return retval;
}

int MODE_LCD_Init(int set)
{
	int retval = 0;
#ifdef Exist_LCD
	// 1
	if (LCD_Target_Model == 0)
	{
		LCD_Set_TargetModel(m_LCD_TYPE_1_30);
	}
	// 2
	LCD_Set_Horizontal(LCD_Horizontal);
	// 3
	if ((LCD_Target_Model > 0 && LCD_Target_Model < 10))
	{
		retval = 0;
	}
	else if (LCD_Target_Model > 10 && LCD_Target_Model < 20)
	{
		retval = MODE_st7789_dever_Init(set);
	}
	else
	{
		retval = 1;
	}
	// 4
	LCD_Fill_Fun(0, 0, LCD_W_Max, LCD_H_Max, LCD_Back_Color);
#endif
	return retval;
}
