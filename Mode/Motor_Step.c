#include "Motor_Step.h"

int step_gpiox = 0;
int step_DIR = 0;
int step_PUL = 0;
static int Reduction = 200;

void Motor_Step_Init(int gpiox,int PUL_pin,int DIR_pin,int Set)
{
#if Exist_Motor_STEP
	step_gpiox = gpiox;
	step_DIR = DIR_pin;
	step_PUL = PUL_pin;
    User_GPIO_config(step_gpiox,step_DIR,Set);	// 初始GPIO
	User_GPIO_config(step_gpiox,step_PUL,Set);	// 初始GPIO

#endif
}

#if Exist_Motor_STEP
static void Motor_Step_delay(int time)
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

/*
方向
*/
static void Motor_Step_SetDirection(int dir)
{
    if(dir)
	{
		User_GPIO_set(step_gpiox,step_DIR,1);
	}
	else
	{
		User_GPIO_set(step_gpiox,step_DIR,0);
	}
}

static void Motor_Step_Enable(int en)
{
    if(en)
	{
		
	}
	else
	{
		
	}
}

/*
发送一个脉冲
*/
void Motor_Step_pulse(void)
{
    User_GPIO_set(step_gpiox,step_PUL,1);
    Motor_Step_delay(1);
    User_GPIO_set(step_gpiox,step_PUL,0);
    Motor_Step_delay(1);
}

#endif

char Motor_Step_Drive(char Rotation,char Mode,int Code)
{
    /*
    Rotation    方向(0/1)
    Mode = 0: Code = 转动角度(0-360)
    Mode = 1: Code = 转动圈数
    */
    char Retval = 0;
#if Exist_Motor_STEP
    if(Mode != 0 && Mode != 1)
    {
        return Retval;
    }
    int temp = Code;
	
	Motor_Step_SetDirection(Rotation);
    if(Mode)
    {
        while(temp)
        {
            for (int k = 0;k < Reduction;k++)   //转动一圈
            {
                Motor_Step_pulse();
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
            Motor_Step_pulse();
        }
        temp--;
    }
#endif
    return Retval;
}
