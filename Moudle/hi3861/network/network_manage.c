#include "network_manage.h"

static int netId = -1;
static struct netif* iface;

static volatile int g_hotspotStarted = 0;  
static volatile int g_apdevicenub = 0;         
static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nCache-Control: no-cache\r\n\r\n";

static char g_targetssid[30];
static char g_targetpassword[30]; 

static const char http_index_html[] =  "<!DOCTYPE html>\n"
                                      "<html lang=\"zh-CN\">\n"
                                      "<head>\n"
                                      "<meta charset=\"UTF-8\">\n"
                                      "</head>\n"
                                      "<body>\n"
                                      "<h1>设置WiFi账号和密码</h1>\n"
                                      "<form id=\"wifiForm\" method=\"POST\">\n\
                                        <input type=\"text\" id=\"ssidInput\" name=\"ssid\" placeholder=\"Enter WiFi SSID\">\n\
                                        <input type=\"password\" id=\"passwordInput\" name=\"password\" placeholder=\"Enter WiFi Password\">\n\
                                        <button type=\"button\" onclick=\"submitForm()\">Submit</button>\n\
                                    </form>\n\
                                    \n\
                                    <script>\n\
                                    function submitForm() {\n\
                                        var ssid = document.getElementById(\"ssidInput\").value;\n\
                                        var password = document.getElementById(\"passwordInput\").value;\n\
                                        var action = \"/ssid=\" + ssid + \"&password=\" + password;\n\
                                        document.getElementById(\"wifiForm\").action = action;\n\
                                        document.getElementById(\"wifiForm\").submit();\n\
                                    }\n\
                                    </script>\n"
                                      "</body>\n"
                                      "</html>";



static void OnHotspotStateChanged(int state)
{
    printf("OnHotspotStateChanged: %d.\r\n", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        g_hotspotStarted = 1;
    } else {
        g_hotspotStarted = 0;
    }
}


static void PrintStationInfo(StationInfo* info)
{
    if (!info) return;
    static char macAddress[32] = {0};
    unsigned char* mac = info->macAddress;
    if (sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[ZERO], mac[ONE], mac[TWO], mac[THREE], mac[FOUR], mac[FIVE]) == TRUE) {
    printf("OK");
}
    printf(" PrintStationInfo: mac=%s, reason=%d.\r\n", macAddress, info->disconnectedReason);
}

static void OnHotspotStaJoin(StationInfo* info)
{
    g_apdevicenub ++;
    //PrintStationInfo(info);
    printf("+OnHotspotStaJoin: active stations = %d.\r\n", g_apdevicenub);
}

static void OnHotspotStaLeave(StationInfo* info)
{
    g_apdevicenub --;
    //PrintStationInfo(info);
    printf("-OnHotspotStaLeave: active stations = %d.\r\n", g_apdevicenub);
}

WifiEvent g_defaultWifiEventListener = {
    .OnHotspotStaJoin = OnHotspotStaJoin,
    .OnHotspotStaLeave = OnHotspotStaLeave,
    .OnHotspotStateChanged = OnHotspotStateChanged,
}; 

static struct netif* g_iface = NULL;

