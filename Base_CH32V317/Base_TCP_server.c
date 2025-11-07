#include "Base_TCP_Server.h"
#include "Base_ETH.h"

#include "API.h"
#include "eth_driver.h"

#if Exist_ETH

static int server = 0;
int Base_TCP_Server_Config (char *port_str,int enable)
{
    if(port_str != NULL)
    {
        server = atoi(port_str);

    }
}

int Base_TCP_Server_Send (uint8_t *data, int len)
{

}

void Base_TCP_Server_Receive_Bind_Fun (D_pFun Fun)
{

}

#endif
