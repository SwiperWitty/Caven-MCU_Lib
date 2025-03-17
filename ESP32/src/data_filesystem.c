/* Example of FAT filesystem on external Flash.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This sample shows how to store files inside a FAT filesystem.
   FAT filesystem is stored in a partition inside SPI flash, using the
   flash wear levelling library.
*/
#include "data_filesystem.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_flash.h"
#include "esp_flash_spi_init.h"
#include "esp_partition.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "filesys";
#define BASE_PATH "storage"
#define MOUNT_PATH "/data"
#define Start_ADDR 128      // 这个位置放一些必要信息 

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char *base_path = "/extflash";
const char *flash_file = "tag.txt";
size_t bytes_total, bytes_free;

// static esp_flash_t* example_init_ext_flash(void);
static const esp_partition_t* example_add_partition(esp_flash_t* ext_flash, const char* partition_label);
static void example_list_data_partitions(void);
static bool example_mount_fatfs(const char* partition_label);
static void example_get_fatfs_usage(size_t* out_total_bytes, size_t* out_free_bytes);
static void initialize_filesystem(void);

static char fat_file_str[100];
static char Start_str_len = 0;
static int title_a = 0,title_b = 0;

void filesystem_save_title (int a,int b)
{
    FILE *f = NULL;
    sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    ESP_LOGI(TAG, "file_str %s", fat_file_str);
    f = fopen(fat_file_str, "rb+");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        Start_str_len = 0;
        while(1)
        {
            vTaskDelay(100);
        }
    }
    else
    {
        fprintf(f, "ESP32 File system version %s ", esp_get_idf_version());      // 写入内容
        fprintf(f, "dataA:[%d] dataB:[%d]",a,b);
        fclose(f);
        ESP_LOGI(TAG, "File written title,dataA:[%d] dataB:[%d]",a,b);
        title_a = a;
        title_b = b;
        Start_str_len = 1;
    }
}

void filesystem_load_title (int *a,int *b)
{
    if (a != NULL && b != NULL)
    {
        *a = title_a;
        *b = title_b;
    }
    
}

void filesystem_init (void)
{
    int retval = 0;
    FILE *f = NULL;
    char *str_pointer = NULL;
    int temp_num,str_len;
    uint8_t temp_data[64];
    char line[128];
    initialize_filesystem();
    // Open file for reading
    ESP_LOGI(TAG, "Reading file");
    sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    ESP_LOGI(TAG, "file_str %s", fat_file_str);
    f = fopen(fat_file_str, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
    }
    else
    {
        fgets(line, sizeof(line), f);
        fclose(f);
        Start_str_len = strlen(line);
        if (Start_str_len)
        {
            if (Start_str_len > 128)
            {
                Start_str_len = 128;
            }
            
            ESP_LOGI(TAG, "Read from file: '%s' <-- Start_str_len: %d", line,Start_str_len);
            str_pointer = strstr(line,"dataA:[");
            if (str_pointer != NULL)
            {
                str_len = strlen("dataA:[");
                temp_num = atoi(str_pointer+str_len);   // 指针偏移
                ESP_LOGI(TAG, "%s -> get dataA: %d",str_pointer+str_len,temp_num);
                title_a = temp_num;
            }
            str_pointer = strstr(line,"dataB:[");
            if (str_pointer != NULL)
            {
                str_len = strlen("dataB:[");
                temp_num = atoi(str_pointer+str_len);
                ESP_LOGI(TAG, "%s -> get dataB: %d",str_pointer+str_len,temp_num);
                title_b = temp_num;
            }
        }
    }

    if(Start_str_len > 0)
    {
        ESP_LOGW(TAG, "file using ,Don't write ");
        retval = 0;
    }
    else
    {
        ESP_LOGW(TAG, "build & Opening file");
        f = fopen(fat_file_str, "w");       // 新建文件，清空文件内容
        vTaskDelay(100);
        fclose(f);
        retval = 1;
    }

    if(retval == 1)
    {
        filesystem_save_title (0,0);
    }
    filesystem_get_space_size (&temp_num);
}

/*
    ftell fseek fread fwrite fopen fclose
*/

/*
    指针是返回当前位置
    返回值是总内存大小
*/
size_t filesystem_get_space_size (int *addr)
{
    size_t retval = 0;
    FILE *f = NULL;
    sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    ESP_LOGI(TAG, "file_str %s", fat_file_str);
    f = fopen(fat_file_str, "rb");

    if(f != NULL)
    {
        fseek(f,0,SEEK_END);    // 把光标移到文件末尾
        retval = ftell(f);      // 大文件,可能需要更换函数
        fclose(f);
        if (addr != NULL)
        {
            *addr = retval;
        }
        retval = bytes_total;
        ESP_LOGW(TAG, "fat: %d byte,file: %d byte \n", bytes_total,*addr);
    }

    return retval;
}

FILE *read_open_f = NULL;
int filesystem_read_mode (int set)
{
    int retval = 0;

    sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    // ESP_LOGI(TAG, "file_str %s", fat_file_str);
    if (set != 0 && read_open_f == NULL)
    {
        read_open_f = fopen(fat_file_str, "rb");
        if (read_open_f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for reading");
        }
        else if (Start_str_len == 0)
        {
            retval = 1;
            ESP_LOGW(TAG, "file empty !!! ");
        }
        else
        {
            // ESP_LOGI(TAG, "read file open ");
        }
    }
    else if(set == 0)
    {
        if (read_open_f != NULL) {
            fclose(read_open_f);
            read_open_f = NULL;
            // ESP_LOGI(TAG, "read file close ");
        }
    }

    return retval;
}

