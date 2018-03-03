/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧ 
Description : 485�ӿ�
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
Function   : ��ʼ��485�ķ���������ţ�Ĭ��Ϊ����״̬
-----------------------------------------------------------------------------*/
void Meter485_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Meter485_Send(U8 *str,U16 len)
Parameters : (Nothing)
Return     : (Nothing)
Function   : ͨ��485 �������ݣ����֮���л��ɽ���ģʽ
-----------------------------------------------------------------------------*/
void Meter485_Send(uint8_t *str, uint16_t len);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Meter485_ReceiveData(void *rec_buf,uint32_t msec)
Parameters     : rec_buf - ��Ž������ݵĻ�����
		 msec - �����ȴ���ʱ��(ms)���������0
Return         : ���ض������ݵĳ��ȣ�����0���ʾʧ�ܻ�������û��׼����
Function   : ������׼����ϣ�ֱ�Ӷ�ȡ�����������ȴ����ݾ��������߳�ʱ(msecʱ��)
-----------------------------------------------------------------------------*/
uint8_t Meter485_ReceiveData(void *rec_buf, uint32_t msec);

#endif /* End of BINGOBIN_DRIVER_M485_H_ */

