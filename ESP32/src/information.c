#include"information.h"

/*
    ESP_LOGE - 错误（最低）0
    ESP_LOGW - 警告
    ESP_LOGI - 信息
    ESP_LOGD - 调试
    ESP_LOGV - 详细（最高）
*/

// static const char *TAG = "information";


int information_init (void)
{
    int retval = 0;
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("-> \n");
    ESP_LOGI(" ", "\n <------------------RUN------------------> \n ");
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.full_revision / 100;
    unsigned minor_rev = chip_info.full_revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    printf("FreeRTOS HZ is %d \n \n", CONFIG_FREERTOS_HZ);
    return retval;
}


