#include "Heap.h"
#include "Log.h"

static void traceCallback(enum LOG_LEVELS level, const char* message)
{
    printf("[%d]:%s", level, message);
    return;
}

int main()
{

    (void)Heap_initialize();
    (void)Log_initialize(NULL);

    heap_info *hi = Heap_get_info();

    char *buf1 = malloc(1024);

    char *buf2 = malloc(1024);

    char *buf3 = malloc(1024);

    Log_setTraceLevel(TRACE_MAXIMUM);
    Log_setTraceCallback(traceCallback);

    Log(TRACE_PROTOCOL, -1, "Current heap size: %zu, max heap size: %zu.\n", hi->current_size, hi->max_size);

    free(buf1);

    free(buf2);

    free(buf3);

    Log(TRACE_PROTOCOL, -1, "Current heap size: %zu, max heap size: %zu.\n", hi->current_size, hi->max_size);

    Log_terminate();
    Heap_terminate();

    return 0;
}
