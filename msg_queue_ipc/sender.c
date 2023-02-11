#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <mqueue.h>


#define MAX_MSG_SIZE        256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)

int main(int argc, char* argv[])
{
    char buffer[MSG_BUFFER_SIZE];
    int msg_q_fd = 0;

    if(argc <= 1)
    {
        printf("Please provide a message queue name : format </msgq-name>\n");
        return 0;
    }

    if( (msg_q_fd = mq_open(argv[1], O_WRONLY|O_CREAT, 0, 0)) == -1)
    {
        perror("ERROR: message queue creation problem \n");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, MSG_BUFFER_SIZE);
    printf("Please enter a message to receiver : %s\n", argv[1]);
    if( scanf("%s", buffer) )
    {
        if(mq_send(msg_q_fd, buffer, strlen(buffer)+1, 0) == -1)
        {
            perror("ERROR: sending data to message queue is a problem...\n");
            exit(EXIT_SUCCESS);

        }
    }

    mq_close(msg_q_fd);
    return 0;
}