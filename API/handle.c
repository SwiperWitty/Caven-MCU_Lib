#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"libcollections.h"
 

#define MAX(a,b)	(a>b)?a:b				//比较函数
#define MIN(a,b)	(a<b)?a:b				//比较函数

char show[8][31];

struct _frame
{
    char *string;           //每次来的数据
    int x,y,z;              //x是长度，y是层数，z是当前的排序数

    fifo_t fifo_data;       //size = y *（x+1）
};


void String_to_frame(struct _frame *Item)
{
    if(Item->string == NULL || Item->x == 0)
        return;
    if (Item->fifo_data.size == 0)                          //如果没有定义，就为Item中的 fifo_data 申请一个 Item->y * (Item->x+1)的框架
    {
        fifo_alloc(&Item->fifo_data,Item->y,Item->x+1);
    }
    
    int i,num;
    int len = strlen(Item->string) + 1;             //+1是把'\0'也加上,测量要处理的字符长度
    char sort[8];
    sprintf(sort,"%d->",Item->z);                           //创建一个前缀
    int new_len = strlen(sort) + len;                       //计算加上前缀的总长
    char *new_string = (char *)malloc(new_len);             //创建一个NEW，长度为new_len（总长）
    memcpy(new_string,sort,strlen(sort));                   //将 前缀 copy到new_string
    memcpy(new_string + strlen(sort),Item->string,len);     //将string copy到 前缀 后面
    char *temp = (char *) malloc(Item->x + 1);              //申请一个Item->x + 1长度的缓存区

    // printf("%d %d\n",len,new_len);
    // printf("str :%s \n",Item->string);
    // printf("new_str :%s \n \n",new_string);

    for (i = 0; i <= new_len/Item->x; i++)
    {
        
        num = ((i + 1)*Item->x < new_len) ? Item->x : new_len - (Item->x * i);      //需要复制多长到缓存区(不要越阶访问new_string，new_string不一定是Item->x的整数倍)
        
        memcpy(temp,new_string + (i * Item->x),num);                                //截取num个数据到缓存区
        temp[num] = '\0';                                                           //最后一位强制置'\0'

        if(fifo_is_not_full(&Item->fifo_data))
        {
            fifo_enqueue(&Item->fifo_data, temp);                    //入栈     实际上会入 (Item->x + 1) 个
        }
        else
        {
            char *temp_out = (char *) malloc(Item->x + 1);
            fifo_dequeue(&Item->fifo_data, temp_out);                //出栈
            fifo_enqueue(&Item->fifo_data, temp);
            free(temp_out);
        }
        // printf("%s \n",temp);
        memset(temp,0,Item->x);
    }
    free(temp);
    free(new_string);
    Item->z++;
}

int main (void)
{
    struct _frame Item1 = {
        .string = "1234567890........1234567890.......1234567890................abcdefghijklmnopqrstuvwxyz...1234567890............\
.....abcdefghijklmnopqrstuvwxyz1234567890......1234567890..................abcdefghijklmnopqrstuvwxyz1234567890.......1234567890.123",
        .x = 16,
        .y = 10,
        .z = 1
    };

    while(1)
    {
        String_to_frame(&Item1);
        Item1.string = "ppp";
        String_to_frame(&Item1);
        Item1.string = "123456";
        String_to_frame(&Item1);
        Item1.string = "nihao";
        String_to_frame(&Item1);
        Item1.string = "hello !";
        String_to_frame(&Item1);
        Item1.string = "hello world !1234567890";
        String_to_frame(&Item1);

        printf("SHOW : \n");
        for (char i = 0; i < Item1.y; i++)
        {
            char *temp_out = (char *) malloc(Item1.x + 1);
            fifo_dequeue(&Item1.fifo_data, temp_out);
            printf("%s \n",temp_out);
            free(temp_out);
        }
        fifo_free(&Item1.fifo_data);

        return 0;

    }
    
}
