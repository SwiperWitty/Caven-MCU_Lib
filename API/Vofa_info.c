#include "Vofa_info.h"

Send_pFun Vofa_send_fun = NULL;
char Vofa_ch_num = 0;

/*
 *
 */
int Vofa_JustFloat_Init_Fun (char ch_num,Send_pFun send_fun)
{
    int retval = 0;
    if (send_fun != NULL) {
        Vofa_send_fun = send_fun;
        Vofa_ch_num = ch_num;
    }
    else {
        retval = -1;
    }
    return retval;
}

/*
 *
 */
int Vofa_JustFloat_Show_Fun (float *f_num)
{
    int retval = 0;
    unsigned char temp_array[100];
    char tail[] = {0x00, 0x00, 0x80, 0x7f};
    int run_num = 0;
    char data_size = sizeof(float);
    for (int i = 0; i < Vofa_ch_num; ++i)
    {
        if (run_num >= 100) {
            return -2;
        }
        memcpy(&temp_array[run_num],&f_num[i],data_size);
        run_num += data_size;
    }
    data_size = sizeof(tail);
    memcpy(&temp_array[run_num],tail,data_size);
    run_num += data_size;
    if (Vofa_send_fun != NULL) {
        Vofa_send_fun(temp_array,run_num);
        retval = -1;
    }
    return retval;
}