static int StartHotspot(const HotspotConfig* config)
{
    WifiErrorCode errCode = WIFI_SUCCESS;

    errCode = RegisterWifiEvent(&g_defaultWifiEventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    errCode = SetHotspotConfig(config);
    printf("SetHotspotConfig: %d\r\n", errCode);

    g_hotspotStarted = 0;
    errCode = EnableHotspot();
    printf("EnableHotspot: %d\r\n", errCode);

    while (!g_hotspotStarted) {
        osDelay(TEN);
    }
    printf("g_hotspotStarted = %d.\r\n", g_hotspotStarted);

    g_iface = netifapi_netif_find("ap0");
    if (g_iface) {
        ip4_addr_t ipaddr;
        ip4_addr_t gateway;
        ip4_addr_t netmask;

        IP4_ADDR(&ipaddr,  192, 168, 1, 5);     /* input your IP for example: 192.168.1.1 */
        IP4_ADDR(&gateway, 192, 168, 1, 1);     /* input your gateway for example: 192.168.1.1 */
        IP4_ADDR(&netmask, 255, 255, 255, 0);   /* input your netmask for example: 255.255.255.0 */
        err_t ret = netifapi_netif_set_addr(g_iface, &ipaddr, &netmask, &gateway);
        printf("netifapi_netif_add: %d\r\n", ret);

        ret = netifapi_dhcps_stop(g_iface); // 海思扩展的HDCP服务接口
        printf("netifapi_dhcps top: %d\r\n", ret);

        ret = netifapi_dhcps_start(g_iface, 0, 0); // 海思扩展的HDCP服务接口
        printf("netifapi_dhcp tart: %d\r\n", ret);
    }
    return errCode;
}

static void StopHotspot(void)
{
    if (g_iface) {
        err_t ret = netifapi_dhcps_stop(g_iface);  // 海思扩展的HDCP服务接口
        printf("netifapi_dhcps top: %d\r\n", ret);
    }

    WifiErrorCode errCode = UnRegisterWifiEvent(&g_defaultWifiEventListener);
    printf("UnRegisterWifiEvent: %d\r\n", errCode);

    errCode = DisableHotspot();
    printf("EnableHotspot: %d\r\n", errCode);
}

static int DisposeNetMessageType(char* buf)
{
    int buflen = strlen(buf);
    /* 判断是不是HTTP的GET命令*/
    if (    buf[0]=='G' &&
            buf[1]=='E' &&
            buf[2]=='T' &&
            buf[3]==' ' &&
            buf[4]=='/' )
    {
        return 0;
    }
    else if (buf[0]=='P'&&buf[1]=='O'
                 &&buf[2]=='S'&&buf[3]=='T')
    {
        return 1;
    }   
    return -1;
}

void WifiAPConfigStart()
{
  
    WifiErrorCode errCode;
    HotspotConfig config = {0};
    // 配置作为AP热点的ssid和key
    if (strcpy(config.ssid,  "HiSpark-AP")) {
}
    if (strcpy(config.preSharedKey, "12345678")) {
}
    config.securityType = WIFI_SEC_TYPE_PSK;
    config.band = HOTSPOT_BAND_TYPE_2G;
    config.channelNum = SEVEN;

    osDelay(TEN);

    printf("starting AP ...\r\n");
    // 开启热点
    errCode = StartHotspot(&config);
    printf("StartHotspot: %d\r\n", errCode);
    // 热点开启时长为60s
    int timeout = 60;

    osDelay(ONE_HUNDRED);

    //建立tcp连接
    ssize_t retval = 0;
    int backlog = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    int connfd = -1;
    

    struct sockaddr_in clientAddr = {0};
    socklen_t clientAddrLen = sizeof(clientAddr);
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80);  // 端口号，从主机字节序转为网络字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 允许任意主机接入， 0.0.0.0

    retval = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // 绑定端口    
    if (retval < 0) {
        printf("bind failed, %ld!\r\n", retval);
        close(sockfd);
    }
    printf("bind to port %u success!\r\n", 80);

    retval = listen(sockfd, backlog); // 开始监听
    if (retval < 0) {
        printf("listen failed!\r\n");
        printf("do_cleanup...\r\n");
        close(sockfd);
    }
    printf("listen with %d backlog success!\r\n", backlog);

    // 接受客户端连接，成功会返回一个表示连接的 socket ， clientAddr 参数将会携带客户端主机和端口信息 ；失败返回 -1
    // 此后的 收、发 都在 表示连接的 socket 上进行；之后 sockfd 依然可以继续接受其他客户端的连接，
    //  UNIX系统上经典的并发模型是“每个连接一个进程”——创建子进程处理连接，父进程继续接受其他客户端的连接
    //  鸿蒙liteos-a内核之上，可以使用UNIX的“每个连接一个进程”的并发模型
    //     liteos-m内核之上，可以使用“每个连接一个线程”的并发模型

    while (1) {
        static int i=1;

        connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (connfd < 0) {
            printf("accept failed, %d, %d\r\n", connfd, errno);
            printf("do_cleanup...\r\n");
            close(sockfd);
        }
       
        // 后续 收、发 都在 表示连接的 socket 上进行；
        char request[1024] = "";
        retval = recv(connfd, request, sizeof(request), 0 );
        if (retval < 0) {
            printf("recv request failed, %ld!\r\n", retval);
  
            close(sockfd);
            return;
        }

        printf("%s", request);
        int messagetype = DisposeNetMessageType(request);
        if(messagetype == 0)//get请求
        {
                retval = send(connfd, http_html_hdr, strlen(http_html_hdr), 0);
                if (retval <= 0) {
                    //printf("send response failed, %ld!\r\n", retval);
          
                    close(sockfd);
                }

                retval = send(connfd, http_index_html, strlen(http_index_html), 0);
                if (retval <= 0) {
                    //printf("222222222send response failed, %ld!\r\n", retval);
               
                    close(sockfd);
                }
        }
        else if (messagetype == 1) //post请求
        {
                //printf("send response failed, %ld!\r\n", retval);
                char *ssid_start = strstr(request, "ssid="); // 找到ssid参数的起始位置
                char *password_start = strstr(request, "password="); // 找到password参数的起始位置
                
                if (ssid_start && password_start) { // 确保找到了ssid和password参数
                    ssid_start += strlen("ssid="); // 跳过参数名
                    password_start += strlen("password="); // 跳过参数名
                    // 查找参数值的结束位置（&符号或字符串结束符）
                    char *ssid_end = strchr(ssid_start, '&');
                    char *password_end = strchr(password_start, ' ');
                    // 如果找不到&符号，表示参数值是字符串的末尾
                    if (!ssid_end) ssid_end = strchr(ssid_start, '\0');
                    if (!password_end) password_end = strchr(password_start, '\0');

                    // 提取参数值并打印                
                    strncpy(g_targetssid,ssid_start,(int)(ssid_end - ssid_start));
                    strncpy(g_targetpassword,password_start,(int)(password_end - password_start));
                    printf("SSID: %s\n",  g_targetssid);
                    printf("Password: %s\n", g_targetpassword);

                    StopHotspot();
                    //WifiConnect(g_targetssid,g_targetpassword);  进行连接wifi 后面改为储存信息，等待连接指令

                    connect_wifi_message wifi_message;
                    sprintf(wifi_message.ssid, g_targetssid);
                    sprintf(wifi_message.passwd, g_targetpassword);

                    hi_u8 id = 0x0B; // 假设要写入的数据的标识符是 0
                    hi_u8 len = sizeof(wifi_message); // 计算要写入的数据的长度
                    hi_u32 flag = 0; // 假设不使用任何标志位
                    // 使用 hi_nv_write 函数写入数据
                    hi_u32 result = hi_factory_nv_write(id, &wifi_message, len, flag);

                    // 检查写入操作的结果
                    if (result == HI_ERR_SUCCESS) {
                        printf("set wifi message hi_factory_nv_write NV success\n");
                        break;
                    } else {
                        printf("set wifi message hi_factory_nv_write NV fiald\n");
                    }

                    while(1)
                    {
                        osDelay(TWO_HUNDRED);
                    }

                } else {
                    printf("SSID and/or Password not found in request.\n");
                }      

                retval = send(connfd, http_html_hdr, strlen(http_html_hdr), 0);
                if (retval <= 0) {
                    close(sockfd);
                }

                retval = send(connfd, http_index_html, strlen(http_index_html), 0);
                if (retval <= 0) {
                    //printf("222222222send response failed, %ld!\r\n", retval);
               
                    close(sockfd);
                }
        } 
        else //未知消息
        {
            //printf("111111:recv requesty%s\n", request);
        }

        /* 关闭 */
        close(connfd);

        printf("\n++++++++++++++++++++i:%d \n",i++);
    }
    // 可以通过串口工具发送：AT+PING=192.168.xxx.xxx(如手机连接到该热点后的IP)去ping连接到该热点的设备的IP地址
    printf("stop AP ...\r\n");
    // 关闭热点
    StopHotspot();
    printf("stop AP ...\r\n");
}

