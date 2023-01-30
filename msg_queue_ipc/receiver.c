#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)
#define QUEUE_PERMISSIONS   0660

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        printf("Please enter message queue name: format </msgqueuename> \n");
        return 0;
    }

    fd_set readfds;
    char buffer[MSG_BUFFER_SIZE];
    int msgq_fd = 0;

    /* To set msg queue attributes */
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if((msgq_fd = mq_open(argv[1], O_RDONLY|O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("ERROR: Creating a new message queue");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(msgq_fd, &readfds);
        printf("Receiver blocked on select() ....\n");

        select(msgq_fd+1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(msgq_fd, &readfds))
        {
            printf("Message reeived msgQ : %s\n", argv[1]);
            memset(buffer, 0, MSG_BUFFER_SIZE);
            if(mq_receive(msgq_fd, buffer, MSG_BUFFER_SIZE,NULL) == -1)
            {
                printf("ERROR: mq_receive error, errno = %d\n", errno);
                exit(EXIT_FAILURE);
            }
            printf("Message received from queue = %s\n", buffer);
        }
    }
    mq_close(msgq_fd);
    return 0;
}