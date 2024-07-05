#ifndef SERVER_COMMUINCATION
#define SERVER_COMMUINCATION

#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "hi_timer.h"

#include "lwip/netifapi.h"
#include "lwip/api_shell.h"
#include "lwip/sockets.h"

int TcpClientConnect(unsigned short port, const char* host);

int TcpSend(unsigned char* buff,int buff_len);
int TcpRecv(unsigned char* buff,int buff_len);
#endif 