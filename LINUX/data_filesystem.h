#ifndef _DATA_FILESYSTEM__H_
#define _DATA_FILESYSTEM__H_

#include <stdlib.h>

#define FILE_DATA_A 0
#define FILE_DATA_B 0x400000

void filesystem_init (void);
void filesystem_rename_file (const char *name);
size_t filesystem_get_space_size (int *addr);

int filesystem_read_data (int addr,void *data,int size);
int filesystem_write_data (int addr,void *data,int size);

void filesystem_save_title (int a,int b);
void filesystem_get_title (int *a,int *b);

int filesystem_get_write_mode_flag (void);
int filesystem_read_mode (int set);
int filesystem_write_mode (int set);

#endif // 
