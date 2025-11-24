#include "Base_TCP_Server.h"
#include "Base_ETH.h"

#include "API.h"
#include "eth_driver.h"

#if Exist_ETH

#define KEEPALIVE_ENABLE    1   // Enable keep alive function
static u8 *p_server_sock;
static u8 SocketIdForListen;
static u8 last_sock = 0;

static u8 server_init_flag = 0;
static u8 *Socket_ptr;                      // socket receive buffer
static u8 (*Socket_buff)[RECE_BUF_LEN];     // socket receive buffer
static D_pFun server_receive_fun = NULL;

int Base_TCP_Server_Config (char *port_str,int enable)
{
    int retval = 0;
    if(port_str != NULL && enable != 0)
    {
        int port = atoi(port_str);

        SOCK_INF TmpSocketInf;
        memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
        p_server_sock = Base_ETH_Server_Bind();

        TmpSocketInf.SourPort = port;
        TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
        WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
        printf("Server_Config post %d,SocketId %d\r\n", port,SocketIdForListen);
        WCHNET_SocketListen(SocketIdForListen);             // listen for connections

        Base_ETH_Server_pFun_Bind (Base_TCP_Server_Task);
        server_init_flag = 1;
    }
    else if(enable == 0)
    {
        p_server_sock = Base_ETH_Server_Bind();
        WCHNET_SocketClose(*p_server_sock,TCP_CLOSE_ABANDON);
        *p_server_sock = 0xff;
        server_init_flag = 0;
    }
    return retval;
}

int Base_TCP_Server_Send (u8 *data, int len)
{
    int retval = 0;
    u32 temp_num = len;
    u8 i;
    if(*p_server_sock && *p_server_sock != 0xff && server_init_flag > 0)
    {
        i = WCHNET_SocketSend(*p_server_sock, data, &temp_num);        //send data
        if (i == WCHNET_ERR_SUCCESS) {
            WCHNET_SocketRecv(*p_server_sock, NULL, &temp_num);        //Clear sent data
        }
        else
        {
            retval = 1;
        }
    }
    else
    {
        retval = 1;
    }
    return retval;
}

void Base_TCP_Server_Receive_Bind_Fun (D_pFun Fun)
{
    server_receive_fun = Fun;
}

void Base_TCP_Server_Task (u8 sock,u8 intstat)
{
    u8 i;
    if(server_init_flag)
    {
        u8 temp_data = 0;
        Socket_ptr = Base_ETH_SockBuff_Bind ();
        Socket_buff = Base_ETH_SockBuff_Bind_RecBuf();

        if (intstat & SINT_STAT_RECV)                                   //receive data
        {
            // printf("server socket[%d] stat recv\r\n",sock);
            u32 len;
            u32 endAddr = SocketInf[sock].RecvStartPoint + SocketInf[sock].RecvBufLen;       //Receive buffer end address

            if ((SocketInf[sock].RecvReadPoint + SocketInf[sock].RecvRemLen) > endAddr) {    //Calculate the length of the received data
                len = endAddr - SocketInf[sock].RecvReadPoint;
            }
            else {
                len = SocketInf[sock].RecvRemLen;
            }
            for(int i = 0; i < len; i++)
            {
                temp_data = *(u8 *) (SocketInf[sock].RecvReadPoint + i);
                if(server_receive_fun != NULL)
                {
                    server_receive_fun (&temp_data);
                }
            }

            // Base_TCP_Server_Send ((u8 *) SocketInf[sock].RecvReadPoint, len);
        }
        if (intstat & SINT_STAT_CONNECT)                                //connect successfully
        {
    #if KEEPALIVE_ENABLE
            WCHNET_SocketSetKeepLive(sock, ENABLE);
    #endif
            WCHNET_ModifyRecvBuf(sock, (u32) Socket_buff[sock],RECE_BUF_LEN);

            if(last_sock != 0 && last_sock != sock)
            {
                printf("server socket[%d] has been abandon\r\n",last_sock);
                WCHNET_SocketClose(last_sock,TCP_CLOSE_ABANDON);
            }
            last_sock = sock;
            *p_server_sock = sock;
            printf("server socket[%d] con\r\n",sock);
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
                if (Socket_ptr[i] == 0xff)                  // save connected socket id
                {
                    Socket_ptr[i] = sock;
                    break;
                }
            }
        }
        if (intstat & SINT_STAT_DISCONNECT)                           //disconnect
        {
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
                if (Socket_ptr[i] == sock) {
                    Socket_ptr[i] = 0xff;
                    break;
                }
            }
            printf("server socket[%d] discon \r\n",sock);
            *p_server_sock = 0;
        }
        if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
        {
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
                if (Socket_ptr[i] == sock) {
                    Socket_ptr[i] = 0xff;
                    break;
                }
            }
            WCHNET_SocketClose(sock,TCP_CLOSE_ABANDON);
            printf("server socket[%d] over time\r\n",sock);
            *p_server_sock = 0;
        }
    }
}

#endif
