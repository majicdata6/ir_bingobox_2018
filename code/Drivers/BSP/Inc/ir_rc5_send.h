/**
  ******************************************************************************
  * @file    Iinc/IR_RC5_Send.h
  * @author  WXF
  * @version V1.0.0
  * @date    2-2-2018
  * @brief   This file contains the RC5 functions prototypes.
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
#ifndef IR_RC5_SEND_H_
#define IR_RC5_SEND_H_

#ifdef __cplusplus
 extern "C" {
#endif 
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
	 
/* Exported constants --------------------------------------------------------*/

#define RC5_SEND_GPIO_CLK_EN()          __HAL_RCC_GPIOB_CLK_ENABLE()        /* RC5 send the pin port CLOCK to enable. */
#define RC5_SEND_GPIO_PORT              GPIOB                               /* RC5 send the pin PORT */
#define RC5_SEND_GPIO_PIN               GPIO_PIN_9                          /* RC5 send PIN. */
#define RC5_SEND_GPIO_MODE              GPIO_MODE_AF_PP                     /* RC5 send the pin MODE. */
#define RC5_SEND_GPIO_PULL              GPIO_PULLDOWN                       /* RC5 send the pin PULL. */
#define RC5_SEND_GPIO_ALTERANTE         GPIO_AF2_TIM17


#define RC5_SEND_PWM_TIMx               TIM17
#define RC5_SEND_PWM_CHANNEL            TIM_CHANNEL_1
#define RC5_SEND_PWM_PRESCALER          0

//8M
//#define RC5_SEND_PWM_PERIOD             210
//#define RC5_SEND_sConfigOC_PULSE        70
//48M
#define RC5_SEND_PWM_PERIOD             1333
#define RC5_SEND_sConfigOC_PULSE        444

#define RC5_SEND_PWM_TIMx_IRQn          TIM17_IRQn
#define RC5_SEND_PWM_TIMx_IRQnHandler   TIM17_IRQnHandler
#define RC5_SEND_PWM_TIMx_CLK_EN()      __HAL_RCC_TIM17_CLK_ENABLE();


#define RC5_SEND_TIMx                   TIM16
#define RC5_SEND_PRESCALER              (uint32_t)(SystemCoreClock/100000)-1
#define RC5_SEND_PERIOD                 11000
#define RC5_SEND_TIMx_IRQn              TIM16_IRQn
#define RC5_SEND_TIMx_CLK_EN()	        __HAL_RCC_TIM16_CLK_ENABLE();


/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef rc5_compare_tim;
extern TIM_HandleTypeDef rc5_36kPWM_tim;

/* Exported functions ------------------------------------------------------- */
void RC5_Send_Time_Out_Back(TIM_HandleTypeDef *htim);

void RC5_Send_Init(void);
void RC5_Data_Decod(uint32_t data);

#ifdef __cplusplus
}
#endif

#endif /* IR_RC5_SEND_H_ */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
