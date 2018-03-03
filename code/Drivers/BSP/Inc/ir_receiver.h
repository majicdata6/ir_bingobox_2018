/**
  ******************************************************************************
  * @file    inc/IR_Receiver.h
  * @author  WXF
  * @version V1.0.0
  * @date    24-2-2018
  * @brief   This file contains the IR_Receiver functions prototypes.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IR_RECEIVER_H
#define __IR_RECEIVER_H

#ifdef __cplusplus
 extern "C" {
#endif 
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "app_infrared.h"

/* Exported constants --------------------------------------------------------*/
#define RECEIVE_GPIO_CLK_EN()           __HAL_RCC_GPIOA_CLK_ENABLE()            /* receives the pin port CLOCK to enable. */
#define RECEIVE_GPIO_PORT               GPIOA                                   /* receives the pin PORT */
#define RECEIVE_GPIO_PIN                GPIO_PIN_8                              /* receives PIN. */
#define RECEIVE_GPIO_MODE               GPIO_MODE_IT_RISING_FALLING             /* receives the pin MODE. */
#define RECEIVE_GPIO_PULL               GPIO_NOPULL                             /* receives the pin PULL. */
#define RECEIVE_GPIO_EXTIx_IRQn	        EXTI4_15_IRQn                           /* receives the pin EXTIx_IRQn. */
#define RECEIVE_GPIO_EXTIx_START()      HAL_NVIC_EnableIRQ(EXTI4_15_IRQn)       /* receives the pin EXTI START. */
#define RECEIVE_GPIO_EXTIx_STOP()       HAL_NVIC_DisableIRQ(EXTI4_15_IRQn)      /* receives the pin EXTI START. */

#define RECEIVE_TIMx                    TIM16
#define RECEIVE_PRESCALER               (uint32_t)(SystemCoreClock/100000)-1
#define RECEIVE_PERIOD                  11000
	 
#define RECEIVE_TIMx_IRQn               TIM16_IRQn
#define RECEIVE_TIMx_IRQHandler         TIM16_IRQHandler
#define RECEIVE_TIMx_CLK_EN()           __HAL_RCC_TIM16_CLK_ENABLE();
#define RECEIVE_TIMx_CLK_DISEN()        __HAL_RCC_TIM16_CLK_DISABLE()


#define Receive_Timer_Count_Start() HAL_TIM_Base_Start_IT(&receive_tim)       //HAL_TIM_Base_Start_IT(&receive_tim) __HAL_TIM_ENABLE(&receive_tim) 
#define Receive_Timer_Count_Stop()  HAL_TIM_Base_Stop_IT(&receive_tim)      //HAL_TIM_Base_Stop_IT(&receive_tim) __HAL_TIM_DISABLE(&receive_tim)
#define Receive_Timer_Count_Get()   __HAL_TIM_GET_COUNTER(&receive_tim)
#define Receive_Timer_Count_Reset() __HAL_TIM_SET_COUNTER(&receive_tim, 0)
//extern IRFrameStatus_e rc5_framereceived;     			/* Will be used by the user to check if an RC5 has been received or not */


/* Exported macro ------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/

//typedef enum 
//{ 
//	FAIL = -1,
//	NO   = 0, 
//	YES,
//}IRFrameStatus_e;

extern TIM_HandleTypeDef receive_tim;

extern uint16_t s_receive_data[];
extern uint16_t s_receive_data_len; 

/* Exported functions ------------------------------------------------------- */


void IR_Receiver_GPIO_EXTI_Back(uint16_t GPIO_Pin);
void IR_Receiver_Time_Out_Back(TIM_HandleTypeDef *htim);

void NEC_Timer_Start(void);

void IR_Receiver_EXTI_Start(void);
void IR_Receiver_EXTI_Stop(void);

void IR_Receiver_Init(void);
void IR_Receiver_DEInit(void);
//void IR_Receive_Data(void);
uint32_t RC5_IR_Receiver_Turn(void);
uint32_t NEC_IR_Receiver_Turn(void);
#ifdef __cplusplus
}
#endif

#endif /* IR_RECEIVER_H_ */
/**
  * @}
  */ 

/**
  * @}
  */
  
/**
  * @}
  */
/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
