#include "Base_TCP_Client.h"
#include "Base_ETH.h"

#include "API.h"
#include "eth_driver.h"

#if Exist_ETH

#define KEEPALIVE_ENABLE    1   // Enable keep alive function
static u8 *p_client_sock;

static u8 client_init_flag = 0;
static u8 client_con = 0;
static u8 *Socket_ptr;                      // socket receive buffer
static u8 (*Socket_buff)[RECE_BUF_LEN];     // socket receive buffer
static D_pFun client_receive_fun = NULL;

static char client_ip[40];
static char client_port[40];

int Base_TCP_Client_Config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    if(ip_str != NULL && port_str != NULL && enable != 0)
    {
        int port = atoi(port_str);
        SOCK_INF TmpSocketInf;
        u8 ip_array[10];

        strcpy(client_ip,ip_str);
        strcpy(client_port,port_str);
        Base_ETH_Client_pFun_Bind (Base_TCP_Client_Task);

        Caven_Str_To_ip (ip_str,ip_array,4);
        memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
        memcpy((void *) TmpSocketInf.IPAddr, ip_array, 4);
        TmpSocketInf.DesPort = port;
        TmpSocketInf.SourPort = port;
        TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
        TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
        p_client_sock = Base_ETH_Client_Bind();
        if(*p_client_sock == 0)
        {
            WCHNET_SocketCreat(p_client_sock, &TmpSocketInf);
            WCHNET_SocketConnect(*p_client_sock);                        // make a TCP connection
            printf("Client_Config ip[%s:%s] %d\r\n", client_ip,client_port,*p_client_sock);
        }
        client_init_flag = 1;
    }
    else if(enable == 0)
    {
        p_client_sock = Base_ETH_Client_Bind();
        WCHNET_SocketClose(*p_client_sock,TCP_CLOSE_ABANDON);
        *p_client_sock = 0;
        client_init_flag = 0;
        client_con = 0;
    }
    return retval;
}

int Base_TCP_Client_Send (uint8_t *data, int len)
{
    int retval = 0;
    u32 temp_num = len;
    u8 i;
    if(client_con && *p_client_sock && *p_client_sock != 0xff && client_init_flag > 0)
    {
        i = WCHNET_SocketSend(*p_client_sock, data, &temp_num);        //send data
        if (i == WCHNET_ERR_SUCCESS) {
            WCHNET_SocketRecv(*p_client_sock, NULL, &temp_num);        //Clear sent data
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

void Base_TCP_Client_Receive_Bind_Fun (D_pFun Fun)
{
    client_receive_fun = Fun;
}

void Base_TCP_Client_Task (u8 sock,u8 intstat)
{
    u8 i;
    if (*p_client_sock == 0)
    {
        if(client_init_flag)
        {
            Base_TCP_Client_Config (client_ip,client_port,1);
        }
    }
    if(sock > 0 && *p_client_sock > 0 && client_init_flag)
    {
        u8 temp_data = 0;
        Socket_ptr = Base_ETH_SockBuff_Bind ();
        Socket_buff = Base_ETH_SockBuff_Bind_RecBuf();

        if (intstat & SINT_STAT_RECV)                                   //receive data
        {
            // printf("client socket[%d] stat recv\r\n",sock);
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
                if(client_receive_fun != NULL)
                {
                    client_receive_fun (&temp_data);
                }
            }

            // Base_TCP_Client_Send ((u8 *) SocketInf[sock].RecvReadPoint, len);
        }
        if (intstat & SINT_STAT_CONNECT)                                //connect successfully
        {
    #if KEEPALIVE_ENABLE
            WCHNET_SocketSetKeepLive(sock, ENABLE);
    #endif
            WCHNET_ModifyRecvBuf(sock, (u32) Socket_buff[sock],RECE_BUF_LEN);

            printf("clinet socket[%d] con\r\n",sock);
            client_con ++;
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
            printf("clinet socket[%d] discon \r\n",sock);
            WCHNET_SocketClose(*p_client_sock,TCP_CLOSE_ABANDON);
            *p_client_sock = 0;
            client_con = 0;
        }
        if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
        {
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
                if (Socket_ptr[i] == sock) {
                    Socket_ptr[i] = 0xff;
                    break;
                }
            }
            printf("clinet socket[%d] over time\r\n",sock);
            WCHNET_SocketClose(sock,TCP_CLOSE_ABANDON);
            *p_client_sock = 0;
            client_con = 0;
        }
    }
}

#endif
