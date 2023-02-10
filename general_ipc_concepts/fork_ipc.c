// This example program shows how fork works in Linux 

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void fork_example_1()
{
        int numberOfFork = 0;
    printf("This program helps to fork new process.\n");
    // printf("Enter number of process and inner process you want to fork? : \n");
    // scanf("%d", &numberOfFork);

    // for(int index=0; index < numberOfFork; index++)
    // {
    pid_t p1 = fork();
    if(p1 == 0)
    {//1st Child
        printf("1st Child: New process was forked here: new process id: %d\n", getpid());
        printf("1st Child Parent process id for this Child process is: %d\n", getppid());
    }
    else{//Parent
        pid_t p2 = fork();
        if(p2 == 0)
        {//2nd Child
            pid_t status;
            pid_t r1 = waitpid(p1, &status, 0);
            printf("2nd Child: wait status of process id: %d is ret: %d and status: %d \n", p1, r1, WIFEXITED(status));
            printf("2nd Child: New process was forked here: new process id: %d\n", getpid());
            printf("2nd Child Parent process id for this Child process is: %d\n", getppid());
        }
        else
        {//Parent
            pid_t status;
            pid_t r1 = wait(&status);
            printf("Parent: wait status of process id: %d is ret: %d and status: %d\n", p1, r1, WIFEXITED(status));
            //wait(NULL); //Parent process wait for all the child process to complete
            printf("Parent: Current process forks a new process with new process id: %d %d\n", p1, p2);
            printf("Parent: Current process id: %d\n", getpid());
            printf("Parent: Parent process id for this current process is: %d\n", getppid());
        }        

    }
    printf("I will come from both child and parent %d: \n", getpid());
}


void fork_with_execl_example()
{
    pid_t pid = fork();
    if(pid == 0)
    {//Child
        printf("I am the child with pid: %d\n", getpid());
        printf("Now I am going to replace my self with ps process image \n");
        execl("/bin/ps", "ps", NULL);
        printf("It doesn't matter what I put after execl, since this is not going to run \n");
    }
    else
    {
        printf("I am the parent with pid: %d\n", getpid());
    }
}

int main(int argc, char* argv[])
{
    //fork_example_1();
    fork_with_execl_example();
}