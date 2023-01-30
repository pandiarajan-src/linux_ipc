#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/Pandi_Linux_IPC_server_socket"
#define BUFF_SIZE 128

int main(int argc, char** argv)
{

    /* Create a unix domain socket */
    int data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(data_socket == -1)
    {
        perror("ERROR: socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("INFO: Socket creation is successful...\n");

    /* Prepare the socket address buffer */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path)-1);

    /* Connect to the domain socket */
    int conn_ret = connect(data_socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un));
    if(conn_ret == -1)
    {
        perror("ERROR: connect() system api fails...");
        exit(EXIT_FAILURE);
    }
    printf("INFO: connect() to server is successful...\n");

    /* Send data to the server */
    int input = 0;
    do
    {
        input = 0;        
        printf("Enter the number to send to server: \n");
        scanf("%d", &input);
        int ret = write(data_socket, &input, sizeof(int));
        if(ret == -1)
        {
            perror("ERROR: sending data to server fails...");
            break;
        }
        printf("No of bytes sent: %d, data sent: %d\n", ret, input);

    }while(input);

    /* Read data from the server */
    char buffer[BUFF_SIZE];
    memset(buffer, 0, BUFF_SIZE);
    int ret = read(data_socket, buffer, BUFF_SIZE);
    if(ret == -1)
    {
        perror("ERROR: receiving data from server fails...");
        exit(EXIT_FAILURE);
    }
    printf("Recd from server: %s\n", buffer);

    /* Close the socket */
    close(data_socket);

    exit(EXIT_SUCCESS);
}
