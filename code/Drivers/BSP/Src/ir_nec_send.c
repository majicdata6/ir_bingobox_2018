/**
  ******************************************************************************
  * @file    src/ir_nec_send.c 
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
#include "ir_nec_send.h"
#include "app_infrared.h"

/* Private macro -------------------------------------------------------------*/
//#define IR_DEBUGE

/* Private define ------------------------------------------------------------*/
#define   NOMINAL_NEC_HEAD1_TIME_US		9000 /* Nominal head1 time in  */
#define   MIN_NEC_HEAD1_TIME_US 		8832 /* Min head1 time in  */
#define   MAX_NEC_HEAD1_TIME_US			9168 /* Max head1 time in  */

#define   NOMINAL_NEC_HEAD2_TIME_US		4500 /* Nominal head2 time in */
#define   MIN_NEC_HEAD2_TIME_US			4332 /* Min head2 time in  */
#define   MAX_NEC_HEAD2_TIME_US			4668 /* Max head2 time in  */

#define   NOMINAL_NEC_HEAD3_TIME_US		2250 /* Nominal head2 time in  */
#define   MIN_NEC_HEAD3_TIME_US			2082 /* Min head2 time in  */
#define   MAX_NEC_HEAD3_TIME_US			2418 /* Max head2 time in  */

#define   NOMINAL_NEC_H_TIME_US			560  /* Nominal signal carrier time in  */
#define   MIN_NEC_H_TIME_US 			392  /* Min signal carrier time in  */
#define   MAX_NEC_H_TIME_US 			728  /* Max signal carrier time in  */

#define   NOMINAL_NEC_L0_TIME_US		560  /* Nominal no signal carrier time in  */
#define   MIN_NEC_L0_TIME_US			392  /* Min no signal carrier time in  */
#define   MAX_NEC_L0_TIME_US			728  /* Max no signal carrier time in  */

#define   NOMINAL_NEC_L1_TIME_US		1690 /* Nominal no signal carrier time in  */
#define   MIN_NEC_L1_TIME_US			1522 /* Min no signal carrier time in  */
#define   MAX_NEC_L1_TIME_US			1858 /* Max no signal carrier time in  */

#define NOMINAL_NEC_HEAD_NUM			1 

#define NOMINAL_NEC_ADDR1_LEN			8
#define NOMINAL_NEC_ADDR2_LEN			8
#define NOMINAL_NEC_COMMAND1_LEN		8
#define NOMINAL_NEC_COMMAND2_LEN		8

#define NOMINAL_NEC_FRAME_DATA_LEN		32
#define NEC_FRAME_DATA_HALF_BIT_TIME_COUNT	68 /* Time total = head*2 + data32*2 +last1*2(for test last bit, time=560us )*/

//Users need to configure
#define NEC_SEND_TIMER_COUNT_PERIOD			10 /* Timer count cycle time £¨us£©.  */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint16_t nec_time_bit_first;
	uint16_t nec_time_bit_second;
}IRNECSendBitTime_t;

typedef struct
{
	IRNECSendBitTime_t head;
	IRNECSendBitTime_t add1[NOMINAL_NEC_ADDR1_LEN];
	IRNECSendBitTime_t add2[NOMINAL_NEC_ADDR2_LEN];
	IRNECSendBitTime_t command1[NOMINAL_NEC_COMMAND1_LEN];
	IRNECSendBitTime_t command2[NOMINAL_NEC_COMMAND2_LEN];
	IRNECSendBitTime_t last;
}IRNECSendDataTime_t;

typedef union
{
	uint16_t nec_data_time_arr[NEC_FRAME_DATA_HALF_BIT_TIME_COUNT];
	IRNECSendDataTime_t nec_bit_time;
}IRNECDateBitTime_u;



TIM_HandleTypeDef nec_compare_tim;
TIM_HandleTypeDef nec_38kPWM_tim;

IRNECDateBitTime_u nec_frame_send;           				/* Bit time per frame data.  */
IRCommand_u ir_nec_data;
/* Private variable ---------------------------------------------------------*/
uint8_t  	s_nec_add1_time = 0;
uint8_t  	s_nec_add2_time = 0;
uint8_t  	s_nec_command1_time = 0;
uint8_t  	s_nec_command2_time = 0;

uint8_t  	s_nec_bit_half_time = 0; 			/* To count the one-bit first or the second */
uint8_t  	s_nec_frame_bit_half_len = 0;
uint32_t	s_nec_frame_data = 0;				/* A frame of infrared nec data. */
uint8_t 	s_nec_frame_data_len = 0;			/* One frame of nec data length */




/* Private function prototypes -----------------------------------------------*/
static void NEC_Send_GPIO_Configuration(void);
static void NEC_Send_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
static void NEC_Send_TIM_Base_Init(TIM_HandleTypeDef *htim);
static void NEC_38kPWM_Configuration(void);
static void NEC_Timer_Compare_Configuration(void);
static void NEC_38kPWM_Out(void);
static void NEC_38kPWM_Stop(void);
static void NEC_Timer_Compare_Set(uint16_t compare_value);
static void NEC_Timer_Compare_Start(void);
static void NEC_Timer_Compare_Stop(void);
static void NEC_Send_Data(void);
static void NEC_Send_Start(void);

