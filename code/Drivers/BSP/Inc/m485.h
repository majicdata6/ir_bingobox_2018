/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅 
Description : 485接口
-----------------------------------------------------------------------------*/

#ifndef BINGOBIN_DRIVER_M485_H_
#define BINGOBIN_DRIVER_M485_H_

#include "stm32f0xx_hal.h"
/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/
/** @defgroup   485RE
  * @{
  */
#define M485REDE_PIN                         GPIO_PIN_11
#define M485REDE_GPIO_PORT                   GPIOA
#define M485REDE_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define M485REDE_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

/* Definition for USARTx clock resources */
#define USART1x                           USART1
#define USART1x_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USART1x_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1x_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1x_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1x_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USART1x_TX_PIN                    GPIO_PIN_9
#define USART1x_TX_GPIO_PORT              GPIOA
#define USART1x_TX_AF                     GPIO_AF1_USART1
#define USART1x_RX_PIN                    GPIO_PIN_10
#define USART1x_RX_GPIO_PORT              GPIOA
#define USART1x_RX_AF                     GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define USART1x_IRQn                      USART1_IRQn
#define USART1x_IRQHandler                USART1_IRQHandler
 
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Constants
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Variables
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Function prototypes
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype  : void Meter485_Init(void)
Parameters : (Nothing)
Return     : (Nothing)
Function   : 初始化485的方向控制引脚，默认为输入状态
-----------------------------------------------------------------------------*/
void Meter485_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Meter485_Send(U8 *str,U16 len)
Parameters : (Nothing)
Return     : (Nothing)
Function   : 通过485 发送数据，完毕之后，切换成接收模式
-----------------------------------------------------------------------------*/
void Meter485_Send(uint8_t *str, uint16_t len);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Meter485_ReceiveData(void *rec_buf,uint32_t msec)
Parameters     : rec_buf - 存放接收数据的缓冲区
		 msec - 阻塞等待的时间(ms)，必须大于0
Return         : 返回读回数据的长度，返回0则表示失败或者数据没有准备好
Function   : 若数据准备完毕，直接读取，否则阻塞等待数据就绪，或者超时(msec时间)
-----------------------------------------------------------------------------*/
uint8_t Meter485_ReceiveData(void *rec_buf, uint32_t msec);

#endif /* End of BINGOBIN_DRIVER_M485_H_ */

