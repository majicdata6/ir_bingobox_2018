/*-----------------------------------------------------------------
Copyright 2017,bingo Inc.All Rights Reserved
License    :内部
Author	   :
Description:
-----------------------------------------------------------------*/
#ifndef BINGOBIN_APP_W5500_H_
#define BINGOBIN_APP_W5500_H_
/*-----------------------------------------------------------------
Global Defines and Macros
-----------------------------------------------------------------*/
#define W5500_RST_PIN                         GPIO_PIN_6
#define W5500_RST_GPIO_PORT                   GPIOB
#define W5500_RST_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define W5500_RST_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

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
brief:初始化W5500功能
param:void
Return:void
-----------------------------------------------------------------*/
void APP_W5500_Init(void);
/*-----------------------------------------------------------------
brief:测试W5500功能
param:void
Return:void
-----------------------------------------------------------------*/
void APP_W5500_NetworkInit(void);
/*-----------------------------------------------------------------
brief:W5500复位
param:void
Return:void
-----------------------------------------------------------------*/
void APP_W5500_Reset(void);
#endif // BINGOBIN_APP_W5500_H_
