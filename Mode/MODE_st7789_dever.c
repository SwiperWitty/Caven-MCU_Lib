#include "MODE_st7789_dever.h"

static uint8_t LCD_Horizontal = 0;
static char LCD_Target_Model = 0;
int flag_dc = 0;

#ifdef Exist_LCD
void st7789_dever_gpio_init(int set)
{
    if (set)
    {
#if (PIN_LCD_DC != (-1))
#ifdef CONFIG_IDF_TARGET_ESP32
        gpio_pad_select_gpio(PIN_LCD_DC);
        gpio_set_direction(PIN_LCD_DC, GPIO_MODE_OUTPUT);
        gpio_set_level(PIN_LCD_DC, 1);
#else
        User_GPIO_config(1, PIN_LCD_DC, 1); // PA10 out DC
        User_GPIO_set(1, PIN_LCD_DC, 1);
#endif
#endif

#if (PIN_LCD_RST != (-1))
#ifdef CONFIG_IDF_TARGET_ESP32
        gpio_pad_select_gpio(PIN_LCD_RST);
        gpio_set_direction(PIN_LCD_RST, GPIO_MODE_OUTPUT);
        gpio_set_level(PIN_LCD_RST, 1);
#else
        User_GPIO_config(2, PIN_LCD_RST, 1); // PB0  out RES
        User_GPIO_set(2, PIN_LCD_RST, 1);
#endif
#endif

#if (PIN_LCD_CS != (-1))
#ifdef CONFIG_IDF_TARGET_ESP32
        gpio_pad_select_gpio(PIN_LCD_CS);
        gpio_set_direction(PIN_LCD_CS, GPIO_MODE_OUTPUT);
        gpio_set_level(PIN_LCD_CS, 0);
#else
        User_GPIO_config(2, PIN_LCD_CS, 1);
        User_GPIO_set(2, PIN_LCD_CS, 1);
#endif
#endif
    }
    else
    {
    }
}

void st7789_dever_delay(int time)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    vTaskDelay(time);
#else
#ifdef NOP
    SYS_Base_Delay(time, 10000);
#endif
#endif
}

#ifdef CONFIG_IDF_TARGET_ESP32

#define MODE_st7789_dever_Writ_Bus(x) LCD_Writ_Bus(x)
#define MODE_st7789_dever_GPIO_CS(x) gpio_set_level(PIN_LCD_CS, x);
#define MODE_st7789_dever_GPIO_DC(x) gpio_set_level(PIN_LCD_DC, x);
#define MODE_st7789_dever_GPIO_RST(x) gpio_set_level(PIN_LCD_RST, x);

void preposition_DC_Fun(spi_transaction_t *t)
{
    if (flag_dc)
    {
        MODE_st7789_dever_GPIO_DC(1);
    }
    else
    {
        MODE_st7789_dever_GPIO_DC(0);
    }
}

spi_device_handle_t LCD_spi = {0};
int Base_SPI_Init(int Set)
{
    int retval = 0;
    if (Set)
    {
        esp_err_t ret;
        spi_bus_config_t buscfg = {
            .miso_io_num = PIN_LCD_MISO,
            .mosi_io_num = PIN_LCD_MOSI,
            .sclk_io_num = PIN_LCD_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = (16 * 320 * 2 + 8)};
        spi_device_interface_config_t devcfg = {
            .clock_speed_hz = 26 * 1000 * 1000, // Clock out at 26 MHz
            .mode = 3,                          // SPI mode 0-3
            .spics_io_num = -1,                 // CS pin
            .queue_size = 7,                    // We want to be able to queue 7 transactions at a time
            .pre_cb = preposition_DC_Fun,       // 传输开始时的回调函数
        };
        // Initialize the SPI bus
        ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
        ESP_ERROR_CHECK(ret);
        // Attach the LCD to the SPI bus
        ret = spi_bus_add_device(LCD_HOST, &devcfg, &LCD_spi);
        ESP_ERROR_CHECK(ret);
    }
    else
    {
    }
    return retval;
}

