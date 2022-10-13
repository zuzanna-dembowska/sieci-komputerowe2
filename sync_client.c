#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 512

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Specify server IP\n");
    }
    
    char buff[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    int client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Creating a client-side socket failed");
        exit(EXIT_SUCCESS);
    }

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1337);
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(client_socket, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1)
    {
        perror("Connecting to a server failed");
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        printf("CLIENT> ");
        fgets(input, BUFFER_SIZE, stdin);

        write(client_socket, input, strlen(input) + 1);

        int read_bytes = read(client_socket, buff, BUFFER_SIZE);
        if (read_bytes == 0)
        {
            break;
        }
        printf("SERVER> %s\n", buff);
    }

    close(client_socket);
    return EXIT_SUCCESS;
}