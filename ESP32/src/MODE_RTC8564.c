#include "MODE_RTC8564.h"
#include "time.h"

#ifdef Exist_RTC_Clock
struct tm timeinfo;
static int RTC8564_Init_flag = 0;
#endif


#ifdef CONFIG_IDF_TARGET_ESP32
static const char *TAG = "RTC_IIC";
static char iic_link_flag = 0;
static i2c_cmd_handle_t cmd;

int Base_IIC_Init (int set)
{
    int retval = 0;
    int i2c_master_port = RTC_I2C_MASTER_NUM;
    if (set)
    {
        ESP_LOGW(TAG, "start ");
        i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = PIN_RTC_SDA,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = PIN_RTC_SCL,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = RTC_I2C_MASTER_FREQ_HZ, /* 标准模式(100 kbit/s) */
        };

        i2c_param_config(i2c_master_port, &conf);
        retval = i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
    }
    else
    {

    }
    return retval;
}

/*
 * 返回的是是否成功ACK
 * ACK意味着快速跳过ACK(OLED这类只发不收的)
 * continuous 参数为1意味着不发结束
 */
char Base_IIC_Send_DATA(uint8_t addr,const uint8_t *Data,char ACK,int Length,int Speed,char continuous)
{
    char BK = 0;

    if (Data && Length)
    {
        if (iic_link_flag == 0)
        {
            cmd = i2c_cmd_link_create();
            iic_link_flag = 1;
        }
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write(cmd, (void *)Data, Length, true);

        if (continuous == 0)
        {
            i2c_master_stop(cmd);
            esp_err_t res = i2c_master_cmd_begin(RTC_I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
            if (res != ESP_OK)
            {
                ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d", addr, RTC_I2C_MASTER_NUM, res);
                BK = 0;
            }
            else
            {
                BK = 1;
            }
            if (iic_link_flag)
            {
                i2c_cmd_link_delete(cmd);
                iic_link_flag = 0;
            }
        }
        else
        {
            BK = 1;
        }
    }
    return BK;
}

/*
 * 返回的是是否成功ACK(1)
 * 主机也是要回答的,接收函数自动应答(IIC_ASK)
 */
char Base_IIC_Receive_DATA(uint8_t addr,uint8_t *Data,char ACK,int Length,int Speed)
{
    char BK = 0;

    if (Data && Length)
    {
        if (iic_link_flag == 0)
        {
            cmd = i2c_cmd_link_create();
            iic_link_flag = 1;
        }
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, true);
        i2c_master_read(cmd, Data, Length, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        esp_err_t res = i2c_master_cmd_begin(RTC_I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "Could not read from device [0x%02x at %d]: %d", addr, RTC_I2C_MASTER_NUM, res);
            
        }
        else
        {
            BK = 1;
        }
        if (iic_link_flag)
        {
            i2c_cmd_link_delete(cmd);
            iic_link_flag = 0;
        }
    }
    return BK;
}

static void SYS_Base_Delay(int time,int Speed)
{
    if (Speed)
    {
        vTaskDelay(pdMS_TO_TICKS(time));
    }
}

#else

#endif

/*
 * 初始成功返回0
 */
int MODE_RTC8564_Init (int set)
{
    int retval = 0;
#ifdef Exist_RTC_Clock
    uint32_t temp_time;
    RTC8564_Init_flag = 0;
    if(set)
    {
        RTC8564_Init_flag = -1;
        Base_IIC_Init(set);
        SYS_Base_Delay(100,1000);
        retval = MODE_RTC8564_Read_time (&temp_time);
        if (retval)
        {
            RTC8564_Init_flag = 1;
            retval = 0;
        }
        else
        {
            RTC8564_Init_flag = 0;
            retval = 1;
        }
    #ifdef CONFIG_IDF_TARGET_ESP32
        ESP_LOGI(TAG,"RTC8564 Init get utc [%d] (%d)",temp_time,RTC8564_Init_flag);
    #endif
    }
#endif
    return retval;
}

#ifdef Exist_RTC_Clock
static uint8_t bcdTOdec(uint8_t val)
{
    uint8_t retval = (val >> 4) * 10 + (val & 0x0f);
    return retval;
}

static uint8_t decTObcd(uint8_t val)
{
    uint8_t retval = ((val / 10) << 4) + (val % 10);
    return retval;
}
#endif

// 正常返回 1
int MODE_RTC8564_Read_time (uint32_t *sec)
{
    int retval = 0;
    time_t temp_time = 123;
#ifdef Exist_RTC_Clock
    uint8_t temp_array[10];
    if (RTC8564_Init_flag == 0)
    {
        return retval;
    }
    
    temp_array[0] = 0x02;
    retval = Base_IIC_Send_DATA(RTC8564_ADDR,temp_array,1,1,1,1);
    memset(temp_array,0,sizeof(temp_array));
    if (retval) {
        if(Base_IIC_Receive_DATA(RTC8564_ADDR,temp_array,1,7,1) != 1)
        {
            retval = 0;
        }
    }
    if (retval) {
        timeinfo.tm_sec = bcdTOdec(temp_array[0] & 0x7F);
        timeinfo.tm_min = bcdTOdec(temp_array[1] & 0x7F);
        timeinfo.tm_hour = bcdTOdec(temp_array[2] & 0x3F);
        timeinfo.tm_mday = bcdTOdec(temp_array[3] & 0x3F);
        timeinfo.tm_wday = bcdTOdec(temp_array[4] & 0x07);
        timeinfo.tm_mon = bcdTOdec(temp_array[5] & 0x1F) - 1;
        timeinfo.tm_year = (bcdTOdec(temp_array[6]) + 2000);
        timeinfo.tm_isdst = 0;

        timeinfo.tm_year -= 1900;
        timeinfo.tm_hour -= 8;
        temp_time = mktime(&timeinfo);
    }
#endif
    *sec = temp_time;
    return retval;
}

// 正常返回 1
int MODE_RTC8564_Write_time (uint32_t sec)
{
    int retval = 0;
#ifdef Exist_RTC_Clock
    int temp_run = 0;
    time_t temp_time = sec;
    uint8_t temp_array[10];
    if (RTC8564_Init_flag == 0)
    {
        return retval;
    }
    localtime_r(&temp_time, &timeinfo);
    if (timeinfo.tm_year >= 100)
    {
        timeinfo.tm_year = timeinfo.tm_year - 100;
    }
    temp_array[temp_run++] = 2;
    temp_array[temp_run++] = decTObcd(timeinfo.tm_sec);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_min);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_hour + 8);

    temp_array[temp_run++] = decTObcd(timeinfo.tm_mday);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_wday);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_mon + 1);
    temp_array[temp_run++] = decTObcd(timeinfo.tm_year);
    retval = 1;
    if(Base_IIC_Send_DATA(RTC8564_ADDR,temp_array,1,temp_run,1,0) == 0)
    {
        retval = 0;
    }
#endif
    return retval;
}

