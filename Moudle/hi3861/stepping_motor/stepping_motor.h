#ifndef CTR_STEPPING_MOTOR
#define CTR_STEPPING_MOTOR

#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "hi_timer.h"

#define __CONCATENATE(x, y) x ## y
#define CONCATENATE(x, y) __CONCATENATE(x, y)

#define STEPPING_MOTOR42_PUL_GPIOX 6
#define STEPPING_MOTOR42_DIR_GPIOX 8
#define STEPPING_MOTOR42_ENA_GPIOX 9

#define STEPPING_MOTOR57_PUL_GPIOX 7
#define STEPPING_MOTOR57_DIR_GPIOX 14
#define STEPPING_MOTOR57_ENA_GPIOX 2

#define STEPPING_MOTOR42_PUL_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR42_PUL_GPIOX)
#define STEPPING_MOTOR42_DIR_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR42_DIR_GPIOX)
#define STEPPING_MOTOR42_ENA_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR42_ENA_GPIOX)

#define STEPPING_MOTOR57_PUL_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR57_PUL_GPIOX)
#define STEPPING_MOTOR57_DIR_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR57_DIR_GPIOX)
#define STEPPING_MOTOR57_ENA_IO_NAME_GPIO   CONCATENATE(HI_IO_NAME_GPIO_,STEPPING_MOTOR57_ENA_GPIOX)

#define STEPPING_MOTOR42_PUL_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR42_PUL_GPIOX)
#define STEPPING_MOTOR42_DIR_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR42_DIR_GPIOX)
#define STEPPING_MOTOR42_ENA_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR42_ENA_GPIOX)

#define STEPPING_MOTOR57_PUL_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR57_PUL_GPIOX)
#define STEPPING_MOTOR57_DIR_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR57_DIR_GPIOX)
#define STEPPING_MOTOR57_ENA_GPIO_IDX   CONCATENATE(HI_GPIO_IDX_,STEPPING_MOTOR57_ENA_GPIOX)

#define STEPPING_MOTOR42_PUL_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR42_PUL_GPIOX),_GPIO)
#define STEPPING_MOTOR42_DIR_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR42_DIR_GPIOX),_GPIO)
#define STEPPING_MOTOR42_ENA_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR42_ENA_GPIOX),_GPIO)

#define STEPPING_MOTOR57_PUL_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR57_PUL_GPIOX),_GPIO)
#define STEPPING_MOTOR57_DIR_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR57_DIR_GPIOX),_GPIO)
#define STEPPING_MOTOR57_ENA_GPIO_IO_FUNC CONCATENATE(CONCATENATE(HI_IO_FUNC_GPIO_,STEPPING_MOTOR57_ENA_GPIOX),_GPIO)

#define STEPPING_MOTOR42_1TURN_MS 1
#define STEPPING_MOTOR57_1TURN_MS 1


#define TURN1_57ANGLE 1.8 / 32
#define TURN1_42ANGLE 1.8

typedef enum {
    CLOCKWISE = HI_GPIO_VALUE1,     /**< Single-period timer.CNcomment:表示单次定时器 CNend */
    UNCLOCKWISE = HI_GPIO_VALUE0
} electrical_machinery_dir;

typedef enum {
    STEPPING_MOTOR_ENABLE = HI_GPIO_VALUE0,     /**< Single-period timer.CNcomment:表示单次定时器 CNend */
    STEPPING_MOTOR_UNENABLE = HI_GPIO_VALUE1
} electrical_machinery_ena;

void Stepping_Mtor42_Init();

void Stepping_Mtor57_Init();

void Stepping_Mtor42_Start();//使电机旋转到一定角度，会自动停止电机
void Stepping_Mtor57_Start(float angle);

void Stepping_Mtor42_SetDir(electrical_machinery_dir dir);
void Stepping_Mtor57_SetDir(electrical_machinery_dir dir);

void Stepping_Mtor42_Stop();
void Stepping_Mtor57_Stop();

void Calibration42_RotationAngle();//校准点应该在最下方，也就是猫砂盆旋转置右侧最下角
void Calibration57_RotationAngle();//校准点应该在最下方，也就是猫砂盆旋转置右侧最下角
float Get42_RotationAngle();
float Get57_RotationAngle();

int Get57_RotationENDFlag();

#endif