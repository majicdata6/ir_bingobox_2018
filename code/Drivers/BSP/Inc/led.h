/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BINGOBIN_DRIVER_LED_H_
#define BINGOBIN_DRIVER_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/** @defgroup   LED
  * @{
  */
#define LED1_PIN                         GPIO_PIN_13
#define LED1_GPIO_PORT                   GPIOC
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()
/**
  * @}
  */ 
																				 
/**
  * @}
  */ 
  
/** @defgroup STM32F0308_DISCOVERY_Exported_Functions Exported Functions
  * @{
  */
void     BSP_LED_Init(void);
void     BSP_LED_On(void);
void     BSP_LED_Off(void);
void     BSP_LED_Toggle(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* BINGOBIN_DRIVER_LED_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
