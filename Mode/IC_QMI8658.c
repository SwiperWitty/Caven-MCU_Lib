#include "IC_QMI8658.h"
#include "Base_Sys_Time.h"

#if Exist_QMI8658
// 读取QMI8658寄存器的值
static int qmi8658_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
	int retval = 0;
	retval = Base_IIC_Send_DATA(IC_QMI8658_ADDR,&reg_addr,1,sizeof(reg_addr),10,1);
	retval |= Base_IIC_Receive_DATA(IC_QMI8658_ADDR,data,1,len,10);
	return retval;
    // return i2c_master_write_read_device(I2C_NUM_0, QMI8658_SENSOR_ADDR,  &reg_addr, 1, data, len, 10 / portTICK_PERIOD_MS);
}

// 给QMI8658的寄存器写值
static int qmi8658_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2];
    write_buf[0] = reg_addr;
    write_buf[1] = data;
	Base_IIC_Send_DATA(IC_QMI8658_ADDR,write_buf,1,sizeof(write_buf),10,0);
	return 0;
    // return i2c_master_write_to_device(I2C_NUM_0, QMI8658_SENSOR_ADDR, write_buf, sizeof(write_buf), 1000 / portTICK_PERIOD_MS);
}
#endif

int MODE_QMI8658_Init (int set)
{
    int retval = 0;
#if Exist_QMI8658
    uint8_t id = 0; // 芯片的ID号
    
    qmi8658_register_read(QMI8658_WHO_AM_I, &id ,1); // 读芯片的ID号
    while (id != 0x05)  // 判断读到的ID号是否是0x05
    {
        SYS_Delay_ms(200);
        qmi8658_register_read(QMI8658_WHO_AM_I, &id ,1); // 读取ID号
		retval ++;
        if (retval > 3)
        {
            return retval;
        }
    }
    qmi8658_register_write_byte(QMI8658_RESET, 0xb0);  // 复位
    
    //
    qmi8658_register_write_byte(QMI8658_CTRL1, 0x40); // CTRL1 设置地址自动增加
    qmi8658_register_write_byte(QMI8658_CTRL7, 0x03); // CTRL7 允许加速度和陀螺仪
    qmi8658_register_write_byte(QMI8658_CTRL2, 0x95); // CTRL2 设置ACC 4g 250Hz
    qmi8658_register_write_byte(QMI8658_CTRL3, 0xd5); // CTRL3 设置GRY 512dps 250Hz
#endif
    return retval = 0;
}

int MODE_QMI8658_Get (void *data)
{
    int retval = 0;
#if Exist_QMI8658

#endif
    return retval;
}
