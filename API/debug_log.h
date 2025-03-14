/*
 * Copyright (C) 2017-2025 Cavendish Group Holding Limited
 * Change Logs:
 *   Date        Author       ad
 *   2025-02-12  DRIVER       the first version
 * linux or rtos log
 */

#ifndef _DEBUG__LOG_H__
#define _DEBUG__LOG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_View    0
#define LOG_Info    1
#define LOG_Warn    2
#define LOG_Error   3

void debug_log (int type,const char *tag,const char * format,...);
void debug_log_hex (uint8_t *data,int len);
void my_system(const char *cmd,char *ret_str,int str_size);
int my_systemd(const char *command);

#ifdef __cplusplus
}
#endif

#endif
