#include "DS18B20.h"

unsigned int Templ,Temp2,Temperature;  //Templ低八位，Temp2高八位
unsigned char Minus_Flag=0;  //负温度标志位

void aa (int Tenp2)
{
    if (Tenp2 & 0xFC)  //判断符号位是否为1
    {
        Minus_Flag=1; //负温度标志位置1
        Temperature=((Temp2<<8)|Templ); //高八位第八位进行整合
        Temperature=((Temperature)+1); //讲补码转换为原码，求反，补1
        Temperature*=0.0625;//求出十进制
    }
    else   //温度为正值
    {
        Minus_Flag=0;  //负温度标志位置0
        Temperature =((Temp2<<8) |Templ)*0.0625;
    }
}


