#include "server_commuincation.h"
static char g_request[] = "Hello,I am Lwip";
static char g_response[128] = "";
static int  g_sockfd = -1;

int TcpSend(unsigned char* buff,int buff_len)
{
    ssize_t retval = send(g_sockfd, buff, buff_len, 0);

    if (retval <= 0) {
        printf("send g_request failed!\r\n");
        //lwip_close(g_sockfd);
        return -1;
    }
    return retval;
}

int TcpRecv(unsigned char* buff,int buff_len)
{
    ssize_t retval = recv(g_sockfd, buff, buff_len, 0);
    if (retval <= 0) {
        printf("recv buff from server failed or done, %ld!\r\n", retval);
        //lwip_close(g_sockfd);
        return -1;
    }
    if(retval < buff_len) buff[retval] = '\0';
    printf("recv buff{%s} %ld from server done!\r\n", buff, retval);
    return retval;
}

int TcpClientConnect(unsigned short port,const char* host)
{ 
    if(g_sockfd >= 0) lwip_close(g_sockfd);
    g_sockfd = -1;
    g_sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;  // AF_INET表示IPv4协议
    serverAddr.sin_port = htons(port);  // 端口号，从主机字节序转为网络字节序
    if (inet_pton(AF_INET, host, &serverAddr.sin_addr) <= 0) {  // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
        printf("inet_pton failed!\r\n");
        lwip_close(g_sockfd);
        return -1;
    }

    // 尝试和目标主机建立连接，连接成功会返回0 ，失败返回 -1
    if (connect(g_sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("connect failed!\r\n");
        lwip_close(g_sockfd);
        return -1;
    }
    printf("connect to server %s success!\r\n", host);

    // 建立连接成功之后，这个TCP socket描述符 —— g_sockfd 就具有了 “连接状态”，发送、接收 对端都是 connect 参数指定的目标主机和端口
    // ssize_t retval = send(g_sockfd, g_request, sizeof(g_request), 0);
    // if (retval < 0) {
    //     printf("send g_request failed!\r\n");
    //     lwip_close(g_sockfd);
    //     return -1;
    // }
    // printf("send g_request{%s} %ld to server done!\r\n", g_request, retval);

    // retval = recv(g_sockfd, &g_response, sizeof(g_response), 0);
    // if (retval <= 0) {
    //     printf("send g_response from server failed or done, %ld!\r\n", retval);
    //     lwip_close(g_sockfd);
    //     return -1;
    // }
    // g_response[retval] = '\0';
    // printf("recv g_response{%s} %ld from server done!\r\n", g_response, retval);
    //lwip_close(g_sockfd);
    return 0;
}

