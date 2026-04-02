#include "Base_TCP_MQTT.h"
#include "Base_ETH.h"
#include "Base_TCP_Client.h"
#include "Base_Sys_Time.h"
#include "Time_Handle.h"
#include "Data_Handle.h"

// MQTTЭ�鳣��
#define MQTT_PROTOCOL_LEVEL  0x04        // MQTT 3.1.1
#define MQTT_CONNECT         0x10
#define MQTT_CONNACK         0x20
#define MQTT_PUBLISH         0x30
#define MQTT_PUBACK          0x40
#define MQTT_PINGREQ         0xC0
#define MQTT_PINGRESP        0xD0
#define MQTT_DISCONNECT      0xE0
#define MQTT_QOS1            0x02

// ״̬����
#define MQTT_STATE_DISCONNECTED    0
#define MQTT_STATE_CONNECTING      1
#define MQTT_STATE_WAIT_CONNACK    2
#define MQTT_STATE_IDLE            3
#define MQTT_STATE_SEND_PUBLISH    4
#define MQTT_STATE_WAIT_PUBACK     5
#define MQTT_STATE_SEND_PING       6
#define MQTT_STATE_WAIT_PINGRESP   7

#if Exist_ETH
static u8 MQTT_init_flag = 0;

static char mqtt_array[200];
static char mqtt_broker_ip[100];
static char mqtt_broker_port[40];
static char mqtt_Ptopic[60];
static char mqtt_Stopic[60];
static char mqtt_default_topic[128];
static char mqtt_client_id[64];
static char mqtt_user[30];
static char mqtt_pass[30];

static char mqtt_send_cache[0x1000];       // ��������Ӧ������
static int mqtt_cache_run = 0;
static char mqtt_recv_cache[0x1000];       // ���յ���Ӧ����Ϣ����������·���
static int mqtt_cache_get = 0;
static int mqtt_cache_read = 0;

static u8 mqtt_packet_id = 1;             // ���ı�ʶ��

// ������ʱ���룩������Keep Alive����
#define MQTT_KEEP_ALIVE    60
static SYS_BaseTIME_Type last_ping_time;  // �ϴη���PINGREQ��ʱ��
static u8 ping_outstanding = 0;           // �Ƿ���δ��Ӧ��PINGREQ
#endif

/*
���ջص���ԭʼ���ݴ�����ջ��棬ͬʱ�������Ʊ���
*/
static void Base_TCP_MQTT_GET_Fun(void *data)
{
    #if Exist_ETH
    char temp_data = *(char *)data;
    if (mqtt_cache_get < sizeof(mqtt_recv_cache))
    {
        mqtt_recv_cache[mqtt_cache_get++] = temp_data;
    }
    #endif
}

#if Exist_ETH
// ����CONNECT����
static int build_connect_packet(uint8_t *buffer, const char *client_id,
                                const char *username, const char *password)
{
    int pos = 0;
    buffer[pos++] = MQTT_CONNECT;                     // �̶�ͷ
    int remain_len_pos = pos;
    buffer[pos++] = 0;                                 // ʣ�೤��ռλ

    // Э���� "MQTT"
    buffer[pos++] = 0; buffer[pos++] = 4;
    buffer[pos++] = 'M'; buffer[pos++] = 'Q'; buffer[pos++] = 'T'; buffer[pos++] = 'T';
    buffer[pos++] = MQTT_PROTOCOL_LEVEL;               // Э�鼶��

    // ���ӱ�־
    uint8_t connect_flags = 0x02;                      // Clean Session
    if (username && username[0] != '\0') {
        connect_flags |= 0x80;                          // �û�����־
        if (password && password[0] != '\0') {
            connect_flags |= 0x40;                      // �����־
        }
    }
    buffer[pos++] = connect_flags;

    // Keep Alive
    buffer[pos++] = (MQTT_KEEP_ALIVE >> 8) & 0xFF;
    buffer[pos++] = MQTT_KEEP_ALIVE & 0xFF;

    // Client ID
    int id_len = strlen(client_id);
    buffer[pos++] = (id_len >> 8) & 0xFF;
    buffer[pos++] = id_len & 0xFF;
    memcpy(buffer + pos, client_id, id_len);
    pos += id_len;

    // ������û����������û���
    if (username && username[0] != '\0') {
        int user_len = strlen(username);
        buffer[pos++] = (user_len >> 8) & 0xFF;
        buffer[pos++] = user_len & 0xFF;
        memcpy(buffer + pos, username, user_len);
        pos += user_len;
    }

    // ��������룬��������
    if (password && password[0] != '\0' && (connect_flags & 0x40)) {
        int pwd_len = strlen(password);
        buffer[pos++] = (pwd_len >> 8) & 0xFF;
        buffer[pos++] = pwd_len & 0xFF;
        memcpy(buffer + pos, password, pwd_len);
        pos += pwd_len;
    }

    // ����ʣ�೤�Ȳ����
    buffer[remain_len_pos] = pos - 2;                   // ʣ�೤�� = �ɱ�ͷ + payload �����ֽ���
    return pos;
}

