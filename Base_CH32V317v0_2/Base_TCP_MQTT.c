#include "Base_TCP_MQTT.h"
#include "API.h"
// #include "eth_driver.h"
#include "Base_ETH.h"

#define Exist_TCPMQTT 1

#if Exist_TCPMQTT

char Glob_s_url[128] = {0};
char Glob_user[30] = {0};
char Glob_passwd[30] = {0};
char Glob_push_topic[128] = {0};
char Glob_subscribe_topic[128] = {0};
char Glob_client_id[30] = {0};
static D_pFun server_receive_fun = NULL;
static struct mg_connection *s_mqtt_conn = NULL;
extern Caven_event_Type g_SYS_events;
static uint8_t s_qos = 0;                      // MQTT QoS

// static const char *s_url = "mqtt://broker.hivemq.com:1883";
// static const char *s_sub_topic = "mg/123/rx";  // Subscribe topic
// static const char *s_pub_topic = "mg/123/tx";  // Publish topic
// static struct mg_connection *s_mqtt_conn;      // Client connection

static void subscribe(struct mg_connection *c, struct mg_str topic)
{
    struct mg_mqtt_opts opts = {};
    memset(&opts, 0, sizeof(opts));
    opts.topic = topic;
    opts.qos = 0;
    mg_mqtt_sub(c, &opts);
    MG_INFO(("%lu SUBSCRIBED to %.*s", c->id, topic.len, topic.buf));
}

static void publish(struct mg_connection *c, struct mg_str topic,
                    struct mg_str message)
{
    struct mg_mqtt_opts opts = {};
    memset(&opts, 0, sizeof(opts));
    opts.topic = topic;
    opts.message = message;
    opts.qos = s_qos;
    mg_mqtt_pub(c, &opts);
    MG_INFO(("%lu PUBLISHED %.*s -> %.*s", c->id, topic.len, topic.buf,
             message.len, message.buf));
}
static int mqtt_connected = 0;
static void mqtt_ev_handler(struct mg_connection *c, int ev, void *ev_data)
{
    // printf("MG_EV: %d\r\n",ev);
    if (ev == MG_EV_OPEN)
    {
        MG_INFO(("%lu CREATED", c->id));
        // c->is_hexdumping = 1;
    }
    else if (ev == MG_EV_CONNECT)
    {
        MG_INFO(("MG_EV_CONNECT"));
        // if (c->is_tls) {
        //   struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ca.pem"),
        //                              .name = mg_url_host(s_url)};
        //   mg_tls_init(c, &opts);
        // }
    }
    else if (ev == MG_EV_ERROR)
    {
        // On error, log error message
        MG_ERROR(("%lu ERROR %s", c->id, (char *)ev_data));
    }
    else if (ev == MG_EV_MQTT_OPEN)
    {
        mqtt_connected = 1;
        // MQTT connect is successful
        MG_INFO(("%lu CONNECTED to %s", c->id, Glob_s_url));
        subscribe(c, mg_str(Glob_subscribe_topic));
    }
    else if (ev == MG_EV_MQTT_MSG)
    {
        MG_INFO(("MG_EV_MQTT_MSG"));
        // When we get echo response, print it
        // char response[100];
        struct mg_mqtt_message *mm = (struct mg_mqtt_message *)ev_data;
        // mg_snprintf(response, sizeof(response), "Got %.*s -> %.*s", mm->topic.len,
        //             mm->topic.buf, mm->data.len, mm->data.buf);
        // publish(c, mg_str(s_pub_topic), mg_str(response));
        // printf("MQTT RX Topic %.*s Data %.*s\r\n", (int)mm->topic.len, mm->topic.buf, (int)mm->data.len, mm->data.buf);
        if (server_receive_fun)
        {
            for (int i = 0; i < mm->data.len; i++)
            {
                server_receive_fun(&mm->data.buf[i]);
            }
        }
    }
    else if (ev == MG_EV_MQTT_CMD)
    {
        MG_INFO(("MG_EV_MQTT_CMD"));
        struct mg_mqtt_message *mm = (struct mg_mqtt_message *)ev_data;
        if (mm->cmd == MQTT_CMD_PINGREQ)
            mg_mqtt_pong(c);
    }
    else if (ev == MG_EV_CLOSE)
    {
        MG_INFO(("%lu CLOSED", c->id));
        s_mqtt_conn = NULL; // Mark that we're closed
        mqtt_connected = 0;
    }
}