static void EPS_SPI_SendData(const uint8_t *data, int num, int cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    int send_size = 0;

    if (data == 0)
    {
        return;
    }

    memset(&t, 0, sizeof(t)); // Zero out the transaction
    if (cmd)
    {
        t.user = (void *)1;
    }
    else
    {
        t.user = (void *)0;
    }
    send_size = 8 * num;
    t.length = send_size;                           // Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;                             // Data
    ret = spi_device_polling_transmit(LCD_spi, &t); // Transmit!
    // ret = spi_device_transmit(LCD_spi, &t);			// Transmit!

    assert(ret == ESP_OK);
    if (ret == ESP_OK)
    {
        // printf("spi send run : %d \n",num);
    }
}

static void LCD_Writ_Bus(uint8_t data)
{
    // SPI_Send_DATA(data);
    esp_err_t ret;
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));                       // Zero out the transaction
    t.length = 1 * 8;                               // Len is in bytes, transaction length is in bits.
    t.tx_buffer = &data;                            // Data
    ret = spi_device_polling_transmit(LCD_spi, &t); // Transmit!
    assert(ret == ESP_OK);                          // Should have had no issues.
}

#else
#define MODE_st7789_dever_Writ_Bus(x) Base_SPI_Send_Data(m_SPI_CH2, x)
#define MODE_st7789_dever_GPIO_CS(x) Base_SPI_CS_Set(m_SPI_CH2, 1, x);
#define MODE_st7789_dever_GPIO_DC(x) User_GPIO_set(1, PIN_LCD_DC, x);
#define MODE_st7789_dever_GPIO_RST(x) User_GPIO_set(0, PIN_LCD_RST, x);
#endif

void set_flag_dc(int n)
{
    flag_dc = n;
#ifndef CONFIG_IDF_TARGET_ESP32
    MODE_st7789_dever_GPIO_DC(n); // 写命令
#endif
}

/*
    支持同型号同时使用
*/
void MODE_st7789_dever_CS(uint8_t data)
{
#ifdef Exist_LCD
    switch (LCD_Target_Model)
    {
    case 11:
        // MODE_st7789_dever_GPIO_CS(data);
        break;
    default:
        // MODE_st7789_dever_GPIO_CS(data);
        break;
    }
#endif
}

