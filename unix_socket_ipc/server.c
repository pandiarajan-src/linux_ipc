#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


#define SOCKET_NAME "/tmp/Pandi_Linux_IPC_server_socket"
#define BUFF_SIZE 128

int main(int argc, char** argv)
{

    /* This is result value server returns back to client */
    int result = 0;

#if 0
    struct sockaddr_un
    {
        sa_family_t sa_family;          /* AF_UNIX */
        char        sa_path;            /* path name */
    }
#endif

    /*
    Incase if the previous socket is open due to some half-backed exit.
    Close any existing socket connection
    */
   unlink(SOCKET_NAME);

   /*
   Create a Master Socket File descriptor
   Use SOCKET_DATAGRAM for communication
   */
    int connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1)
    {
        perror("ERROR: master socket file descriptor creation error...");
        exit(EXIT_FAILURE);
    }
    printf("INFO: Master socket file descriptor was successfully created... socket: %d\n", connection_socket);

    /*
    Initialize the memory and values
    Specify the socket credentials...
    */
    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path)-1);

    /*
    Bind socket to socket name
    Purpose of the bind() system call is that application() dictate the u nderlying
    opeating system the criteria of recieving the data. Here, bind() system call is
    telling the IS that if sender process sends the data destined to sokcet "/tmp/socketname",
    then such data needs to be delivered to this process me(the server process)
    */
    int bind_ret = bind(connection_socket, (const struct sockaddr*)&name, sizeof(struct sockaddr_un));
    if(bind_ret == -1)
    {
        perror("ERROR: Creating bind() call on the server has error...");
        exit(EXIT_FAILURE);
    }
    printf("INFO: bind() call succeeded...bind ret: %d\n", bind_ret);

    /*
    Prepare for accepting connection
    The backlog size is set to 20.
    so while one request is being processed other requests can be waiting.
    */
    int lis_ret = listen(connection_socket, 20);
    if(lis_ret == -1)
    {
        perror("ERROR: listen api call on the server has errror...");
        exit(EXIT_FAILURE);
    }
    printf("INFO: listen() call succeeded...\n");

    /*
    This is the main loop for handling connections.
    All server process usually runs 24x7.
    Good servers should always up and running and should never go down.
    Have  you ever seen facebook or google pae failed to load??
    */
   int data_socket = 0;
   char buffer[BUFF_SIZE];
   for(;;)
   {
        /* Wait for incoming connections. */
        printf("DEBUG: waiting on accept() sys call\n");
        /* accept() call is system call block */
        data_socket = accept(connection_socket, NULL, NULL);
        if(data_socket == -1){
            perror("ERROR: accept() failure to create a new connection...");
            exit(EXIT_FAILURE);
        }
        printf("INFO: connection accepted from client...\n");

        int result = 0;
        for(;;)
        {
            /* Prepare the buffer to receive the data */            
            memset(buffer, 0, BUFF_SIZE);

            /* Wait for the next data packet to arrive */
            /* Server is blicked here. Waiting for the data to arrive from client */
            /* read() is blocking system call */
            printf("Waiting for data from the client \n");
            int read_ret = read(data_socket, buffer, BUFF_SIZE);
            if(read_ret == -1)
            {
                perror("read() data socket calls fail ");
                exit(EXIT_FAILURE);
            }

            /* Add received summand. */
            int data = 0;
            memcpy(&data, buffer, sizeof(int()));
            if(data == 0) break;
            result += data;
        }

        /* send the result back to client */
        memset(buffer, 0, BUFF_SIZE);
        sprintf(buffer, "Result = %d", result);

        printf("INFO: sending final result back to client.\n");
        int write_ret = write(data_socket, buffer, BUFF_SIZE);
        if(write_ret == -1)
        {
            perror("ERROR: Write back fails...");
            exit(EXIT_FAILURE);
        }
        printf("INFO: write() to client was successful...\n");

        /* Close the socket */
        close(data_socket);
   }

   /* Close the connection socket */
   close(connection_socket);
   printf("INFO: Connection closed successfully...\n");

   /* Server should release resources before getting terminated */
   unlink(SOCKET_NAME);
   exit(EXIT_SUCCESS);

    return EXIT_SUCCESS;
}