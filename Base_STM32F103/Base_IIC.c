#include "Base_IIC.h"


#if Exist_IIC
extern void SYS_Base_Delay(int time, int speed);
static void IIC_Delay (int time)
{
    SYS_Base_Delay(time,IIC_Base_Speed);
}

static void IIC_SDA_Satar (char GPIO_Mode)
{
    static char last_mode = 6;
    if (GPIO_Mode != last_mode) {
        last_mode = GPIO_Mode;
        User_GPIO_config(GPIO_IIC,IIC_SDA,GPIO_Mode);
    }
}

void IIC_StartBit(void) // 开始
{
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);  // Set SCL line
    IIC_Delay(1);
    IIC_SDA_Satar (1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,1);  // Set SDA line
    IIC_Delay(1); // Wait a few microseconds
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);  // Set SCL line
    IIC_Delay(1); // Generate bus free time between Stop
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);  // Clear SDA line
    IIC_Delay(1); // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)在SCK=1时，检测到SDA由1到0表示通信开始（下降沿）
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);  // Clear SCL line
    IIC_Delay(1); // Wait a few microseconds
}

void IIC_StopBit(void)  // 停止
{
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);  // Clear SCL line
    IIC_SDA_Satar (1);
    IIC_Delay(1); // Wait a few microseconds
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);  // Clear SDA line
    IIC_Delay(1); // Wait a few microseconds
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);  // Set SCL line           /*  END    */
    IIC_Delay(1); // Stop condition setup time(Tsu:sto=4.0us min)
    User_GPIO_set(GPIO_IIC,IIC_SDA,1);  // Set SDA line在SCL=1时，检测到SDA由0到1表示通信结束（上升沿）
}

void IIC_ASK(void)      // 主机应答
{
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
    IIC_SDA_Satar (1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);
    IIC_Delay(2);
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);
    IIC_Delay(2);
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
}

void IIC_NASK(void)     // 主机不应答
{
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
    IIC_SDA_Satar (1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,1);
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
}

char IIC_WaitASK(char num)  // 一定要有从设备响应
{
    char temp = 0;
    int Time = 0;
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
    IIC_SDA_Satar (0);
    do {
        IIC_Delay(1);
        Time++;
        if (User_GPIO_get(GPIO_IIC,IIC_SDA) == 0)      // 找到数据，即可跳出
        {
            temp = 1;
            User_GPIO_set(GPIO_IIC,IIC_SDA,0);
            User_GPIO_set(GPIO_IIC,IIC_SCL,1);
            IIC_SDA_Satar (1);
            break;
        }
    } while (Time < num);
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);
    IIC_Delay(1);
    return temp;
}

void IIC_Write_DATA(uint8_t Data,int Speed)
{
    uint8_t temp;
    IIC_SDA_Satar (1);
    for (int i = 0; i < 8; i++) {
        User_GPIO_set(GPIO_IIC,IIC_SCL,0);      // 准备数据变更
        IIC_Delay(Speed);
        temp = (Data << i) & 0x80;
        if (temp)
            User_GPIO_set(GPIO_IIC,IIC_SDA,1);
        else
            User_GPIO_set(GPIO_IIC,IIC_SDA,0);
        IIC_Delay(Speed);
        User_GPIO_set(GPIO_IIC,IIC_SCL,1);      // 数据变更完成
        IIC_Delay(Speed);
    }
}

uint8_t IIC_Read_DATA(int Speed)
{
    uint8_t retval = 0;
    uint8_t temp = 0;
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
    IIC_SDA_Satar (0);
    for (int i = 7; i >= 0; i--) {
        User_GPIO_set(GPIO_IIC,IIC_SCL,0);      // 准备数据变更
        IIC_Delay(Speed);
        temp = User_GPIO_get(GPIO_IIC,IIC_SDA);
        retval |= temp << i;
//        IIC_Delay(Speed);
        User_GPIO_set(GPIO_IIC,IIC_SCL,1);      // 数据变更完成
        IIC_Delay(Speed);
    }
    return retval;
}
#endif

/*
 * 返回的是是否成功ACK
 * ACK意味着快速跳过ACK(OLED这类只发不收的)
 * continuous 参数为1意味着不发结束
 */
char Base_IIC_Send_DATA(char Addr,const uint8_t *Data,char ACK,int Length,int Speed,char continuous)
{
    char BK = 0;
#if Exist_IIC
    IIC_StartBit();
    IIC_Write_DATA((Addr << 1) & 0xFE,Speed);      // 写模式
    if(ACK != 0)      // 需要使用ACK
    {
        BK = IIC_WaitASK(20);
        if (BK == 0)
        {
            IIC_StopBit();
            return BK;
        }
    }
    else if(ACK == 0)
    {
        IIC_ASK();
        BK = 1;
    }

    for (int i = 0; i < Length; i++)
    {
        IIC_Write_DATA(Data[i],Speed);
        if(ACK != 0)
        {
            BK = IIC_WaitASK(10);
            if (BK == 0)    // 有一次不成功
            {
                break;
            }
        }
        else if(ACK == 0)
            IIC_ASK();
    }// 数据结束
    if (continuous) {
    }
    else {
        IIC_StopBit();
    }// IIC结束
#endif
    return BK;
}

/*
 * 返回的是是否成功ACK
 * 主机也是要回答的(IIC_ASK)
 */
char Base_IIC_Receive_DATA(char Addr,uint8_t *Data,char ACK,int Length,int Speed)
{
    char BK = 0;
#if Exist_IIC
    IIC_StartBit();
    IIC_Write_DATA((Addr << 1) | 0x01,Speed);   // 读模式
    if(ACK != 0)        // 需要使用ACK
    {
        BK = IIC_WaitASK(10);
        if (BK == 0)
        {
            IIC_StopBit();
            return BK;
        }
    }
    else if(ACK == 0)   // 不需要使用ACK
    {
        IIC_ASK();
        BK = 1;
    }
    for (int i = 0; i < Length; i++)
    {
        *(Data + i) = IIC_Read_DATA(Speed);
        IIC_ASK();      // 给从机回应
    }
    IIC_StopBit();
    //IIC结束
#endif
    return BK;
}

void Base_IIC_Init(int set)
{
#if Exist_IIC
    if (set)
    {
        User_GPIO_config(GPIO_IIC,IIC_SCL,1);
        User_GPIO_config(GPIO_IIC,IIC_SDA,1);
        User_GPIO_set(GPIO_IIC,IIC_SCL,1);
        User_GPIO_set(GPIO_IIC,IIC_SDA,1);
    }
    else
    {
        User_GPIO_config(GPIO_IIC,IIC_SCL,0);
        User_GPIO_config(GPIO_IIC,IIC_SDA,0);
    }
#endif
}

