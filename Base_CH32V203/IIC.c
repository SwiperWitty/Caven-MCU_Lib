#include "IIC.h"


void IIC_SDA_Satar (char GPIO_Mode)
{
#ifdef Exist_IIC
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = IIC_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode;

    GPIO_Init(GPIO_IIC, &GPIO_InitStructure);
#endif
}

void IIC_Init(int SET)
{
#ifdef Exist_IIC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //GPIOB
        GPIO_InitStructure.GPIO_Pin = IIC_SCL;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_IIC, &GPIO_InitStructure);

        IIC_SDA_Satar (IIC_Mode_OUT);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = IIC_SCL | IIC_SDA;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_IIC, &GPIO_InitStructure);
    }
#endif
}
// **   //

void IIC_Delay (int time)
{
    for (int var = 0; var < time; ++var)
    {
        int temp = IIC_Base_Speed;            //SET
        while(temp--);
    }
}

void IIC_StartBit(void)//  开始
{
    IIC_SCL_L();  // Set SCL line
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SDA_H();  // Set SDA line
    IIC_Delay(1); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line
    IIC_Delay(1); // Generate bus free time between Stop
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(1); // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)在SCK=1时，检测到SDA由1到0表示通信开始（下降沿）
    IIC_SCL_L();  // Clear SCL line
    IIC_Delay(1); // Wait a few microseconds
}

void IIC_StopBit(void)//  停止
{
    IIC_SCL_L();  // Clear SCL line
    IIC_Delay(1); // Wait a few microseconds
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(1); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line
    IIC_Delay(1); // Stop condition setup time(Tsu:sto=4.0us min)
    IIC_SDA_H();  // Set SDA line在SCL=1时，检测到SDA由0到1表示通信结束（上升沿）
}

void IIC_ASK(void)
{
    IIC_SCL_L();
    IIC_SDA_L();        //
    IIC_Delay(2);
    IIC_SCL_H();
    IIC_Delay(2);
    IIC_SCL_L();        //以SCL_L结束
}

void IIC_NASK(void)
{
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_Delay(2);
    IIC_SCL_H();
    IIC_Delay(2);
    IIC_SCL_L();        //以SCL_L结束
}

char IIC_WaitASK(void)
{
    char temp = 0;
    int Time = 0;
    IIC_SCL_L();
    IIC_Delay(2);
    IIC_SCL_H();
    IIC_SDA_Satar (IIC_Mode_IN);
    do {
        IIC_Delay(1);
        Time++;
        if (IIC_SDA_IN() == 0) {
            temp = 1;
        }
    } while (Time < 20);
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SCL_L();        //以SCL_L结束
    return temp;
}

// **   //
void IIC_Write_Addr(char WR,char Addr,int speed)
{
#ifdef Exist_IIC
    char temp;
    if (WR)
        Addr |= 0x01;
    else
        Addr &= 0xFE;
    IIC_StartBit();
    for (int i = 0; i < 8; i++)
    {
        IIC_SCL_L();      //准备数据变更
        IIC_Delay(speed);
        temp = (Addr << i) & 0x80;
        if (temp)
            IIC_SDA_H();
        else
            IIC_SDA_L();
        IIC_Delay(speed);
        IIC_SCL_H();      //数据变更完成
        IIC_Delay(speed);
    }
    IIC_ASK();
//    IIC_StopBit();

#endif
}
void IIC_Write_DATA(char DATA,int speed)
{
#ifdef Exist_IIC
    char temp;
    IIC_SDA_Satar (IIC_Mode_OUT);
    for (int i = 0; i < 8; i++) {
        IIC_SCL_L();      //准备数据变更
        IIC_Delay(speed);
        temp = (DATA << i) & 0x80;
        if (temp)
            IIC_SDA_H();
        else
            IIC_SDA_L();
        IIC_Delay(speed);
        IIC_SCL_H();      //数据变更完成
        IIC_Delay(speed);
    }
//    IIC_NASK();
    IIC_WaitASK();
    IIC_StopBit();

#endif
}


char IIC_Read_DATA(char Addr,char DATA)
{
#ifdef Exist_IIC

#endif
}

