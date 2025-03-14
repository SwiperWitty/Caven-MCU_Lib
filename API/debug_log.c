#include "debug_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TAG "Debug"

// int printf ( const char * format, ... );
void debug_log (int type,const char *tag,const char * format,...)
{
	char my_buf[1024]={0};
    char type_str[20];
	va_list my_ap;              //定义参数指针，获取可选参数
	va_start(my_ap,format);     //初始化参数指针，将ap指向第一个实际参数的地址
	
	vsprintf(my_buf,format,my_ap);
    switch (type)
    {
    case 0:
        sprintf(type_str,"%s","view");
        break;
    case 1:
        sprintf(type_str,"%s","info");
        break;
    case 2:
        sprintf(type_str,"%s","warn");
        break;
    
    case 3:
        sprintf(type_str,"%s","error");
        break;
    default:
        sprintf(type_str,"%s","info");
        break;
    }
    
	printf("[%s] %s:%s\n",type_str,tag,my_buf);
	va_end(my_ap);
}

void debug_log_hex (uint8_t *data,int len)
{
    char array[10];
    if (data == NULL)
    {
        return;
    }
    
    printf("Log HEX:[");
    for (int i = 0; i < len; i++)
    {
        printf("%02X ",data[i]);
    }
    printf("](%d byte)\n",len);
}

void my_system(const char *cmd,char *ret_str,int str_size)
{
    FILE *fp = NULL;
    char buf[1024] = {0};
    char *temp_ptr;
    int temp_num = 0,temp_run = 0;
    if (ret_str == NULL)
    {
        return;
    }
    debug_log(LOG_View,TAG,"my_system cmd:[%s]",cmd);
    if( (fp = popen(cmd, "r")) == NULL ) {
        printf("popen error!\n");
        return;
    }
    do
    {
        memset(buf,0,sizeof(buf));
        temp_ptr = fgets(buf, sizeof(buf), fp);
        if (temp_ptr != NULL)
        {
            temp_num = strlen(buf);
            temp_run += temp_num;
            if (temp_run < str_size)
            {
                strcat(ret_str, buf);
            }
        }
        else
        {
            break;
        }
    }while (temp_ptr != NULL);
    pclose(fp);
    // printf("ret_str: \n%s\n", ret_str);
}

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int my_systemd(const char *command) {
    pid_t pid;
    int status;

    // 创建一个子进程
    pid = fork();
    if (pid < 0) {
        // fork失败
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // 子进程
        // 关闭所有打开的文件描述符
        int max_fd = sysconf(_SC_OPEN_MAX);
        for (int fd = 3; fd < max_fd; fd++) {
            close(fd);
        }

        // 执行命令
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);

        // 如果execl失败
        perror("execl");
        _exit(127);
    } else {
        // 父进程
        // 等待子进程结束
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }

        // 返回子进程的退出状态
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}
