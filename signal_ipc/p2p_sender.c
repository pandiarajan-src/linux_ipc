// This program demonstrate Process A to Process B IPC communication
// This process is sender who sends the signal to destination process

#include <signal.h>
#include <stdio.h>

#define PROCESS_ID 4489 // Find the proess id of receiver

int main(int argc, char* argv[])
{
    printf("Singal sender proess starting\n");
    kill(PROCESS_ID, SIGUSR1);
    scanf("waiting for input to teriminate sender process\n");
    return 0;
}