/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_ComDMA/Src/stm32f0xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"


extern UART_HandleTypeDef Uart1Handle;
extern UART_HandleTypeDef Uart2Handle;
extern TIM_HandleTypeDef Tim15Handle;
extern SPI_HandleTypeDef SpiHandle;
extern ADC_HandleTypeDef AdcHandle;
extern GPIO_InitTypeDef   GPIO_InitStruct;
extern TIM_HandleTypeDef Tim14Handle;


/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup SPI_FullDuplex_ComDMA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USART1x_IRQHandler(void)
{
        HAL_UART_IRQHandler(&Uart1Handle);
}

/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USART2x_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart2Handle);
}
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM15x_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Tim15Handle);
}

/**
  * @brief  This function handles IWDG interrupt request.
  * @param  None
  * @retval None
  */
void TIM14x_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Tim14Handle);
}

/**
  * @brief  This function handles EXTI interrupt request.
  * @param  None
  * @retval None
  */
#ifdef BG_SMOKE_ALARM
void EXTI6x_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(SMOKE_GPIO_PIN);
}
#endif

#ifdef  BG_INFRARED
void EXTI4_15_IRQHandler(void)
{
  if(__HAL_GPIO_EXTI_GET_IT(RECEIVE_GPIO_PIN) != RESET) 
  {
    __HAL_GPIO_EXTI_CLEAR_IT(RECEIVE_GPIO_PIN);
	IR_Receiver_GPIO_EXTI_Back(RECEIVE_GPIO_PIN);
  }
}

/**
  * @brief  Infrared transceiver timer..
  * @param  None
  * @retval None
  */
//void TIM16_IRQHandler(void)
void RECEIVE_TIMx_IRQHandler(void)
{
  Infrared_TIM_IRQHandler(ir_time_compare);
	
}

/**
  * @brief  Infrared sending PWM..
  * @param  None
  * @retval None
  */
void NEC_SEND_PWM_TIMx_IRQnHandler(void)
{
  HAL_TIM_IRQHandler(ir_send_time_pwm);
}
#endif

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SpiHandle);
}
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