/*
    这个对速度要求不高
    支持同型号同时使用
*/
void MODE_st7789_dever_RST(uint8_t data)
{
#ifdef Exist_LCD
    switch (LCD_Target_Model)
    {
    case 11:
        MODE_st7789_dever_GPIO_RST(data);
        break;
    default:
        MODE_st7789_dever_GPIO_RST(data);
        break;
    }
#endif
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
static void LCD_WR_CMD(uint8_t data)
{
    MODE_st7789_dever_CS(1);
    set_flag_dc(0);
    MODE_st7789_dever_Writ_Bus(data);
    set_flag_dc(1);
    // MODE_st7789_dever_CS(0); // 写命令之后必然要写数据，所以不要取消片选
}

#endif
/*
 */
int MODE_st7789_dever_Set_TargetModel(char set)
{
    int retval = 0;
    if (set > 10 && set < 20) // MODE_st7789
    {
        LCD_Target_Model = set;
        retval = set;
    }
    return retval;
}

/*
    符合则设置方向
    不符合则理解为查询方向
*/
int MODE_st7789_dever_Set_Horizontal(char set)
{
    int retval = 0;
    if (set < 4) // 0 1 2 3
    {
        LCD_Horizontal = set;
    }
    else
    {
    }
    retval = LCD_Horizontal;
    return retval;
}

/*

*/
void MODE_st7789_dever_WR_Data(uint16_t data)
{
#ifdef Exist_LCD
    uint8_t temp_array[2];
    temp_array[0] = (data >> 8) & 0xff;
    temp_array[1] = data & 0xff;
    MODE_st7789_dever_CS(1); // 写数据之前必然要写命令，所以不要开启片选
    MODE_st7789_dever_Writ_Bus(temp_array[0]);
    MODE_st7789_dever_Writ_Bus(temp_array[1]);
    MODE_st7789_dever_CS(0);
#endif
}

/*
 */
void MODE_st7789_dever_Send_Data(uint8_t *data, int num)
{
#ifdef Exist_LCD
    MODE_st7789_dever_CS(1); // 写数据之前必然要写命令，所以不要开启片选
#ifdef CONFIG_IDF_TARGET_ESP32
    EPS_SPI_SendData(data, num, 1);
#else
    Base_SPI_DMA_Send_Data(m_SPI_CH2, data, num);
#endif
//	MODE_st7789_dever_CS (0);   // 应该被中断处理了
#endif
}

void MODE_st7789_dever_1_28_config(void)
{
#ifdef Exist_LCD
    LCD_WR_CMD(0xEF);
    LCD_WR_CMD(0xEB);
    MODE_st7789_dever_Writ_Bus(0x14);

    LCD_WR_CMD(0xFE);
    LCD_WR_CMD(0xEF);
    LCD_WR_CMD(0xEB);
    MODE_st7789_dever_Writ_Bus(0x14);
    LCD_WR_CMD(0x84);
    MODE_st7789_dever_Writ_Bus(0x40);
    LCD_WR_CMD(0x85);
    MODE_st7789_dever_Writ_Bus(0xFF);
    LCD_WR_CMD(0x86);
    MODE_st7789_dever_Writ_Bus(0xFF);
    LCD_WR_CMD(0x87);
    MODE_st7789_dever_Writ_Bus(0xFF);
    LCD_WR_CMD(0x88);
    MODE_st7789_dever_Writ_Bus(0x0A);
    LCD_WR_CMD(0x89);
    MODE_st7789_dever_Writ_Bus(0x21);
    LCD_WR_CMD(0x8A);
    MODE_st7789_dever_Writ_Bus(0x00);
    LCD_WR_CMD(0x8B);
    MODE_st7789_dever_Writ_Bus(0x80);
    LCD_WR_CMD(0x8C);
    MODE_st7789_dever_Writ_Bus(0x01);
    LCD_WR_CMD(0x8D);
    MODE_st7789_dever_Writ_Bus(0x01);
    LCD_WR_CMD(0x8E);
    MODE_st7789_dever_Writ_Bus(0xFF);
    LCD_WR_CMD(0x8F);
    MODE_st7789_dever_Writ_Bus(0xFF);

    LCD_WR_CMD(0xB6);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x20);

    LCD_WR_CMD(0x36);

    switch (LCD_Horizontal)
    {
    case 0:
        MODE_st7789_dever_Writ_Bus(0x08);
        break;
    case 1:
        MODE_st7789_dever_Writ_Bus(0xC8);
        break;
    case 2:
        MODE_st7789_dever_Writ_Bus(0x68);
        break;
    default:
        MODE_st7789_dever_Writ_Bus(0xA8);
        break;
    }
    LCD_WR_CMD(0x3A);
    MODE_st7789_dever_Writ_Bus(0x05);

    LCD_WR_CMD(0x90);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x08);
    LCD_WR_CMD(0xBD);
    MODE_st7789_dever_Writ_Bus(0x06);
    LCD_WR_CMD(0xBC);
    MODE_st7789_dever_Writ_Bus(0x00);
    LCD_WR_CMD(0xFF);
    MODE_st7789_dever_Writ_Bus(0x60);
    MODE_st7789_dever_Writ_Bus(0x01);
    MODE_st7789_dever_Writ_Bus(0x04);
    LCD_WR_CMD(0xC3);
    MODE_st7789_dever_Writ_Bus(0x13);
    LCD_WR_CMD(0xC4);
    MODE_st7789_dever_Writ_Bus(0x13);
    LCD_WR_CMD(0xC9);
    MODE_st7789_dever_Writ_Bus(0x22);
    LCD_WR_CMD(0xBE);
    MODE_st7789_dever_Writ_Bus(0x11);
    LCD_WR_CMD(0xE1);
    MODE_st7789_dever_Writ_Bus(0x10);
    MODE_st7789_dever_Writ_Bus(0x0E);
    LCD_WR_CMD(0xDF);
    MODE_st7789_dever_Writ_Bus(0x21);
    MODE_st7789_dever_Writ_Bus(0x0c);
    MODE_st7789_dever_Writ_Bus(0x02);
    LCD_WR_CMD(0xF0);
    MODE_st7789_dever_Writ_Bus(0x45);
    MODE_st7789_dever_Writ_Bus(0x09);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x26);
    MODE_st7789_dever_Writ_Bus(0x2A);
    LCD_WR_CMD(0xF1);
    MODE_st7789_dever_Writ_Bus(0x43);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x72);
    MODE_st7789_dever_Writ_Bus(0x36);
    MODE_st7789_dever_Writ_Bus(0x37);
    MODE_st7789_dever_Writ_Bus(0x6F);

    LCD_WR_CMD(0xF2);
    MODE_st7789_dever_Writ_Bus(0x45);
    MODE_st7789_dever_Writ_Bus(0x09);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x26);
    MODE_st7789_dever_Writ_Bus(0x2A);

    LCD_WR_CMD(0xF3);
    MODE_st7789_dever_Writ_Bus(0x43);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x72);
    MODE_st7789_dever_Writ_Bus(0x36);
    MODE_st7789_dever_Writ_Bus(0x37);
    MODE_st7789_dever_Writ_Bus(0x6F);

    LCD_WR_CMD(0xED);
    MODE_st7789_dever_Writ_Bus(0x1B);
    MODE_st7789_dever_Writ_Bus(0x0B);
    LCD_WR_CMD(0xAE);
    MODE_st7789_dever_Writ_Bus(0x77);
    LCD_WR_CMD(0xCD);
    MODE_st7789_dever_Writ_Bus(0x63);
    LCD_WR_CMD(0x70);
    MODE_st7789_dever_Writ_Bus(0x07);
    MODE_st7789_dever_Writ_Bus(0x07);
    MODE_st7789_dever_Writ_Bus(0x04);
    MODE_st7789_dever_Writ_Bus(0x0E);
    MODE_st7789_dever_Writ_Bus(0x0F);
    MODE_st7789_dever_Writ_Bus(0x09);
    MODE_st7789_dever_Writ_Bus(0x07);
    MODE_st7789_dever_Writ_Bus(0x08);
    MODE_st7789_dever_Writ_Bus(0x03);
    LCD_WR_CMD(0xE8);
    MODE_st7789_dever_Writ_Bus(0x34);
    LCD_WR_CMD(0x62);
    MODE_st7789_dever_Writ_Bus(0x18);
    MODE_st7789_dever_Writ_Bus(0x0D);
    MODE_st7789_dever_Writ_Bus(0x71);
    MODE_st7789_dever_Writ_Bus(0xED);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x18);
    MODE_st7789_dever_Writ_Bus(0x0F);
    MODE_st7789_dever_Writ_Bus(0x71);
    MODE_st7789_dever_Writ_Bus(0xEF);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x70);
    LCD_WR_CMD(0x63);
    MODE_st7789_dever_Writ_Bus(0x18);
    MODE_st7789_dever_Writ_Bus(0x11);
    MODE_st7789_dever_Writ_Bus(0x71);
    MODE_st7789_dever_Writ_Bus(0xF1);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x18);
    MODE_st7789_dever_Writ_Bus(0x13);
    MODE_st7789_dever_Writ_Bus(0x71);
    MODE_st7789_dever_Writ_Bus(0xF3);
    MODE_st7789_dever_Writ_Bus(0x70);
    MODE_st7789_dever_Writ_Bus(0x70);
    LCD_WR_CMD(0x64);
    MODE_st7789_dever_Writ_Bus(0x28);
    MODE_st7789_dever_Writ_Bus(0x29);
    MODE_st7789_dever_Writ_Bus(0xF1);
    MODE_st7789_dever_Writ_Bus(0x01);
    MODE_st7789_dever_Writ_Bus(0xF1);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x07);
    LCD_WR_CMD(0x66);
    MODE_st7789_dever_Writ_Bus(0x3C);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0xCD);
    MODE_st7789_dever_Writ_Bus(0x67);
    MODE_st7789_dever_Writ_Bus(0x45);
    MODE_st7789_dever_Writ_Bus(0x45);
    MODE_st7789_dever_Writ_Bus(0x10);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x00);
    LCD_WR_CMD(0x67);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x3C);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x01);
    MODE_st7789_dever_Writ_Bus(0x54);
    MODE_st7789_dever_Writ_Bus(0x10);
    MODE_st7789_dever_Writ_Bus(0x32);
    MODE_st7789_dever_Writ_Bus(0x98);
    LCD_WR_CMD(0x74);
    MODE_st7789_dever_Writ_Bus(0x10);
    MODE_st7789_dever_Writ_Bus(0x85);
    MODE_st7789_dever_Writ_Bus(0x80);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x00);
    MODE_st7789_dever_Writ_Bus(0x4E);
    MODE_st7789_dever_Writ_Bus(0x00);
    LCD_WR_CMD(0x98);
    MODE_st7789_dever_Writ_Bus(0x3e);
    MODE_st7789_dever_Writ_Bus(0x07);

    LCD_WR_CMD(0x35);
    LCD_WR_CMD(0x21);
    LCD_WR_CMD(0x11);
    st7789_dever_delay(120);
    LCD_WR_CMD(0x29);
    st7789_dever_delay(100);
