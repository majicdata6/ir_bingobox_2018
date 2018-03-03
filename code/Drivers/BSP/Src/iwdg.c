#include "main.h"

IWDG_HandleTypeDef IwdgHandle;
TIM_HandleTypeDef Tim14Handle;

uint16_t tmpCC4[2] = {0, 0};
__IO uint32_t uwLsiFreq = 0;
__IO uint32_t uwCaptureNumber = 0;
uint32_t WaitingDelay = 0;

/*-----------------------------------------------------------------------------
Prototype      : 
Parameters     : 
Return         : void
Implementation : 
-----------------------------------------------------------------------------*/
void WDT_init(void)
{	
	/* Clear reset flags in any cases */
	__HAL_RCC_CLEAR_RESET_FLAGS();

	/*##-2- Get the LSI frequency: TIM14 is used to measure the LSI frequency ###*/
	uwLsiFreq = GetLSIFrequency();
	
	IWDG_Config();
}



 /**
  * @file   IWDG_Config
  * @brief  初始化独立看门狗
  * @param  无
  * @retval 无
  */
/*##-2- Configure & Start the IWDG peripheral ##############################*/
  /* Set counter reload value to obtain 762ms IWDG TimeOut.
     Counter Reload Value = (LsiFreq(Hz) * Timeout(ms)) / (prescaler * 1000)                         
  */
void IWDG_Config(void)
{
	/*##-3- Configure & Start the IWDG peripheral #########################################*/
	/* Set counter reload value to obtain 1 sec. IWDG TimeOut.
	IWDG counter clock Frequency = uwLsiFreq
	Set Prescaler to 32 (IWDG_PRESCALER_32)
	Timeout Period = (Reload Counter Value * 32) / uwLsiFreq
	So Set Reload Counter Value = (1 * uwLsiFreq) / 32 */
	IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_256;
	IwdgHandle.Init.Reload = (uwLsiFreq / 32);
	IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

	if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
}


/**
  * @brief TIM MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/*## Enable peripherals and GPIO Clocks ####################################*/
	/* RCC LSI clock enable */
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}


	/* TIMx Peripheral clock enable */
	__HAL_RCC_TIM14_CLK_ENABLE();

	/*## Configure the NVIC for TIMx ###########################################*/
	HAL_NVIC_SetPriority(TIM14_IRQn,0,0);

	/* Enable the TIM14 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM14_IRQn);
}

/**
  * @brief  Configures TIM14 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
uint32_t GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;


  /* Configure the TIM peripheral *********************************************/ 
  /* Set TIMx instance */  
  Tim14Handle.Instance = TIM14;
  
  /* TIM14 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM14 CH1.
     The Rising edge is used as active edge.
     The TIM14 CCR1 is used to compute the frequency value. 
  ------------------------------------------------------------ */
  Tim14Handle.Init.Prescaler         = 0;
  Tim14Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  Tim14Handle.Init.Period            = 0xFFFF;
  Tim14Handle.Init.ClockDivision     = 0;
  Tim14Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if(HAL_TIM_IC_Init(&Tim14Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Connect internally the TIM14_CH1 Input Capture to the LSI clock output */
  HAL_TIMEx_RemapConfig(&Tim14Handle, TIM_TIM14_MCO);

  /* Connect internally the MCO to LSI */
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_LSI, RCC_MCODIV_1);

  /* Configure the Input Capture of channel 1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&Tim14Handle, &TIMInput_Config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&Tim14Handle, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the TIM14 get 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable TIM14 CC1 Interrupt Request */
  HAL_TIM_IC_Stop_IT(&Tim14Handle, TIM_CHANNEL_1);
  
  /* Deinitialize the TIM14 peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&Tim14Handle);

  return uwLsiFreq;
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  uint32_t lsiperiod = 0;

  /* Get the Input Capture value */
  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&Tim14Handle, TIM_CHANNEL_1);

  if (uwCaptureNumber >= 2)
  {
    /* Compute the period length */
    lsiperiod = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);

    /* Frequency computation */ 
    uwLsiFreq = (uint32_t) SystemCoreClock / lsiperiod;
    uwLsiFreq *= 8;
  }
}

/* Refresh IWDG: reload counter */
void IWDG_Refresh(void)
{
	if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
	{
		/* Refresh Error */
		Error_Handler();
	}
}
