/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BINGOBIN_DRIVER_FLASH_H_
#define BINGOBIN_DRIVER_FLASH_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
/* Private define ------------------------------------------------------------*/
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0800F800) /* Base @ of Page 62, 1 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0800FC00) /* Base @ of Page 63, 1 Kbytes */

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_63   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE   /* End @ of user Flash area */
#define FLASH_USER_FLAG_ADDR   FLASH_USER_START_ADDR   /* Start @ of user Flash area */
#define FLASH_USER_DATA_ADDR   FLASH_USER_START_ADDR+4   /* Start @ of user Flash area */

/**
  * @brief  FLASH User infomation structure definition
  */
typedef struct
{
  uint8_t flash_firmware_ver[4]; 
  uint8_t flash_hardware_ver[4];  
  uint8_t flash_w5500_ip[4];  
  uint8_t flash_light1_level;  
  uint8_t flash_light2_level;  
  uint16_t flash_audo_lock_time; 
  uint32_t falsh_is_init_flag;
} FLASH_UserInfoTypeDef;
  
/** @defgroup STM32F0308_DISCOVERY_Exported_Functions Exported Functions
  * @{
  */
void     Flash_User_Init(void);
/**
  * @}
  */
void Flash_User_Erase(void);
void Flash_User_Write_One_Word(uint32_t Address,uint32_t data);
uint32_t Flash_User_Read_One_Word(uint32_t Address);
#endif /* BINGOBIN_DRIVER_FLASH_H_ */
