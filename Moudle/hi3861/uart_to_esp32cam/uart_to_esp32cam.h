#ifndef UART_TO_ESP32CAM
#define UART_TO_ESP32CAM

#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_uart.h"
#include "hi_io.h"
#include "hi_gpio.h"

#define U_SLEEP 100000

void Esp32UartInit();
int GetPhoto(unsigned char buff[],int buff_len);

#endif