#ifndef _MODE_RTC8564_H_
#define _MODE_RTC8564_H_

#include "stdint.h"
#include "string.h"

#define RTC8564_ADDR    0x51

#ifdef CONFIG_IDF_TARGET_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"

#include"Items.h"

    #define PIN_RTC_SDA    (16)
    #define PIN_RTC_SCL    (17)
    #define RTC_I2C_MASTER_FREQ_HZ  1000
    #define RTC_I2C_MASTER_NUM      I2C_NUM_0
#else
#include "Base.h"
#include "Caven_Type.h"
    #define PIN_RTC_SDA    (16)
    #define PIN_RTC_SCL    (17)
#endif

int MODE_RTC8564_Init (int set);
int MODE_RTC8564_Write_time (uint32_t sec);
int MODE_RTC8564_Read_time (uint32_t *sec);

#endif
