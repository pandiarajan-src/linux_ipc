// This program demonstrate a sample where app send message to itself

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void catch_internal_sig_handler(int c)
{
    printf("got the singal %d\n", c);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, catch_internal_sig_handler);

    printf("INFO: Going to raise SIGINT (app customized)\n");
    raise(SIGINT);

    printf("Exiting....");
    return 0;
}