// 注释：MAC
static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    if (!info) return;

    static char macAddress[32] = {0};
    unsigned char* mac = info->bssid;
    if (sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[ZERO], mac[ONE], mac[TWO], mac[THREE], mac[FOUR], mac[FIVE]) == TRUE) {
    printf("OK");
}
    printf("bssid: %s, rssi: %d, connState: %d, reason: %d, ssid: %s\r\n",
        macAddress, info->rssi, info->connState, info->disconnectedReason, info->ssid);
}

static int g_connected = 0;
static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info) return;

    printf("%s %d, state = %d, info = \r\n", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == 1) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

void OnWifiScanStateChanged(int state, int size)
{
    printf("%s %d, state = %X, size = %d\r\n", __FUNCTION__, __LINE__, state, size);
}

int WifiConnect(char* ssid ,char* key)
{
    static int f = 0;
    printf("----------%d------------\n",f++);
    if(!ssid || !key)
    {
        return -1;
    }

    WifiErrorCode errCode;
    WifiEvent eventListener = {
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};

    // setup your AP params
    strncpy(apConfig.ssid, ssid,sizeof(apConfig.ssid));

    strncpy(apConfig.preSharedKey, key,sizeof(apConfig.preSharedKey));
printf("---------RegisterWifiEvent------------\n");
    osDelay(TEN);
    // 注册事件监听
    errCode = RegisterWifiEvent(&eventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    // setup your AP params

    apConfig.securityType = WIFI_SEC_TYPE_PSK;
printf("---------EnableWifi------------\n");
    errCode = EnableWifi();
    osDelay(TEN);
printf("---------AddDeviceConfig------------\n");
    // 添加热点配置，成功会通过result传出netld

    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("AddDeviceConfig: %d\r\n", errCode);
printf("---------ConnectTo------------\n");
    g_connected = 0;
    errCode = ConnectTo(netId);
    int i = 0;
    while (!g_connected) {
        osDelay(TEN);
        if( ++i >= 250) return -1; 
    }

    printf("g_connected: %d\r\n", g_connected);
    osDelay(FIFTY);
printf("---------netifapi_netif_find------------\n");
    // 联网业务开始
    iface = netifapi_netif_find("wlan0");
    if (!iface) {
        return -1;
    }
    err_t ret = netifapi_dhcp_start(iface);
    printf("netifapi_dhcp_start: %d\r\n", ret);

    osDelay(TWO_HUNDRED); // wait DHCP server give me IP
    ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
    printf("netifapi_netif_common: %d\r\n", ret);
    return 0;
}

void WifiDisconnect(void)
{
printf("-----------------------IN WifiDisconnect----------------\r\n");
    // 联网业务结束
    err_t ret = netifapi_dhcp_stop(iface);

printf("----------------------- Disconnect----------------\r\n");
    RemoveDevice(netId); // remove AP config
    netId = -1;
printf("----------------------- RemoveDevice----------------\r\n");
    int errCode = DisableWifi();
    printf("DisableWifi: %d\r\n", errCode);
    osDelay(TWO_HUNDRED);
}

int IsConnect()
{
    return g_connected;
}


void Inspect_Set_NetWork()
{
    connect_wifi_message wifi_message;
    hi_factory_nv_read(0x0B, &wifi_message, sizeof(wifi_message), 0);
    printf(" ssid  :[%s]  psswd [%s]\n", wifi_message.ssid, wifi_message.passwd);
    
    WifiDisconnect();
    int ret = WifiConnect(wifi_message.ssid,wifi_message.passwd);//尝试使用flash中的信息进行连接wifi
    printf("%d---------------------------\n",ret);
    while(ret != 0)//wifi连接失败 转换为wifi配置模式
    {
        WifiAPConfigStart();

        hi_factory_nv_read(0x0B, &wifi_message, sizeof(wifi_message), 0);//重新读取连接
        printf(" ssid  :[%s]  psswd [%s]\n", wifi_message.ssid, wifi_message.passwd);
        ret = WifiConnect(wifi_message.ssid,wifi_message.passwd);//尝试使用flash中的信息进行连接wifi

    }

    TcpClientConnect(TCP_SERVER_PORT,TCP_SERVER_ADDR);
    printf("------------------TcpClientConnec---------------------------\n");
}

void Set_NetWork()
{
    connect_wifi_message wifi_message;
    int ret = 0;
    WifiDisconnect();
    WifiAPConfigStart();
    hi_factory_nv_read(0x0B, &wifi_message, sizeof(wifi_message), 0);//重新读连接信息
    ret = WifiConnect(wifi_message.ssid,wifi_message.passwd);
    if(ret == -1)
    {
        printf("Set_NetWork WifiConnect error \n");
    }
}