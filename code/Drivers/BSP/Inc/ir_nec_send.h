/**
  ******************************************************************************
  * @file    inc/IR_NEC_Send.h
  * @author  WXF
  * @version V1.0.0
  * @date    2-2-2018
  * @brief   This file contains the NEC functions prototypes.
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
#ifndef IR_NEC_SEND_H_
#define IR_NEC_SEND_H_

#ifdef __cplusplus
 extern "C" {
#endif 
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
	 

/* Exported constants --------------------------------------------------------*/

#define NEC_SEND_GPIO_CLK_EN()        __HAL_RCC_GPIOB_CLK_ENABLE()        /* NEC send the pin port CLOCK to enable. */
#define NEC_SEND_GPIO_PORT            GPIOB                               /* NEC send the pin PORT */
#define NEC_SEND_GPIO_PIN             GPIO_PIN_9                          /* NEC send PIN. */
#define NEC_SEND_GPIO_MODE            GPIO_MODE_AF_PP                     /* NEC send the pin MODE. */
#define NEC_SEND_GPIO_PULL            GPIO_PULLDOWN                       /* NEC send the pin PULL. */
#define NEC_SEND_GPIO_ALTERANTE	      GPIO_AF2_TIM17

#define NEC_SEND_PWM_TIMx             TIM17
#define NEC_SEND_PWM_CHANNEL          TIM_CHANNEL_1
#define NEC_SEND_PWM_PRESCALER        0
	 
//TIM_CLK=8M 38k分频系数
//#define NEC_SEND_PWM_PERIOD           222
//#define NEC_SEND_sConfigOC_PULSE      74

//TIM_CLK=48M 38k分频系数
#define NEC_SEND_PWM_PERIOD           1263
#define NEC_SEND_sConfigOC_PULSE      421

#define NEC_SEND_PWM_TIMx_IRQn        TIM17_IRQn
#define NEC_SEND_PWM_TIMx_IRQnHandler TIM17_IRQHandler
#define NEC_SEND_PWM_TIMx_CLK_EN()    __HAL_RCC_TIM17_CLK_ENABLE();


#define NEC_SEND_TIMx                 TIM16
#define NEC_SEND_PRESCALER            (uint32_t)(SystemCoreClock/100000)-1
#define NEC_SEND_PERIOD               11000
#define NEC_SEND_TIMx_IRQn            TIM16_IRQn
#define NEC_SEND_TIMx_CLK_EN()        __HAL_RCC_TIM16_CLK_ENABLE();

/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef nec_compare_tim;
extern TIM_HandleTypeDef nec_38kPWM_tim;

/* Exported functions ------------------------------------------------------- */

void NEC_Send_Time_Out_Back(TIM_HandleTypeDef *htim);

void NEC_Send_Init(void);
void NEC_Data_Decod(uint32_t data);


#ifdef __cplusplus
}
#endif

#endif /* IR_NEC_SEND_H_ */
/**
  * @}
  */ 

/**
  * @}
  */
  
/**
  * @}
  */
/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
