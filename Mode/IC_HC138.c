#include "IC_HC138.h"

void MODE_HC138_Init (int Set)
{
#if Exist_HC138
    User_GPIO_config(HC138_GPIO,HC138_D1_IO,1);
    User_GPIO_config(HC138_GPIO,HC138_D2_IO,1);
    User_GPIO_config(HC138_GPIO,HC138_D3_IO,1);

    User_GPIO_set(HC138_GPIO,HC138_D1_IO,1);
    User_GPIO_set(HC138_GPIO,HC138_D2_IO,1);
    User_GPIO_set(HC138_GPIO,HC138_D3_IO,1);
#endif
}

//结果（构想）--动作--结果（实现）
void HC138_Set_Data_Fun (int Data)
{
#if Exist_HC138
    if(Data < 1 || Data > 8)
        return ;
    Data--;
    if (Data & 0x01)
    {
        User_GPIO_set(HC138_GPIO,HC138_D1_IO,1);
    }
    else
    {
        User_GPIO_set(HC138_GPIO,HC138_D1_IO,0);
    }
    if (Data & 0x02)
    {
        User_GPIO_set(HC138_GPIO,HC138_D2_IO,1);
    }
    else
    {
        User_GPIO_set(HC138_GPIO,HC138_D2_IO,0);
    }
    if (Data & 0x04)
    {
        User_GPIO_set(HC138_GPIO,HC138_D3_IO,1);
    }
    else
    {
        User_GPIO_set(HC138_GPIO,HC138_D3_IO,0);
    }
#endif
}
