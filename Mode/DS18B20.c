#include "DS18B20.h"

unsigned int Templ,Temp2,Temperature;  //Templ低八位，Temp2高八位
unsigned char Minus_Flag=0;  //负温度标志位

int DS18B20_Time = 0;
int DS18B20_Exist_Flag = 0;

void DS18B20_Delay (int Num)
{
    int i = Num;
    for (i *= DS18B20_Time; i > 0; i--)
    {
        /* code */
    }

}

void DS18B20_Init (int Set)
{
#ifdef Exist_DS18B20
    DS18B20_GPIO_Init(Set);
    DS18B20_Time = (MCU_SYS_Freq/6000000);

    DS18B20_IO_H();
    DS18B20_Delay (80);
    DS18B20_IO_L();
    DS18B20_Delay (800);
    DS18B20_IO_H();
    DS18B20_Delay (80);
    DS18B20_IO_Config(0);

    int time = 0;
    do{
        DS18B20_Delay (50);
        time++;
        if(time > 20)
        {
            DS18B20_GPIO_Init(0);
            DS18B20_Exist_Flag = 1;         //
            return ;                    //启动失败了
        }
    }
    while(DS18B20_IO_R() == 1);
    DS18B20_Exist_Flag = 1;             //成功
    DS18B20_Get_Temp();                         //启动一次
#endif
}

void Write_Byte (char Data)
{
#ifdef Exist_DS18B20
    char Temp = Data;
    DS18B20_GPIO_Init(1);
    DS18B20_IO_H();
    DS18B20_Delay (80);

    for (char i = 8; i > 0; i--)
    {
        DS18B20_IO_L();
        DS18B20_Delay (8);

        if(Temp & 0x01)             //与1按位与运算，Data最低位为1时DQ总线为1，Data最低位为0时DQ总线为0
        {
            DS18B20_IO_H();
            DS18B20_Delay (100);
        }
        else
        {

            DS18B20_Delay (100);
        }
        Temp >>= 1;
        DS18B20_IO_H();
        DS18B20_Delay (80);

    }
    DS18B20_Delay (50);
#endif
}

char Read_Byte (void)
{
    char Data;
#ifdef Exist_DS18B20
    for (char i = 0; i < 8; i++)
    {
        DS18B20_IO_Config(1);
        DS18B20_IO_H();
        DS18B20_Delay (8);
        DS18B20_IO_L();         //低脉冲
        DS18B20_IO_Config(0);

        DS18B20_Delay (60);

        Data &= DS18B20_IO_R();
        Data <<= 1;
    }

    DS18B20_IO_Config(1);
#endif
    return Data;
}

float DS18B20_Get_Temp(void)
{
    float Temp = 0;

    if(DS18B20_Exist_Flag)      //  在DB18B20存在的情况下
    {
        int Data;
        char Temp_H,Temp_L;

        Write_Byte(0xcc);	// skip rom
        Write_Byte(0x44);	// convert
        DS18B20_Delay (1000);

        Write_Byte(0xcc);	// skip rom
        Write_Byte(0xbe);	// convert
        DS18B20_Delay (1000);

        Temp_L = Read_Byte ();
        Temp_H = Read_Byte ();

        Data = Temp_H;
        Data <<= 8;
        Data |= Temp_L;

        if (Data & 0x8000)           //看看是不是负的
        {
            Data = ~(Data - 1);
        }
        Temp = Data;
        Temp /= 16;
    }
    return Temp;
}


