#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MQTTAsync.h"
#define ADDRESS     "tcp://link.router7.com:1883"
#define CLIENTID    "ExampleClientSub2"
#define TOPIC       "gateway/test0"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTAsync_token deliveredtoken;
int disc_finished = 0;
int subscribed = 0;
int finished = 0;

/**
 * Command callback
 * */

void onDisconnect(void* context, MQTTAsync_successData* response)
{
        printf("Successful disconnection\n");
        disc_finished = 1;
}

void onDisconnectFailure(void* context, MQTTAsync_successData* response)
{
        printf("Fail disconnection\n");
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
    printf("\nSuccessful connection\n");
}

void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("\nConnection failure\n");
}


/**
 * Global callback
 * */

void connected(void* context, char* cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    //MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
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
    return;
}

void disconnected(void* context, MQTTProperties* properties, enum MQTTReasonCodes reasonCode)
{
    printf("Server Disconnect connection\n");
    return;
}


void connlost(void *context, char *cause)
{

        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);

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

/**
 * Trace callback
 * */

static void traceCallback(enum MQTTASYNC_TRACE_LEVELS level, char* message)
{
    printf("[ %d ]: %s\n", level, message);
    return;
}

int main(int argc, char* argv[])
{
        MQTTAsync client;
        MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
        MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
        //MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        //MQTTAsync_token token;
        int rc;
        int ch;
        MQTTAsync_setTraceLevel(MQTTASYNC_TRACE_MAXIMUM);
        MQTTAsync_setTraceCallback(traceCallback);

        MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        MQTTAsync_setCallbacks(client, (void *)client, connlost, msgarrvd, NULL);
        MQTTAsync_setConnected(client, (void *)client, connected);
        MQTTAsync_setDisconnected(client, (void *)client, disconnected);

        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        conn_opts.automaticReconnect = 1;
        conn_opts.minRetryInterval = 10;
        conn_opts.maxRetryInterval = 10;
        conn_opts.onSuccess = onConnect;
        conn_opts.onFailure = onConnectFailure;
        conn_opts.context = client;
        conn_opts.username = "admin";
        conn_opts.password = "admin";
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
