/**
  ******************************************************************************
  * @file    InfraRed/src/ir_rc5_send.c 
  * @author  WXF
  * @version V1.0.0
  * @date    2-2-2018
  * @brief   This file provides all the NEC firmware functions. 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ir_rc5_send.h"
#include "app_infrared.h"
/* Private macro -------------------------------------------------------------*/
#define IR_DEBUGE

/* Private define ------------------------------------------------------------*/
#define NOMINAL_RC5_HALF_BIT_TIME_US  		890    	/* Nominal half bit time (us)  */

#define RC5_SEND_TIMER_COUNT_PERIOD         10     	/* Timer count cycle time £¨us£©.  */


#define NOMINAL_RC5_HEAD_LEN    			2      
#define NOMINAL_RC5_KSTATUS_LEN 			1
#define NOMINAL_RC5_ADDR_LEN    			5
#define NOMINAL_RC5_COMMAND_LEN    			6
#define NOMINAL_RC5_FRAME_DATA_LEN    		14

#define RC5_FRAME_DATA_HALF_BIT_TIME_COUNT 	28

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint16_t rc5_time_bit_first[NOMINAL_RC5_FRAME_DATA_LEN];
	uint16_t rc5_time_bit_second[NOMINAL_RC5_FRAME_DATA_LEN];
}IRRC5SendBitTime_t;

typedef union
{
	uint16_t rc5_data_time_arr[RC5_FRAME_DATA_HALF_BIT_TIME_COUNT];
	IRRC5SendBitTime_t rc5_bit_time;
}IRRC5DateBitTime_u;


TIM_HandleTypeDef rc5_compare_tim;
TIM_HandleTypeDef rc5_36kPWM_tim;
IRCommand_u ir_rc5_data;

static __IO uint8_t 	s_rc5_key_bit = 0;
uint8_t 	s_rc5_addr = 0;
uint8_t 	s_rc5_command = 0;
uint8_t  	s_rc5_bit_half_time = 0;   				/* RC5 data bit half part time indication. 1=first,0=second*/
uint32_t	s_rc5_frame_data = 0;		 			/* A frame of infrared RC5 data. */
uint8_t  	s_rc5_frame_data_len = 0;      			/* A frame of infrared RC5 data length. */

//IRRC5DateBitTime_u rc5_frame_send;               				/*  */


/* Private function prototypes -----------------------------------------------*/
static void RC5_Send_GPIO_Configuration(void);
static void RC5_Send_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
static void RC5_Send_TIM_Base_Init(TIM_HandleTypeDef *htim);
static void RC5_36kPWM_Configuration(void);
static void RC5_Timer_Compare_Configuration(void);
static void RC5_36kPWM_Out(void);
static void RC5_36kPWM_Stop(void);
static void RC5_Timer_Compare_Set(uint16_t compare_value);
static void RC5_Timer_Compare_Start(void);
static void RC5_Timer_Compare_Stop(void);
static void RC5_Send_Start(void);
static void RC5_Send_Data(void);
/* Private functions ---------------------------------------------------------*/

static void RC5_Send_GPIO_Configuration(void)
{
	/* User code GPIO initialization */
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = RC5_SEND_GPIO_PIN;
	GPIO_InitStruct.Mode = RC5_SEND_GPIO_MODE;
	GPIO_InitStruct.Pull = RC5_SEND_GPIO_PULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = RC5_SEND_GPIO_ALTERANTE;
	
	HAL_GPIO_Init(RC5_SEND_GPIO_PORT, &GPIO_InitStruct);
	
	RC5_SEND_GPIO_CLK_EN();
	
}

static void RC5_Send_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==RC5_SEND_PWM_TIMx)
	{
		/* Peripheral clock enable */
		RC5_SEND_PWM_TIMx_CLK_EN();
		/* PWM interrupt Init */
		HAL_NVIC_SetPriority(RC5_SEND_PWM_TIMx_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(RC5_SEND_PWM_TIMx_IRQn);
	}
	else if(htim_base->Instance==RC5_SEND_TIMx)
	{
		/* Peripheral clock enable */
		RC5_SEND_TIMx_CLK_EN();
		/* NEC_SEND interrupt Init */
		HAL_NVIC_SetPriority(RC5_SEND_TIMx_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(RC5_SEND_TIMx_IRQn);
	}
}

static void RC5_Send_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
	
	/* Allocate lock resource and initialize it */
	if(htim->State == HAL_TIM_STATE_RESET)
	{
		/* Allocate lock resource and initialize it */
		htim->Lock = HAL_UNLOCKED;

		/* Init the low level hardware : GPIO, CLOCK, NVIC */
		RC5_Send_TIM_Base_MspInit(htim);
	}

	/* Set the TIM state */
	htim->State= HAL_TIM_STATE_BUSY;

	/* Set the Time Base configuration */
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	/* Initialize the TIM state*/
	htim->State= HAL_TIM_STATE_READY;
}

