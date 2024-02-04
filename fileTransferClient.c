#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_SIZE 1024
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_SIZE];

    // 创建客户端套接字
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // 输入要传输的文件名
    printf("Enter file name: ");
    fgets(buffer, MAX_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    // 发送文件名给服务器
    if (send(client_socket, buffer, strlen(buffer), 0) == -1)
    {
        perror("send");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // 创建新文件以保存接收的数据
    FILE *file = fopen("received_file.txt", "wb");
    if (file == NULL)
    {
        perror("fopen");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // 接收服务器发送的数据并写入文件
    while (1)
    {
        ssize_t bytesRead = recv(client_socket, buffer, MAX_SIZE, 0);
        if (bytesRead <= 0)
        {
            break;
        }

        size_t bytesWritten = fwrite(buffer, sizeof(char), bytesRead, file);
        if (bytesWritten < bytesRead && ferror(file))
        {
            perror("fwrite");
            fclose(file);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    }

    printf("File received successfully.\n");

    // 关闭文件和套接字
    fclose(file);
    close(client_socket);

    return 0;
}