FILE *write_open_f = NULL;

// 返回是否可写状态，0为可写
int filesystem_get_write_mode_flag (void)
{
    int retval = 0;
    if (write_open_f != NULL)
    {
        retval = 1;
    }
    return retval;
}

int filesystem_write_mode (int set)
{
    int retval = 0;

    sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    // ESP_LOGI(TAG, "file_str %s", fat_file_str);
    if (set != 0 && write_open_f == NULL)
    {
        write_open_f = fopen(fat_file_str, "rb+");
        if (write_open_f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for reading");
        }
        else if (Start_str_len == 0)
        {
            retval = 1;
            ESP_LOGW(TAG, "file empty !!! ");
        }
        else
        {
            // ESP_LOGI(TAG, "write file open ");
        }
    }
    else if(set == 0)
    {
        if (write_open_f != NULL) {
            fclose(write_open_f);
            write_open_f = NULL;
            // ESP_LOGI(TAG, "write file close ");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to open file for reading");
        }
    }

    return retval;
}

/*
    写文件
    fwrite
*/
int filesystem_write_data (int addr,void *data,int size)
{
    int retval = 0;
    if (addr < 0 || data == NULL || size <= 0 || write_open_f == NULL)
    {
        retval = 1;
        ESP_LOGW(TAG, "Write warning");
    }
    else 
    {
        if (Start_str_len == 0)
        {
            retval = 1;
            ESP_LOGW(TAG, "file empty !!! ");
        }
        else
        {
            addr = addr + Start_ADDR;
            fseek(write_open_f,addr,SEEK_SET);             // 开始偏移
            fwrite(data,sizeof(uint8_t),size,write_open_f);

            ESP_LOGI(TAG, "Write succ,Start addr:0x%X",addr);
        }
    }
    return retval;
}

/*
    读取文件内容
    fread
*/
int filesystem_read_data (int addr,void *data,int size)
{
    int retval = 0;
    int temp_run = 0;
    if (addr < 0 || data == NULL || size <= 0 || read_open_f == NULL)
    {
        retval = 1;
        ESP_LOGW(TAG, "Read warning");
    }
    else 
    {
        temp_run = addr + Start_ADDR;
        fseek(read_open_f,temp_run,SEEK_SET);             // 开始偏移
        fread(data,sizeof(uint8_t),size,read_open_f);
        // ESP_DEBUG_Hex_Fun ((uint8_t *)data,20);
        // ESP_LOGI(TAG, "Read from file succ ,Start addr: %d ",addr);
    }
    return retval;
}

/*
    打开一个文件夹
*/
static bool example_mount_fatfs(const char* partition_label)
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, partition_label, &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return false;
    }
    return true;
}

static const esp_partition_t* example_add_partition(esp_flash_t* ext_flash, const char* partition_label)
{
    ESP_LOGI(TAG, "Adding external Flash as a partition, label=\"%s\", size=%d KB", partition_label, ext_flash->size / 1024);
    const esp_partition_t* fat_partition;
    ESP_ERROR_CHECK(esp_partition_register_external(ext_flash, 0, ext_flash->size, partition_label, ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, &fat_partition));
    return fat_partition;
}

/*
    列出文件
*/
static void example_list_data_partitions(void)
{
    ESP_LOGI(TAG, "Listing data partitions:");
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *part = esp_partition_get(it);
        ESP_LOGI(TAG, "- partition '%s', subtype %d, offset 0x%x, size %d kB",
        part->label, part->subtype, part->address, part->size / 1024);
    }

    esp_partition_iterator_release(it);
}

/*
    查询内存
*/
static void example_get_fatfs_usage(size_t* out_total_bytes, size_t* out_free_bytes)
{
    FATFS *fs;
    size_t free_clusters;
    int res = f_getfree("0:", &free_clusters, &fs);
    assert(res == FR_OK);
    size_t total_sectors = (fs->n_fatent - 2) * fs->csize;
    size_t free_sectors = free_clusters * fs->csize;

    // assuming the total size is < 4GiB, should be true for SPI Flash
    if (out_total_bytes != NULL) {
        *out_total_bytes = total_sectors * fs->ssize;
    }
    if (out_free_bytes != NULL) {
        *out_free_bytes = free_sectors * fs->ssize;
    }
}


/*
    打开一个文件夹并初始化
*/
static void initialize_filesystem(void)
{
    size_t temp_num;
    float temp_fnum;

    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true
    };
    spi_flash_init();
    vTaskDelay(100);
    temp_num = spi_flash_get_chip_size();
    ESP_LOGW(TAG,"chip_size is (0x%X)",temp_num);
    if (temp_num < 0x800000)
    {
        ESP_LOGE(TAG,"flash undersize !");
        while(1)
        {
            vTaskDelay(100);
        }
    }

    sprintf(fat_file_str,"/%s%s",BASE_PATH,MOUNT_PATH);
    ESP_LOGI(TAG, "file_str %s", fat_file_str);
    esp_err_t err = esp_vfs_fat_spiflash_mount(fat_file_str, BASE_PATH, &mount_config, &wl_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    example_get_fatfs_usage(&bytes_total, &bytes_free);
    temp_fnum = bytes_total-bytes_free;
    temp_fnum /= bytes_total;
    ESP_LOGW(TAG, "FAT FS: %d kB total, %d kB free ,proportion: %f", bytes_total / 1024, bytes_free / 1024,temp_fnum);
    ESP_LOGW(TAG,"initialize succ \n");

    example_list_data_partitions();
}

