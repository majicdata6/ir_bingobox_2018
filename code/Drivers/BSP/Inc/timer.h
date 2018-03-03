/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BINGOBIN_DRIVER_TIMER_H_
#define BINGOBIN_DRIVER_TIMER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define TIM15x                           TIM15
#define TIM15x_CLK_ENABLE()              __HAL_RCC_TIM15_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIM15x_IRQn                      TIM15_IRQn
#define TIM15x_IRQHandler                TIM15_IRQHandler

/* Exported functions ------------------------------------------------------- */
void Timer_Init(void);
#endif /* BINGOBIN_DRIVER_TIMER_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
