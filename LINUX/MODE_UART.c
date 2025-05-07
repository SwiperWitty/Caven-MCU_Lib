#include "MODE_UART.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <linux/serial.h>

#define UART0_PORT  "/dev/ttymxc0"      // rs232
#define UART1_PORT  "/dev/ttyUSB1"
#define UART2_PORT  "/dev/ttymxc2"      // GPS
#define UART3_PORT  "/dev/ttymxc3"      // UHF
int fd_com0 = 0;
int fd_com1 = 0;
int fd_com2 = 0;
int fd_com3 = 0;


static void clear_custom_speed_flag(int _fd)
{
    struct serial_struct ss;
    if (ioctl(_fd, TIOCGSERIAL, &ss) < 0) {
        // return silently as some devices do not support TIOCGSERIAL
        return;
    }

    if ((ss.flags & ASYNC_SPD_MASK) != ASYNC_SPD_CUST)
        return;

    ss.flags &= ~ASYNC_SPD_MASK;

    if (ioctl(_fd, TIOCSSERIAL, &ss) < 0) {
        perror("TIOCSSERIAL failed");
        exit(1);
    }
}

static int get_baud(int baud)
{
    switch (baud) 
    {
    case 9600:return B9600;
    case 19200:return B19200;
    case 38400:return B38400;
    case 57600:return B57600;
    case 115200:return B115200;
    case 230400:return B230400;
    case 460800:return B460800;
    case 500000:return B500000;
    case 576000:return B576000;
    case 921600:return B921600;
#ifdef B1000000
    case 1000000:return B1000000;
#endif
#ifdef B1152000
    case 1152000:return B1152000;
#endif
#ifdef B1500000
    case 1500000:return B1500000;
#endif
#ifdef B2000000
    case 2000000:return B2000000;
#endif
#ifdef B2500000
    case 2500000:return B2500000;
#endif
#ifdef B3000000
    case 3000000:return B3000000;
#endif
#ifdef B3500000
    case 3500000:return B3500000;
#endif
#ifdef B4000000
    case 4000000:return B4000000;
#endif
    default:return -1;
    }
}

int MODE_UART_Init(char Channel,int Baud,int Set)
{
    int retavl;
#ifdef Exist_UART
    // Base_UART_Init((UART_mType)Channel,Baud,SET);
    struct termios opt;
	char uart_array[50];
    int *fd = NULL;
    memset(&opt, 0, sizeof(opt));
    memset(uart_array,0,sizeof(uart_array));
    /* 忽略modem，使能读模式 */
    opt.c_cflag |= CLOCAL | CREAD;
    /* 设置波特率 */
    opt.c_cflag |= get_baud(Baud);  //!!!!
    /* 设置数据位 */
    opt.c_cflag |= CS8;
    /* 设置奇偶校验位 */
    //n
    opt.c_iflag &= ~INPCK;     
    opt.c_cflag &= ~PARENB;
    //o
    // opt.c_iflag |= (INPCK | ISTRIP); 
    // opt.c_cflag |= (PARODD | PARENB);
    //e
    // opt.c_iflag |= (INPCK | ISTRIP); 
    // opt.c_cflag |= PARENB;
    // opt.c_cflag &= ~PARODD;
    /* 设置停止位 */
    //1
    opt.c_cflag &= ~CSTOPB;
    //2
    // opt.c_cflag |= CSTOPB;
    /* 设置流控制 */
    opt.c_cflag &= ~CRTSCTS;
    // opt.c_lflag &= ~ICANON; //raw Added by czj不等回车输入
    // opt.c_lflag &= ~ECHO;   //raw Added bu czj不回显

    /* 最小字节数与等待时间 */
    opt.c_cc[VMIN] = 1;
    opt.c_cc[VTIME] = 0;

    switch (Channel)
    {
    case 0:
        strcpy(uart_array,UART0_PORT);
        fd = &fd_com0;
        break;
    case 1:
        strcpy(uart_array,UART1_PORT);
        fd = &fd_com1;
        break;
    case 2:
        strcpy(uart_array,UART2_PORT);
        fd = &fd_com2;
        break;
    case 3:
        strcpy(uart_array,UART3_PORT);
        fd = &fd_com3;
        break;
    default:
        fd = &fd_com0;
        break;
    }
    if (SET)
    {
        *fd = open(uart_array, O_RDWR|O_NOCTTY|O_NONBLOCK);
        if(*fd == -1){
            perror("open uart error");
            retavl = -1;
        }
        else{
            /* 刷新串口，更新配置 */
            tcflush(*fd, TCIOFLUSH);
            tcsetattr(*fd, TCSANOW, &opt);
            printf("uart[%d],fd[%d] \n",Channel,*fd);
            clear_custom_speed_flag(*fd);
        }
    }
    else
    {
        if (*fd > 0)
        {
            close(*fd);
            *fd = 0;
        }
    }

#endif
    return retavl;
}


void MODE_UART_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
	int temp = MIN(Length,BUFF_MAX);
    int *fd = NULL;
    switch (Channel)
    {
    case 0:
        fd = &fd_com0;
        break;
    case 1:
        fd = &fd_com1;
        break;
    case 2:
        fd = &fd_com2;
        break;
    case 3:
        fd = &fd_com3;
        break;
    default:
        break;
    }
    if (fd != NULL && Data != NULL && *fd > 0)
    {
        write(*fd, Data, temp);
    }
    
#endif
}

void MODE_UART_DMA_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
    int temp = MIN(Length,BUFF_MAX);

    MODE_UART_Send_Data_Fun(Channel,Data,temp);
#endif
}


void MODE_UART_Send_String_Fun(char Channel, const char *String)
{
    int Length = strlen(String);
    // printf("Channel:[%d] send:\n%s \n----\n",Channel,String);
    MODE_UART_Send_Data_Fun(Channel,(U8 *)String,Length);
}

void MODE_UART_Receive_Bind_Fun(char Channel, D_pFun UART_pFun)
{
#ifdef Exist_UART
    // State_Machine_Bind ((UART_mType)Channel,UART_pFun);
#endif
}

int MODE_UART_Receive_Get_Fun(char Channel,uint8_t* Data,int Length)
{
    int retval = 0;
    int *fd = NULL;
    switch (Channel)
    {
    case 0:
        fd = &fd_com0;
        break;
    case 1:
        fd = &fd_com1;
        break;
    case 2:
        fd = &fd_com2;
        break;
    case 3:
        fd = &fd_com3;
        break;
    default:
        break;
    }
    if (fd != NULL && Data != NULL && *fd > 0)
    {
        retval = read(*fd, Data, Length);
        if (retval < 0)
        {
            // char array[100];
            // memset(array,0,sizeof(array));
            // sprintf(array,"uart[%d] read",Channel);
            // perror(array);
            // sleep(1);
        }
        else
        {
            // printf("uart[%d] num[%d]\n",Channel,retval);
        }
    }
    return retval;
}