#endif
}

/*

*/
void MODE_st7789_dever_Set_Address(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#ifdef Exist_LCD
    uint16_t x_sta = 0, y_sta = 0, x_end = 0, y_end = 0;
    uint8_t retval = 0;
    switch (LCD_Target_Model)
    {
    case 11: // 1.14
    {
        retval = 1;
    }
    break;
    case 12: // 1.28
    {
        if (LCD_Horizontal == 0 || LCD_Horizontal == 2)
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1);
            y_end = (y2);
        }
        else if (LCD_Horizontal == 1)
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1 + 80);
            y_end = (y2 + 80);
        }
        else
        {
            x_sta = (x1 + 80);
            x_end = (x2 + 80);
            y_sta = (y1);
            y_end = (y2);
        }
        retval = 1;
    }
    break;
    case 13: // 1.3
    {
        if (LCD_Horizontal == 0 || LCD_Horizontal == 2)
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1);
            y_end = (y2);
        }
        else if (LCD_Horizontal == 1)
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1 + 80);
            y_end = (y2 + 80);
        }
        else
        {
            x_sta = (x1 + 80);
            x_end = (x2 + 80);
            y_sta = (y1);
            y_end = (y2);
        }
        retval = 1;
    }
    break;
    case 14: // 1.69
    {
        if (LCD_Horizontal == 1)
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1 + 20);
            y_end = (y2 + 20);
        }
        else
        {
            x_sta = (x1 + 20);
            x_end = (x2 + 20);
            y_sta = (y1);
            y_end = (y2);
        }
        retval = 1;
    }
    break;
    case 15: // 1.90
    {
        if (LCD_Horizontal == 0 || LCD_Horizontal == 1)
        {
            x_sta = (x1 + 35);
            x_end = (x2 + 35);
            y_sta = (y1);
            y_end = (y2);
        }
        else
        {
            x_sta = (x1);
            x_end = (x2);
            y_sta = (y1 + 35);
            y_end = (y2 + 35);
        }
        retval = 1;
    }
    break;
    default:
        retval = 0;
        break;
    }
    if (retval)
    {
        LCD_WR_CMD(0x2a); // 列地址设置
        MODE_st7789_dever_WR_Data(x_sta);
        MODE_st7789_dever_WR_Data(x_end);
        LCD_WR_CMD(0x2b); // 行地址设置
        MODE_st7789_dever_WR_Data(y_sta);
        MODE_st7789_dever_WR_Data(y_end);
        LCD_WR_CMD(0x2c); // 储存器写
    }