// ����PUBLISH���ģ�QoS 1��
static int build_publish_packet(uint8_t *buffer, int buf_len,const char *topic, const char *payload, int payload_len, u8 *pkt_id)
{
    if(buf_len < (payload_len + 0x100))
    {
        return 0;
    }
    *pkt_id = mqtt_packet_id++;
    int pos = 0;
    buffer[pos++] = MQTT_PUBLISH | MQTT_QOS1;
    int remain_len_pos = pos;
    buffer[pos++] = 0;

    int topic_len = strlen(topic);
    buffer[pos++] = (topic_len >> 8) & 0xFF;
    buffer[pos++] = topic_len & 0xFF;
    memcpy(buffer + pos, topic, topic_len);
    pos += topic_len;

    buffer[pos++] = (*pkt_id >> 8) & 0xFF;
    buffer[pos++] = *pkt_id & 0xFF;

    memcpy(buffer + pos, payload, payload_len);
    pos += payload_len;

    buffer[remain_len_pos] = topic_len + 2 + 2 + payload_len;
    return pos;
}

// ����PINGREQ����
static int build_pingreq_packet(uint8_t *buffer)
{
    buffer[0] = MQTT_PINGREQ;
    buffer[1] = 0;
    return 2;
}

// �ӽ��ջ�������ȡһ��������MQTT���ģ��򻯣��������̶�ͷ+ʣ�೤�ȣ�
static int parse_received_packet(u8 *packet_type, u16 *packet_id)
{
    // ��ʵ�֣����������������2�ֽڣ�������һ���ֽڵı�������
    if (mqtt_cache_get < 2) return 0;

    u8 first = mqtt_recv_cache[0];
    *packet_type = first & 0xF0;

    // ����ʣ�೤��С��128���ڶ��ֽھ���ʣ�೤��
    int remain = mqtt_recv_cache[1];
    int total = 2 + remain;

    if (mqtt_cache_get < total) return 0;  // ���Ĳ�����

    // ��������а�ID��PUBACK�����ӿɱ�ͷ��ȡ
    if (*packet_type == MQTT_PUBACK && remain >= 2)
    {
        *packet_id = (mqtt_recv_cache[2] << 8) | mqtt_recv_cache[3];
    }

    // �Ƴ��Ѵ����ı���
    memmove(mqtt_recv_cache, mqtt_recv_cache + total, mqtt_cache_get - total);
    mqtt_cache_get -= total;
    return 1;
}
#endif

// ����MQTT�ͻ���
int Base_TCP_MQTT_Config(char *config, int enable)
{
    int retval = 0;
    #if Exist_ETH
    if (config != NULL && enable)
    {
        int temp_data = 0,temp_num;
        temp_num = strlen(config);
        memset(mqtt_user, 0, sizeof(mqtt_user));
        memset(mqtt_pass, 0, sizeof(mqtt_pass));
        memset(mqtt_Ptopic, 0, sizeof(mqtt_pass));
        memset(mqtt_Stopic, 0, sizeof(mqtt_pass));
        memset(mqtt_broker_ip, 0, sizeof(mqtt_broker_ip));
        memset(mqtt_broker_port, 0, sizeof(mqtt_broker_port));
        memset(mqtt_default_topic, 0, sizeof(mqtt_default_topic));
        memset(mqtt_client_id, 0, sizeof(mqtt_client_id));

        temp_data = Caven_gain_str_by_sign(config,temp_num,mqtt_array,"URL",'<');
        if(temp_data)
        {
            temp_data = Base_ETH_data_To_url(mqtt_array,mqtt_client_id,mqtt_default_topic);
            if(temp_data)
            {
                temp_data = Caven_URL_IPprot (mqtt_client_id,mqtt_broker_ip,mqtt_broker_port);
            }
        }
        Caven_gain_str_by_sign(config,temp_num,mqtt_user,"User",'<');
        Caven_gain_str_by_sign(config,temp_num,mqtt_pass,"Pass",'<');
        Caven_gain_str_by_sign(config,temp_num,mqtt_Ptopic,"Ptopic",'<');
        Caven_gain_str_by_sign(config,temp_num,mqtt_Stopic,"Stopic",'<');
        Caven_gain_str_by_sign(config,temp_num,mqtt_client_id,"Id",'<');

        Debug_printf("MQTT %s\n",config);
        if (temp_data) {
            Base_ETH_MQTT_pFun_Bind(Base_TCP_MQTT_Task);
            Base_TCP_Client_Receive_Bind_Fun(Base_TCP_MQTT_GET_Fun);
            MQTT_init_flag = 1;
            Debug_printf("MQTT url %s:%s/%s,Init succ\n",mqtt_broker_ip,mqtt_broker_port,mqtt_default_topic);
            Debug_printf("%s\n",mqtt_user);
            Debug_printf("%s\n",mqtt_pass);
            Debug_printf("%s\n",mqtt_Ptopic);
            Debug_printf("%s\n",mqtt_Stopic);
            Debug_printf("%s\n",mqtt_client_id);
        }
        else
        {
            Debug_printf("MQTT url null,Init fail\n");
        }
    }
    else if(enable == 0)
    {
        Base_TCP_Client_Config(NULL, NULL, 0);
        MQTT_init_flag = 0;
    }
    retval = MQTT_init_flag;
    #endif
    return retval;
}

