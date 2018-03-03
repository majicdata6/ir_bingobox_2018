/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;
/* Private define ------------------------------------------------------------*/
enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;
/* Private function prototypes -----------------------------------------------*/
/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	if(hspi->Instance == SPIx)
	{     
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		SPIx_CS_GPIO_CLK_ENABLE();
		SPIx_SCK_GPIO_CLK_ENABLE();
		SPIx_MISO_GPIO_CLK_ENABLE();
		SPIx_MOSI_GPIO_CLK_ENABLE();
		/* Enable SPI clock */
		SPIx_CLK_ENABLE(); 

		/*##-2- Configure peripheral GPIO ##########################################*/  
			  /* Enable the GPIO_CS clock */
		//LEDx_GPIO_CLK_ENABLE(Led);

		/* Configure the GPIO_CS pin */
		GPIO_InitStruct.Pin = SPIx_CS_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(SPIx_CS_GPIO_PORT, &GPIO_InitStruct);
			
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = SPIx_SCK_AF;
		HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MISO_PIN;
		GPIO_InitStruct.Alternate = SPIx_MISO_AF;
		HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
		GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
		HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
		    
		/*##-3- Configure the NVIC for SPI #########################################*/
		/* NVIC for SPI */
//		HAL_NVIC_SetPriority(SPIx_IRQn, 1, 0);
//		HAL_NVIC_EnableIRQ(SPIx_IRQn);
	}
}

/**
  * @brief SPI MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to its default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPIx)
	{   
		/*##-1- Reset peripherals ##################################################*/
		SPIx_FORCE_RESET();
		SPIx_RELEASE_RESET();

		/*##-2- Disable peripherals and GPIO Clocks ################################*/
		/* Configure SPI SCK as alternate function  */
		HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
		/* Configure SPI MISO as alternate function  */
		HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
		/* Configure SPI MOSI as alternate function  */
		HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
		
		/*##-3- Disable the NVIC for SPI ###########################################*/
//		HAL_NVIC_DisableIRQ(SPIx_IRQn);
	}
}
void SPI_Init(void)
{
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	SpiHandle.Instance               = SPIx;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	SpiHandle.Init.Mode = SPI_MODE_MASTER;

	if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
}
/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of Interrupt TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	/* Turn LED on: Transfer in transmission/reception process is correct */
	BSP_LED_Toggle();
	wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	wTransferState = TRANSFER_ERROR;
}
void SPI_Send(uint8_t transmitData)
{
	while(__HAL_SPI_GET_FLAG(&SpiHandle,SPI_FLAG_TXE) != SET);
	HAL_SPI_Transmit(&SpiHandle,&transmitData,1,0);
	while(__HAL_SPI_GET_FLAG(&SpiHandle,SPI_FLAG_BSY) == SET);
	/*
	while (wTransferState != TRANSFER_COMPLETE)
	{
	}
	wTransferState = TRANSFER_WAIT;
	*/
}
uint8_t SPI_Recv(void)
{        
	uint8_t rec_data;
	while(__HAL_SPI_GET_FLAG(&SpiHandle,SPI_SR_RXNE) == SET);
	HAL_SPI_Receive(&SpiHandle,&rec_data,1,2000);
	while(__HAL_SPI_GET_FLAG(&SpiHandle,SPI_FLAG_BSY) == SET);
	/*
	while (wTransferState != TRANSFER_COMPLETE)
	{
	}
	wTransferState = TRANSFER_WAIT;
	*/
	return rec_data;
}

void SPI_CrisEnter(void)
{
}
void SPI_CrisExit(void)
{
}

void SPI_CsSelect(void)
{  	
	HAL_GPIO_WritePin(SPIx_CS_GPIO_PORT, SPIx_CS_PIN, GPIO_PIN_RESET);
}
void SPI_CsDeselect(void)
{	
	HAL_GPIO_WritePin(SPIx_CS_GPIO_PORT, SPIx_CS_PIN, GPIO_PIN_SET);
}
