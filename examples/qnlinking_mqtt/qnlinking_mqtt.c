#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hmac_sha1/hmac_sha1.h"
#include "cJSON/cJSON.h"
#include "b64/urlsafe_b64.h"

#include "MQTTAsync.h"
#define ADDRESS     "ws://mqtt.qnlinking.com:1883"
#define CLIENTID    ""
#define TOPIC       ""
#define PAYLOAD     ""
#define QOS         0
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;
int disc_finished = 0;
int subscribed = 0;
int finished = 0;




static int GetUsernameSign(char *_pUsername, int *_pLen, const char *_pDak)
{
        char query[256] = {0};
        long timestamp = 0.0;
        timestamp = (long)time(NULL);
        if (!_pDak) {
                return LINK_MQTT_ERROR;
        }
        *_pLen = sprintf(query, "dak=%s&timestamp=%ld&version=v1", _pDak, timestamp);
        if (*_pLen <= 0) {
                return LINK_MQTT_ERROR;
        }
        strncpy(_pUsername, query, *_pLen + 1);
        return LINK_MQTT_SUCCESS;
}


static int GetPasswordSign(const char *_pInput, int _nInLen,
                char *_pOutput, int *_pOutLen, const char *_pDsk)
{
        int ret = 0;
        char hsha1[20] = {0};

        if (!_pInput || !_pDsk) {
                return LINK_MQTT_ERROR;
        }
        ret = hmac_sha1(_pDsk, strlen(_pDsk), _pInput, _nInLen, hsha1, sizeof(hsha1));

        if (ret != 20) {
                return LINK_MQTT_ERROR;
        }
        int outlen = urlsafe_b64_encode(hsha1, 20, _pOutput, _pOutLen);
        *_pOutLen = outlen;

        return LINK_MQTT_SUCCESS;
}

static int UpdateUserPasswd(const void *_pInstance)
{
        if (_pInstance) {
                struct MqttInstance* pInstance = (struct MqttInstance*) (_pInstance);
                int nUsernameLen = 0;
                int nPasswdLen = 0;
                GetUsernameSign(pInstance->options.userInfo.pUsername, &nUsernameLen, gLinkDAK);
                GetPasswordSign(pInstance->options.userInfo.pUsername, nUsernameLen,
                                pInstance->options.userInfo.pPassword, &nPasswdLen, gLinkDSK);
                return LINK_MQTT_SUCCESS;
        }
        return LINK_MQTT_ERROR;
}






int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}
void onDisconnect(void* context, MQTTAsync_successData* response)
{
        printf("Successful disconnection\n");
        disc_finished = 1;
}
void onSubscribe(void* context, MQTTAsync_successData* response)
{
        printf("Subscribe succeeded\n");
        subscribed = 1;
}
void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
        printf("Subscribe failed, rc %d\n", response ? response->code : 0);
        finished = 1;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        int rc;
        printf("Successful connection\n");
        printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
        opts.onSuccess = onSubscribe;
        opts.onFailure = onSubscribeFailure;
        opts.context = client;
        deliveredtoken = 0;
        if ((rc = MQTTAsync_subscribe(client, TOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start subscribe, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;
    printf("\nConnection failure\n");
    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.automaticReconnect = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}

void connlost(void *context, char *cause)
{
        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
        int rc;
        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);
        printf("Reconnecting\n");
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        conn_opts.automaticReconnect = 1;
        conn_opts.onSuccess = onConnect;
        conn_opts.onFailure = onConnectFailure;
        conn_opts.context = client;
//        if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
//        {
//                printf("Failed to start connect, return code %d\n", rc);
//            finished = 1;
//        }
}

int main(int argc, char* argv[])
{
        MQTTAsync client;
        MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
        MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
        MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        MQTTAsync_token token;
        int rc;
        int ch;
        MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTAsync_setCallbacks(client, NULL, connlost, msgarrvd, NULL);
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        conn_opts.automaticReconnect = 1;
        conn_opts.minRetryInterval = 10;
        conn_opts.maxRetryInterval = 10;
        conn_opts.onSuccess = onConnect;
        conn_opts.onFailure = onConnectFailure;
        conn_opts.context = client;
        if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start connect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
        while   (!subscribed)
                #if defined(WIN32) || defined(WIN64)
                        Sleep(100);
                #else
                        usleep(10000L);
                #endif
        if (finished)
                goto exit;
        do
        {
                ch = getchar();
        } while (ch!='Q' && ch != 'q');
        disc_opts.onSuccess = onDisconnect;
        if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start disconnect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
        while   (!disc_finished)
                #if defined(WIN32) || defined(WIN64)
                        Sleep(100);
                #else
                        usleep(10000L);
                #endif
exit:
        MQTTAsync_destroy(&client);
        return rc;
}
