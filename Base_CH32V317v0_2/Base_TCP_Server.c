#include "Base_TCP_Server.h"
#include "Base_ETH.h"

#include "API.h"
// #include "eth_driver.h"
#include "Base_ETH.h"


#define Exist_TCPSERVER 1

#if Exist_TCPSERVER

static D_pFun server_receive_fun = NULL;
static struct mg_connection *con = NULL;
extern Caven_event_Type g_SYS_events;


static void tcp_event_handler_fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_OPEN && c->is_listening == 1) {
    MG_INFO(("SERVER is listening"));
  } else if (ev == MG_EV_ACCEPT) {
    MG_INFO(("SERVER accepted a connection"));
    // if (mg_url_is_ssl(s_lsn)) {
    //   struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ss_ca.pem"),
    //                              .cert = mg_unpacked("/certs/ss_server.pem"),
    //                              .key = mg_unpacked("/certs/ss_server.pem")};
    //   mg_tls_init(c, &opts);
    // }
    if(con == NULL){
        con = c;
    }
    else
    {
        mg_close_conn(c);
    }
  } else if (ev == MG_EV_READ) {
    struct mg_iobuf *r = &c->recv;
    // MG_INFO(("SERVER got data:%.*s", r->len, r->buf));
    // mg_log("%.*s", r->len, r->buf);
    // mg_send(c, r->buf, r->len);  // echo it back
    if(server_receive_fun)
    {
        for(size_t i = 0; i < r->len; i++)
        {
            u8 temp_data = r->buf[i];
            server_receive_fun (&temp_data);
        }
    }
    r->len = 0;                  // Tell Mongoose we've consumed data
  } else if (ev == MG_EV_CLOSE) {
    MG_INFO(("SERVER disconnected"));
    if (con == c) con = NULL;
  } else if (ev == MG_EV_ERROR) {
    MG_INFO(("SERVER error: %s", (char *) ev_data));
  }
}

char listen_addr[30] ;


int server_event = 0;

void Base_TCP_Server_start (void *data)
{
    if (get_eth_mgr()->ifp == NULL) {
        // Caven_new_event_Fun(&g_SYS_events,Base_TCP_Server_start,&server_event);
        // Caven_trigger_event_Fun(&g_SYS_events,server_event,1);
        return;
    }
    mg_listen(get_eth_mgr(), listen_addr, tcp_event_handler_fn, NULL);
    Caven_delete_event_Fun(&g_SYS_events,&server_event);
}




int Base_TCP_Server_Config (char *port_str,int enable)
{
    strcpy(listen_addr, "tcp://0.0.0.0:");
    int retval = -1;
    if(enable)
    {
        strcat(listen_addr,port_str);
        Caven_new_event_Fun(&g_SYS_events,Base_TCP_Server_start,&server_event);
        Caven_trigger_event_Fun(&g_SYS_events,server_event,1);
    }
    return 0;
}

int Base_TCP_Server_Send (u8 *data, int len)
{
    int retval = -1;
    if(con != NULL)
    {
        mg_send(con, data, len);
        retval = 0;
    }
    return retval;
}

void Base_TCP_Server_Receive_Bind_Fun (D_pFun Fun)
{
    server_receive_fun = Fun;
}




#endif