static void RC5_36kPWM_Configuration(void)
{
	/* User code  */
	TIM_OC_InitTypeDef sConfigOC;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

	rc5_36kPWM_tim.Instance = RC5_SEND_PWM_TIMx;
	rc5_36kPWM_tim.Init.Prescaler = RC5_SEND_PWM_PRESCALER;
	rc5_36kPWM_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	rc5_36kPWM_tim.Init.Period = RC5_SEND_PWM_PERIOD;
	rc5_36kPWM_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	rc5_36kPWM_tim.Init.RepetitionCounter = 0;
	rc5_36kPWM_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
	RC5_Send_TIM_Base_Init(&rc5_36kPWM_tim);

	if (HAL_TIM_PWM_Init(&rc5_36kPWM_tim) != HAL_OK)
	{
	//    _Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = RC5_SEND_sConfigOC_PULSE;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&rc5_36kPWM_tim, &sConfigOC, RC5_SEND_PWM_CHANNEL) != HAL_OK)
	{
//    _Error_Handler(__FILE__, __LINE__);
	}

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&rc5_36kPWM_tim, &sBreakDeadTimeConfig) != HAL_OK)
	{
//    _Error_Handler(__FILE__, __LINE__);
	}
	
//	RC5_Send_GPIO_Configuration();
}

static void RC5_Timer_Compare_Configuration(void)
{
	/* User code  */
	rc5_compare_tim.Instance = RC5_SEND_TIMx;
	rc5_compare_tim.Init.Prescaler = RC5_SEND_PRESCALER;
	rc5_compare_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	rc5_compare_tim.Init.Period = RC5_SEND_PERIOD;
	rc5_compare_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	rc5_compare_tim.Init.RepetitionCounter = 0;
	rc5_compare_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	RC5_Send_TIM_Base_Init(&rc5_compare_tim);
}


static void RC5_36kPWM_Out(void)
{
	/* User code  */
	HAL_TIM_PWM_Start_IT(&rc5_36kPWM_tim, RC5_SEND_PWM_CHANNEL);
}

static void RC5_36kPWM_Stop(void)
{
	/* User code  */
	HAL_TIM_PWM_Stop_IT(&rc5_36kPWM_tim, RC5_SEND_PWM_CHANNEL);
}

static void RC5_Timer_Compare_Set(uint16_t compare_value)
{
	/* User code  */
	__HAL_TIM_SET_COUNTER(&rc5_compare_tim, 0);
	__HAL_TIM_SET_COMPARE(&rc5_compare_tim, RC5_SEND_PWM_CHANNEL, compare_value);
}

static void RC5_Timer_Compare_Start(void)
{
	/* User code  */
	HAL_TIM_OC_Start_IT(&rc5_compare_tim, RC5_SEND_PWM_CHANNEL);
}

static void RC5_Timer_Compare_Stop(void)
{
	/* User code  */
	HAL_TIM_OC_Stop_IT(&rc5_compare_tim, RC5_SEND_PWM_CHANNEL);
}

static void RC5_Send_Start(void)
{
	RC5_Timer_Compare_Set(0);
	RC5_Timer_Compare_Start();
}

void RC5_Send_Time_Out_Back(TIM_HandleTypeDef *htim)
{
	if(htim == &rc5_compare_tim)
	{
		RC5_Send_Data();
	}
}

/**
  * @brief  void RC5_Send_Init(void) 
  * @param  None
  * @retval None
  */
void RC5_Send_Init(void)
{
	
	/* Configure the GPIO port for RC5 send */
	RC5_Send_GPIO_Configuration();
	
	/* PWM 36k-1/3 set*/
	RC5_36kPWM_Configuration();
	
	/* Timer Compare interrupt set*/
	RC5_Timer_Compare_Configuration();
	
}

/**
  * @brief  RC5_Data_Decod(uint32_t data) 
  * @param  uint32_t data 
  * @retval None
  */
void RC5_Data_Decod(uint32_t data)
{
	ir_rc5_data.command = data;
//	s_rc5_frame_data = data & 0x00003ffe;
	s_rc5_frame_data |= ir_rc5_data.array[1];
	s_rc5_frame_data <<= 8;
	s_rc5_frame_data |= ir_rc5_data.array[0];
	s_rc5_key_bit ^= 0x01;
	s_rc5_frame_data |= s_rc5_key_bit;
	s_rc5_frame_data <<= 2;
	s_rc5_frame_data |= 0x03;
	s_rc5_bit_half_time = 0;
	
	s_rc5_bit_half_time = 0;
	s_rc5_frame_data_len = 0;
	RC5_Send_Start();
}

/**
  * @brief  void RC5_Send_Data(void) 
  * @param  None
  * @retval None
  */
static void RC5_Send_Data(void)
{
	if(s_rc5_frame_data_len < RC5_FRAME_DATA_HALF_BIT_TIME_COUNT)
	{
		s_rc5_frame_data_len ++;
		RC5_Timer_Compare_Set(NOMINAL_RC5_HALF_BIT_TIME_US/RC5_SEND_TIMER_COUNT_PERIOD);
		RC5_Timer_Compare_Start();
		s_rc5_bit_half_time ^= 0x01;
		if(s_rc5_bit_half_time)
		{
			if(s_rc5_frame_data & 0x00000001)
			{
				RC5_36kPWM_Stop();
			}
			else
			{
				RC5_36kPWM_Out();
			}
		}
		else
		{
			if(s_rc5_frame_data & 0x00000001)
			{
				RC5_36kPWM_Out();
			}
			else
			{
				RC5_36kPWM_Stop();
			}
			s_rc5_frame_data = s_rc5_frame_data >> 1;
		}
		
	}
	else
	{
		s_rc5_bit_half_time = 0;
		RC5_36kPWM_Stop();
		RC5_Timer_Compare_Stop();
		
	}
}  

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