/* Private functions ---------------------------------------------------------*/

static void NEC_Send_GPIO_Configuration(void)
{
	/* User code GPIO initialization */
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = NEC_SEND_GPIO_PIN;
	GPIO_InitStruct.Mode = NEC_SEND_GPIO_MODE;
	GPIO_InitStruct.Pull = NEC_SEND_GPIO_PULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = NEC_SEND_GPIO_ALTERANTE;
	
	HAL_GPIO_Init(NEC_SEND_GPIO_PORT, &GPIO_InitStruct);
	
	NEC_SEND_GPIO_CLK_EN();
	
}

static void NEC_Send_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==NEC_SEND_PWM_TIMx)
	{
		/* Peripheral clock enable */
		NEC_SEND_PWM_TIMx_CLK_EN();
		/* PWM interrupt Init */
		HAL_NVIC_SetPriority(NEC_SEND_PWM_TIMx_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(NEC_SEND_PWM_TIMx_IRQn);
	}
	else if(htim_base->Instance==NEC_SEND_TIMx)
	{
		/* Peripheral clock enable */
		NEC_SEND_TIMx_CLK_EN();
		/* NEC_SEND interrupt Init */
		HAL_NVIC_SetPriority(NEC_SEND_TIMx_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(NEC_SEND_TIMx_IRQn);
	}
}

static void NEC_Send_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
	
	/* Allocate lock resource and initialize it */
	if(htim->State == HAL_TIM_STATE_RESET)
	{
		/* Allocate lock resource and initialize it */
		htim->Lock = HAL_UNLOCKED;

		/* Init the low level hardware : GPIO, CLOCK, NVIC */
		NEC_Send_TIM_Base_MspInit(htim);
	}

	/* Set the TIM state */
	htim->State= HAL_TIM_STATE_BUSY;

	/* Set the Time Base configuration */
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	/* Initialize the TIM state*/
	htim->State= HAL_TIM_STATE_READY;
}

