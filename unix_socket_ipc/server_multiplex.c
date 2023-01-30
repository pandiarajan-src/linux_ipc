#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/Pandi_Linux_IPC_server_socket"
#define BUFF_SIZE 128
#define MAX_CLIENTS_ALLOWED 64

/*
An array of file descriptor which the server process maintains
in order to talk to clients.
Master socket is also a member of this array
*/
int monitored_fd_set[MAX_CLIENTS_ALLOWED];

/*
Each connected client's intermediate result is maintained in this client array
*/
int client_results[MAX_CLIENTS_ALLOWED] = {0};

/*
Remove all the FDs, if any from the arrary
*/
static void initialize_monitor_fd_set()
{
    for(int index = 0; index < MAX_CLIENTS_ALLOWED; index++)
    {
        monitored_fd_set[index] = -1;
    }
}

/*
Add new FD to the monitored arrary, including the master FD
*/
static void add_2_monitored_fd_set(int skt_fd)
{
    for(int index = 0; index < MAX_CLIENTS_ALLOWED; index++)
    {
        if( monitored_fd_set[index] != -1 )
            continue;
        monitored_fd_set[index] = skt_fd;
        break;
    }
}

/*
Remove the monitored FD set from the array
*/
static void remove_from_monitored_fd_set(int skt_fd)
{
    for(int index = 0; index < MAX_CLIENTS_ALLOWED; index++)
    {
        if(monitored_fd_set[index] == skt_fd)
        {
            monitored_fd_set[index] = -1;
            break;
        }
    }
}

/*
Clone all the FDs in monitored_fd_set arrary into fd_set Data-Structure
*/
static void refresh_fd_set(fd_set *fd_set_ptr)
{
    FD_ZERO(fd_set_ptr);
    for(int index = 0; index < MAX_CLIENTS_ALLOWED; index++)
    {
        if(monitored_fd_set[index] != -1)
        {
            FD_SET(monitored_fd_set[index], fd_set_ptr);
        }
    }
}

/*
Get the numberical max value among all the FDs which server is monitoring
*/
static int get_max_fd()
{
    int max = -1;
    for(int index = 0; index < MAX_CLIENTS_ALLOWED; index++)
    {
        if(monitored_fd_set[index] > max)
        {
            max = monitored_fd_set[index];
        }
    }
    return max;
}

int main(int argc, char* argv[])
{

    /* define required data sets */
    fd_set readfs;

    /* Initialize monitoring file descriptor set */
    initialize_monitor_fd_set();
    add_2_monitored_fd_set(0);

    /* Remove any existing orphan sockets if any */
    unlink(SOCKET_NAME);    

    /* Create master socket file descriptor */
    int connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1)
    {
        perror("ERROR: Creating master socket file descriptor error...");
        exit(EXIT_FAILURE);
    }
    printf("INFO: Master socket file descriptor created successfully... fd:%d\n", connection_socket);

    /* Prepare the socket structure memory */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path)-1);

    /* Create the bind with communication socket */
    int ret = bind(connection_socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un));
    if(ret == -1)
    {
        perror("ERROR: error in creating bind...\n");
        exit(EXIT_FAILURE);
    }
    printf("INFO: binding to communication socket was successful...ret:%d\n", ret);

    /* Prepare for listen */
    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     * */    
    ret = listen(connection_socket, 20);
    if(ret == -1)
    {
        perror("ERROR: error in creating listen...\n");
        exit(EXIT_FAILURE);
    }
    printf("INFO: listening to communication socket was successful...ret:%d\n", ret); 

    /* Add master socket file descriptor to fd_set for monitoring */
    add_2_monitored_fd_set(connection_socket);

    /* This is the main loop for handling connections. */
    /*All Server process usually runs 24 x 7. Good Servers should always up
     * and running and shold never go down. Have you ever seen Facebook Or Google
     * page failed to load ??*/
    for(;;)
    {
        /* Refresh or Clone the existing FD set. Copy all the monitored fd with readfds*/
        refresh_fd_set(&readfs);

        printf("Waiting on select() sys call...\n");

        /* 
        Call the select system call, server process blocks  here
        Linux OS keeps this process blocked until the connection initiation request 
        Or
        Data requests arrives on any of the file descriptor in the 'readfs' set
        */
        select(get_max_fd()+1, &readfs, NULL, NULL, NULL);
        if(FD_ISSET(connection_socket, &readfs))
        {
            /* 
            Data arrives on Master socket only when new client connects with the server
            that is 'connect' call is invoked on the client side.
            */
            printf("New connection received, accept the new connection\n");
            int data_socket = accept(connection_socket, NULL, NULL);
            if(data_socket == -1)
            {
                perror("ERROR: error in creating data_socket...");
                exit(EXIT_FAILURE);
            }
            printf("INFO: connection accepted for the data_socket: %d\n", data_socket); 

            /* Add data socket file descriptor to fd_set for monitoring */
            add_2_monitored_fd_set(data_socket);
        }
        else if( FD_ISSET(0, &readfs))
        {
            char buffer[BUFF_SIZE];
            memset(buffer, 0, BUFF_SIZE);
            ret = read(0, buffer, BUFF_SIZE);
            printf("Input read from console: %s\n", buffer);
        }
        else /* Data arrives on some other client FD */
        {
            printf("No new client request, existing client sending data \n");
            /* Find the client who sent the data request */
            int client_data_socket_fd = -1;
            char buffer[BUFF_SIZE];
            int data = 0;
            for(int client_index = 0; client_index < MAX_CLIENTS_ALLOWED; client_index++)
            {
                if(FD_ISSET(monitored_fd_set[client_index], &readfs))
                {
                    client_data_socket_fd = monitored_fd_set[client_index];

                    /* Prepare memory to receive the data fromt he client */
                    memset(buffer, 0, BUFF_SIZE);

                    /* Wait for the next data packet fromt he client */
                    /* Server is blocked here, read() is a system blocking call */
                    printf("Waiting for data from the client \n");
                    ret = read(client_data_socket_fd, buffer, BUFF_SIZE);
                    if(ret == -1)
                    {
                        perror("ERROR: error in reading data from client...");
                        exit(EXIT_FAILURE);
                    }
                    printf("INFO: connection read the data:\n"); 

                    /* Received new data from the client */
                    memcpy(&data, buffer, sizeof(int));
                    if(data == 0)
                    {
                        /* Send result back to client */
                        memset(buffer, 0, BUFF_SIZE);
                        sprintf(buffer, "Result = %d", client_results[client_index]);

                        ret = write(client_data_socket_fd, buffer, BUFF_SIZE);
                        if(ret == -1)
                        {
                            perror("ERROR: error in writing data to client...");
                            exit(EXIT_FAILURE);
                        }
                        printf("INFO: data was sent back to client successfully...\n");                         
                        
                        /* Close client socket communication */
                        close(client_data_socket_fd);
                        client_results[client_index] = 0;
                        remove_from_monitored_fd_set(client_data_socket_fd);
                        continue;
                    }
                    client_results[client_index] += data;
                }
            }
        }
    }

    close(connection_socket);
    printf("Connection closed \n");
    remove_from_monitored_fd_set(connection_socket);

    /* Remove any existing orphan sockets if any */
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}