#endif
}

/*
    这个函数属于初始化函数，所以如果初始成功就会返回0，不成功就返回1，然初始化检测函数while()卡死
*/
int MODE_st7789_dever_Init(int set)
{
    int retval = 0;
#ifdef Exist_LCD
    if (LCD_Target_Model == 0)
    {
        retval = 1;
        return retval;
    }

    st7789_dever_gpio_init(set);
    MODE_st7789_dever_RST(0);
#ifdef CONFIG_IDF_TARGET_ESP32
    Base_SPI_Init(set);
#else
    Base_SPI_Init(m_SPI_CH2, 8, set);
#endif
    st7789_dever_delay(50);
    // st7789_dever_gpio_init(set);
    MODE_st7789_dever_Writ_Bus(0x00);

    st7789_dever_delay(200);
    MODE_st7789_dever_RST(1);
    st7789_dever_delay(100);

    //************* Start Initial Sequence **********//
    if (LCD_Target_Model == 12)
    {
        MODE_st7789_dever_1_28_config();
        retval = 0;
        return retval;
    }

    LCD_WR_CMD(0x36); // res
    switch (LCD_Horizontal)
    {
    case 0:
        MODE_st7789_dever_Writ_Bus(0x00);
        break;
    case 1:
        MODE_st7789_dever_Writ_Bus(0xC0);
        break;
    case 2:
        MODE_st7789_dever_Writ_Bus(0x70);
        break;
    default:
        MODE_st7789_dever_Writ_Bus(0xA0);
        break;
    }
    //
    switch (LCD_Target_Model)
    {
    case 11:
        /* code */
        break;
    case 12: // 1.28
        /* other */
        break;
    case 13: // 1.3
    {
        LCD_WR_CMD(0x3A);
        MODE_st7789_dever_Writ_Bus(0x05);

        LCD_WR_CMD(0xB2);
        MODE_st7789_dever_Writ_Bus(0x0C);
        MODE_st7789_dever_Writ_Bus(0x0C);
        MODE_st7789_dever_Writ_Bus(0x00);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x33);

        LCD_WR_CMD(0xB7); // Gate Control
        MODE_st7789_dever_Writ_Bus(0x35);
        LCD_WR_CMD(0xBB); // VCOM Setting
        MODE_st7789_dever_Writ_Bus(0x19);

        LCD_WR_CMD(0xC0); // LCM Control
        MODE_st7789_dever_Writ_Bus(0x2C);

        LCD_WR_CMD(0xC2); // VDV and VRH Command Enable
        MODE_st7789_dever_Writ_Bus(0x01);
        LCD_WR_CMD(0xC3); // VRH Set
        MODE_st7789_dever_Writ_Bus(0x12);
        LCD_WR_CMD(0xC4); // VDV Set
        MODE_st7789_dever_Writ_Bus(0x20);

        LCD_WR_CMD(0xC6); // Frame Rate Control in Normal Mode
        MODE_st7789_dever_Writ_Bus(0x0F);

        LCD_WR_CMD(0xD0); // Power Control 1
        MODE_st7789_dever_Writ_Bus(0xA4);
        MODE_st7789_dever_Writ_Bus(0xA1);

        LCD_WR_CMD(0xE0); // Positive Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xD0);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x0D);
        MODE_st7789_dever_Writ_Bus(0x11);
        MODE_st7789_dever_Writ_Bus(0x13);
        MODE_st7789_dever_Writ_Bus(0x2B);
        MODE_st7789_dever_Writ_Bus(0x3F);
        MODE_st7789_dever_Writ_Bus(0x54);
        MODE_st7789_dever_Writ_Bus(0x4C);
        MODE_st7789_dever_Writ_Bus(0x18);
        MODE_st7789_dever_Writ_Bus(0x0D);
        MODE_st7789_dever_Writ_Bus(0x0B);
        MODE_st7789_dever_Writ_Bus(0x1F);
        MODE_st7789_dever_Writ_Bus(0x23);

        LCD_WR_CMD(0xE1); // Negative Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xD0);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x0C);
        MODE_st7789_dever_Writ_Bus(0x11);
        MODE_st7789_dever_Writ_Bus(0x13);
        MODE_st7789_dever_Writ_Bus(0x2C);
        MODE_st7789_dever_Writ_Bus(0x3F);
        MODE_st7789_dever_Writ_Bus(0x44);
        MODE_st7789_dever_Writ_Bus(0x51);
        MODE_st7789_dever_Writ_Bus(0x2F);
        MODE_st7789_dever_Writ_Bus(0x1F);
        MODE_st7789_dever_Writ_Bus(0x1F);
        MODE_st7789_dever_Writ_Bus(0x20);
        MODE_st7789_dever_Writ_Bus(0x23);

        LCD_WR_CMD(0x21); // Display Inversion On
        LCD_WR_CMD(0x11); // Sleep Out
        LCD_WR_CMD(0x29); // Display On
    }
    break;
    case 14: // 1.69
    {
        LCD_WR_CMD(0x3A); /* RGB 5-6-5-bit  */
        MODE_st7789_dever_Writ_Bus(0x05);

        LCD_WR_CMD(0xB2); /* Porch Setting */
        MODE_st7789_dever_Writ_Bus(0x0B);
        MODE_st7789_dever_Writ_Bus(0x0B);
        MODE_st7789_dever_Writ_Bus(0x00);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x35);

        LCD_WR_CMD(0xB7); // Gate Control
        MODE_st7789_dever_Writ_Bus(0x11);
        LCD_WR_CMD(0xBB); // VCOM Setting
        MODE_st7789_dever_Writ_Bus(0x35);

        LCD_WR_CMD(0xC0); // LCM Control
        MODE_st7789_dever_Writ_Bus(0x2C);

        LCD_WR_CMD(0xC2); // VDV and VRH Command Enable
        MODE_st7789_dever_Writ_Bus(0x01);
        LCD_WR_CMD(0xC3); // VRH Set
        MODE_st7789_dever_Writ_Bus(0x0D);
        LCD_WR_CMD(0xC4); // VDV Set
        MODE_st7789_dever_Writ_Bus(0x20);

        LCD_WR_CMD(0xC6); // Frame Rate Control in Normal Mode
        MODE_st7789_dever_Writ_Bus(0x13);

        LCD_WR_CMD(0xD0); // Power Control 1
        MODE_st7789_dever_Writ_Bus(0xA4);
        MODE_st7789_dever_Writ_Bus(0xA1);

        LCD_WR_CMD(0xD6);
        MODE_st7789_dever_Writ_Bus(0xA1);

        LCD_WR_CMD(0xE0); // Positive Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xF0);
        MODE_st7789_dever_Writ_Bus(0x06);
        MODE_st7789_dever_Writ_Bus(0x0B);
        MODE_st7789_dever_Writ_Bus(0x0A);
        MODE_st7789_dever_Writ_Bus(0x09);
        MODE_st7789_dever_Writ_Bus(0x26);
        MODE_st7789_dever_Writ_Bus(0x29);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x41);
        MODE_st7789_dever_Writ_Bus(0x18);
        MODE_st7789_dever_Writ_Bus(0x16);
        MODE_st7789_dever_Writ_Bus(0x15);
        MODE_st7789_dever_Writ_Bus(0x29);
        MODE_st7789_dever_Writ_Bus(0x2D);

        LCD_WR_CMD(0xE1); // Negative Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xF0);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x08);
        MODE_st7789_dever_Writ_Bus(0x08);
        MODE_st7789_dever_Writ_Bus(0x07);
        MODE_st7789_dever_Writ_Bus(0x03);
        MODE_st7789_dever_Writ_Bus(0x28);
        MODE_st7789_dever_Writ_Bus(0x32);
        MODE_st7789_dever_Writ_Bus(0x40);
        MODE_st7789_dever_Writ_Bus(0x3B);
        MODE_st7789_dever_Writ_Bus(0x19);
        MODE_st7789_dever_Writ_Bus(0x18);
        MODE_st7789_dever_Writ_Bus(0x2A);
        MODE_st7789_dever_Writ_Bus(0x2E);

        LCD_WR_CMD(0xE4);
        MODE_st7789_dever_Writ_Bus(0x25);
        MODE_st7789_dever_Writ_Bus(0x00);
        MODE_st7789_dever_Writ_Bus(0x00);

        LCD_WR_CMD(0x21); // Display Inversion On
        LCD_WR_CMD(0x11); // Sleep Out
        LCD_WR_CMD(0x29); // Display On
    }
    break;
    case 15: // 1.90
    {
        LCD_WR_CMD(0x3A); /* RGB 5-6-5-bit  */
        MODE_st7789_dever_Writ_Bus(0x05);

        LCD_WR_CMD(0xB2); /* Porch Setting */
        MODE_st7789_dever_Writ_Bus(0x0C);
        MODE_st7789_dever_Writ_Bus(0x0C);
        MODE_st7789_dever_Writ_Bus(0x00);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x35);

        LCD_WR_CMD(0xB7); // Gate Control
        MODE_st7789_dever_Writ_Bus(0x35);
        LCD_WR_CMD(0xBB); // VCOM Setting
        MODE_st7789_dever_Writ_Bus(0x1A);

        LCD_WR_CMD(0xC0); // LCM Control
        MODE_st7789_dever_Writ_Bus(0x2C);
        LCD_WR_CMD(0xC2); // VDV and VRH Command Enable
        MODE_st7789_dever_Writ_Bus(0x01);
        LCD_WR_CMD(0xC3); // VRH Set
        MODE_st7789_dever_Writ_Bus(0x0B);
        LCD_WR_CMD(0xC4); // VDV Set
        MODE_st7789_dever_Writ_Bus(0x20);
        LCD_WR_CMD(0xC6); // Frame Rate Control in Normal Mode
        MODE_st7789_dever_Writ_Bus(0x0F);

        LCD_WR_CMD(0xD0); // Power Control 1
        MODE_st7789_dever_Writ_Bus(0xA4);
        MODE_st7789_dever_Writ_Bus(0xA1);

        LCD_WR_CMD(0x21);
        LCD_WR_CMD(0xE0); // Positive Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xF0);
        MODE_st7789_dever_Writ_Bus(0x00);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x04);
        MODE_st7789_dever_Writ_Bus(0x05);
        MODE_st7789_dever_Writ_Bus(0x29);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x3E);
        MODE_st7789_dever_Writ_Bus(0x38);
        MODE_st7789_dever_Writ_Bus(0x12);
        MODE_st7789_dever_Writ_Bus(0x12);
        MODE_st7789_dever_Writ_Bus(0x28);
        MODE_st7789_dever_Writ_Bus(0x30);

        LCD_WR_CMD(0xE1); // Negative Voltage Gamma Control
        MODE_st7789_dever_Writ_Bus(0xF0);
        MODE_st7789_dever_Writ_Bus(0x07);
        MODE_st7789_dever_Writ_Bus(0x0A);
        MODE_st7789_dever_Writ_Bus(0x0D);
        MODE_st7789_dever_Writ_Bus(0x0B);
        MODE_st7789_dever_Writ_Bus(0x07);
        MODE_st7789_dever_Writ_Bus(0x28);
        MODE_st7789_dever_Writ_Bus(0x33);
        MODE_st7789_dever_Writ_Bus(0x3E);
        MODE_st7789_dever_Writ_Bus(0x36);
        MODE_st7789_dever_Writ_Bus(0x14);
        MODE_st7789_dever_Writ_Bus(0x14);
        MODE_st7789_dever_Writ_Bus(0x29);
        MODE_st7789_dever_Writ_Bus(0x32);

        LCD_WR_CMD(0x21); // Display Inversion On
        LCD_WR_CMD(0x11); // Sleep Out
        LCD_WR_CMD(0x29); // Display On
    }
    break;
    default:
        retval = 1;
        break;
    }
    MODE_st7789_dever_CS(0); // important
    st7789_dever_delay(100);
#endif
    return retval;
}
