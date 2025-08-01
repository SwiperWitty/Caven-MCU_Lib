#ifndef _DATA_HANDLE__H_
#define _DATA_HANDLE__H_

/*
        【API】      ————C语言环境即可运行
        本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    #include "Data_Handle.h"
*/

#include "Caven_Type.h"


int Caven_String_To_Hex (char *str);
int Caven_Hex_To_String (uint8_t *array,int len,char *ret_str);
int Caven_Str_To_ip (char *str,uint8_t *ip,int IpSize);

char *memstr(void* full_data, char* substr,int full_data_len);
int Caven_gain_str_by_sign(char *file,int file_len,char *pData,char *sign_str,char sign);

int Caven_math_approximate (int num,int num_step,int num_min,int num_max);
int Caven_math_approximate_float(float num);
int Caven_Data_Median_filtering_Handle (float data,float *array,float *reverse,char *run,char array_num);

#endif

