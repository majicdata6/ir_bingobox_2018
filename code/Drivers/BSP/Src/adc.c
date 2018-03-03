/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
  /* Definition of ADCx conversions data table size */
  #define ADC_CONVERTED_DATA_BUFFER_SIZE   ((uint32_t)  32)   /* Size of array aADCxConvertedData[] */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ADC handle declaration */
ADC_HandleTypeDef             AdcHandle;
/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        AdcSConfig;

/* Private function prototypes -----------------------------------------------*/

static int32_t ADC_GetOneceValue(void)
{
        uint16_t adc_value;
        HAL_ADC_Start(&AdcHandle);
        if(HAL_ADC_PollForConversion(&AdcHandle,2000) == HAL_OK)
        {
                adc_value = HAL_ADC_GetValue(&AdcHandle);
        }
        else
        {
               return -1; 
        }
        return adc_value;
}

static int32_t ADC_GetAve(uint16_t count)
{
        uint32_t adc_value = 0;
        int32_t adc_value_tmp = 0;
        
        if((adc_value_tmp = ADC_GetOneceValue()) != -1)
        {
                adc_value = adc_value_tmp;
        }
        else
        {
                return -1;
        }
        --count;
        do
        { 
                if((adc_value_tmp = ADC_GetOneceValue()) != -1)
                {
                        adc_value = ( adc_value_tmp + adc_value) /2;
                }
                else
                {
                        return -1;
                }    
        }
        while(--count);
	return adc_value;
}
/** @defgroup HAL_MSP_Private_Functions
  * @{
  */
void ADC_Init(void)
{
	/* ### - 1 - Initialize ADC peripheral #################################### */

	AdcHandle.Instance          = ADCx;
	if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
	{
		/* ADC de-initialization Error */
		Error_Handler();
	}

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;      /* Synchronous clock mode, input ADC clock with prscaler 2 */

	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
	AdcHandle.Init.ScanConvMode          = DISABLE;    /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.EOCSelection          = DISABLE;           /* EOC flag picked-up to indicate conversion end */
	AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                        /* Continuous mode enabled (automatic conversion restart after each conversion) */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
	AdcHandle.Init.DMAContinuousRequests = DISABLE;                        /* ADC DMA continuous request to match with DMA circular mode */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
	AdcHandle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_239CYCLES_5;

	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}
  
  
	/* ### - 2 - Start calibration ############################################ */
	if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
	{
		Error_Handler();
	}
  
	/* ### - 3 - Channel configuration ######################################## */
	AdcSConfig.Channel      = ADCx_CHANNEL;               /* Channel to be converted */
	AdcSConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &AdcSConfig) != HAL_OK)
	{
		Error_Handler();
	}
}
/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
* @brief  ADC MSP Init
* @param  hadc : ADC handle
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef          GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock ****************************************/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/* ADC1 Periph clock enable */
	ADCx_CLK_ENABLE();

	/*##- 2- Configure peripheral GPIO #########################################*/
	/* ADC Channel GPIO pin configuration */
	GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
	/*##-1- Reset peripherals ##################################################*/
	ADCx_FORCE_RESET();
	ADCx_RELEASE_RESET();
	/* ADC Periph clock disable
	(automatically reset all ADC instances of the ADC common group) */
	__HAL_RCC_ADC1_CLK_DISABLE();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* De-initialize the ADC Channel GPIO pin */
	HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);
}

int32_t ADC_GetValue(void)
{  
	int32_t adc_value;
	if((adc_value = ADC_GetAve(20)) != -1)
        {
#ifdef BG_UART_DEBUG
		//printf("aADCxConvertedData=%d\r\n",adc_value);
#endif                
	}
	else
	{
                return -1;
#ifdef BG_UART_DEBUG
		printf("the adc converte failure!\r\n");
#endif                
	}
	return adc_value;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