static void timer_fn(void *arg)
{
    if (s_mqtt_conn == NULL)
    {
        struct mg_mgr *mgr = get_eth_mgr();
        struct mg_mqtt_opts opts;
        memset(&opts, 0, sizeof(opts));
        opts.client_id = strlen(Glob_client_id) ?  mg_str("caven_mcu_mqtt_client") : mg_str(Glob_client_id);
        opts.user = mg_str(Glob_user);
        opts.pass = mg_str(Glob_passwd);
        opts.keepalive = 30;
        opts.clean = true;
        opts.version = 4;
        s_mqtt_conn = mg_mqtt_connect(mgr, Glob_s_url, &opts, mqtt_ev_handler, NULL);
    }
    else
    {
        if(mqtt_connected)
            mg_mqtt_ping(s_mqtt_conn);
    }
}

static int server_event = 0;

static void Base_TCP_Mqtt_start(void *data)
{
    if (get_eth_mif()->state != MG_TCPIP_STATE_READY)
    {
        return;
    }
    mg_timer_add(get_eth_mgr(), 3000, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, timer_fn, NULL);
    Caven_delete_event_Fun(&g_SYS_events, &server_event);
}

int Base_TCP_Mqtt_Config (char *cfg, int enable)
{
    strcpy(Glob_s_url, "mqtt://");
    int retval = -1;
    if (enable)
    {   
        // printf("MQTT Config String: %s\n", cfg); // 调试输出
        char* temp_post ;
        temp_post = strstr(cfg, "url<");
        if(temp_post != NULL) sscanf(temp_post, "url<%128[^>]>", Glob_s_url + 7);
        temp_post = strstr(cfg, "U<");
        if(temp_post != NULL) sscanf(temp_post, "U<%30[^>]>", Glob_user);
        temp_post = strstr(cfg, "P<");
        if(temp_post != NULL) sscanf(temp_post, "P<%30[^>]>", Glob_passwd);
        temp_post = strstr(cfg, "Ptopic<");
        if(temp_post != NULL) sscanf(temp_post, "Ptopic<%128[^>]>", Glob_push_topic);
        temp_post = strstr(cfg, "Stopic<");
        if(temp_post != NULL) sscanf(temp_post, "Stopic<%128[^>]>", Glob_subscribe_topic);
        temp_post = strstr(cfg, "Id<");
        if(temp_post != NULL) sscanf(temp_post, "Id<%30[^>]>", Glob_client_id);
        // printf("MQTT Config:\n URL: %s\n User: %s\n Passwd: %s\n Ptopic: %s\n Stopic: %s\n Id: %s\n", 
        //     Glob_s_url, Glob_user, Glob_passwd, Glob_push_topic, Glob_subscribe_topic, Glob_client_id);
        Caven_new_event_Fun(&g_SYS_events, Base_TCP_Mqtt_start, &server_event);
        Caven_trigger_event_Fun(&g_SYS_events, server_event, 1);
    }
    return 0;
}

int Base_TCP_Mqtt_Send(u8 *data, int len)
{
    int retval = -1;
    if (s_mqtt_conn != NULL)
    {
        publish(s_mqtt_conn, mg_str(Glob_push_topic), mg_str_n((char *)data, len));
        retval = 0;
    }
    return retval;
}

void Base_TCP_Mqtt_Sub_Bind_Fun(D_pFun Fun)
{
    server_receive_fun = Fun;
}

#endif
