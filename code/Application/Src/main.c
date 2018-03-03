/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_ComDMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32F0xx SPI HAL API to transmit
  *          and receive a data buffer with a communication process based on
  *          DMA transfer.
  *          The communication is done using 2 Boards.
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define W5500_TEST_DATA_BUF_SIZE  512    // It is depend on Target System Memory
#define SOCK_TCPS        0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef BG_485_METER
	uint32_t energy;
#endif
uint8_t gDATABUF[W5500_TEST_DATA_BUF_SIZE];
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Prototype      : void Platform_Init(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : init module
-----------------------------------------------------------------------------*/
void Platform_Init(void)
{
	HAL_Init();
	/* Configure the system clock to 48 MHz */
	SystemClock_Config();
	/* Configure LED*/
	BSP_LED_Init();

	Timer_Init();
#ifdef BG_UART_DEBUG
	USART2_Init();
#endif	
	
#ifdef BG_INFO
       Info_MemoryInit();
#endif
	
#ifdef BG_LED
	Ligthbox_Init();
#endif
	
#ifdef BG_DOOR_LOCK
	Lock_Init();
#endif
	
#ifdef BG_485_METER
	Meter485_Init();
#endif

#ifdef BG_SMOKE_ALARM
        Smoke_Alarm_Init();
#endif

#ifdef BG_W5500
        APP_W5500_Init();
#endif

#ifdef BG_WDT
        WDT_init();
#endif

#ifdef BG_INFRARED
        APP_Infrared_Init();
#endif

#ifdef BG_UART_DEBUG
	printf("\n");
	printf("***************************************\n");
	#ifdef BG_485_METER
		printf("* [Main]: BingoBin BG_485_METER\n");
	#endif
	#ifdef BG_LED 
		printf("* [Main]: BingoBin BG_LED\n");
	#endif
	#ifdef BG_DOOR_LOCK
		printf("* [Main]: BingoBin BG_DOOR_LOCK\n");
	#endif
	#ifdef BG_SMOKE_ALARM
		printf("* [Main]: BingoBin BG_SMOKE_ALARM\n");
	#endif
	#ifdef BG_INFRARED
        printf("* [Main]: BingoBin BG_INFRARED\n");
	#endif
	printf("***************************************\n");
#endif
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
	Platform_Init();
	
#ifdef BG_W5500
        uint8_t tmp = 0;
	
	uint8_t tmpstr[6];
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	printf("\r\n=== %s ===\r\n", (char*)tmpstr);
	/* PHY link status check */
	do
	{
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
		{
		}
	}while(tmp == PHY_LINK_OFF);
	
	/* Network initialization */
	APP_W5500_NetworkInit();
#endif  
	/* Output SYSCLK  on MCO1 pin(PA.08) */
	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
	/* Infinite loop */  
	while (1)
	{
	#ifdef BG_W5500
		int32_t ret = 0;
		 //TCP server loopback test
		if( (ret=Communication_Tcps(SOCK_TCPS, gDATABUF, 8000)) < 0) 
		{
		   #ifdef BG_UART_DEBUG
				printf("SOCKET ERROR : %d\r\n", ret);
				Communication_Error_Handle(SOCK_TCPS);
		   #endif
		}
	#endif 			
	#ifdef	BG_DOOR_LOCK
		Lock_CheckEvent();
	#endif
	#ifdef BG_WDT
		/* Refresh IWDG: reload counter */	
		IWDG_Refresh();
	#endif
	
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            PREDIV                         = 1
  *            PLLMUL                         = 6
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Select HSE Oscillator as PLL source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
	{
		/* Initialization Error */
		while(1); 
	}

	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
	{
		/* Initialization Error */
		while(1); 
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
	BSP_LED_Off();
	/* Turn LED1 on */
	BSP_LED_On();
	while(1)
	{
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
