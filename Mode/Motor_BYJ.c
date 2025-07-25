#include "Motor_BYJ.h"

int Reduction_BYJ = 512;

void Motor_BYJ_Init(int Set)
{
#if Exist_Motor_BYJ
    User_GPIO_config(1,0,Set);       // 初始GPIO
	User_GPIO_config(1,1,Set);
	User_GPIO_config(1,6,Set);
	User_GPIO_config(1,7,Set);
    if(Set)
    {
        User_GPIO_set(1,0,0);
        User_GPIO_set(1,1,0);
		User_GPIO_set(1,6,0);
		User_GPIO_set(1,7,0);
    }
#endif
}

#if Exist_Motor_BYJ
static void Motor_BYJ_delay(int time)
{
    int j;
    for(int i = 0;i < time;i++)
    {
        j = 1000;
        do
        {
            NOP();
            NOP();
            NOP();
        }
        while(j--);
    }
    
}

void Motor_BYJ_Angle (char Step)
{
    char temp = Step;
	User_GPIO_set(1,0,0);
	User_GPIO_set(1,1,0);
	User_GPIO_set(1,6,0);
	User_GPIO_set(1,7,0);
    
    switch (temp)
    {
    case 0:
        User_GPIO_set(1,0,1);
        break;
    case 1:
        User_GPIO_set(1,0,1);
		User_GPIO_set(1,1,1);
        break;
    case 2:
        User_GPIO_set(1,1,1);
        break;
    case 3:
        User_GPIO_set(1,1,1);
        User_GPIO_set(1,6,1);
        break;
    case 4:
        User_GPIO_set(1,6,1);
        break;
    case 5:
        User_GPIO_set(1,6,1);
        User_GPIO_set(1,7,1);
        break;
    case 6:
        User_GPIO_set(1,7,1);
        break;
    case 7:
        User_GPIO_set(1,7,1);
        User_GPIO_set(1,0,1);
        break;

    default:
        break;
    }
}
#endif

char Motor_BYJ_Drive(char Rotation,char Mode,int Code)
{
    /*
    Rotation    方向(0/1)
    Mode = 0: Code = 转动角度(0-360)
    Mode = 1: Code = 转动圈数
    */
    char Retval = 0;
#if Exist_Motor_BYJ
    if(Mode != 0 && Mode != 1)
    {
        return Retval;
    }
    int temp = Code;
    int num;
    if(Mode)
    {
        while(temp)
        {
            for (int k = 0;k < Reduction_BYJ;k++)   //转动一圈
            {
                for(int i = 0;i < 8;i++)        //转动一周
                {
                    num = i;
                    if(Rotation)                //方向
                    {
                        num = 7 - i;
                    }
                    Motor_BYJ_Angle (num);
                    Motor_BYJ_delay(20);
                }
            }
            temp--;
        }
    }
    else 
    {
        if(Code > 360 || Code < 0)
        {
            return Retval;
        }
        float Angle = temp;
        Angle = (Angle / 360) * Reduction_BYJ;
        temp = Angle;
        for (int k = 0;k < temp;k++)   //转动一圈
        {
            for(int i = 0;i < 8;i++)        //转动一周
            {
                num = i;
                if(Rotation)                //方向
                {
                    num = 7 - i;
                }
                Motor_BYJ_Angle (num);
                Motor_BYJ_delay(20);
            }
        }
        temp--;
    }

    Retval = 1;
#endif
    return Retval;
}
