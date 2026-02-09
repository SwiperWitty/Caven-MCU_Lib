#include "Base_TCP_Client.h"
#include "Base_ETH.h"

#include "API.h"


// #define Exist_TCPCLIENT 1

#if Exist_TCPCLIENT

static D_pFun client_receive_fun = NULL;
static struct mg_connection *con = NULL;
extern Caven_event_Type g_SYS_events;


static void tcp_event_handler_fn(struct mg_connection *c, int ev, void *ev_data) {
//   int *i = &((struct c_res_s *) c->fn_data)->i;
  if (ev == MG_EV_OPEN) {
    MG_INFO(("CLIENT has been initialized"));
  } else if (ev == MG_EV_CONNECT) {
    con = c;
    MG_INFO(("CLIENT connected"));
    // if (mg_url_is_ssl(s_conn)) {
    //   struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ss_ca.pem"),
    //                              .cert = mg_unpacked("/certs/ss_client.pem"),
    //                              .key = mg_unpacked("/certs/ss_client.pem")};
    //   mg_tls_init(c, &opts);
    // }
    // *i = 1;  // do something
    
  } else if (ev == MG_EV_READ) {
    struct mg_iobuf *r = &c->recv;
    // MG_INFO(("CLIENT got data: %.*s", r->len, r->buf));
    if(client_receive_fun)
    {
        for(size_t i = 0; i < r->len; i++)
        {
            u8 temp_data = r->buf[i];
            client_receive_fun (&temp_data);
        }
    }

    r->len = 0;  // Tell Mongoose we've consumed data
  } else if (ev == MG_EV_CLOSE) {
    con = NULL;
    MG_INFO(("CLIENT disconnected"));
    // signal we are done
    // ((struct c_res_s *) c->fn_data)->c = NULL;
  } else if (ev == MG_EV_ERROR) {
    MG_INFO(("CLIENT error: %s", (char *) ev_data));
  }
}

char Client_addr[30] ;


int client_event = 0;

void Base_TCP_Client_start (void *data)
{
    if (get_eth_mif()->state != MG_TCPIP_STATE_READY)
    {
        return;
    }
    // mg_listen(get_eth_mgr(), Client_addr, tcp_event_handler_fn, NULL);
    mg_connect(get_eth_mgr(), Client_addr, tcp_event_handler_fn, NULL);
    Caven_delete_event_Fun(&g_SYS_events,&client_event);
}




int Base_TCP_Client_Config (char *tcp_url, char *post_str,int enable)
{
    if(tcp_url && post_str)
    {
        strcpy(Client_addr, "tcp://");
        strcat(Client_addr, tcp_url);
        strcat(Client_addr, ":");
        strcat(Client_addr, post_str);
    }
    int retval = -1;
    if(enable)
    {
        Caven_new_event_Fun(&g_SYS_events,Base_TCP_Client_start,&client_event);
        Caven_trigger_event_Fun(&g_SYS_events,client_event,1);
    }
    return 0;
}

int Base_TCP_Client_Send (u8 *data, int len)
{
    int retval = -1;
    if(con != NULL)
    {
        mg_send(con, data, len);
        retval = 0;
    }
    return retval;
}

void Base_TCP_Client_Receive_Bind_Fun (D_pFun Fun)
{
    client_receive_fun = Fun;
}




#endif
