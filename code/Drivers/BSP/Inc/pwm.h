/*-----------------------------------------------------------------
Copyright 2017,bingo Inc.All Rights Reserved
License    :内部
Author	   :
Description:
-----------------------------------------------------------------*/
#ifndef BINGOBIN_DRIVER_PWM_H_
#define BINGOBIN_DRIVER_PWM_H_
/*-----------------------------------------------------------------
Global Defines and Macros
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Types and Enums
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Variables
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Function prototypes
-----------------------------------------------------------------*/

#include "stm32f0xx_hal.h"
#define PWM_TIMx                           TIM3
#define PWM_TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for TIMx Channel Pins */

#define TIMx_PWM_CLOCK 			1000	   
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        GPIOA
#define TIMx_GPIO_PORT_CHANNEL2        GPIOA
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_6
#define TIMx_GPIO_PIN_CHANNEL2         GPIO_PIN_7
#define TIMx_GPIO_AF_CHANNEL1          GPIO_AF1_TIM3
#define TIMx_GPIO_AF_CHANNEL2          GPIO_AF1_TIM3
/*
    To get TIM3 output clock at 24 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM3 counter clock / TIM3 output clock) - 1
           = 665
*/
#define  PERIOD_VALUE       		(uint32_t)(16000000/TIMx_PWM_CLOCK - 1)  /* Period Value  */
#define  PULSE1_VALUE       		(uint32_t)(PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       		(uint32_t)(PERIOD_VALUE*37.5/100) /* Capture Compare 2 Value  */
/*改变PWM的超时值*/
#define PWM_CHANGE_TIMEROUT		1
#define PWM_CHANGE_OF_EACH_TIME         5       //(单位：s)每次改变pwm的完成所需的时间，默认3s
//#define TIMx_IRQn                      TIM3_IRQn
//#define TIMx_IRQHandler                TIM3_IRQHandler
typedef enum
{
	PWM_VALUE_UP_DOWN_INIT = 0,
	PWM_VALUE_UP = 1,
	PWM_VALUE_DOWN = 2,
}PWM_Channel_Value_Up_Down;
typedef struct
{
	uint8_t pwm_flag;		//是否需要更改PWM的标
	uint8_t pwm_up_down_flag;	//PWM的值是增加还是较少标志
	uint8_t pwm_channel;		//PWM的通道
	uint32_t current_pwm_value ;	//当前PWM的值
	uint32_t pwm_value ;		//需要变更的PWM的值
	uint32_t pwm_timer_count;	//PWM 1ms定时器计数值
        
        float   pwm_change_rate;        //PWM 1ms改变速率
        float   current_pwm_value_f;    //当前PWM的值float
}PWM_ChannelTypeDef;
/*-----------------------------------------------------------------
brief:初始化PWM
param:void
Return:void
-----------------------------------------------------------------*/
void PWM_Init(void);
void PWM_Start(uint8_t lightbox_type,uint8_t duty_cycle);
void PWM_Change_Use_Timer(void);
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
