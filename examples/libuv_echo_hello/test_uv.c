#include <stdio.h>
#include <signal.h>

int interrupted = 0;

static void handleInterrupt(int sig) {
    interrupted = 1;
}

int main(int argc, const char * argv[])
{
    signal(SIGINT, handleInterrupt);
    printf("Hello, World!\n");
    while(!interrupted) {
    }
    return 0;
}

