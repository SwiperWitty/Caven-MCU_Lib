#include "Data_Handle.h"
#include "debug_log.h"

int Caven_Hex_To_String (uint8_t *array,int len,char *ret_str)
{
    int retval = 0,offset = 0;
    if (array != NULL && ret_str != NULL)
    {
        for (int i = 0; i < len; i++)
        {
            sprintf(&ret_str[offset],"%02X ",array[i]);
            offset += 3;
        }
    }
    return retval; 
}

int Caven_String_To_Hex (char *str)
{
    int retval = 0,str_len,offset = 0;
    if (str != NULL)
    {
        str_len = strlen(str);
        for (int i = 0; i < str_len; i++)
        {
            if (str[offset] >= '0' && str[offset] <= '9')
            {
                retval <<= 4;
                retval |= 0X00 + (str[offset] - '0');
            }
            else if (str[offset] >= 'A' && str[offset] <= 'F')
            {
                retval <<= 4;
                retval |= 0X0A + (str[offset] - 'A');
            }
            else if (str[offset] >= 'a' && str[offset] <= 'f')
            {
                retval <<= 4;
                retval |= 0X0A + (str[offset] - 'a');
            }
            else
            {
                return retval;
            }
            offset ++;
        }
    }
    return retval; 
}

/*
	find substr in full_data,return pointer
*/
char* memstr(void* full_data, char* substr,int full_data_len)
{
    if (full_data == NULL || substr == NULL || full_data_len <= 0) 
    {
        return NULL;
    }
    if (*substr == '\0') {
        return NULL;
    }
    int sublen = strlen(substr);
 
    char* cur = full_data;
    int last_possible = full_data_len - sublen + 1;
    for (int i = 0; i < last_possible; i++) 
	{
		if (((char*)full_data - cur) + sublen > full_data_len)
		{
			return NULL;
		}
        if (*cur == *substr) {
            //assert(full_data_len - i >= sublen);
            if (memcmp(cur, substr, sublen) == 0) {
                //found
                return cur;
            }
        }
        cur++;
    }
    return NULL;
}

/*
    retval = Caven_gain_str_by_sign
    retval < 0:未获得
    retval > 0:获得,且为数据距file偏移
    retval = 0:获得,但是[sign]可能没找到
    在[file]找[sign_str]，如果找到则继续匹配[sign]
    如果[sign] == 0，那就不找了，认为[sign_str]后是str_num，即atoi(str_num),再sprintf(pData,"%d",atoi(str_num))
    如果[sign] == '['，那就认为是组合,即提取[xxx]中内容给 pData
    如果[sign] == '#'，那就认为是##,即提取#xxx#中内容给 pData
    run:
        data_gain_str_by_sign("caven777",strlen("caven777"),temp_array,"caven",0);
        printf("%s",temp_array);
    ret:
        777
    run:
        data_gain_str_by_sign("caven:[996]",strlen("caven[996]"),temp_array,"caven",'[');
        printf("%s",temp_array);
    ret:
        996
*/
int Caven_gain_str_by_sign(char *file,int file_len,char *pData,char *sign_str,char sign)
{
    int retval = -1;    // 未获得
    char *str_pointer = NULL;
    int temp_num,str_len;
    if (file == NULL || pData == NULL || sign_str == NULL)
    {
        return retval;
    }
    str_pointer = memstr(file,sign_str,file_len);
    if (str_pointer != NULL)
    {
        str_len = strlen(sign_str);
        // debug_log(LOG_Info,TAG,"file gain 1 ->%s",str_pointer);
        if (sign == 0)
        {
            temp_num = atoi(str_pointer+str_len);   // 指针偏移
            sprintf(pData,"%d",temp_num);
            retval = str_pointer - file;
        }
        else
        {
            char *temp_pointer = NULL;
            if (sign == '[' || sign == ']')
            {
                temp_num = file_len - (str_pointer+str_len-file);
                temp_pointer = memstr(str_pointer+str_len,"[",temp_num);
                if (temp_pointer != NULL)
                {
                    temp_num = file_len - (temp_pointer+1-file);
                    str_pointer = memstr(temp_pointer+1,"]",temp_num);
                    if (str_pointer != NULL)
                    {
                        temp_num = str_pointer-temp_pointer;
                        if (temp_num > 0)
                        {
                            memcpy(pData,temp_pointer+1,temp_num-1);
                            // debug_log(LOG_Info,TAG,"file gain 2 ->%s",temp_pointer);
                        }
                        retval = temp_pointer - file;
                    }
                }
                else
                {
                    retval = 0;
                }
            }
            else
            {
                char temp_str[6];
                memset(temp_str,0,sizeof(temp_str));
                temp_str[0] = sign;
                temp_num = file_len - (str_pointer+str_len-file);
                temp_pointer = memstr(str_pointer+str_len,temp_str,temp_num);
                if (temp_pointer != NULL)
                {
                    temp_num = file_len - (temp_pointer+1-file);
                    str_pointer = memstr(temp_pointer+1,temp_str,temp_num);
                    if (str_pointer != NULL)
                    {
                        temp_num = str_pointer-temp_pointer;
                        if (temp_num > 0)
                        {
                            memcpy(pData,temp_pointer+1,temp_num-1);
                            // debug_log(LOG_Info,TAG,"file gain 2 ->%s",temp_pointer);
                        }
                        retval = temp_pointer - file;
                    }
                }
                else
                {
                    retval = 0;
                }
            }
        }
        // debug_log(LOG_Info,"data","file gain file[%s]",file);
        // debug_log(LOG_Info,"data","file gain sign_str[%s]",sign_str);
        // debug_log(LOG_Info,"data","file gain sign ascii num[%d]",sign);
        // debug_log(LOG_Info,"data","file gain retval[%d] pData->%s\n",retval,pData);
    }
    return retval;
}

