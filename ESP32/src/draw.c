#include"draw.h"


/*
    ESP_LOGE - 错误（最低）0
    ESP_LOGW - 警告
    ESP_LOGI - 信息
    ESP_LOGD - 调试
    ESP_LOGV - 详细（最高）
*/

static const char *TAG = "draw";
static char image[240][240];     //y x 

/*
    draw_coordinate_line_handle
    从 a(x0,y0) 画到 b(x1,y1) 
*/
char draw_coordinate_line_handle(int x0,int y0,int x1,int y1)
{
    float diff_x = (x1 - x0),diff_y = (y1 - y0);
    float k = diff_y / diff_x;
    float b = y0 - k * x0;
    float y = 0;
    float temp = 1.20;
    int target = 0;
    printf("y = %f * x + (%f) \n",k,b);
    printf("test : %f -> %d ; %f -> %d  \n",temp,(int)(temp),temp+0.5,(int)(temp+0.5));
    printf("test ROUND_OFF : %f -> %d ; %f -> %d  \r\n \n",temp,ROUND_OFF(temp),temp+0.5,ROUND_OFF(temp+0.5));
    memset(image,0,sizeof(image));      // clean

    for (int i = x0; i <= x1; i++)
    {
        temp = i;
        y = k * temp + b;
        target = ROUND_OFF(y);
        image[target][i] = 1;
        printf("ROUND_OFF %f -> %d \n",y,target);
    }

    return target;
}



void draw_coordinate_show (int x_len,int y_len)
{
    int run_y = y_len - 1;
    ESP_LOGI(TAG,"start draw ");
    
    printf("  y \n");
    for (int i = run_y; i >= 0; i--)
    {
        printf("%2d|",i);
        for(int j = 1;j < (x_len-1);j++)
        {
            if(image[i][j] == 1)
            {
                printf(" .");
            }
            else
            {
                printf("  ");
            }
        }
        printf(" \n");
    }

    {printf("  ");}
    for (int i = 0; i < x_len; i++)
    {printf("¯¯");}printf(" x\n");

    {printf(" ");}
    for (int i = 0; i < x_len; i++)
    {printf("%2d",i);}printf(" \n");
    
}


