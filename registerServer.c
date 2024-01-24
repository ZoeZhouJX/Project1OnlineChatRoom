#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <error.h>
#include <json-c/arraylist.h>
#include <json-c/json.h>

#define SERVER_PORT 8080
#define MAX_LISTEN 128
#define LOCAL_IPADDRESS "172.28.106.66"
#define BUFFER_SIZE 128
#define COMMUNICATION_SIZE 512

int main()
{
    /* 创建socket套接字 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    /* 设置端口复用 */
    int enableOpt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&enableOpt, sizeof(enableOpt));
    if (ret == -1)
    {
        perror("setsockopt error");
        exit(-1);
    }

    struct sockaddr_in localAddress;
    /* 清除脏数据 */
    memset(&localAddress, 0, sizeof(localAddress));

    /* 地址族 */
    localAddress.sin_family = AF_INET;
    /* 端口需要转成大端 */
    localAddress.sin_port = htons(SERVER_PORT);
    /* ip地址需要转成大端 */
    /* Address to accept any incoming messages.  */
    /* INADDR_ANY = 0x00000000 */
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    /* 绑定 */
    int localAddressLen = sizeof(localAddress);
    ret = bind(sockfd, (struct sockaddr *)&localAddress, localAddressLen);
    if (ret == -1)
    {
        perror("bind error");
        exit(-1);
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret == -1)
    {
        perror("listen error");
        exit(-1);
    }

    /* 客户的信息 */
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(struct sockaddr_in));

    /* 接收客户端的连接 */
    socklen_t clientAddressLen = 0;
    int acceptfd = accept(sockfd, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (acceptfd == -1)
    {
        perror("accpet error");
        exit(-1);
    }

    char recvBuf[COMMUNICATION_SIZE];
    memset(recvBuf, 0, sizeof(recv));

    char sendBuf[COMMUNICATION_SIZE];
    memset(sendBuf, 0, sizeof(sendBuf));

    read(acceptfd, recvBuf, sizeof(recvBuf));
    printf("%s\n", recvBuf);

    strncpy(sendBuf, "available", sizeof(sendBuf) - 1);
    write(acceptfd, sendBuf, sizeof(sendBuf));

    memset(recvBuf, 0, sizeof(recv));
    read(acceptfd, recvBuf, sizeof(recvBuf));
    printf("%s\n", recvBuf);

    memset(sendBuf, 0, sizeof(sendBuf));
    strncpy(sendBuf, "registerSuccessful", sizeof(sendBuf) - 1);
    write(acceptfd, sendBuf, sizeof(sendBuf));

    close(acceptfd);
    close(sockfd);
}