/*-----------------------------------------------------------------
Copyright 2017,bingo Inc.All Rights Reserved
License    :内部
Author	   :
Description:
-----------------------------------------------------------------*/
#ifndef BINGOBIN_DRIVER_SMOKE_H_
#define BINGOBIN_DRIVER_SMOKE_H_
#include "stdint.h"
/*-----------------------------------------------------------------
Global Defines and Macros
-----------------------------------------------------------------*/
#define SMOKE_ALARM_UPLOAD_TIME 300000
#define SMOKE_RCC_CLK_ENABLE    __HAL_RCC_GPIOA_CLK_ENABLE
#define SMOKE_GPIO_PIN			GPIO_PIN_6
#define SMOKE_GPIO			    GPIOA
#define EXTI6x_IRQHandler		EXTI4_15_IRQHandler
#define SMOKE_EXTI_IRQn			EXTI4_15_IRQn 
/*-----------------------------------------------------------------
Global Types and Enums
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Variables
-----------------------------------------------------------------*/
extern uint8_t g_smoke_alarm_status;
/*-----------------------------------------------------------------
Global Function prototypes
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
brief: 烟雾报警器初始化
param: void
Return: void
-----------------------------------------------------------------*/
void Smoke_Alarm_Init(void);

/*-----------------------------------------------------------------
brief: 读取烟雾报警器
param: void
Return:1为有烟雾，0为没有烟雾
-----------------------------------------------------------------*/
uint16_t Smoke_Alarm_Read(void);
#endif //BINGOBIN_DRIVER_SMOKE_H_
