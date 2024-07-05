#include "uart_to_esp32cam.h"

unsigned char g_uartWriteBuff[] = "hello uart1111!";
unsigned char g_uartReadBuff[1024] = {0};
static int usr_uart_config(void)
{
    int ret;
    // 初始化UART配置，115200，数据bit为8,停止位1，奇偶校验为NONE
    hi_uart_attribute uart_attr = {
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
    };
    ret = hi_uart_init( HI_UART_IDX_2, &uart_attr,NULL);
    if (ret != 0) {
        printf("uart2 init fail\r\n");
    }else 
    {
        printf("uart2 init success \n");
    }
    return ret;
}
 
int GetPhoto(unsigned char buff[],int buff_len)
{
    int buf_index = 0;
    while(1)
    {
        int len = hi_uart_read(HI_UART_IDX_2, buff, buff_len);
                
        if(buff[0] == 0xff && buff[1] == 0xd8 && buff[2] == 0xff)
        {
            buf_index += len;

            break;
        }
    }

    while(1)
    {
        if(buff[buf_index  - 1]  == 0xd9 && buff[buf_index -2] == 0xff)
        {
            break;
        }else if(buf_index >= buff_len)
        {
            return -1;

        }

        int len = hi_uart_read(HI_UART_IDX_2, &buff[buf_index], buff_len - buf_index);


        if (len > 0) {
            buf_index += len;
        } else  {
            //printf("IoTUartRead error\n");
            continue;
        }

    }
    return buf_index;
} 

// 1.任务处理函数
void Esp32UartInit()
{
    unsigned int len = 0;
    int ret;

   
    ret  = hi_io_set_func(HI_IO_NAME_GPIO_11,HI_IO_FUNC_GPIO_11_UART2_TXD);
    if(ret != 0 )
    {
        printf("hi_io_set_func 11 error\n");
    }
    ret = hi_io_set_func(HI_IO_NAME_GPIO_12,HI_IO_FUNC_GPIO_12_UART2_RXD);
    if(ret != 0 )
    {
         printf("hi_io_set_func 12 error\n");
    }

    usr_uart_config();


    return NULL;
}
 