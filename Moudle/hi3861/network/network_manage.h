#ifndef NETWORK_
#define NETWORK_

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hi_nv.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_hotspot.h"
#include "lwip/sockets.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"

#include "server_commuincation.h"

#define ATTR_STACK_SIZE 10240
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SEVEN 7
#define TEN 10
#define FIFTY 50
#define ONE_HUNDRED 100
#define TWO_HUNDRED 200

#define TCP_SERVER_PORT 8080
#define TCP_SERVER_ADDR "1.92.94.194" //公网ip

typedef struct {
    char ssid[50];
    char passwd[50];
} connect_wifi_message;
//最好只连接一个wifi，切换wifi前disconnect  如果不确定是否有连接 先disconnect

void AgainConnect_Set_NetWork();
void Inspect_Set_NetWork();//使用flash中的信息重新连接wifi，如果没有连接成功转换为ap模式重新配置wifi信息
void Set_NetWork();//直接重新配置连接wifi
//sta
int WifiConnect(char* ssid ,char* key);
void WifiDisconnect(void);

//ap
void WifiAPConfigStart();

int IsConnect();//1 connected 
#endif