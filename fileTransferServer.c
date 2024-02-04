#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_SIZE 1024

typedef struct
{
    int socket;
    struct sockaddr_in address;
} client_info;

void *handle_client(void *arg)
{
    client_info *client = (client_info *)arg;
    int client_socket = client->socket;
    struct sockaddr_in client_addr = client->address;
    char buffer[MAX_SIZE];

    // 接收文件名
    ssize_t bytesReceived = recv(client_socket, buffer, MAX_SIZE, 0);
    if (bytesReceived <= 0)
    {
        perror("recv");
        close(client_socket);
        free(client);
        pthread_exit(NULL);
    }

    buffer[bytesReceived] = '\0';
    printf("Received file name from client %s: %s\n", inet_ntoa(client_addr.sin_addr), buffer);

    // 打开要传输的文件
    FILE *file = fopen(buffer, "rb");
    if (file == NULL)
    {
        perror("fopen");
        close(client_socket);
        free(client);
        pthread_exit(NULL);
    }

    // 逐块读取文件内容并发送给客户端
    while (!feof(file))
    {
        size_t bytesRead = fread(buffer, sizeof(char), MAX_SIZE, file);
        if (bytesRead < MAX_SIZE && ferror(file))
        {
            perror("fread");
            fclose(file);
            close(client_socket);
            free(client);
            pthread_exit(NULL);
        }

        if (send(client_socket, buffer, bytesRead, 0) == -1)
        {
            perror("send");
            fclose(file);
            close(client_socket);
            free(client);
            pthread_exit(NULL);
        }
    }

    printf("File transfer completed successfully to client %s\n", inet_ntoa(client_addr.sin_addr));

    // 关闭文件和套接字
    fclose(file);
    close(client_socket);
    free(client);
    pthread_exit(NULL);
}

int main()
{
    int server_socket;
    struct sockaddr_in server_addr;
    client_info *clients[MAX_CLIENTS];
    pthread_t threads[MAX_CLIENTS];
    int num_clients = 0;

    // 创建服务器端套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // 绑定服务器地址和端口
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (listen(server_socket, MAX_CLIENTS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections...\n");

    while (1)
    {
        // 接受客户端连接请求
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Connected with client: %s\n", inet_ntoa(client_addr.sin_addr));

        // 创建客户端信息结构体
        client_info *client = (client_info *)malloc(sizeof(client_info));
        client->socket = client_socket;
        client->address = client_addr;

        // 创建线程处理客户端请求
        pthread_create(&threads[num_clients], NULL, handle_client, (void *)client);

        clients[num_clients] = client;
        num_clients++;
    }

    // 等待所有线程结束
    for (int i = 0; i < num_clients; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // 关闭服务器套接字
    close(server_socket);

    return 0;
}