/*
 * Copyright (C) 2022 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hi_nv.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_hotspot.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "iot_gpio.h"

#include "network_manage.h"
#include "uart_to_esp32cam.h"
#include "stepping_motor.h"
#include "3861_button.h"

#include "appEntrance.h"

#define MAX_XANGLE (-150)
#define MIN_XANGLE (0)

static unsigned char g_uartReadBuff[1024*30];
static unsigned char g_tcpReadBuff[256];
bool g_shit_clearing = 0;


static void UartTask()
{
    while(1)
    {
        int len = GetPhoto(g_uartReadBuff,sizeof(g_uartReadBuff));
        
        if (len > 0) {

            int ret = TcpSend(g_uartReadBuff,len);
            if(ret == -1)
            {
                printf("tcpsend g_uartReadBuff failed!\r\n");
                //TcpClientConnect(TCP_SERVER_PORT,TCP_SERVER_ADDR);//检查网络 重新连接
                if(!IsConnect()) 
                {
                    Inspect_Set_NetWork();
                } else {//有wlan连接
                    if(TcpClientConnect(TCP_SERVER_PORT,TCP_SERVER_ADDR) == -1)
                    {
                        sleep(1);
                    }
                }
            }
        } else  {
            printf("GetPhoto error\n");
        }
    }
}


static void ControlCatLitterBox(ControlCatLitterBoxEvent event)
{
    if(Back_To_Middle == event) 
    {
        Stepping_Mtor57_Start(225);
    }
    else if (Tub_Back_To_Base == event)
    {
        Stepping_Mtor57_Start(0);
    }
    else if (Tub_Back_To_Top == event)
    {
        Stepping_Mtor57_Start(300);
    }
}

static void ListenUserTask()
{
    int cnt = 0;
    while(1)
    {
        if(get_key_state())//没有按下
        {
            if(cnt < 5){
                sleep(1);
                cnt = 0;
            }else if (cnt >= 10){//>10s 手动校准
                Calibration57_RotationAngle();
                ControlCatLitterBox(Back_To_Middle);
                cnt = 0;
            }else if(cnt >=5 && cnt <= 10){//5s ~ 10s重设wifi
                Set_NetWork();
                cnt = 0;
            }
        }else {//按下了
            cnt++;
            sleep(1);
        }

    }
}


static void ClearMovementTask()
{
    //铲屎系列动作
    ControlCatLitterBox(Back_To_Middle);
    while(!Get57_RotationENDFlag()) sleep(1);
    //盆回中心
    ControlCatLitterBox(Tub_Back_To_Base);
    while(!Get57_RotationENDFlag()) sleep(1);
    //盆向后
    ControlCatLitterBox(Tub_Back_To_Top);
    while(!Get57_RotationENDFlag()) sleep(1);
    //盆向前
    ControlCatLitterBox(Back_To_Middle);
    while(!Get57_RotationENDFlag()) sleep(1);
    //盆回中心
    g_shit_clearing = 0;
}

static void ControlCatLitterBoxTask()
{
    int ret = 0;
    while(1)
    {
        ret = TcpRecv(g_tcpReadBuff,sizeof(g_tcpReadBuff));
        if(ret == -1)
        {
            if(!IsConnect()) 
            {
                Inspect_Set_NetWork();
            } else {//有wlan连接
                if(TcpClientConnect(TCP_SERVER_PORT,TCP_SERVER_ADDR) == -1)
                {
                    sleep(1);
                }
            }
            continue;
        }
        else if(strcmp("have shit",g_tcpReadBuff) == 0)
        {//有屎 开启电机并设置电机停止角度（启动电机向后翻转，并且在顺时针近最低角度后处停止）
            printf("have shit\n");
            if(g_shit_clearing == 1) {
                memset(g_tcpReadBuff,0,sizeof(g_tcpReadBuff));
                continue;
            }
            memset(g_tcpReadBuff,0,sizeof(g_tcpReadBuff));
            g_shit_clearing = 1;

            osThreadAttr_t attr_contro;
            // 初始化相关配置
            attr_contro.name = "ClearMovementTask";
            attr_contro.attr_bits = 0U;
            attr_contro.cb_mem = NULL;
            attr_contro.cb_size = 0U;
            attr_contro.stack_mem = NULL;
            attr_contro.stack_size = ATTR_STACK_SIZE;
            attr_contro.priority = osPriorityNormal;

            osThreadId_t clear_movement_thread_id;
            clear_movement_thread_id = osThreadNew(ClearMovementTask, NULL, &attr_contro);
            if (clear_movement_thread_id == NULL) {
                printf("[ControlCatLitterBoxTask] Failed to create UsartTask!\n");
            }
            //osThreadDetach(clear_movement_thread_id);

        }
        else {
            printf("g_tcpReadBuff:%s\n",g_tcpReadBuff);
        }
    }
}

void WifiHotspotDemo(void)
{ 
    //LOS_CppSystemInit((UINTPTR)&Wti1ImuT, (UINTPTR)&Wti1ImuT + sizeof(Wti1ImuT));
        unsigned short i = 0x1234;
    unsigned char c = i;
    if(c== 0x12)
    {
        printf("small\n");
    }else {
        printf("big\n");
    }
    hi_gpio_init();
    
    Esp32UartInit();
    //g_tub_wti1 = WT1_Creat(HI_UART_IDX_1,HI_IO_NAME_GPIO_0,HI_IO_NAME_GPIO_1,9600);//串口2esp32使用  串口0usb线使用
    //g_tub_wti2 = WT1_Creat(HI_UART_IDX_0,HI_IO_NAME_GPIO_13,HI_IO_NAME_GPIO_14,9600);//发生错误  HI_IO_NAME_GPIO_13复用值不能为2
    //    printf("get_key_state ing ---------\n");
    //while(get_key_state()) osDelay(100);
    //    printf("get_key_state success ---------\n");
    //WT1_Calibration(g_tub_wti1);
    //printf("WT1_Calibration ing ---------\n");
    //osDelay(3000);
    // while(1)
    // {
    //     printf("WT1_GetX:%f\n",WT1_GetX(g_tub_wti1));
    //     printf("WT1_GetY:%f\n",WT1_GetY(g_tub_wti1));
    //     osDelay(10);
    // }

    Stepping_Mtor42_Init();
    Stepping_Mtor57_Init();

    Calibration57_RotationAngle();
    //ControlCatLitterBox(Back_To_Middle);

    Inspect_Set_NetWork();

    osThreadAttr_t attr_uart;
    // 初始化相关配置
    attr_uart.name = "UartTask";
    attr_uart.attr_bits = 0U;
    attr_uart.cb_mem = NULL;
    attr_uart.cb_size = 0U;
    attr_uart.stack_mem = NULL;
    attr_uart.stack_size = ATTR_STACK_SIZE;
    attr_uart.priority = osPriorityNormal;

    if (osThreadNew(UartTask, NULL, &attr_uart) == NULL) {
        printf("[UsartTask] Failed to create UsartTask!\n");
    }

    osThreadAttr_t attr_contro;
    // 初始化相关配置
    attr_contro.name = "ControlCatLitterBoxTask";
    attr_contro.attr_bits = 0U;
    attr_contro.cb_mem = NULL;
    attr_contro.cb_size = 0U;
    attr_contro.stack_mem = NULL;
    attr_contro.stack_size = ATTR_STACK_SIZE;
    attr_contro.priority = osPriorityNormal;

    if (osThreadNew(ControlCatLitterBoxTask, NULL, &attr_contro) == NULL) {
        printf("[ControlCatLitterBoxTask] Failed to create UsartTask!\n");
    }


    osThreadAttr_t attr_listen_user;
    // 初始化相关配置
    attr_contro.name = "attr_listen_user";
    attr_contro.attr_bits = 0U;
    attr_contro.cb_mem = NULL;
    attr_contro.cb_size = 0U;
    attr_contro.stack_mem = NULL;
    attr_contro.stack_size = ATTR_STACK_SIZE;
    attr_contro.priority = osPriorityNormal;

    if (osThreadNew(ListenUserTask, NULL, &attr_contro) == NULL) {
        printf("[ControlCatLitterBoxTask] Failed to create UsartTask!\n");
    }

}

SYS_RUN(WifiHotspotDemo);
