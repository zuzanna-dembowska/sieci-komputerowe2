#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 512

int g_server_socket;
int g_connection_fd;
char g_net_buffer[BUFFER_SIZE];

void int_signal_handler()
{
    fprintf(stderr, "Exiting server...\n");
    if (g_connection_fd != 0)
        close(g_connection_fd);
    close(g_server_socket);
    exit(EXIT_SUCCESS);
}

struct sockaddr_in prepared_address()
{
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(1337);
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    return srv_addr;
}

void server_init()
{
    g_server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (g_server_socket == -1)
    {
        perror("Creating a server-side socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = prepared_address();

    if (bind(g_server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding a socket failed");
        close(g_server_socket);
        exit(EXIT_FAILURE);
    }

    // One possible request
    if (listen(g_server_socket, 1) == -1)
    {
        perror("Listening failed");
        close(g_server_socket);
        exit(EXIT_FAILURE);
    }
}

void server_loop()
{
    socklen_t sock_size = sizeof(struct sockaddr_in);
    char client_addr[16];
    char input[BUFFER_SIZE];

    for (;;)
    {
        struct sockaddr_in client_info;
        g_connection_fd = accept(g_server_socket, NULL, NULL);

        if (g_connection_fd == -1)
        {
            perror("Accepting failed");
            close(g_server_socket);
            exit(EXIT_FAILURE);
        }

        getpeername(g_connection_fd, (struct sockaddr *)&client_info, &sock_size);

        inet_ntop(AF_INET, &(client_info.sin_addr), client_addr, INET_ADDRSTRLEN);
        
        printf("# A new client from %s came in! Conversation started.\n", client_addr);

        for (;;)
        {
            memset(g_net_buffer, 0, BUFFER_SIZE); // delete

            int read_bytes = read(g_connection_fd, g_net_buffer, BUFFER_SIZE); // buff
            if (read_bytes == 0)
            {
                break;
            }

            printf("CLIENT> %s\n", g_net_buffer); // buff
            printf("SERVER> ");
            fgets(input, BUFFER_SIZE, stdin);
            write(g_connection_fd, input, strlen(input) + 1);
        }
        
        puts("# This conversation has ended. Waiting for a new adventurer!");

        if (shutdown(g_connection_fd, SHUT_RDWR) == -1)
        {
            close(g_connection_fd);
        }
        close(g_connection_fd);
        g_connection_fd = 0;
    }
}

int main()
{
    signal(SIGINT, int_signal_handler);
    signal(SIGTERM, int_signal_handler);

    memset(g_net_buffer, 0, BUFFER_SIZE);

    server_init();
    server_loop();

    close(g_server_socket);
    return EXIT_SUCCESS;
}