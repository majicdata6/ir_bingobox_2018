/*-----------------------------------------------------------------
Copyright 2017,bingo Inc.All Rights Reserved
License    :内部
Author	   :
Description:
-----------------------------------------------------------------*/
#ifndef BINGOBIN_DRIVER_SPI_H_
#define BINGOBIN_DRIVER_SPI_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stdint.h"
/*-----------------------------------------------------------------
Global Defines and Macros
-----------------------------------------------------------------*/
/**
  * @} USER SPI FOR W5500
  */ 
/* User can use this section to tailor SPIx instance used and associated
   resources */
/* Definition for SPIx clock resources */
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()

#define SPIx_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI1
#define SPIx_CS_PIN                      GPIO_PIN_15
#define SPIx_CS_GPIO_PORT                GPIOA

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler

/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/*-----------------------------------------------------------------
Global Types and Enums
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Constants
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Variables
-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
Global Function prototypes
-----------------------------------------------------------------*/


/*-----------------------------------------------------------------
brief:初始化相应的IO口为SPI功能
param:void
Return:void
-----------------------------------------------------------------*/
void SPI_Init(void);
/*-----------------------------------------------------------------
brief:通过SPI发送一个字节
param:发送的数据
Return:void
-----------------------------------------------------------------*/
void SPI_Send(uint8_t transmitData);
/*-----------------------------------------------------------------
brief:通过SPI接收一个字节
param:void
Return:收到的数据
-----------------------------------------------------------------*/
uint8_t SPI_Recv(void);
/*-----------------------------------------------------------------
brief:进入临界区
param:void
Return:void
-----------------------------------------------------------------*/
void SPI_CrisEnter(void);
/*-----------------------------------------------------------------
brief:退出临界区
param:void
Return:void
-----------------------------------------------------------------*/
void SPI_CrisExit(void);
/*-----------------------------------------------------------------
brief:拉低片选信号
param:void
Return:void
-----------------------------------------------------------------*/
void SPI_CsSelect(void);
/*-----------------------------------------------------------------
brief:拉高片选信号
param:void
Return:void
-----------------------------------------------------------------*/
void SPI_CsDeselect(void);
#endif // BINGOBIN_DRIVER_SPI_H_
