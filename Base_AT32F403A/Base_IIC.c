#include "Base_IIC.h"


void IIC_SDA_Satar (char GPIO_Mode)
{
#ifdef Exist_IIC
    gpio_init_type gpio_init_struct;

    if(GPIO_Mode == IIC_Mode_OUT)
    {
        gpio_init_struct.gpio_mode = IIC_Mode_OUT;
    }
    else
    {
        gpio_init_struct.gpio_mode = IIC_Mode_IN;
    }
    gpio_init_struct.gpio_pins = IIC_SDA;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIO_IIC, &gpio_init_struct);
#endif
}

void IIC_Start_Init(int Set)
{
#ifdef Exist_IIC
    gpio_init_type  gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (SET)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
        
        gpio_init_struct.gpio_mode = IIC_Mode_OUT;
        
        gpio_init_struct.gpio_pins = IIC_SCL;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIO_IIC, &gpio_init_struct);             //单纯启动SCL

        IIC_SDA_H();
        IIC_SDA_Satar (IIC_Mode_OUT);                       //启动SDA
        IIC_SCL_H();
    }
    else
    {
        gpio_init_struct.gpio_pins = IIC_SCL|IIC_SDA;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_IIC, &gpio_init_struct);
    }
#endif
}
// **   //
#ifdef Exist_IIC
static void IIC_Delay (int time)
{
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = IIC_Base_Speed;            //SET
        do{
            NOP();
        }while((temp--) > 0);
    }
}

void IIC_StartBit(int Speed)//  开始
{
    IIC_SCL_L();  // Set SCL line
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SDA_H();  // Set SDA line
    IIC_Delay(Speed); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line
    IIC_Delay(Speed); // Generate bus free time between Stop
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(Speed); // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)在SCK=1时，检测到SDA由1到0表示通信开始（下降沿）
    IIC_SCL_L();  // Clear SCL line
    IIC_Delay(Speed); // Wait a few microseconds
}   //结束之后SDA:L  SCL: L

void IIC_StopBit(int Speed)//  停止
{
    IIC_SCL_L();  // Clear SCL line
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_Delay(Speed); // Wait a few microseconds
    IIC_SDA_L();  // Clear SDA line
    IIC_Delay(Speed); // Wait a few microseconds
    IIC_SCL_H();  // Set SCL line           /*  END    */
    IIC_Delay(Speed); // Stop condition setup time(Tsu:sto=4.0us min)
    IIC_SDA_H();  // Set SDA line在SCL=1时，检测到SDA由0到1表示通信结束（上升沿）
}   //结束之后SDA:H  SCL: H

void IIC_ASK(int Speed)      //速度快，不在乎是否有从设备
{
    IIC_SCL_L();
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SDA_L();    //模拟响应
    IIC_Delay(Speed);
    IIC_SCL_H();
    IIC_Delay(Speed);
}   //结束之后SDA:L  SCL: H

void IIC_NASK(int Speed)     //基本不用
{
    IIC_SCL_L();
    IIC_SDA_H();
    IIC_SDA_Satar (IIC_Mode_IN);  //模拟不响应
    
    IIC_Delay(Speed);
    IIC_SCL_H();
    IIC_Delay(Speed);
}   //结束之后SDA:H  SCL: H

char IIC_WaitASK(int Speed)  //一定要有从设备响应
{
    char temp = 0;
    int Time = 0;
    IIC_SCL_L();
    IIC_Delay(0);
    IIC_SDA_H();
    IIC_SDA_Satar (IIC_Mode_IN);
    do {
        IIC_Delay(Speed);
        Time++;
        if (IIC_SDA_R() == 0)      //找到数据，即可跳出
        {
            temp = 1;
            break;
        }
    } while (Time < 20);
    IIC_SDA_Satar (IIC_Mode_OUT);
    IIC_SDA_H();
    IIC_SCL_H();
    IIC_Delay(Speed);
    return temp;
}   //结束之后SDA:X  SCL: H
#endif

void IIC_Write_DATA(char DATA,int Speed)
{
#ifdef Exist_IIC
    char temp;
    IIC_SCL_L();
    IIC_SDA_Satar (IIC_Mode_OUT);
    for (int i = 0; i < 8; i++) {
        IIC_SCL_L();      //准备数据变更
        IIC_Delay(Speed);
        temp = (DATA << i) & 0x80;
        if (temp)
            IIC_SDA_H();
        else
            IIC_SDA_L();
        IIC_Delay(Speed);
        IIC_SCL_H();      //数据变更完成
        IIC_Delay(Speed);
    }
#endif
}   //结束之后SDA:X  SCL: H

char IIC_Read_DATA(int Speed)
{
    char temp = 0;
#ifdef Exist_IIC
    IIC_SCL_L();      //准备读数据
    IIC_SDA_Satar (IIC_Mode_IN);
    for (int i = 0; i < 8; i++) {
        IIC_SCL_L();      //准备读数据
        IIC_Delay(Speed);
        
        IIC_SCL_H();      //数据读完成
        temp |= ((char)IIC_SDA_R() << i);
        IIC_Delay(Speed);
    }
#endif
    return temp;
}   //结束之后SDA:X  SCL: H

int IIC_Send_DATA(char Addr,const char *Data,char ACK,int Length,int Speed)
{
    int retval = 0;
#ifdef Exist_IIC
    IIC_StartBit(Speed);
    IIC_Write_DATA((Addr << 1) & 0xFE,Speed);      //写模式
    if(ACK)      //需要使用ACK
    {
        retval = IIC_WaitASK(Speed);
        if (retval == 0)
        {
            IIC_StopBit(Speed);
            return retval;
        }
    }
    else
    {
        IIC_ASK(Speed);
        retval = 1;
    }
    //地址结束
    for (int i = 0; i < Length; ++i)
    {
        IIC_Write_DATA(Data[i],Speed);
        if(ACK && retval == 0)
        {
            retval = IIC_WaitASK(Speed);
            if (retval == 0)    //有一次不成功
            {
                break;
            }
        }
        else
        {
            IIC_ASK(Speed);
        }
    }
    //数据结束
    IIC_StopBit(Speed);
//IIC结束
#endif
    return retval;
}

int IIC_Receive_DATA(char Addr,char *Target,char ACK,int Length,int Speed)
{
    int retval = 0;
#ifdef Exist_IIC
    if(Target == NULL || Length <= 0 || Speed < 0)
    {
        retval = 2;
        return retval;
    }
    IIC_StartBit(Speed);
    IIC_Write_DATA((Addr << 1) | IIC_R_BIT,Speed);      //读模式
    if(ACK)                 //需要使用ACK
    {
        retval = IIC_WaitASK(Speed);
        if (retval == 0)
        {
            IIC_StopBit(Speed);
            return retval;
        }
    }
    else                   //不需要使用ACK
    {
        IIC_ASK(Speed);
        retval = 1;
    }
    //地址结束
    for(int i = 0;i < Length;i++)
    {
        *(Target + i) = IIC_Read_DATA(Speed);
        if(ACK && retval == 0)
        {
            retval = IIC_WaitASK(Speed);
            if (retval == 0)    //有一次不成功
            {
                break;
            }
        }
        else
        {
            IIC_ASK(Speed);
        }
    }
    IIC_StopBit(Speed);
//IIC结束
#endif
    return retval;
}

