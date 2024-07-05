#include "wifi_sta.h"

int netId = -1;
static struct netif* iface;

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
    if(!ssid || !key)
    {
        return ;
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

    osDelay(TEN);
    // 注册事件监听
    errCode = RegisterWifiEvent(&eventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    // setup your AP params

    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    errCode = EnableWifi();
    osDelay(TEN);
    // 添加热点配置，成功会通过result传出netld
    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("AddDeviceConfig: %d\r\n", errCode);

    g_connected = 0;
    errCode = ConnectTo(netId);
    int i = 0;
    while (!g_connected) {
        osDelay(TEN);
        if( ++i >= 250) return -1; 
    }

    printf("g_connected: %d\r\n", g_connected);
    osDelay(FIFTY);

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
    // 联网业务结束
    err_t ret = netifapi_dhcp_stop(iface);
    printf("netifapi_dhcp_stop: %d\r\n", ret);

    Disconnect(); // disconnect with your AP

    RemoveDevice(netId); // remove AP config

    int errCode = DisableWifi();
    printf("DisableWifi: %d\r\n", errCode);
    osDelay(TWO_HUNDRED);
}



