#include "Base_IIC.h"
#include "Caven_Type.h"

void IIC_SDA_Satar (char GPIO_Mode)
{
#ifdef Exist_IIC
    static char last_mode = -1;
    if (GPIO_Mode != last_mode) {
        last_mode = GPIO_Mode;
        User_GPIO_config(GPIO_IIC,IIC_SDA,GPIO_Mode);
    }
#endif
}

// **   //
#ifdef Exist_IIC

int IIC_delay_num = 0;
static void IIC_Delay (int time)
{
    int n = IIC_Base_Speed * 10;
    do
    {
        for (int i = 0; i < n; i++)
        {
            NOP();
        }
    } while (time --);
    
}

void IIC_StartBit(void)//  开始
{
    IIC_SDA_Satar(1);      // 输出模式
    User_GPIO_set(GPIO_IIC,IIC_SDA,1);
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);  // 产生下降沿
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);
}

void IIC_StopBit(void)//  停止
{
    IIC_SDA_Satar(1);      // 输出模式
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);  // 拉高 SCL
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);  // 拉低 SDA
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SCL,1);  // 拉高 SCL
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC,IIC_SDA,1);  // 拉高 SDA -> 产生停止条件
}

void IIC_DummyClock(int Speed)
{
    IIC_SDA_Satar(1);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
    IIC_Delay(Speed);
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);
    IIC_Delay(Speed);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 1);
    IIC_Delay(Speed);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
    IIC_Delay(1);
}

void IIC_ASK(void)      //主机应答
{
    IIC_SDA_Satar(1);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
    IIC_Delay(IIC_delay_num);
    User_GPIO_set(GPIO_IIC,IIC_SDA,0);
    IIC_Delay(IIC_delay_num);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 1);
    IIC_Delay(IIC_delay_num);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
    IIC_Delay(1);
}

void IIC_NASK(void)     //主机不应答
{
    IIC_SDA_Satar(1);                       // SDA 设为输出模式
    User_GPIO_set(GPIO_IIC, IIC_SDA, 1);    // 拉高 SDA（非应答）
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 1);    // 产生第9个时钟：拉高 SCL
    IIC_Delay(1);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);    // 拉低 SCL，结束非应答位
    IIC_Delay(1);
    IIC_SDA_Satar(0);                       // 释放 SDA
}

/*
    num 等待次数
    return 0 不成功，1 成功响应
*/
char IIC_WaitASK(char num)
{
    char ack = 0;  // 默认无应答
    IIC_Delay(IIC_delay_num);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
    IIC_SDA_Satar(0);           // 释放 SDA（输入）
    IIC_Delay(1);
    do {
        IIC_Delay(1);
        // 读取 SDA 电平（低=应答，高=非应答）
        int sda_val = User_GPIO_get(GPIO_IIC, IIC_SDA);
        if (sda_val == 0)
        {
            ack = 1;
        }
        IIC_Delay(2);
        if (ack == 1) break;        // 收到应答，成功退出
    } while ((num--) > 0);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 1);   // 拉高 SCL，产生第9个时钟
    IIC_Delay(IIC_delay_num);
    User_GPIO_set(GPIO_IIC, IIC_SCL, 0);   // 拉低 SCL，结束时钟
    return ack;
}

#endif

void IIC_Write_DATA(uint8_t Data,int Speed)
{
#ifdef Exist_IIC
    uint8_t temp;
    IIC_delay_num = Speed;
    IIC_SDA_Satar (1);
    for (int i = 0; i < 8; i++) {
        User_GPIO_set(GPIO_IIC,IIC_SCL,0);      //准备数据变更
        IIC_Delay(Speed);
        temp = (Data << i) & 0x80;
        if (temp)
            User_GPIO_set(GPIO_IIC,IIC_SDA,1);
        else
            User_GPIO_set(GPIO_IIC,IIC_SDA,0);
        IIC_Delay(Speed);
        User_GPIO_set(GPIO_IIC,IIC_SCL,1);      //数据变更完成
        IIC_Delay(Speed);
    }
    IIC_Delay(1);
#endif
}

