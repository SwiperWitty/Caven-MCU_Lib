#include "MODE_OLED.h"
#include "oledfont.h" // 字库


uint16_t OLED_W_Max = 0;
uint16_t OLED_H_Max = 0;
int OLED_PicSize = 0;

#if Exist_OLED

uint8_t OLED_addr = 0x78;		// 0x3c 0x78 
uint8_t OLED_GRAM[144][8];
static char OLED_Horizontal = 0;

#endif

#if Exist_OLED
void OLED_WR_CMD(uint8_t data)
{
	uint8_t array[4];
	array[0] = 0x00;
	array[1] = data;
	if(OLED_PicSize)
	{
		Base_IIC_Send_DATA(OLED_addr,array,0,2,1,0);
	}
}

// 开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_CMD(0x8D);	// 电荷泵使能
	OLED_WR_CMD(0x14);	// 开启电荷泵
	OLED_WR_CMD(0xAF);	// 点亮屏幕
}

// 关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_CMD(0x8D);	// 电荷泵使能
	OLED_WR_CMD(0x10);	// 关闭电荷泵
	OLED_WR_CMD(0xAE);	// 关闭屏幕
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
#if Exist_OLED
	uint8_t i,m,n;
	i =y/8;
	m =y%8;
	n =1<<m;
	if(color)
	{
		OLED_GRAM[x][i] |= n;
	}
	else
	{
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
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
#if Exist_OLED
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // 计算坐标增量
	delta_y = y2 - y1;
	uRow = x1; // 画线起点坐标
	uCol = y1;
	if(OLED_PicSize == 0)
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
		OLED_Draw_Point(uRow, uCol, color); // 画点
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
			sizey 字号 8/12/16/24
			mode:  0非叠加模式  1叠加模式
	返回值：  无
*/
void OLED_Show_Char(uint16_t x, uint16_t y, char num, uint16_t fc, uint16_t bc, char sizey, char mode)
{
#if Exist_OLED
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(sizey == 8)
	{
		size2 = 6;
	}
	else 
	{
		size2 = (sizey/8+((sizey%8)?1:0)) * (sizey/2);  // 得到字体一个字符对应点阵集所占的字节数
	}
	chr1 = num - ' ';  // 计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(sizey==8)	// 调用0806字体
		{
			temp=asc2_0806[chr1][i];
		} 
		else if(sizey==12)	// 调用1206字体
		{
			temp=asc2_1206[chr1][i];
		} 
		else if(sizey==16)	// 调用1608字体
		{
			temp=asc2_1608[chr1][i];
		} 
		else if(sizey==24)	// 调用2412字体
		{
			temp=asc2_2412[chr1][i];
		} 
		else 
		{
			return;
		}
		for(m=0;m<8;m++)
		{
			if(temp&0x01)
			{
				OLED_Draw_Point(x,y,1);
			}
			else 
			{
				OLED_Draw_Point(x,y,0);
			}
			temp >>= 1;
			y++;
		}
		x++;
		if((sizey!=8)&&((x-x0)==sizey/2))
		{
			x=x0;y0=y0+8;
		}
		y=y0;
	}
#endif
}

/*
	x,y都按字体大小自动对齐位置
    sizey 字号 8/12/16/24
*/
void OLED_Show_String(uint16_t x, uint16_t y, const char *p, uint16_t fc, uint16_t bc, char sizey)
{
	int temp_num = 0;
	temp_num = (sizey / 2);
	if (sizey == 8 || sizey == 12 || sizey == 16 || sizey == 24)
	{
		x *= temp_num;
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
		x += temp_num;
		if (x > (OLED_W_Max - temp_num))
		{
			return;
		}
		p++;
	}
}

void OLED_Refresh(void)
{
#if Exist_OLED
	uint16_t i,n,k;
	uint8_t array[300];
	if(OLED_PicSize == 0)
	{
		return;
	}
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
		Base_IIC_Send_DATA(OLED_addr,array,0,k,5,0);
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
#if Exist_OLED
	uint16_t i,n;
	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i] = 0;// 清除所有数据
		}
	}
	OLED_Refresh(); // 更新显示
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
#if Exist_OLED
	int j = 0;
	uint8_t i,n,temp,m;
	uint8_t x0=x,y0=y;
	width = width/8+((width%8)?1:0);
	for(n = 0;n < width;n++)
	{
		for(i = 0;i < length;i++)
		{
			temp = pic[j];
			j++;
			for(m = 0;m < 8;m++)
			{
				if(temp&0x01)
				{
					OLED_Draw_Point(x,y,1);
				}
				else 
				{
					OLED_Draw_Point(x,y,0);
				}
				temp >>= 1;
				y++;
			}
			x++;
			if((x-x0) == length)
			{
				x=x0;
				y0=y0+8;
			}
			y=y0;
		}
	}
#endif
}

/*
	set 0 正常显示
	set 1 反转显示
	addr oled iic addr
*/
int OLED_Set_Horizontal_addr(char set,uint8_t addr)
{
	int retval = 0;
#if Exist_OLED
	OLED_Horizontal = set;
	OLED_addr = addr;
	if(OLED_Horizontal == 0)
	{
		OLED_WR_CMD(0xC8);	// 正常显示
		OLED_WR_CMD(0xA1);
	}
	else
	{
		OLED_WR_CMD(0xC0);	// 反转显示
		OLED_WR_CMD(0xA0);
	}
#endif
	return retval;
}

int MODE_OLED_Init(int set)
{
	int retval = 0;
#if Exist_OLED
	// 1
	OLED_W_Max = 128;
	OLED_H_Max = 64;
	OLED_PicSize = OLED_W_Max * OLED_H_Max;
	// 2
	Base_IIC_Init(set);
	OLED_WR_CMD(0xAE); // --turn off oled panel
	OLED_WR_CMD(0x00); // ---set low column address
	OLED_WR_CMD(0x10); // ---set high column address
	OLED_WR_CMD(0x40); // --set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_CMD(0x81); // --set contrast control register
	OLED_WR_CMD(0xCF); //  Set SEG Output Current Brightness
	OLED_WR_CMD(0xA1); // --Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_CMD(0xC8); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_CMD(0xA6); // --set normal display
	OLED_WR_CMD(0xA8); // --set multiplex ratio(1 to 64)
	OLED_WR_CMD(0x3f); // --1/64 duty
	OLED_WR_CMD(0xD3); // -set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_CMD(0x00); // -not offset
	OLED_WR_CMD(0xd5); // --set display clock divide ratio/oscillator frequency
	OLED_WR_CMD(0x80); // --set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_CMD(0xD9); // --set pre-charge period
	OLED_WR_CMD(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_CMD(0xDA); // --set com pins hardware configuration
	OLED_WR_CMD(0x12); 
	OLED_WR_CMD(0xDB); // --set vcomh
	OLED_WR_CMD(0x30); // Set VCOM Deselect Level
	OLED_WR_CMD(0x20); // -Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_CMD(0x02); //
	OLED_WR_CMD(0x8D); // --set Charge Pump enable/disable
	OLED_WR_CMD(0x14); // --set(0x10) disable
//	MODE_st7789_dever_Init(set);
	// 3
	OLED_Fill_Fun(0, 0, OLED_W_Max, OLED_H_Max, 0);
	OLED_WR_CMD(0xAF);	// 点亮屏幕
//	OLED_WR_CMD(0xAE);	// 关闭屏幕

#endif
	return retval;
}
