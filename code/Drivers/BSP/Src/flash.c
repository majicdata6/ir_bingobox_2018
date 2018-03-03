/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures Flash_User_Init.
  * @param  None
  * @retval None
  */
void Flash_User_Init(void)
{
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
  EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
}
/**
  * @brief  Configures Flash_User_Erase.
  * @param  None
  * @retval None
  */
void Flash_User_Erase(void)
{
	uint32_t  PageError = 0;
	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
		/*
		Error occurred while page erase.
		User can add here some code to deal with this error.
		PageError will contain the faulty page and then to know the code error on this page,
		user can call function 'HAL_FLASH_GetError()'
		*/
		/* Infinite loop */
		while (1)
		{
			BSP_LED_On();
		}
	}	
	/* Lock the Flash to disable the flash control register access (recommended
	to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
}
/**
  * @brief  Configures Flash_User_Write_One_Word.
  * @param  uint32_t Address,uint32_t data
  * @retval None
  */
void Flash_User_Write_One_Word(uint32_t Address,uint32_t data)
{
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address,data) != HAL_OK)
	{
		/* Error occurred while writing data in Flash memory.
		 User can add here some code to deal with this error */
		while (1)
		{
			BSP_LED_On();
		}
	}	
}
/**
  * @brief  Configures Flash_User_Read_One_Word.
  * @param  uint32_t Address
  * @retval None
  */
uint32_t Flash_User_Read_One_Word(uint32_t Address)
{
	__IO uint32_t data32 = 0 ;
	data32 = *(__IO uint32_t *)Address;

	//printf("[%s] addr:0x%X data32:0x%X\r\n",__FUNCTION__,Address,(uint32_t)data32);	
	return data32;
}
/**
  * @}
  */ 
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