// �������������
int Base_TCP_MQTT_cache_Send_Fun(char *data, int len)
{
    int retval = 0;
    #if Exist_ETH
    if (data != NULL && MQTT_init_flag)
    {
        if ((len + mqtt_cache_run) < sizeof(mqtt_send_cache))
        {
            memcpy(&mqtt_send_cache[mqtt_cache_run], data, len);
            mqtt_cache_run += len;
            mqtt_send_cache[mqtt_cache_run] = 0;
        }
    }
    #endif
    return retval;
}

// ��ȡ���յ���Ӧ����Ϣ����������·���PUBLISH��
int Base_TCP_MQTT_cache_Read_Fun(char *data, int len_max)
{
    int retval = 0;
    #if Exist_ETH
    if (data != NULL && len_max > 0 && mqtt_cache_get)
    {
        retval = (len_max < mqtt_cache_get) ? len_max : mqtt_cache_get;
        memcpy(data, mqtt_recv_cache, retval);
        memmove(mqtt_recv_cache, mqtt_recv_cache + retval, mqtt_cache_get - retval);
        mqtt_cache_get -= retval;
        mqtt_cache_read = 1;
    }
    #endif
    return retval;
}

// MQTT�����񣨳�����ģʽ��
void Base_TCP_MQTT_Task(u8 noway, u8 nobady)
{
    #if Exist_ETH
    static u8 state = MQTT_STATE_DISCONNECTED;
    static Task_Overtime_Type task_timer = {
        .Begin_time = {0},
        .Set_time.SYS_Sec = 3,          // ��ʱ3��
        .Set_time.SYS_Us = 0,
        .Switch = 1,
    };
    SYS_BaseTIME_Type now_time;
    Caven_BaseTIME_Type timer_now;
    u8 send_buff[0x1000];
    int len;
    static u8 current_pkt_id;            // ��ǰ�����İ�ID

    if (MQTT_init_flag == 0)
    {
        state = MQTT_STATE_DISCONNECTED;
        return;
    }

    SYS_Time_Get(&now_time);
    timer_now.SYS_Sec = now_time.SYS_Sec;
    timer_now.SYS_Us = now_time.SYS_Us;

    switch (state)
    {
        case MQTT_STATE_DISCONNECTED:
            // ��������TCP
            Base_TCP_Client_Config(NULL, NULL, 0);
            Base_TCP_Client_Config(mqtt_broker_ip, mqtt_broker_port, 1);
            state = MQTT_STATE_CONNECTING;
            break;

        case MQTT_STATE_CONNECTING:
            if (Base_TCP_Client_Config(NULL, NULL, 1) == 1) // TCP������
            {
                mqtt_cache_get = 0;
                mqtt_cache_run = 0;
                mqtt_cache_read = 0;
                len = build_connect_packet(send_buff, mqtt_client_id,mqtt_user,mqtt_pass);
                if (Base_TCP_Client_Send(send_buff, len) == 0)
                {
                    state = MQTT_STATE_WAIT_CONNACK;
                    task_timer.Begin_time = timer_now;
                    task_timer.Trigger_Flag = 0;
                }
            }
            break;

        case MQTT_STATE_WAIT_CONNACK:
            API_Task_Timer(&task_timer, timer_now);
            // �������ջ��棬����Ƿ��յ�CONNACK
            if (mqtt_cache_get >= 4) // CONNACK��С����4�ֽ�
            {
                // ���жϵ�һ���ֽ�Ϊ0x20
                if ((mqtt_recv_cache[0] & 0xF0) == MQTT_CONNACK)
                {
                    // ���ճɹ����Ƴ�����
                    int remain = mqtt_recv_cache[1];
                    int total = 2 + remain;
                    if (mqtt_cache_get >= total)
                    {
                        memmove(mqtt_recv_cache, mqtt_recv_cache + total, mqtt_cache_get - total);
                        mqtt_cache_get -= total;
                        // ������֤�ɹ����������
                        state = MQTT_STATE_IDLE;
                        last_ping_time = now_time;
                        ping_outstanding = 0;
                        Debug_printf("MQTT_CONNACK ACK \n");
                    }
                }
            }
            if (task_timer.Trigger_Flag) // ��ʱ
            {
                state = MQTT_STATE_CONNECTING;
                Debug_printf("MQTT_CONNACK fuck NACK \n");
            }
            break;

        case MQTT_STATE_IDLE:
            // ��������
            if (!ping_outstanding)
            {
                int sec_elapsed = now_time.SYS_Sec - last_ping_time.SYS_Sec;
                if (sec_elapsed >= MQTT_KEEP_ALIVE)
                {
                    len = build_pingreq_packet(send_buff);
                    if (Base_TCP_Client_Send(send_buff, len) == 0)
                    {
                        state = MQTT_STATE_WAIT_PINGRESP;
                        task_timer.Begin_time = timer_now;
                        task_timer.Trigger_Flag = 0;
                        ping_outstanding = 1;
                    }
                }
            }

            // ����Ƿ��д���������
            if (mqtt_cache_run > 0)
            {
                state = MQTT_STATE_SEND_PUBLISH;
            }
            break;

        case MQTT_STATE_SEND_PUBLISH:
            len = build_publish_packet(send_buff,sizeof(send_buff),mqtt_default_topic, mqtt_send_cache, mqtt_cache_run, &current_pkt_id);
            if (Base_TCP_Client_Send(send_buff, len) == 0)
            {
                state = MQTT_STATE_WAIT_PUBACK;
                task_timer.Begin_time = timer_now;
                task_timer.Trigger_Flag = 0;
            }
            else
            {
                // ����ʧ�ܣ����ܶϿ����ص�DISCONNECTED
                state = MQTT_STATE_CONNECTING;
            }
            break;

        case MQTT_STATE_WAIT_PUBACK:
            API_Task_Timer(&task_timer, timer_now);
            // �ӽ��ջ����н���PUBACK
            {
                u8 pkt_type;
                u16 pkt_id;
                while (parse_received_packet(&pkt_type, &pkt_id))
                {
                    if (pkt_type == MQTT_PUBACK && pkt_id == current_pkt_id)
                    {
                        // �յ���ȷ��PUBACK�������ɹ�
                        mqtt_cache_run = 0;          // ��շ��ͻ���
                        state = MQTT_STATE_IDLE;
                        break;
                    }
                    // �������ģ���������·��������ڻ����У��ϲ�ɶ�ȡ
                }
            }
            if (task_timer.Trigger_Flag)
            {
                // ��ʱ����
                state = MQTT_STATE_CONNECTING;
            }
            break;

        case MQTT_STATE_SEND_PING:
            // ʵ�����Ѿ��ϲ���IDLE��ֱ�ӷ�PING�����WAIT_PINGRESP����״̬δʹ�ã�����
            state = MQTT_STATE_DISCONNECTED;
            break;

        case MQTT_STATE_WAIT_PINGRESP:
            API_Task_Timer(&task_timer, timer_now);
            {
                u8 pkt_type;
                u16 pkt_id;
                while (parse_received_packet(&pkt_type, &pkt_id))
                {
                    if (pkt_type == MQTT_PINGRESP)
                    {
                        // ������Ӧ����
                        ping_outstanding = 0;
                        last_ping_time = now_time;
                        state = MQTT_STATE_IDLE;
                        break;
                    }
                }
            }
            if (task_timer.Trigger_Flag)
            {
                // PING��ʱ����Ϊ���ӶϿ�
                state = MQTT_STATE_CONNECTING;
            }
            break;

        default:
            state = MQTT_STATE_DISCONNECTED;
            break;
    }

    // ����TCP��������Ͽ�
    if (Base_TCP_Client_Config(NULL, NULL, 1) != 1 && state > MQTT_STATE_CONNECTING)
    {
        state = MQTT_STATE_DISCONNECTED;
    }
    #endif
}
