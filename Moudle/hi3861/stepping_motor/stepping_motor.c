#include "stepping_motor.h"

static hi_u32 stepping_mtor42_timer_handle;
static hi_u32 stepping_mtor57_timer_handle;
static float g_42rotationangle;
static float g_57rotationangle;
static int g_42dir;
static int g_57dir;
static int g_57endflag;

static float g_57target_angle;

void Stepping_Mtor42_Init()
{
     int ret;   
     ret  = hi_io_set_func(STEPPING_MOTOR42_PUL_IO_NAME_GPIO,STEPPING_MOTOR42_PUL_GPIO_IO_FUNC);
     if(ret != 0 )
     {
         printf("hi_io_set_func 0 error\n");
     }

     ret  = hi_io_set_func(STEPPING_MOTOR42_DIR_IO_NAME_GPIO,STEPPING_MOTOR42_DIR_GPIO_IO_FUNC);
     if(ret != 0 )
     {
         printf("hi_io_set_func 0 error\n");
     }

     ret  = hi_io_set_func(STEPPING_MOTOR42_ENA_IO_NAME_GPIO,STEPPING_MOTOR42_ENA_GPIO_IO_FUNC);
     if(ret != 0 )
     {
         printf("hi_io_set_func 0 error\n");
     }

     hi_gpio_set_dir(STEPPING_MOTOR42_PUL_GPIO_IDX,HI_GPIO_DIR_OUT);
     hi_gpio_set_dir(STEPPING_MOTOR42_DIR_GPIO_IDX,HI_GPIO_DIR_OUT);
     hi_gpio_set_dir(STEPPING_MOTOR42_ENA_GPIO_IDX,HI_GPIO_DIR_OUT);

     hi_gpio_set_ouput_val(STEPPING_MOTOR42_DIR_GPIO_IDX,CLOCKWISE);

     ret = hi_timer_create(&stepping_mtor42_timer_handle);
     if(ret != 0)
     {
          printf("hi_timer_create 42 error\n");
     }
}

void Stepping_Mtor57_Init()
{  
     //  printf("IN Stepping_Mtor57_Init  success\n");
     int ret  = hi_io_set_func(STEPPING_MOTOR57_PUL_IO_NAME_GPIO,STEPPING_MOTOR57_PUL_GPIO_IO_FUNC); 
     if(ret != 0 )
     {
         printf("hi_io_set_func 7 error\n");
     }
 
     ret  = hi_io_set_func(STEPPING_MOTOR57_DIR_IO_NAME_GPIO,STEPPING_MOTOR57_DIR_GPIO_IO_FUNC);
     if(ret != 0 )
     {
         printf("hi_io_set_func 10 error\n");
     }

     ret  = hi_io_set_func(STEPPING_MOTOR57_ENA_IO_NAME_GPIO,STEPPING_MOTOR57_ENA_GPIO_IO_FUNC);
     if(ret != 0 )
     {
         printf("hi_io_set_func 5 error\n");
     }

     hi_gpio_set_dir(STEPPING_MOTOR57_PUL_GPIO_IDX,HI_GPIO_DIR_OUT);
     hi_gpio_set_dir(STEPPING_MOTOR57_DIR_GPIO_IDX,HI_GPIO_DIR_OUT);
     hi_gpio_set_dir(STEPPING_MOTOR57_ENA_GPIO_IDX,HI_GPIO_DIR_OUT);

     ret = hi_gpio_set_ouput_val(STEPPING_MOTOR57_DIR_GPIO_IDX,UNCLOCKWISE);
     g_57dir = UNCLOCKWISE;
     if(ret != 0)
     {
          printf("hi_gpio_set_ouput_val STEPPING_MOTOR57_DIR_GPIO_IDX  error\n");
     }

     ret = hi_timer_create(&stepping_mtor57_timer_handle);
     if(ret != 0)
     {
          printf("hi_timer_create 57 error\n");
     }

}

static hi_void Stepping_Mtor42Simulation(hi_u32 data)
{
     static int gpio_static = HI_GPIO_VALUE1;
     if(HI_GPIO_VALUE0 == gpio_static) 
     {
          hi_gpio_set_ouput_val(STEPPING_MOTOR42_PUL_GPIO_IDX,HI_GPIO_VALUE1);
          gpio_static = HI_GPIO_VALUE1;
     }
     else if(HI_GPIO_VALUE1 == gpio_static) 
     {
          hi_gpio_set_ouput_val(STEPPING_MOTOR42_PUL_GPIO_IDX,HI_GPIO_VALUE0);
          gpio_static = HI_GPIO_VALUE0;
     }
     if(g_42dir == CLOCKWISE) g_42rotationangle += 1.8;
     if(g_42dir == UNCLOCKWISE) g_42rotationangle -= 1.8;
}

