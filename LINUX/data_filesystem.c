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
#include "debug_log.h"
#include "DATA_Handle.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #include "esp_flash.h"
// #include "esp_flash_spi_init.h"
// #include "esp_partition.h"
// #include "esp_vfs.h"
// #include "esp_vfs_fat.h"
// #include "esp_system.h"
// #include "esp_log.h"

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

/*
    ftell fseek fread fwrite fopen fclose
*/

static const char *TAG = "filesys";
#define BASE_PATH "/root/dist"
#define MOUNT_PATH "/data"
#define Start_ADDR 128      // 这个位置放一些必要信息 

// Mount path for the partition
const char *base_path = "/extflash";
char flash_file[100] = "reader_linux_demo.txt";
size_t bytes_total, bytes_free;

static char fat_file_str[100];
static char Start_str_len = 0;
static int title_a = 0,title_b = 0;

void filesystem_rename_file (const char *name)
{
    if (filesystem_get_write_mode_flag () == 0)
    {
        memset(flash_file,0,sizeof(flash_file));
        if (name != NULL)
        {
            strcpy(flash_file,name);
            debug_log(LOG_Info,TAG, "rename [%s]", flash_file);
        }
    }
}

/*
    调用此函数不需要调 filesystem_write_mode
*/
void filesystem_save_title (int a,int b)
{
    FILE *f = NULL;
    sprintf(fat_file_str,"%s/%s",BASE_PATH,flash_file);
    debug_log(LOG_Info,TAG, "file_str [%s]", fat_file_str);
    f = fopen(fat_file_str, "rb+");
    if (f == NULL) {
        debug_log(LOG_Warn,TAG, "Failed to open file for writing");
        Start_str_len = 0;
        while(1)
        {
            // 
        }
    }
    else if(a != title_a || b != title_b || a == 0 || b == 0)
    {
        fprintf(f, "File system version %s ", "1.0");      // 写入内容
        fprintf(f, "dataA:[%d] dataB:[%d]\n",a,b);
        fclose(f);
        debug_log(LOG_Info,TAG, "File written title,dataA:[%d] dataB:[%d]",a,b);
        title_a = a;
        title_b = b;
        Start_str_len = 1;
    }
    else
    {
        debug_log(LOG_Warn,TAG, "File written title fail");
    }
}

void filesystem_get_title (int *a,int *b)
{
    if (a != NULL && b != NULL)
    {
        *a = title_a;
        *b = title_b;
    }
}

/*
    尝试打开目标文件，如果能打开即开始初始化syscfg，如果不能打开[fopen(fat_file_str, "r") == NULL],则创建目标文件,如果打开发现文件为空，也需重建文件
    此文件可分段，比如dataA区和dataB区，可以以两种不同方式解析访问（syscfg+data）
    "r"	打开一个用于读取的文件。该文件必须存在。
    "w"	创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
    "a"	追加到一个文件。写操作向文件末尾追加数据。如果文件不存在，则创建文件。
    "r+"	打开一个用于更新的文件，可读取也可写入。该文件必须存在。
    "w+"	创建一个用于读写的空文件。
    "a+"	打开一个用于读取和追加的文件。
*/
void filesystem_init (void)
{
    int retval = 0;
    FILE *f = NULL;
    char *str_pointer = NULL;
    int temp_num,str_len;
    uint8_t temp_data[64];
    char line[128];
    size_t file_size;     
    // Open file for reading
    Start_str_len = 0;

    sprintf(fat_file_str,"%s/%s",BASE_PATH,flash_file);
    debug_log(LOG_Info,TAG, "filesystem init file_str [%s]...", fat_file_str);
    f = fopen(fat_file_str, "r");
    if (f == NULL) {
        debug_log(LOG_Warn,TAG, "Failed to open file for reading");
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
            
            debug_log(LOG_Info,TAG, "Read from file: [%s] <-- Start_str_len: %d", line,Start_str_len);
            memset(temp_data,0,sizeof(temp_data));
            temp_num = data_gain_str_by_sign(line,strlen(line),temp_data,"dataA",'[');
            if (temp_num >= 0)
            {
                temp_num = atoi(temp_data);   // 指针偏移
                debug_log(LOG_Info,TAG, "[%s] -> get dataA len:[%d]",temp_data,temp_num);
                title_a = temp_num;
            }
            else
            {
                retval = 1;
            }
            memset(temp_data,0,sizeof(temp_data));
            temp_num = data_gain_str_by_sign(line,strlen(line),temp_data,"dataB",'[');
            if (temp_num >= 0)
            {
                temp_num = atoi(temp_data);   // 指针偏移
                debug_log(LOG_Info,TAG, "[%s] -> get dataB: len:[%d]",temp_data,temp_num);
                title_b = temp_num;
            }
            else
            {
                retval = 1;
            }
        }
    }
    if(Start_str_len > 0)
    {
        debug_log(LOG_Info,TAG, "The file exists,Don't build");
        retval = 0;
    }
    else
    {
        debug_log(LOG_Warn,TAG, "file empty build & Opening file");
        f = fopen(fat_file_str, "w");       // 新建文件，清空文件内容
        fclose(f);
        retval = 1;
    }

    if(retval == 1)
    {
        filesystem_save_title (0,0);
    }
    file_size = filesystem_get_space_size (&temp_num);
    debug_log(LOG_Warn,TAG, "filesystem init finish file size :%d",file_size);
}

