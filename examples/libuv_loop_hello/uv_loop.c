#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <uv.h>

uv_async_t async;

void dummy(uv_async_t *handle) {}

int main() {
    uv_loop_t * loop = uv_default_loop();
    uv_async_init(loop, &async, dummy);
    return uv_run(loop, UV_RUN_DEFAULT);
}
