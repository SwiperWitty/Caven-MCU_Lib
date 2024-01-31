#include "DATA_Handle.h"


int Data_Sign_atof_Handle (Sign_atof_Type *Item)
{
    char i,n,m = 0;
    char flag = 1;                            //i是当前找的是 第几个 Sign
	char Data_Array[12] = {0};                  //数字的str数
    char str[50];                               //装source数据到这个缓存区，这个缓存区是可变的
    char Find_Time = strlen(Item->Sign);        //要找多少个
    char Length = strlen(Item->Source);         //源的长度
    strcpy(str,Item->Source);
    if(Length > 50)
        Length = 50;
    for (i = 0; i < Find_Time; i++)
    {
        flag = 0;
        for(n = 0;n < Length;n++)
        {
            if(str[n] == Item->Sign[i])             //找到标志
            {
                str[n] = 0;
                flag = 1;                           //开始识别str
                m = 0;
            }
            if(flag)
            {
                if((str[n] <= '9' && str[n] >= '0') || str[n] == '.' || str[n] == '-')
                {
                    Data_Array[m] = str[n];         //把数字相关str的装起来
                    m++;                            //m是数字在str的长度
                }
                else
                {
                    if(m > 0)                       //数字部分结束
                    {
                        if(Data_Array[m-1] == '.' || Data_Array[m-1] == '-')    //不让最后一位为 '.' and '-'
                            Data_Array[m-1] = '\0';                             //防止 atof()函数出错
                        else
                            Data_Array[m] = '\0';
                        break;
                    }
                }
            }
        
        }
        Item->NUM[i] = atof(Data_Array);
        Item->flag = i;
    }

    return Item->flag;
}

/*
data是要载入的，最终装载到array数组
如果得到结果，reverse会被改变（中值滤波结果），否则reverse不变
run在载入状态会自动++，超过array_num会置零
retval = 1则成功，retval = 0则正在运行，retval = (-1)失败
*/
int Data_Median_filtering_Handle (float data,float *array,float *reverse,char *run,char array_num)
{
	int retval = 0;
	int temp_run = *run;
	char max_sort = 0;
	char min_sort = 0;
	float temp_data_f;
    if (array == NULL && array_num <= 2)
    {
        retval = (-1);
        return retval;
    }
    
	if(temp_run < array_num)
	{
		array[temp_run++] = data;
	}
	else
	{
		for(int i = 0;i < array_num;i++)
		{
			temp_data_f = MAX(array[i],array[max_sort]);
			if(temp_data_f == array[i])
			{
				max_sort = i;
			}
		}
		for(int i = 0;i < array_num;i++)
		{
			temp_data_f = MIN(array[i],array[min_sort]);
			if(temp_data_f == array[i])
			{
				min_sort = i;
			}
		}
		array[max_sort] = 0;
		array[min_sort] = 0;
		temp_data_f = 0;
		for(int i = 0;i < array_num;i++)
		{
			temp_data_f += array[i];
		}
		temp_data_f /= (array_num - 2);
		*reverse = temp_data_f;
		temp_run = 0;
		retval = 1;
	}
	*run = temp_run;
	return retval;
}
