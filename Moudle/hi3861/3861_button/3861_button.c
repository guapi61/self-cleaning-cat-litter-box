#include "3861_button.h"

int get_key_state()//摁下为0
{
    printf("-----------------------------------\n");
    osDelay(300);
    int ret = 0;
    static bool flag = false;
    if(!flag)
    {
        ret  = hi_io_set_func(HI_IO_NAME_GPIO_5,HI_IO_FUNC_GPIO_5_GPIO);
        if(ret != 0 )
        {
            printf("hi_io_set_func 5 error\n");
        }

        // ret = hi_io_set_input_enable(HI_IO_NAME_GPIO_5, 1);
        // if(ret != 0 )
        // {
        //     printf("hi_io_set_func 5 error\n");
        // }

        ret = hi_io_set_pull(HI_IO_NAME_GPIO_5,HI_IO_PULL_UP);
        if(ret != 0 )
        {
            printf("hi_io_set_pull 5 error\n");
        }
        
        ret = hi_gpio_set_dir(HI_GPIO_IDX_5,HI_GPIO_DIR_IN);
        if(ret != 0 )
        {
            printf("hi_gpio_set_dir 5 error\n");
        }

        flag = true;
    }
    hi_gpio_value t = 0;
    if( hi_gpio_get_input_val(HI_GPIO_IDX_5,&t) != 0)
    {
        printf("get key value error\n");
    } 

    
    return t;
}