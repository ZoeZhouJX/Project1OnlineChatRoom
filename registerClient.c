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
#define SERVER_IP "172.28.106.66"
#define BUFFER_SIZE 128
#define COMMUNICATION_SIZE 512

/* 新建一个用户信息 */
int Register(int sockfd, char *buf)
{
    int ret = 0;

    /* 年龄 */
    int age = 0;
    /* 昵称 */
    char name[BUFFER_SIZE];
    memset(name, 0, sizeof(char) * BUFFER_SIZE);
    /* 性别 */
    char sex[BUFFER_SIZE];
    memset(sex, 0, sizeof(char) * BUFFER_SIZE);
    /* 账号 */
    char account[BUFFER_SIZE];
    /* 密码 */
    char password[BUFFER_SIZE];
    memset(password, 0, sizeof(char) * BUFFER_SIZE);
    /* 地址 */
    char address[BUFFER_SIZE];
    memset(address, 0, sizeof(char) * BUFFER_SIZE);
    /* 个性签名 */
    char personalSign[BUFFER_SIZE];
    memset(personalSign, 0, sizeof(char) * BUFFER_SIZE);

    /* 获取合法账号以及访问服务端账号是否存在 */
    /* 向服务端发起确认账号是否已存在 */
    /* 向服务器发送一个包含行动和数据的json格式的字符串 */
    struct json_object *accountObj = json_object_new_object();
    while (1)
    {
        int flag = 0;
        int flag1 = 1;
        /* 获取纯数字的账号 */
        while (flag == 0)
        {
            flag = 1;
            memset(account, 0, sizeof(char) * BUFFER_SIZE);
            system("clear");
            printf("账号(10位整数):");
            scanf("%s", account);
            if (strlen(account) != 10)
            {
                system("clear");
                if (flag1 == 1)
                {
                    printf("请输入10位整数\n");
                }
                else
                {
                    printf("小只因：不识字吗?\n");
                }
                flag1 = 0;
                flag = 0;
                sleep(1);
                while (getchar() != '\n');
            }
            else
            {
                for (int idx = 0; idx < strlen(account); idx++)
                {
                    if (account[idx] < '0' || account[idx] > '9')
                    {
                        printf("无效的输入\n");
                        sleep(1);
                        flag = 0;
                        break;
                    }
                }
            }
        }

        /* 添加行动和数据 */
        json_object_object_add(accountObj, "action", json_object_new_string("duplicateCheck")); // 查重
        json_object_object_add(accountObj, "账号", json_object_new_string(account));
        /* 转换成字符串json格式 */
        const char *sendStr = json_object_to_json_string(accountObj);

        /* 发送缓存区 */
        char sendBuf[COMMUNICATION_SIZE];
        memset(sendBuf, 0, sizeof(sendBuf));

        strncpy(sendBuf, sendStr, sizeof(sendBuf) - 1);
        /* 发送信息 */
        write(sockfd, sendBuf, sizeof(sendBuf));

        /* 接收缓存区 */
        char recvBuf[COMMUNICATION_SIZE];
        memset(recvBuf, 0, sizeof(sendBuf));

        /* 等待服务端回应 */
        read(sockfd, recvBuf, sizeof(recvBuf));
        /* 服务器发送available表示该账号可用 */
        if (strncmp(recvBuf, "available", strlen("available")) == 0)
        {
            break;
        }
        else
        {
            printf("该用户名已存在\n");
        }
    }

    system("clear");
    printf("昵称：");
    scanf("%s", name);

    int sexchoice = 0;
    while (1)
    {
        system("clear");
        printf("选择性别(1.男 2.女):");
        scanf("%d", &sexchoice);
        /* 清空缓存区 */
        while (getchar() != '\n');

        if (sexchoice == 1)
        {
            strncpy(sex, "男", sizeof(sex) - 1);
            break;
        }

        if (sexchoice == 2)
        {
            strncpy(sex, "女", sizeof(sex) - 1);
            break;
        }

        printf("小只因：没长眼睛？\n");
        sleep(1);
    }

    char tmp[BUFFER_SIZE];
    /* age为int型，防止输入字符型出错，保证用户输入为int型 */
    while (1)
    {
        memset(tmp, 0, sizeof(char) * BUFFER_SIZE);
        system("clear");
        printf("年龄：");
        scanf("%s", tmp);
        if (sscanf(tmp, "%d", &age) == 1 && age > 0 && age <= 100)
        {
            if (age < 23)
            {
                printf("小只因：你要叫我哥哥\n");
                sleep(1);
            }
            break;
        }
        else
        {
            printf("小只因：电脑前坐的是个人?\n");
            sleep(1);
        }
    }

/* 用户注册密码 */
    int flag = 0;
    int hasCapitalEnglish = 0;
    int hasLowercaseEnglish = 0;
    int hasNumbers = 0;
    int hasSpecialCharacters = 0;
    while (flag == 0)
    {
        while (getchar() != '\n');
        system("clear");
        printf("密码（必须包含大小写英文、数字和特殊字符（！、#、$、&、@）：");
        scanf("%s", password);
        for (int idx = 0; idx < strlen(password); idx++)
        {
            if ((password[idx] > 63) && (password[idx] < 91))
            {
                hasCapitalEnglish = 1;
            }
            else if ((password[idx] > 96) && (password[idx] < 123))
            {
                hasLowercaseEnglish = 1;
            }
            else if ((password[idx] > 32) && (password[idx] < 39))
            {
                hasSpecialCharacters = 1;
            }
            else if ((password[idx] >= '0') && (password[idx] <= '9'))
            {
                hasNumbers = 1;
            }
        }
        if (hasCapitalEnglish && hasLowercaseEnglish && hasSpecialCharacters && hasNumbers)
        {
            printf("密码注册成功！\n");
            flag = 1;
        }
        else
        {
            printf("密码不满足注册条件！\n");
            sleep(1);
        }
    }
    sleep(1);

    system("clear");
    printf("地址：");
    scanf("%s", address);

    system("clear");
    printf("个性签名：");
    scanf("%s", personalSign);

    struct json_object *userData = json_object_new_object();
    const char *str = NULL;
    json_object_object_add(userData, "昵称", json_object_new_string(name));
    json_object_object_add(userData, "性别", json_object_new_string(sex));
    json_object_object_add(userData, "年龄", json_object_new_int(age));
    json_object_object_add(userData, "账号", json_object_new_string(account));
    json_object_object_add(userData, "密码", json_object_new_string(password));
    json_object_object_add(userData, "地址", json_object_new_string(address));
    json_object_object_add(userData, "个性签名", json_object_new_string(personalSign));

    /* 用户信息json字符串 */
    str = json_object_to_json_string(userData);

    struct json_object *userDataSend = json_object_new_object();
    const char *sendstr = NULL;

    /* 定义register注册行动 */
    json_object_object_add(userDataSend, "action", json_object_new_string("register"));

    /* 将用户信息包装好放入，并标明键为userData */
    json_object_object_add(userDataSend, "userData", userData);
    sendstr = json_object_to_json_string(userDataSend);

    char sendBuf[COMMUNICATION_SIZE];
    memset(sendBuf, 0, sizeof(sendBuf));

    char recvBuf[COMMUNICATION_SIZE];
    memset(recvBuf, 0, sizeof(recvBuf));

    strncpy(sendBuf, sendstr, sizeof(sendBuf) - 1);

    /* 将包装好的行动信息和数据发送 */
    write(sockfd, sendBuf, sizeof(sendBuf));

    read(sockfd, recvBuf, sizeof(recvBuf));
    if (strncmp(recvBuf, "registerSuccessful", strlen("registerSuccessful")) == 0)
    {
        system("clear");
        printf("注册成功！您的用户信息：%s\n", str);
    }

    /* 将字符串复制给传进的字符串数组 */
    strncpy(buf, str, (sizeof(char) * COMMUNICATION_SIZE) - 1);

    /* 释放json对象 */
    json_object_put(accountObj);
    json_object_put(userDataSend);

    return ret;
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(-1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    /* 端口 */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    /* IP地址 */
    int ret = inet_pton(AF_INET, SERVER_IP, (void *)&(serverAddress.sin_addr.s_addr));
    if (ret != 1)
    {
        perror("inet_pton error");
        exit(-1);
    }

    /* ip地址 */
    ret = connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret == -1)
    {
        perror("connect error");
        exit(-1);
    }
    char buf[COMMUNICATION_SIZE];
    memset(buf, 0, sizeof(buf));
    Register(sockfd, buf);

    close(sockfd);
    return 0;
}
