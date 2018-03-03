/* Includes ------------------------------------------------------------------*/
#include "led.h"
/**
  * @brief  Configures LED GPIO.
  * @param  None
  * @retval None
  */
void BSP_LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED clock */
  LED1_GPIO_CLK_ENABLE();  

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED On.
  * @param  None
  * @retval None
  */
void BSP_LED_On(void)
{
  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  Turns selected LED Off.
  * @param  None
  * @retval None
  */
void BSP_LED_Off(void)
{
  HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
}

/**
  * @brief  Toggles the selected LED.
  * @param  None
  * @retval None
  */
void BSP_LED_Toggle(void)
{
  HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
}

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
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