uint8_t IIC_Read_DATA(int Speed)
{
    uint8_t temp = 0;
#ifdef Exist_IIC
    IIC_delay_num = Speed;
    User_GPIO_set(GPIO_IIC,IIC_SCL,0);      //准备数据变更
    IIC_SDA_Satar (0);
    IIC_Delay(Speed);
    for (int i = 0; i < 8; i++) {
        User_GPIO_set(GPIO_IIC, IIC_SCL, 1);   // 拉高 SCL
        IIC_Delay(1);
        if (User_GPIO_get(GPIO_IIC, IIC_SDA))
            temp |= (1 << (7 - i));   // MSB first
        IIC_Delay(Speed);
        User_GPIO_set(GPIO_IIC, IIC_SCL, 0);
        IIC_Delay(Speed);
    }
#endif
    return temp;
}

/*
 * 返回的是是否成功ACK
 * ACK意味着快速跳过ACK(OLED这类只发不收的)
 * continuous 参数意味着不发结束
 */
char Base_IIC_Send_DATA(char Addr, const uint8_t *Data, char NeedAck, int Length, int Speed, char Continuous)
{
    char ret = 1;
    IIC_StartBit();
    // 发送地址+写
    IIC_Write_DATA((Addr << 1) & 0xFE, Speed);
    if (NeedAck) {
        if (IIC_WaitASK(10) == 0) {
            IIC_StopBit();
            return 0;
        }
    } else {
        // 跳过应答：但仍需产生第9个时钟，让从机有机会应答（不关心结果）
        IIC_DummyClock(Speed);   // 需要实现一个只产生时钟而不读 SDA 的函数
    }

    for (int i = 0; i < Length; i++) {
        IIC_Write_DATA(Data[i], Speed);
        if (NeedAck) {
            if (IIC_WaitASK(10) != 1) {
                ret = 0;
                break;
            }
        } else {
            IIC_DummyClock(Speed);
        }
    }

    if (Continuous == 0) {
        IIC_StopBit();
    }
    return ret;
}

/*
 * 返回的是是否成功ACK
 * 主机也是要回答的
 */
char Base_IIC_Receive_DATA(char Addr,uint8_t *Data,char NeedAck,int Length,int Speed)
{
    char BK = 0;
#ifdef Exist_IIC
    if (Data == NULL && Length <= 0)
    {
        return BK;
    }
    IIC_StartBit();
    IIC_Write_DATA((Addr << 1) | 0x01,Speed);   // 读模式
    if(NeedAck)        // 需要使用ACK
    {
        BK = IIC_WaitASK(10);
        if (BK == 0)
        {
            IIC_StopBit();
            return BK;
        }
    }
    else
    {
        IIC_DummyClock(Speed);
        BK = 1;
    }
    for (int i = 0; i < Length; i++)
    {
        *(Data + i) = IIC_Read_DATA(Speed);
        IIC_ASK();      // 给从机回应
    }
    IIC_StopBit();
    // IIC结束
#endif
    return BK;
}

int Base_IIC_Init(int set)
{
    int retval = 0;
#ifdef Exist_IIC
    if (set)
    {
    #if IIC_DMA
        i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = IIC_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = IIC_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = RTC_I2C_MASTER_FREQ_HZ,
        };
        i2c_param_config(I2C_NUM_0, &i2c_conf);
        i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0);
    #else
        User_GPIO_config(GPIO_IIC,IIC_SCL,WRITE_Config);
        User_GPIO_config(GPIO_IIC,IIC_SDA,WRITE_Config);
        User_GPIO_set(GPIO_IIC,IIC_SCL,ENABLE);
        User_GPIO_set(GPIO_IIC,IIC_SDA,ENABLE);
    #endif
    }
    else
    {
        User_GPIO_config(GPIO_IIC,IIC_SCL,READ_Config);
        User_GPIO_config(GPIO_IIC,IIC_SDA,READ_Config);
    }
#endif
    return retval;
}

