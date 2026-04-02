#include "Base_TCP_Server.h"
#include "Base_ETH.h"

#include "API.h"
#include "eth_driver.h"


#define KEEPALIVE_ENABLE    1   // Enable keep alive function
static u8 *p_server_sock = NULL;
static u8 *p_last_sock = NULL;
static u8 SocketIdForListen;

static u8 server_init_flag = 0;
static u8 tcp_server_break_off = 0;
static int server_port = 0;
static u8 *Socket_ptr;                      // socket receive buffer
static u8 (*Socket_buff)[RECE_BUF_LEN];     // socket receive buffer
static D_pFun server_receive_fun = NULL;

void Base_TCP_Server_link (void)
{
    if(server_port && p_server_sock == NULL)
    {
        SOCK_INF TmpSocketInf;
        memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
        p_server_sock = Base_ETH_Server_Bind();
        p_last_sock = Base_ETH_Serlast_Bind ();
        TmpSocketInf.SourPort = server_port;
        TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
        WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
        Debug_printf("Server open port:%d ,SocketId:%d\n",server_port,SocketIdForListen);
        WCHNET_SocketListen(SocketIdForListen);             // listen for connections
    }
}

/*
    port_str:"8160"
    break_off:�Ƿ�����������ȡ�������ӣ�port_str����ʱ����Ч��
    enable 0,��رյ�ǰsock��û������Ч
    enable 1,��server_link,������str,����ʱstrΪNULL������Ϊѯ��tcp_server_sock
    server ֻ���޸Ķ˿�(����������)�������Ҫ�޸�ip���޸�[eth_config_ip]/[wifi_config_ip]

    retval:��ǰ�Ƿ���sock
*/
int Base_TCP_Server_Config (char *port_str,int break_off,int enable)
{
    int retval = 0;
    if(port_str != NULL && enable != 0)
    {
        server_port = atoi(port_str);
        tcp_server_break_off = break_off;
        Debug_printf("Server_Config post %d,break_off %d\r\n", server_port,tcp_server_break_off);
        Base_ETH_Server_pFun_Bind (Base_TCP_Server_Task);
        Base_TCP_Server_link();
        server_init_flag = 1;
    }
    else if(enable == 0)
    {
        p_server_sock = Base_ETH_Server_Bind();
        WCHNET_SocketClose(*p_server_sock,TCP_CLOSE_RST);
        *p_server_sock = 0xff;
        *p_last_sock = 0xff;
        p_server_sock = NULL;
        p_last_sock = NULL;
        server_init_flag = 0;
    }
    if(p_server_sock != NULL && server_init_flag > 0)
    {
        if(*p_server_sock != 0XFF)
        {retval = 1;}
    }
    return retval;
}

int Base_TCP_Server_Send (u8 *data, int len)
{
    int retval = 0;
    u32 temp_num = len;
    u8 i;
    if(p_server_sock != NULL && server_init_flag > 0)
    {
        if(*p_server_sock != 0XFF)
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
        else {
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
    if(p_server_sock == NULL)   // once
    {
        if(server_init_flag && Base_ETH_get_local_ip_status (NULL,NULL,NULL) == 2)
        {
            Base_TCP_Server_link ();
        }
    }
    if(intstat == 0)
    {
        return;
    }
    if(server_init_flag && p_server_sock != NULL)
    {
        u8 temp_data = 0;
        Socket_ptr = Base_ETH_SockBuff_Bind ();
        Socket_buff = Base_ETH_SockBuff_Bind_RecBuf();

        if (intstat & SINT_STAT_RECV)                                   //receive data
        {
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
            if (*p_server_sock != 0XFF)
            {
                WCHNET_SocketRecv(*p_server_sock, NULL, &len);        //Clear sent data
            }
            // Base_TCP_Server_Send ((u8 *) SocketInf[sock].RecvReadPoint, len);
        }
        if (intstat & SINT_STAT_CONNECT)                                //connect successfully
        {
    #if KEEPALIVE_ENABLE
            WCHNET_SocketSetKeepLive(sock, ENABLE);
    #endif
            WCHNET_ModifyRecvBuf(sock, (u32) Socket_buff[sock],RECE_BUF_LEN);
            if(tcp_server_break_off)
            {
                if(*p_last_sock != 0XFF && *p_last_sock != sock)
                {
                    Debug_printf("break_off->server socket[%d] has been abandon\r\n",*p_last_sock);
                    WCHNET_SocketClose(*p_last_sock,TCP_CLOSE_RST);
                }
            }
            else
            {
                if(*p_last_sock != 0XFF && *p_last_sock != sock)
                {
                    Debug_printf("new server socket,but keep last socket[%d] \r\n",*p_last_sock);
                    WCHNET_SocketClose(sock,TCP_CLOSE_RST);
                    return;
                }
            }
            *p_last_sock = sock;
            *p_server_sock = sock;
            Debug_printf("server socket[%d] con\r\n",sock);
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
            Debug_printf("server socket[%d] discon \r\n",sock);
            if(sock == *p_server_sock)
            {
                WCHNET_SocketClose(sock,TCP_CLOSE_RST);
                *p_server_sock = 0XFF;
                *p_last_sock = 0XFF;        // no chance
            }
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
                if (Socket_ptr[i] == sock) {
                    Socket_ptr[i] = 0xff;
                    break;
                }
            }
        }
        if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
        {
            // WCHNET_SocketClose(sock,TCP_CLOSE_ABANDON);
            Debug_printf("server maybe socket[%d] over time\r\n",sock);
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
                if (Socket_ptr[i] == sock) {
                    Socket_ptr[i] = 0xff;
                    break;
                }
            }
        }
    }
}
