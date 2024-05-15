#include "caven_gui.h"

static draw_pixel_pFun Caven_draw_pixel_pFun = NULL;

void Caven_GUI_draw_pixel_bind(draw_pixel_pFun Fun)
{
    if (Fun != NULL)
    {
        Caven_draw_pixel_pFun = Fun;
    }
}


/*
    返回值是中间点((X<<8) + Y)
*/
int Caven_GUI_Draw_Circle(U16 x0, U16 y0, U16 radius, char wide, char percent, U16 color)
{
    int retval = 0;
    int a, b;
    int temp_num;
    int temp_num_b;
    int temp_run;
    char subzone;

    u8 pic_array[256][2];
    U16 show_x, show_y;
    U16 x_point = 0;

    U16 middle_point = 0, middle_maxpointx, middle_maxpointy;
    U16 Outside_sin_max, Outside_cos_max;

    float f_temp_percent;
    float f_temp_num;

    if (Caven_draw_pixel_pFun == NULL)
    {
        return retval;
    }
    if (percent <= 0)
    {
        return retval;
    }

    subzone = percent / 25 + 1;
    f_temp_num = percent % 25;
    f_temp_percent = (f_temp_num / 25) * 90;         // 变成角度
    f_temp_num = (f_temp_percent / 180) * 3.1415926; //

    if (wide % 2)
    {
        middle_point = (wide + 1) / 2;
        middle_point += radius;
    }
    do
    {
        a = 0;
        x_point = 0;

        b = radius;
        temp_run = 0;
        temp_num = b * b;
        temp_num_b = b * b;
        Outside_sin_max = Caven_math_approximate_float(sinf(f_temp_num) * (radius));
        Outside_cos_max = Caven_math_approximate_float(cosf(f_temp_num) * (radius));
        while (a <= b) // 画圆（弧）
        {
            pic_array[temp_run][0] = b;
            pic_array[temp_run++][1] = a;
            pic_array[temp_run][0] = a;
            pic_array[temp_run++][1] = b;
            a++;
            if ((a * a + temp_num_b) > temp_num) // 判断要画的点是否过远
            {
                b--;
                temp_num_b = b * b;
            }
        }

        if (subzone > 1)
        {
            for (int i = 0; i < temp_run; i++)
            { // 1
                show_x = x0 + pic_array[i][0];
                show_y = y0 - pic_array[i][1];
                Caven_draw_pixel_pFun(show_x, show_y, color);
            }
        }
        else if (subzone == 1)
        {
            x_point = Outside_sin_max + x0;
            middle_maxpointx = 0;
            middle_maxpointy = 0;
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 + pic_array[i][0];
                show_y = y0 - pic_array[i][1];
                if (show_x <= x_point)
                {
                    Caven_draw_pixel_pFun(show_x, show_y, color);
                    if (middle_maxpointx <= show_x) // 取大
                    {
                        middle_maxpointx = show_x;
                        if (middle_maxpointy <= show_y) {
                            middle_maxpointy = show_y;  // 取大，注意初始值
                        }
                    }
                }
            }
        }
        if (subzone > 2)
        {
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 + pic_array[i][0];
                show_y = y0 + pic_array[i][1];
                Caven_draw_pixel_pFun(show_x, show_y, color);
            }
        }
        else if (subzone == 2)
        {
            x_point = Outside_cos_max + x0;
            middle_maxpointx = ~0;
            middle_maxpointy = 0;
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 + pic_array[i][0];
                show_y = y0 + pic_array[i][1];
                if (show_x >= x_point)
                {
                    Caven_draw_pixel_pFun(show_x, show_y, color);
                    if (middle_maxpointx >= show_x) // 取小
                    {
                        middle_maxpointx = show_x;
                        if (middle_maxpointy <= show_y) {
                            middle_maxpointy = show_y;  // 取大，注意初始值
                        }
                    }
                }
            }
        }
        if (subzone > 3)
        {
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 - pic_array[i][0];
                show_y = y0 + pic_array[i][1];
                Caven_draw_pixel_pFun(show_x, show_y, color);
            }
        }
        else if (subzone == 3)
        {
            x_point = x0 - Outside_sin_max;
            middle_maxpointx = ~0;
            middle_maxpointy = ~0;
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 - pic_array[i][0];
                show_y = y0 + pic_array[i][1];
                if (show_x >= x_point)
                {
                    Caven_draw_pixel_pFun(show_x, show_y, color);
                    if (middle_maxpointx >= show_x) // 取小
                    {
                        middle_maxpointx = show_x;
                        if (middle_maxpointy >= show_y) {
                            middle_maxpointy = show_y;  // 取小，注意初始值
                        }
                    }
                }
            }
        }
        if (subzone > 4)
        {
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 - pic_array[i][0];
                show_y = y0 - pic_array[i][1];
                Caven_draw_pixel_pFun(show_x, show_y, color);
            }
        }
        else if (subzone == 4)
        {
            x_point = x0 - Outside_cos_max;
            middle_maxpointx = 0;
            middle_maxpointy = ~0;
            for (int i = 0; i < temp_run; i++)
            {
                show_x = x0 - pic_array[i][0];
                show_y = y0 - pic_array[i][1];
                if (show_x <= x_point)
                {
                    Caven_draw_pixel_pFun(show_x, show_y, color);
                    if (middle_maxpointx <= show_x)
                    {
                        middle_maxpointx = show_x;      // 取大
                        if (middle_maxpointy >= show_y) {
                            middle_maxpointy = show_y;  // 取小，注意初始值
                        }
                    }
                }
            }
        }
        if (radius == middle_point)
        {
            retval = middle_maxpointx;
            retval <<= 8;
            retval += middle_maxpointy;
        }
        radius++;
    } while (wide--);
    return retval;
    //    printf("x : %d %%,quadrant: %d,x: %5.2f -> sin x = %5.3f,cos y = %5.3f \n",percent,quadrant,f_temp_percent,x_point,y_point);
    //    sprintf(send_array,"%3d%% ",percent);
    //    Mode_Use.LCD.Show_String_pFun (8,0,send_array,LCD_BLACK,LCD_Back_Color,24);
}
