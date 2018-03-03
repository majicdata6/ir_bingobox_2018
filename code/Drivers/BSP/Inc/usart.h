#ifndef BINGOBIN_DRIVER_USART_H_
#define BINGOBIN_DRIVER_USART_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stdint.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
#define USART2x                           USART2
#define USART2x_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USART2x_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2x_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2x_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USART2x_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USART2x_TX_PIN                    GPIO_PIN_2
#define USART2x_TX_GPIO_PORT              GPIOA
#define USART2x_TX_AF                     GPIO_AF1_USART2
#define USART2x_RX_PIN                    GPIO_PIN_3
#define USART2x_RX_GPIO_PORT              GPIOA
#define USART2x_RX_AF                     GPIO_AF1_USART2

/* Definition for USARTx's NVIC */
#define USART2x_IRQn                      USART2_IRQn
#define USART2x_IRQHandler                USART2_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

void USART2_Init(void);
int fputc(int ch, FILE *f);
#endif  /* BINGOBIN_DRIVER_USART_H_ */