/*
    指针是返回当前位置
    返回值是总内存大小
*/
size_t filesystem_get_space_size (int *addr)
{
    size_t retval = 0;
    FILE *f = NULL;
    sprintf(fat_file_str,"%s/%s",BASE_PATH,flash_file);
    // debug_log(LOG_Info,TAG, "file_str %s", fat_file_str);
    f = fopen(fat_file_str, "rb");

    if(f != NULL)
    {
        fseek(f,0,SEEK_END);    // 把光标移到文件末尾
        retval = ftell(f);      // 获取文件指针当前位置相对于文件头的偏移，当文件指针指向文件末尾时，ftell() 返回的就是文件的大小
        fclose(f);
        if (addr != NULL)
        {
            *addr = retval;
        }
        bytes_total = retval;
        retval = bytes_total;
        debug_log(LOG_Info,TAG, "file size: %d byte", bytes_total);
    }

    return retval;
}

FILE *read_open_f = NULL;
int filesystem_read_mode (int set)
{
    int retval = 0;

    sprintf(fat_file_str,"%s/%s",BASE_PATH,flash_file);
    if (set != 0 && read_open_f == NULL)
    {
        read_open_f = fopen(fat_file_str, "rb");
        if (read_open_f == NULL) {
            debug_log(LOG_Error,TAG, "Failed to open file for reading");
        }
        else if (Start_str_len == 0)
        {
            retval = 1;
            debug_log(LOG_Warn,TAG, "read file empty !!! ");
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

    // sprintf(fat_file_str,"/%s%s/%s",BASE_PATH,MOUNT_PATH,flash_file);
    sprintf(fat_file_str,"%s/%s",BASE_PATH,flash_file);
    // ESP_LOGI(TAG, "file_str %s", fat_file_str);
    if (set != 0 && write_open_f == NULL)
    {
        write_open_f = fopen(fat_file_str, "rb+");
        if (write_open_f == NULL) {
            debug_log(LOG_Error,TAG, "Failed to open file for reading");
        }
        else if (Start_str_len == 0)
        {
            retval = 1;
            debug_log(LOG_Warn,TAG, "write file empty !!! ");
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
            debug_log(LOG_Warn,TAG, "Failed to open file for reading");
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
    int offset;
    if (addr < 0 || data == NULL || size <= 0 || write_open_f == NULL)
    {
        retval = 1;
        debug_log(LOG_Warn,TAG, "Write warning,addr[%d],pData[%x],size[%d]",addr,data,size);
    }
    else 
    {
        if (Start_str_len == 0)
        {
            retval = 1;
            debug_log(LOG_Warn,TAG, "write data file empty !!! ");
        }
        else
        {
            offset = addr + Start_ADDR;
            fseek(write_open_f,offset,SEEK_SET);             // 开始偏移
            fwrite(data,sizeof(uint8_t),size,write_open_f);

            debug_log(LOG_Info,TAG, "Write succ,Start Absolute address[0x%X],Relative address[0x%X]",offset,addr);
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
        debug_log(LOG_Warn,TAG, "Read warning,addr[%d],pData[%x],size[%d]",addr,data,size);
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

