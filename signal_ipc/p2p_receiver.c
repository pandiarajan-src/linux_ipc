// This program demonstrate Process A to Process B IPC communication
// This process is receiver who receives and handles the signal

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sig_handler_from_other_process(int c)
{
    printf("Received message from other process\n");
    return;
}

int main(int argc, char* arv[])
{
    signal(SIGUSR1, sig_handler_from_other_process);
    printf("starting the receiver process\n");
    scanf("waiting for some input to break\n");
    return 0;
}