#include "MODE_OLED.h"
#include "oledfont.h" // 字库


uint16_t OLED_W_Max = 0;
uint16_t OLED_H_Max = 0;
int OLED_PicSize = 0;

#ifdef Exist_OLED

uint8_t OLED_GRAM[144][8];
static char OLED_Horizontal = 0;

#endif

#ifdef Exist_OLED
void OLED_WR_CMD(uint8_t data)
{
	char array[4];
	array[0] = 0x00;
	array[1] = data;
	IIC_Send_DATA(OLED_ADDR,array,0,2,1);
}
#endif

/*
	函数说明：在指定位置画点
	入口数据：
			x,y 画点坐标
			color 点的颜色
	返回值：  无
*/
void OLED_Draw_Point(uint16_t x, uint16_t y, uint16_t color)
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
void OLED_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
#ifdef Exist_LCD
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; // 画线起点坐标
	uCol = y1;
	if(LCD_PicSize == 0)
	{
		return;
	}
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
void OLED_Draw_Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	OLED_Draw_Line(x1, y1, x2, y1, color);
	OLED_Draw_Line(x1, y1, x1, y2, color);
	OLED_Draw_Line(x1, y2, x2, y2, color);
	OLED_Draw_Line(x2, y1, x2, y2, color);
}

/*
	函数说明：画圆
	入口数据：
			x0,y0   圆心坐标
			r       半径
			color   圆的颜色
	返回值：  无
*/
void OLED_Draw_Circle(uint16_t x0, uint16_t y0, char r, uint16_t color)
{
	int a, b;
	a = 0;
	b = r;
	if(OLED_PicSize == 0)
	{
		return;
	}
	while (a <= b)
	{
		OLED_Draw_Point(x0 - b, y0 - a, color); // 3
		OLED_Draw_Point(x0 + b, y0 - a, color); // 0
		OLED_Draw_Point(x0 - a, y0 + b, color); // 1
		OLED_Draw_Point(x0 - a, y0 - b, color); // 2
		OLED_Draw_Point(x0 + b, y0 + a, color); // 4
		OLED_Draw_Point(x0 + a, y0 - b, color); // 5
		OLED_Draw_Point(x0 + a, y0 + b, color); // 6
		OLED_Draw_Point(x0 - b, y0 + a, color); // 7
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
void OLED_Show_Char(uint16_t x, uint16_t y, char num, uint16_t fc, uint16_t bc, char sizey, char mode)
{
#ifdef Exist_LCD
	#if LCD_LIB_ASCII
	char temp, sizex, t, m = 0;
	uint16_t i, TypefaceNum; // 一个字符所占字节大小
	uint16_t x0 = x;
	int temp_num;
	if(LCD_PicSize == 0)
	{
		return;
	}
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	temp_num = num - ' ';	// 得到偏移后的值
	s_LCD_Set_Address_pFun(x, y, x + sizex - 1, y + sizey - 1);	// 设置光标位置
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
#endif
}

void OLED_Show_String(uint16_t x, uint16_t y, const char *p, uint16_t fc, uint16_t bc, char sizey)
{
	if (sizey == 16 || sizey == 24 || sizey == 32)
	{
		x *= (sizey / 2);
		y *= (sizey);
		if (x > OLED_W_Max)
		{
			x = OLED_W_Max;
		}
		if (y > OLED_H_Max)
		{
			y = OLED_W_Max;
		}
	}
	else
		return;

	while (*p != '\0')
	{
		OLED_Show_Char(x, y, *p, fc, bc, sizey, 0);
		x += sizey / 2;
		p++;
	}
}

static void OLED_Refresh(void)
{
#ifdef Exist_OLED
	uint16_t i,n,k;
	char array[300];
	for(i=0;i<8;i++)
	{
		OLED_WR_CMD(0xb0+i); // 设置行起始地址
		OLED_WR_CMD(0x00);   // 设置低列起始地址
		OLED_WR_CMD(0x10);   // 设置高列起始地址
		
		k = 0;
		array[k++] = 0x40;
		for(n=0;n<128;n++)
		{
			array[k++] = OLED_GRAM[n][i];
		}
		IIC_Send_DATA(OLED_ADDR,array,0,k,5);
		
	}
#endif
}
/*
	函数说明：在指定区域填充颜色
	入口数据：
			xsta,ysta   起始坐标
			xend,yend   终止坐标
			color       要填充的颜色
	返回值：  无
*/
void OLED_Fill_Fun(uint16_t x_sta, uint16_t y_sta, uint16_t x_end, uint16_t y_end, uint16_t color)
{
#ifdef Exist_OLED
	uint16_t i,n;
	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i]=0;//清除所有数据
		}
	}
	OLED_Refresh();//更新显示
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
void OLED_Show_Picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
#ifdef Exist_LCD
	int temp_run = 0;
	// int temp_num = 0;
	// uint8_t pic_buff[650]; /* 一个y 320 * 2	*/
	uint16_t temp_data;
	if(LCD_PicSize == 0)
	{
		return;
	}
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
		// temp_num = 0;
	}
#endif
}

/*
	符合则设置方向
	不符合则理解为查询方向
	此函数实现在其他文件
*/
int OLED_Set_Horizontal(char set)
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
    if (LCD_PicSize) {
	    LCD_Horizontal = 0;
    }
#endif
	return retval;
}

int MODE_OLED_Init(int set)
{
	int retval = 0;
#ifdef Exist_OLED

	// 1
	OLED_Set_Horizontal(OLED_Horizontal);
	OLED_W_Max = 128;
	OLED_H_Max = 64;
	OLED_PicSize = OLED_W_Max * OLED_H_Max;
	// 2
	IIC_Start_Init(set);
	OLED_WR_CMD(0xAE); //--turn off oled panel
	OLED_WR_CMD(0x00); //---set low column address
	OLED_WR_CMD(0x10); //---set high column address
	OLED_WR_CMD(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_CMD(0x81); //--set contrast control register
	OLED_WR_CMD(0xCF); // Set SEG Output Current Brightness
	OLED_WR_CMD(0xA1); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_CMD(0xC8); //Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_CMD(0xA6); //--set normal display
	OLED_WR_CMD(0xA8); //--set multiplex ratio(1 to 64)
	OLED_WR_CMD(0x3f); //--1/64 duty
	OLED_WR_CMD(0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_CMD(0x00); //-not offset
	OLED_WR_CMD(0xd5); //--set display clock divide ratio/oscillator frequency
	OLED_WR_CMD(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_CMD(0xD9); //--set pre-charge period
	OLED_WR_CMD(0xF1); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_CMD(0xDA); //--set com pins hardware configuration
	OLED_WR_CMD(0x12); 
	OLED_WR_CMD(0xDB); //--set vcomh
	OLED_WR_CMD(0x30); //Set VCOM Deselect Level
	OLED_WR_CMD(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_CMD(0x02); //
	OLED_WR_CMD(0x8D); //--set Charge Pump enable/disable
	OLED_WR_CMD(0x14); //--set(0x10) disable
//	MODE_st7789_dever_Init(set);
	// 3
	OLED_Fill_Fun(0, 0, OLED_W_Max, OLED_H_Max, 0);
	OLED_WR_CMD(0xAF);	// 点亮屏幕
//	OLED_WR_CMD(0xAE);	// 关闭屏幕

#endif
	return retval;
}