/*
	字符串转ip
str:192.168.1.10,IpSize:4
run
retval = 0
ip:c0 a8 01 0a
*/
int Caven_Str_To_ip (char *str,uint8_t *ip,int IpSize)
{
    int retval = 1,offset,temp_num,temp_run;
    char *temp_pointer,temp_array[100];
    if (str != NULL && ip != NULL && IpSize > 1)
    {
        temp_pointer = str;
        temp_num = atoi(temp_pointer);
        temp_run = 0;
        ip[temp_run++] = temp_num & 0xff;
        for (int i = 0; i < (IpSize - 1); i++)
        {
            memset(temp_array,0,sizeof(temp_array));
            offset = Caven_gain_str_by_sign(temp_pointer,strlen(temp_pointer),temp_array,".",0);
            if (offset >= 0)
            {
                ip[temp_run++] = atoi(temp_array) & 0xff;
                temp_pointer += offset + 1;     // skip '.'
                // debug_log(LOG_Info,"Caven_Str_To_ip","temp_array->%s",temp_array);
            }
            else
            {
                ip[temp_run++] = 0;
            }
        }
        retval = 0;
    }
    return retval;
}

/*
	软件中值滤波
data是要载入的，最终装载到array数组
如果得到结果，reverse会被改变（中值滤波结果），否则reverse不变
run在载入状态会自动++，超过array_num会置零
retval = 1则成功，retval = 0则正在运行，retval = (-1)失败
*/
int Caven_Data_Median_filtering_Handle (float data,float *array,float *reverse,char *run,char array_num)
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
		for(int i = array_num - 1;i > 0;i--)
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

/*
    近似值
num:12,num_step:5,num_min:0,num_max:100
retval:10
*/
int Caven_math_approximate (int num,int num_step,int num_min,int num_max)
{
    int retval = 0;
    int temp_num;
    float f_temp_num;
    float f_temp_temp;
    temp_num = num / num_step;
    f_temp_temp = num_step;
    f_temp_temp /= 2;

    f_temp_num = num % num_step;

    if (f_temp_num > f_temp_temp) {
        retval = (temp_num + 1) * num_step;
    }
    else {
        retval = temp_num * num_step;
    }
    retval = MIN(num_max,retval);
    retval = MAX(num_min,retval);
    return retval;
}

/*
四舍五入
*/
int Caven_math_approximate_float(float num)
{
	int retval = 0;
	retval = num + 0.5;
	return retval;
}

