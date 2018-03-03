/*-----------------------------------------------------------------
Copyright 2017,bingo Inc.All Rights Reserved
License    :�ڲ�
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
/*�ı�PWM�ĳ�ʱֵ*/
#define PWM_CHANGE_TIMEROUT		1
#define PWM_CHANGE_OF_EACH_TIME         5       //(��λ��s)ÿ�θı�pwm����������ʱ�䣬Ĭ��3s
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
	uint8_t pwm_flag;		//�Ƿ���Ҫ����PWM�ı�
	uint8_t pwm_up_down_flag;	//PWM��ֵ�����ӻ��ǽ��ٱ�־
	uint8_t pwm_channel;		//PWM��ͨ��
	uint32_t current_pwm_value ;	//��ǰPWM��ֵ
	uint32_t pwm_value ;		//��Ҫ�����PWM��ֵ
	uint32_t pwm_timer_count;	//PWM 1ms��ʱ������ֵ
        
        float   pwm_change_rate;        //PWM 1ms�ı�����
        float   current_pwm_value_f;    //��ǰPWM��ֵfloat
}PWM_ChannelTypeDef;
/*-----------------------------------------------------------------
brief:��ʼ��PWM
param:void
Return:void
-----------------------------------------------------------------*/
void PWM_Init(void);
void PWM_Start(uint8_t lightbox_type,uint8_t duty_cycle);
void PWM_Change_Use_Timer(void);
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
