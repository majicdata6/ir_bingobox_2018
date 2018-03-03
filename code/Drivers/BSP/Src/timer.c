/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal_tim.h"
/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t led_timer = 0;
#ifdef BG_485_METER

//串口接收数据完成判断时间
uint16_t g_uart_rx_end_time=0;
uint16_t g_485uart_rx_end_time=0;
//Meter485_ReceiveData()用
uint32_t g_485_rx_wait_time=0;
extern uint8_t g_uart_rx_finish_b;

#endif

/* TIM handle declaration */
TIM_HandleTypeDef    Tim15Handle;

#ifdef BG_DOOR_LOCK
extern uint8_t g_door_auto_lock_flag;
uint32_t g_door_lock_auto_time = 0;
#endif
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */


/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* TIMx Peripheral clock enable */
	TIM15x_CLK_ENABLE();

	/*##-2- Configure the NVIC for TIMx ########################################*/
	/* Set the TIMx priority */
	HAL_NVIC_SetPriority(TIM15x_IRQn, 3, 0);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM15x_IRQn);
}
void Timer_Init(void)
{
	/* Prescaler declaration */
	uint32_t uwPrescalerValue = 0;
	/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
	uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;

	/* Set TIMx instance */
	Tim15Handle.Instance = TIM15x;

	/* Initialize TIMx peripheral as follows:
	+ Period = 10 - 1 (以10000Hz的频率运行,增加10为1ms)
	+ Prescaler = (SystemCoreClock/10000) - 1
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	Tim15Handle.Init.Period            = 10 - 1;
	Tim15Handle.Init.Prescaler         = uwPrescalerValue;
	Tim15Handle.Init.ClockDivision     = 0;
	Tim15Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	Tim15Handle.Init.RepetitionCounter = 0;
	Tim15Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&Tim15Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */
	if (HAL_TIM_Base_Start_IT(&Tim15Handle) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}
}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(led_timer++ == 1000)
	{
		led_timer = 0;
		BSP_LED_Toggle();
	}
#ifdef BG_485_METER
	if(g_485uart_rx_end_time)
	{
		--g_485uart_rx_end_time;
		if(!g_485uart_rx_end_time)
		{
			g_uart_rx_finish_b = TRUE;
		}
	}
	
	if(g_485_rx_wait_time)
	{
		--g_485_rx_wait_time;
		//大于0时只做一次操作
	}
        
#endif
#ifdef BG_LED
        PWM_Change_Use_Timer();	
#endif
#ifdef BG_DOOR_LOCK
        //自动挂锁时间
        if(g_door_lock_auto_time)
        {
                --g_door_lock_auto_time;
		if(!g_door_lock_auto_time)
		{
			g_door_auto_lock_flag = 1;
		}
        }
#endif
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
