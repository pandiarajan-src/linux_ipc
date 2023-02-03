//This program demonstrate a sample that send signal from OS to process

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

static void ctrlC_SigHandler(int c)
{
    printf("Ctrl+C pressed by user\n");
    printf("Bye Bye!!!");
    exit(0);
}

static void abort_SigHandler(int c)
{
    printf("abort() call received from OS\n");
    printf("Bye Bye!!!");
    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, ctrlC_SigHandler);
    signal(SIGABRT, abort_SigHandler);

    char ch;
    printf("Abort process (y/n)?\n");
    scanf("%c", &ch);
    if(ch == 'y')
    {
        abort();
    }
    return 0;
}