static void NEC_38kPWM_Configuration(void)
{
	/* User code  */
	TIM_OC_InitTypeDef sConfigOC;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

	nec_38kPWM_tim.Instance = NEC_SEND_PWM_TIMx;
	nec_38kPWM_tim.Init.Prescaler = NEC_SEND_PWM_PRESCALER;
	nec_38kPWM_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	nec_38kPWM_tim.Init.Period = NEC_SEND_PWM_PERIOD;
	nec_38kPWM_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	nec_38kPWM_tim.Init.RepetitionCounter = 0;
	nec_38kPWM_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	NEC_Send_TIM_Base_Init(&nec_38kPWM_tim);

	if (HAL_TIM_PWM_Init(&nec_38kPWM_tim) != HAL_OK)
	{
	//    _Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = NEC_SEND_sConfigOC_PULSE;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&nec_38kPWM_tim, &sConfigOC, NEC_SEND_PWM_CHANNEL) != HAL_OK)
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
	if (HAL_TIMEx_ConfigBreakDeadTime(&nec_38kPWM_tim, &sBreakDeadTimeConfig) != HAL_OK)
	{
	//    _Error_Handler(__FILE__, __LINE__);
	}
	
//	NEC_Send_GPIO_Configuration();
}

static void NEC_Timer_Compare_Configuration(void)
{
	/* User code  */
	nec_compare_tim.Instance = NEC_SEND_TIMx;
	nec_compare_tim.Init.Prescaler = NEC_SEND_PRESCALER;
	nec_compare_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	nec_compare_tim.Init.Period = NEC_SEND_PERIOD;
	nec_compare_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	nec_compare_tim.Init.RepetitionCounter = 0;
	nec_compare_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	NEC_Send_TIM_Base_Init(&nec_compare_tim);
}

static void NEC_38kPWM_Out(void)
{
	HAL_TIM_PWM_Start_IT(&nec_38kPWM_tim, NEC_SEND_PWM_CHANNEL);
}

static void NEC_38kPWM_Stop(void)
{
	HAL_TIM_PWM_Stop_IT(&nec_38kPWM_tim, NEC_SEND_PWM_CHANNEL);
}

static void NEC_Timer_Compare_Set(uint16_t compare_value)
{
	__HAL_TIM_SET_COUNTER(&nec_compare_tim, 0);
	__HAL_TIM_SET_COMPARE(&nec_compare_tim, NEC_SEND_PWM_CHANNEL, compare_value);
}


static void NEC_Timer_Compare_Start(void)
{
	HAL_TIM_OC_Start_IT(&nec_compare_tim, NEC_SEND_PWM_CHANNEL);
}

static void NEC_Timer_Compare_Stop(void)
{
	HAL_TIM_OC_Stop_IT(&nec_compare_tim, NEC_SEND_PWM_CHANNEL);
}

static void NEC_Send_Start(void)
{
	NEC_Timer_Compare_Set(0);
	NEC_Timer_Compare_Start();
}

void NEC_Send_Time_Out_Back(TIM_HandleTypeDef *htim)
{
	if(htim == &nec_compare_tim)
	{
		NEC_Send_Data();
	}
}

/**
  * @brief  void NEC_Send_Init(void)
  * @param  None
  * @retval None
  */
void NEC_Send_Init(void)
{
	/* Configure the GPIO port for nec send */
	NEC_Send_GPIO_Configuration();
	/* PWM 38k-1/3 set*/
	NEC_38kPWM_Configuration();
	/* Timer Compare interrupt set*/
	NEC_Timer_Compare_Configuration();
	
}

/**
  * @brief  void NEC_Data_Decod(uint32_t data) 
  * @param  uint32_t data 
  * @retval None
  */
void NEC_Data_Decod(uint32_t data)
{
////	#ifdef 	IR_DEBUGE			
//		//print
//		printf("Infrared nec command= %lx!\r\n",data);
////	#endif
	ir_nec_data.command = data;
	
	s_nec_add1_time = ir_nec_data.array[0];
	s_nec_add2_time = ir_nec_data.array[1];
	s_nec_command1_time = ir_nec_data.array[2];
	s_nec_command2_time = ir_nec_data.array[3];
	
	s_nec_bit_half_time = 0;
	
	nec_frame_send.nec_bit_time.head.nec_time_bit_first = NOMINAL_NEC_HEAD1_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
	nec_frame_send.nec_bit_time.head.nec_time_bit_second = NOMINAL_NEC_HEAD2_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
	
	for(uint8_t i=0; i<NOMINAL_NEC_ADDR1_LEN; i++)
	{
		nec_frame_send.nec_bit_time.add1[i].nec_time_bit_first = NOMINAL_NEC_H_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		if(s_nec_add1_time & 0x01)
		{
			nec_frame_send.nec_bit_time.add1[i].nec_time_bit_second = NOMINAL_NEC_L1_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		else
		{
			nec_frame_send.nec_bit_time.add1[i].nec_time_bit_second = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		s_nec_add1_time = s_nec_add1_time >> 1;
	}
	
	for(uint8_t i=0; i<NOMINAL_NEC_ADDR2_LEN; i++)
	{
		nec_frame_send.nec_bit_time.add2[i].nec_time_bit_first = NOMINAL_NEC_H_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		if(s_nec_add2_time & 0x01)
		{
			nec_frame_send.nec_bit_time.add2[i].nec_time_bit_second = NOMINAL_NEC_L1_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		else
		{
			nec_frame_send.nec_bit_time.add2[i].nec_time_bit_second = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		s_nec_add2_time = s_nec_add2_time >> 1;
	}
	
	for(uint8_t i=0; i<NOMINAL_NEC_COMMAND1_LEN; i++)
	{
		nec_frame_send.nec_bit_time.command1[i].nec_time_bit_first = NOMINAL_NEC_H_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		if(s_nec_command1_time & 0x01)
		{
			nec_frame_send.nec_bit_time.command1[i].nec_time_bit_second = NOMINAL_NEC_L1_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		else
		{
			nec_frame_send.nec_bit_time.command1[i].nec_time_bit_second = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		s_nec_command1_time = s_nec_command1_time >> 1;
	}
	
	for(uint8_t i=0; i<NOMINAL_NEC_COMMAND2_LEN; i++)
	{
		nec_frame_send.nec_bit_time.command2[i].nec_time_bit_first = NOMINAL_NEC_H_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		if(s_nec_command2_time & 0x01)
		{
			nec_frame_send.nec_bit_time.command2[i].nec_time_bit_second = NOMINAL_NEC_L1_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		else
		{
			nec_frame_send.nec_bit_time.command2[i].nec_time_bit_second = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
		}
		s_nec_command2_time = s_nec_command2_time >> 1;
	}
	nec_frame_send.nec_bit_time.last.nec_time_bit_first = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;
	nec_frame_send.nec_bit_time.last.nec_time_bit_second = NOMINAL_NEC_L0_TIME_US/NEC_SEND_TIMER_COUNT_PERIOD;

	s_nec_bit_half_time = 0;
	s_nec_frame_bit_half_len = 0 ;
	NEC_Send_Start();
}
/**
  * @brief  void NEC_Send_Data(void)
  * @param  None
  * @retval None
  */
static void NEC_Send_Data(void)
{
	if(s_nec_frame_bit_half_len < NEC_FRAME_DATA_HALF_BIT_TIME_COUNT)
	{
		NEC_Timer_Compare_Set(nec_frame_send.nec_data_time_arr[s_nec_frame_bit_half_len++]);
		NEC_Timer_Compare_Start();
		s_nec_bit_half_time = s_nec_bit_half_time ^ 0x01;
		if(s_nec_bit_half_time)
		{
			NEC_38kPWM_Out();
		}
		else
		{
			NEC_38kPWM_Stop();
		}
	}
	else
	{
		s_nec_bit_half_time = 0;
		NEC_38kPWM_Stop();
		NEC_Timer_Compare_Stop();
	}
}  



/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
