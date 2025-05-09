#include "MODE_Motor_BYJ.h"


void Motor_BYJ_Init(int Set)
{
#if Exist_STEP_Motor
    STEP_Motor_GPIO_Init (SET);       // 初始GPIO
    if(SET)
    {
        STEP_OUT_L(0);
        STEP_OUT_L(1);
        STEP_OUT_L(2);
        STEP_OUT_L(3); 
    }
#endif
}

#if Exist_STEP_Motor
static void Motor_BYJ_delay(int time)
{
    int j;
    for(int i = 0;i < time;i++)
    {
        j = 1000;
        do
        {
            __nop();
            __nop();
            __nop();
        }
        while(j--);
    }
    
}

void Motor_BYJ_Angle (char Step)
{
    char temp = Step;
    STEP_OUT_L(0);
    STEP_OUT_L(1);
    STEP_OUT_L(2);
    STEP_OUT_L(3); 
    
    switch (temp)
    {
    case 0:
        STEP_OUT_H(0);
        break;
    case 1:
        STEP_OUT_H(0);
        STEP_OUT_H(1);
        break;
    case 2:
        STEP_OUT_H(1);
        break;
    case 3:
        STEP_OUT_H(1);
        STEP_OUT_H(2);
        break;
    case 4:
        STEP_OUT_H(2);
        break;
    case 5:
        STEP_OUT_H(2);
        STEP_OUT_H(3);
        break;
    case 6:
        STEP_OUT_H(3);
        break;
    case 7:
        STEP_OUT_H(3);
        STEP_OUT_H(0);
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
#if Exist_STEP_Motor
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
            for (int k = 0;k < Reduction;k++)   //转动一圈
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
        Angle = (Angle / 360) * Reduction;
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