void Stepping_Mtor42_Start()//
{
     hi_gpio_set_ouput_val(STEPPING_MOTOR42_ENA_GPIO_IDX,STEPPING_MOTOR_ENABLE);
     int ret = hi_timer_start(stepping_mtor42_timer_handle,HI_TIMER_TYPE_PERIOD,STEPPING_MOTOR42_1TURN_MS,Stepping_Mtor42Simulation,0);
     if(ret != 0)
     {
          printf("hi_timer_start error\n");
     }
}

static hi_void Stepping_Mtor57Simulation(hi_u32 data)
{
     static int frequency = 0;

     static int gpio_static = HI_GPIO_VALUE1;
     if(HI_GPIO_VALUE0 == gpio_static) 
     {
          hi_gpio_set_ouput_val(STEPPING_MOTOR57_PUL_GPIO_IDX,HI_GPIO_VALUE1);
          gpio_static = HI_GPIO_VALUE1;
     }
     else if(HI_GPIO_VALUE1 == gpio_static) 
     {
          hi_gpio_set_ouput_val(STEPPING_MOTOR57_PUL_GPIO_IDX,HI_GPIO_VALUE0);
          gpio_static = HI_GPIO_VALUE0;
     }
     if(g_57dir == CLOCKWISE) g_57rotationangle -= TURN1_57ANGLE;
     if(g_57dir == UNCLOCKWISE) g_57rotationangle += TURN1_57ANGLE;


     if(g_57dir == CLOCKWISE && g_57rotationangle <= g_57target_angle) {
          g_57endflag = 1;
          Stepping_Mtor57_Stop();
          // Stepping_Mtor57_SetDir(UNCLOCKWISE);
          // g_57dir = UNCLOCKWISE;
          printf("g_57rotationangle:%f\n",g_57rotationangle);
     } else if(g_57dir == UNCLOCKWISE && g_57rotationangle >= g_57target_angle){
          g_57endflag = 1;
          Stepping_Mtor57_Stop();
          //Stepping_Mtor57_SetDir(CLOCKWISE);
          //g_57dir = CLOCKWISE;
          printf("g_57rotationangle:%f\n",g_57rotationangle);
     }
     
}

void Stepping_Mtor57_Start(float angle)//旋转电机到一定角度
{
     g_57endflag = 0;
     g_57target_angle = angle;
     printf("angle57%f,angle%f\n",g_57rotationangle,g_57target_angle);
     if(g_57rotationangle == angle) return ;
     if(g_57rotationangle > g_57target_angle)
     {
          Stepping_Mtor57_SetDir(CLOCKWISE);
     } else if(g_57rotationangle < g_57target_angle)
     {
          Stepping_Mtor57_SetDir(UNCLOCKWISE);
     }

     hi_gpio_set_ouput_val(STEPPING_MOTOR57_ENA_GPIO_IDX,STEPPING_MOTOR_ENABLE);
     int ret = hi_timer_start(stepping_mtor57_timer_handle,HI_TIMER_TYPE_PERIOD,STEPPING_MOTOR57_1TURN_MS,Stepping_Mtor57Simulation,0);
     if(ret != 0)
     {
          printf("hi_timer_start error\n");
     }
}


void Stepping_Mtor42_SetDir(electrical_machinery_dir dir)
{
     hi_gpio_set_ouput_val(STEPPING_MOTOR42_DIR_GPIO_IDX,dir);
     g_42dir = dir;
}

void Stepping_Mtor57_SetDir(electrical_machinery_dir dir)
{
     hi_gpio_set_ouput_val(STEPPING_MOTOR57_DIR_GPIO_IDX,dir);
     g_57dir = dir;
}

void Stepping_Mtor42_Stop()
{
     //hi_gpio_set_ouput_val(STEPPING_MOTOR42_ENA_GPIO_IDX,STEPPING_MOTOR_UNENABLE);
     hi_timer_stop(stepping_mtor42_timer_handle);
}

void Stepping_Mtor57_Stop()
{
     //hi_gpio_set_ouput_val(STEPPING_MOTOR57_ENA_GPIO_IDX,STEPPING_MOTOR_UNENABLE);
     hi_timer_stop(stepping_mtor57_timer_handle);
}

void Calibration57_RotationAngle()
{
     g_57rotationangle = 0;
}

float Get57_RotationAngle()
{
     return g_57rotationangle;
}

int Get57_RotationENDFlag()
{
     return g_57endflag;
}