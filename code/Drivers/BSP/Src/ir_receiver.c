/**
  ******************************************************************************
  * @file    /src/ir_rc5_receiver.c 
  * @author  WXF
  * @version V1.0.0
  * @date    2-2-2018
  * @brief   This file provides all the RC-5 firmware functions. 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * s_bit_time_valueE. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
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
#include "ir_receiver.h"

/* Private macro -------------------------------------------------------------*/
//#define IR_DEBUGE


/* Private define ------------------------------------------------------------*/

#define   IR_READ_DATA_BUFF_LEN           100  

//rc5 define
#define   RC5_NOMINAL_HALF_BIT_TIME_US    890     /* Nominal half bit s_bit_time_valuee in  */
#define   RC5_MIN_HALF_BIT_TIME_US        640     /* Minimum half bit s_bit_time_valuee in  */
#define   RC5_MAX_HALF_BIT_TIME_US        1140    /* Maximum half bit s_bit_time_valuee in  */

#define   RC5_NOMINAL_FULL_BIT_TIME_US    1778    /* Nominal Full bit s_bit_time_valuee in  */
#define   RC5_MIN_FULL_BIT_TIME_US        1340    /* Minimum Full bit s_bit_time_valuee in  */
#define   RC5_MAX_FULL_BIT_TIME_US        2220    /* Maximum Full bit s_bit_time_valuee in  */

#define   RC5_NOMINAL_READ_FRAME_DATA_LEN 12      /* RC5 frame data length.  */
#define   RC5_FRAME_DATA_HALF_BIT_LEN     24      /* RC5 half bit total len.  */

//nec define
#define   NEC_NOMINAL_HEAD1_TIME_US       9000    /* Nominal head1 time in  */
#define   NEC_MIN_HEAD1_TIME_US           8832    /* Min head1 time in  */
#define   NEC_MAX_HEAD1_TIME_US           9668    /* Max head1 time in  */

#define   NEC_NOMINAL_HEAD2_TIME_US       4500    /* Nominal head2 time in */
#define   NEC_MIN_HEAD2_TIME_US           4332    /* Min head2 time in  */
#define   NEC_MAX_HEAD2_TIME_US           4768    /* Max head2 time in  */

#define   NEC_NOMINAL_HEAD3_TIME_US       2250    /* Nominal head2 time in  */
#define   NEC_MIN_HEAD3_TIME_US           2082    /* Min head2 time in  */
#define   NEC_MAX_HEAD3_TIME_US           2418    /* Max head2 time in  */

#define   NEC_NOMINAL_H_TIME_US           560     /* Nominal signal carrier time in  */
#define   NEC_MIN_H_TIME_US               392     /* Min signal carrier time in  */
#define   NEC_MAX_H_TIME_US               728     /* Max signal carrier time in  */

#define   NEC_NOMINAL_L0_TIME_US          560     /* Nominal no signal carrier time in  */
#define   NEC_MIN_L0_TIME_US              392     /* Min no signal carrier time in  */
#define   NEC_MAX_L0_TIME_US              728     /* Max no signal carrier time in  */

#define   NEC_NOMINAL_L1_TIME_US          1690    /* Nominal no signal carrier time in  */
#define   NEC_MIN_L1_TIME_US              1522    /* Min no signal carrier time in  */
#define   NEC_MAX_L1_TIME_US              1858    /* Max no signal carrier time in  */

#define   NEC_NOMINAL_READ_FRAME_DATA_LEN 32      /* NEC frame data length.  */
#define   NEC_FRAME_DATA_HALF_BIT_LEN     64      /* NEC half bit total len. */


//Users need to configure
#define TIMER_COUNT_PERIOD                10      /* Timer count cycle time £¨us£©.  */
#define IR_RECEIVE_FILTER_TIME            108000  /* Infrared receiving filtering time £¨us£©.  */



/* Private typedef -----------------------------------------------------------*/
typedef enum 
{ 
	IR_RECEIVER_STATE_IDLE = 0, 
	IR_RECEIVER_STATE_S1,
	IR_RECEIVER_STATE_RC5_S2,
	IR_RECEIVER_STATE_NEC_S2,
	IR_RECEIVER_STATE_RC5_DATA,
	IR_RECEIVER_STATE_NEC_DATA
	
}IRReceiveStateNECOrRC5_e;




TIM_HandleTypeDef receive_tim;

//IRFrameStatus_e rc5_framereceived = NO;        /* Will be used by the user to check if an RC5 has been received or not */

IRReceiveStateNECOrRC5_e IrReceiveState = IR_RECEIVER_STATE_IDLE;             /* Infrared receiving state */

/* Private variables ---------------------------------------------------------*/

uint8_t  s_bit_edges_count = 0;                    /* To count the one-bit first edges or the second Edge */
//uint32_t s_bit_time_value = 0;                     /* RC5-Infrared frame data receiving timer.NEC-signal carrier or no signal carrier time .  */

//rc5_Private_variables
uint8_t s_bit_value = 1;                           /* One bit value of Ir data*/
uint8_t s_rc5_receive_frame_data[RC5_NOMINAL_READ_FRAME_DATA_LEN];        /* A frame of infrared RC5 data not containing the first two initial bits. */
uint8_t s_rc5_bit_count = 0;                       /* One frame of RC5 data bit length */
//static uint32_t rc5_receive_data = 0;

//nec_Private_variables 

//static uint16_t s_nec_receive_frame_data[NEC_NOMINAL_READ_FRAME_DATA_LEN]; 	           /* A frame of infrared NEC data not containing the first two initial bits. */
uint8_t  s_nec_receive_frame_data[NEC_NOMINAL_READ_FRAME_DATA_LEN]; 	 
uint8_t  s_nec_bit_count = 0;                       /* One frame of NEC data bit length */

uint16_t  s_receive_data[IR_READ_DATA_BUFF_LEN];
uint16_t  s_receive_data_len = 0; 

/* Private function prototypes -----------------------------------------------*/
static void Receive_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
static void Receive_TIM_Base_Init(TIM_HandleTypeDef *htim);
static void Timer_Frame_Time_Out_Configuration(void);
static void Receive_Timer_Configuration(void);
static void Receive_GPIO_Configuration(void);
static uint8_t Receive_GPIO_Read(void);

//static void Receive_Timer_Count_Start(void);
//static void Receive_Timer_Count_Stop(void);
//static uint16_t Receive_Timer_Count_Get(void);
//static void Receive_Timer_Count_Reset(void);

static void NEC_Timer_Compare_Set(uint16_t compare_value);
static void NEC_Timer_Compare_Start(void);
static void NEC_Timer_Compare_Stop(void);

static uint32_t Receive_Get_Timer_Count(void);
static void IR_Receive_Data(void);
static void IR_Decod_Data(void);


/* Private functions ---------------------------------------------------------*/

static void Receive_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==RECEIVE_TIMx)
	{
		/* Peripheral clock enable */
		RECEIVE_TIMx_CLK_EN();

		/* PWM interrupt Init */
		HAL_NVIC_SetPriority(RECEIVE_TIMx_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(RECEIVE_TIMx_IRQn);

	}
}

static void Receive_TIM_Base_Init(TIM_HandleTypeDef *htim)
{
	
	/* Allocate lock resource and initialize it */
	if(htim->State == HAL_TIM_STATE_RESET)
	{
		/* Allocate lock resource and initialize it */
		htim->Lock = HAL_UNLOCKED;

		/* Init the low level hardware : GPIO, CLOCK, NVIC */
		Receive_TIM_Base_MspInit(htim);
	}

	/* Set the TIM state */
	htim->State= HAL_TIM_STATE_BUSY;

	/* Set the Time Base configuration */
	TIM_Base_SetConfig(htim->Instance, &htim->Init);

	/* Initialize the TIM state*/
	htim->State= HAL_TIM_STATE_READY;
}

static void Timer_Frame_Time_Out_Configuration(void)
{
	receive_tim.Instance = RECEIVE_TIMx;                   //
	receive_tim.Init.Prescaler = RECEIVE_PRESCALER;        //    
	receive_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	receive_tim.Init.Period = RECEIVE_PERIOD;              //
	receive_tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	receive_tim.Init.RepetitionCounter = 0;
	receive_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	Receive_TIM_Base_Init(&receive_tim);
}

static void Receive_Timer_Configuration(void)
{
	/* User code timer initialization for IR_RC5*/
	Timer_Frame_Time_Out_Configuration();
	
}

static void Receive_GPIO_Configuration(void)
{
	/* User code GPIO initialization for IR_RC5 receive*/
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* GPIO Ports Clock Enable */
	RECEIVE_GPIO_CLK_EN();
	
	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin 	= RECEIVE_GPIO_PIN;
	GPIO_InitStruct.Mode 	= RECEIVE_GPIO_MODE;
	GPIO_InitStruct.Pull 	= RECEIVE_GPIO_PULL;
	
	HAL_GPIO_Init(RECEIVE_GPIO_PORT, &GPIO_InitStruct);
	
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(RECEIVE_GPIO_EXTIx_IRQn, 1, 0);
}

static uint8_t Receive_GPIO_Read(void)
{
	return((uint8_t)HAL_GPIO_ReadPin(RECEIVE_GPIO_PORT,RECEIVE_GPIO_PIN));
}


static void NEC_Timer_Compare_Set(uint16_t compare_value)
{
	__HAL_TIM_SET_COUNTER(&receive_tim, 0);
	__HAL_TIM_SET_COMPARE(&receive_tim, TIM_CHANNEL_1, compare_value);
}
static void NEC_Timer_Compare_Start(void)
{
	HAL_TIM_OC_Start_IT(&receive_tim, TIM_CHANNEL_1);
}
static void NEC_Timer_Compare_Stop(void)
{
	HAL_TIM_OC_Stop_IT(&receive_tim, TIM_CHANNEL_1);
}
void NEC_Timer_Start(void)
{
	NEC_Timer_Compare_Set(11000);
	NEC_Timer_Compare_Start();
}

static uint32_t Receive_Get_Timer_Count(void)
{
	volatile uint16_t t_count = 0;
	//timer stop
//	Receive_Timer_Count_Stop();
	//get timer count
	t_count = Receive_Timer_Count_Get();
	//reset timer count
	Receive_Timer_Count_Reset();
	//timer start
//	Receive_Timer_Count_Start();
	return t_count;
}

/**
  * @brief  IR_Receive IO external interrupt callback.
  * @param  None
  * @retval None
  */
//void IR_Receive_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
void IR_Receiver_GPIO_EXTI_Back(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == RECEIVE_GPIO_PIN)
	{
//		if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET) 
//		{ 
//			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
//			HAL_GPIO_EXTI_Callback(GPIO_Pin);
//		}
		IR_Receive_Data();

	}
}

/**
  * @brief  IR_Receive timer interrupts the callback.
  * @param  TIM_HandleTypeDef *htim
  * @retval None
  */
void IR_Receiver_Time_Out_Back(TIM_HandleTypeDef *htim)
{	
	if(htim == &receive_tim)
	{
//		BSP_LED_Toggle();
		IR_Receiver_EXTI_Stop();
		NEC_Timer_Compare_Stop();
		if(s_receive_data_len > RC5_NOMINAL_READ_FRAME_DATA_LEN)
		IR_Decod_Data();
	}
//	if(htim == &receive_tim)
//	{
////		BSP_LED_Toggle();
//		IR_Receiver_EXTI_Stop();
//		Receive_Timer_Count_Stop();
//		if(s_receive_data_len > RC5_NOMINAL_READ_FRAME_DATA_LEN)
//		IR_Decod_Data();
//	}
}
/**
  * @brief  IR Receiver gpio exti enable.
  * @param  None
  * @retval None
  */
void IR_Receiver_EXTI_Start(void)
{
	HAL_NVIC_EnableIRQ(RECEIVE_GPIO_EXTIx_IRQn);
}
/**
  * @brief  IR Receiver gpio exti disable.
  * @param  None
  * @retval None
  */
void IR_Receiver_EXTI_Stop(void)
{
	HAL_NVIC_DisableIRQ(RECEIVE_GPIO_EXTIx_IRQn);
}

/**
  * @brief  uint32_t RC5_IR_Receiver_Turn(void)
  * @param  None
  * @retval uint32_t rc5_receive_data(00+command_6b+add_5b+key_1b)
  */
uint32_t RC5_IR_Receiver_Turn(void)
{
	uint32_t rc5_receive_data = 0;
	for(uint8_t i=RC5_NOMINAL_READ_FRAME_DATA_LEN; i>0; i--)
	{
		rc5_receive_data <<=1;
		rc5_receive_data |= s_rc5_receive_frame_data[i-1] & 0x01;
	}
	return (rc5_receive_data);
}

/**
  * @brief  uint32_t NEC_IR_Receiver_Turn(void)
  * @param  None
  * @retval None
  */
uint32_t NEC_IR_Receiver_Turn(void)
{
	uint32_t nec_receive_data = 0;
	for(uint8_t i=NEC_NOMINAL_READ_FRAME_DATA_LEN; i>0; i--)
	{
		nec_receive_data <<=1;
		nec_receive_data |= s_nec_receive_frame_data[i-1] & 0x01;
	}
	return (nec_receive_data);
}

/**
  * @brief  Initialize the IR reception.
  * @param  None
  * @retval None
  */

void IR_Receiver_Init(void)
{
	
	/* Configure the GPIO port for IR_Receiver reception */
	Receive_GPIO_Configuration();
	/* Timer Configuration for IR_Receiver reception */
	Receive_Timer_Configuration();
//	__HAL_TIM_ENABLE_IT(&receive_tim, TIM_IT_UPDATE);
	
		
}

/**
  * @brief  Receive infrared data.
  * @param  None
  * @retval None
  */
static void IR_Receive_Data(void)
{
	if(s_receive_data_len<IR_READ_DATA_BUFF_LEN)
	{
		s_receive_data[s_receive_data_len++] = Receive_Timer_Count_Get();
		NEC_Timer_Start();
	}
	else
	{
		NEC_Timer_Compare_Stop();
	}
	
//	if(s_receive_data_len == 0)
//		Receive_Timer_Count_Start();
//	if(s_receive_data_len<IR_READ_DATA_BUFF_LEN)
//	{
//		s_receive_data[s_receive_data_len++] = Receive_Get_Timer_Count();
//	}
//	else
//	{
//		IR_Receiver_EXTI_Stop();
////		infrared_framereceived = FAIL;
//	}

}

/**
  * @brief  static void IR_Decod_Data(void)
  * @param  None
  * @retval None
  */
static void IR_Decod_Data(void)
{
	uint16_t ir_head1 = s_receive_data[1];
	uint16_t ir_head2 = s_receive_data[2];
	uint16_t* p_ir_data  = &s_receive_data[3];
	uint16_t ir_half_data_tim = 0;
	
	s_bit_edges_count = 0;
	s_rc5_bit_count = 0;
	s_nec_bit_count = 0;
	
	if((ir_head1 > RC5_MIN_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD) && (ir_head1 < RC5_MAX_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD))
	{
		if((ir_head2 > RC5_MIN_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD) && (ir_head2 < RC5_MAX_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD))
		{
//			IrReceiveState = IR_RECEIVER_STATE_RC5_DATA;
			s_bit_value = 1;
			for(uint8_t i = 0; i<RC5_FRAME_DATA_HALF_BIT_LEN; i++)
			{
				ir_half_data_tim = *(p_ir_data + i);

				if((ir_half_data_tim > RC5_MIN_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < RC5_MAX_HALF_BIT_TIME_US/TIMER_COUNT_PERIOD))
				{
					if(1 ==  s_bit_edges_count)
					{
						s_bit_edges_count = 0;
						s_rc5_receive_frame_data[s_rc5_bit_count++] = s_bit_value;
						if(s_rc5_bit_count == RC5_NOMINAL_READ_FRAME_DATA_LEN)    //RC5 receive ok
						{
							infrared_framereceived = RC5_YES;
							break;
							
	//					#ifdef 	IR_DEBUGE			
	//						//print
	//						printf("Infrared reception RC5_YES!\r\n");
	//					#endif
						}
					}
					else
					{
						s_bit_edges_count++;
					}
				}
				else if((ir_half_data_tim > RC5_MIN_FULL_BIT_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < RC5_MAX_FULL_BIT_TIME_US/TIMER_COUNT_PERIOD))
				{
					s_bit_edges_count = 0;
					s_bit_value ^=  0x01;
					s_rc5_receive_frame_data[s_rc5_bit_count++] = s_bit_value & 0x01;
					if(s_rc5_bit_count == RC5_NOMINAL_READ_FRAME_DATA_LEN)    //RC5 receive ok
					{
						infrared_framereceived = RC5_YES;
						break;
						
	//				#ifdef 	IR_DEBUGE			
	//				//print
	//				printf("Infrared reception RC5_YES!\r\n");
	//				#endif
					}
				}
				else
				{
					infrared_framereceived = FAIL;
					
	//			#ifdef 	IR_DEBUGE			
	//				//print
	//				printf("Infrared infrared reception RC5_DATA failure!\r\n");
	//			#endif
				}
			}
		}
		else
		{
			infrared_framereceived = FAIL;
			
//			#ifdef 	IR_DEBUGE			
//				//print
//				printf("Infrared infrared reception RC5_S2 failure!\r\n");
//			#endif
		}
	}
	else if((ir_head1 > NEC_MIN_HEAD1_TIME_US/TIMER_COUNT_PERIOD) && (ir_head1 < NEC_MAX_HEAD1_TIME_US/TIMER_COUNT_PERIOD))
	{
		//A single instruction
		if((ir_head2 > NEC_MIN_HEAD2_TIME_US/TIMER_COUNT_PERIOD) && (ir_head2 < NEC_MAX_HEAD2_TIME_US/TIMER_COUNT_PERIOD))
		{
			for(uint8_t i = 0; i<NEC_FRAME_DATA_HALF_BIT_LEN; i++)
			{
				ir_half_data_tim = *(p_ir_data + i);
				if(i%2)
				{
					if((ir_half_data_tim > NEC_MIN_L0_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < NEC_MAX_L0_TIME_US/TIMER_COUNT_PERIOD))
					{
						s_nec_receive_frame_data[s_nec_bit_count++] = 0;
						
						if(s_nec_bit_count == NEC_NOMINAL_READ_FRAME_DATA_LEN)
						{
							infrared_framereceived = NEC_YES;

	//					#ifdef 	IR_DEBUGE			
	//						//print
	//						printf("Infrared reception NEC_YES!\r\n");
	//					#endif
						}
					}
					else if((ir_half_data_tim > NEC_MIN_L1_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < NEC_MAX_L1_TIME_US/TIMER_COUNT_PERIOD))
					{
						s_nec_receive_frame_data[s_nec_bit_count++] = 1;
						
						if(s_nec_bit_count == NEC_NOMINAL_READ_FRAME_DATA_LEN)
						{
							infrared_framereceived = NEC_YES;

	//					#ifdef 	IR_DEBUGE			
	//						//print
	//						printf("Infrared reception NEC_YES!\r\n");
	//					#endif
						}
					}
					else
					{
						infrared_framereceived = FAIL;
						
	//				#ifdef 	IR_DEBUGE			
	//					//print
	//					printf("Infrared infrared reception NEC_DATA failure!\r\n");
	//				#endif
					}
				}
			}
		}
		//Continuous instruction
		else if((ir_head2 > NEC_MIN_HEAD3_TIME_US/TIMER_COUNT_PERIOD) && (ir_head2 < NEC_MAX_HEAD3_TIME_US/TIMER_COUNT_PERIOD))
		{
			for(uint8_t i = 0; i<NEC_NOMINAL_READ_FRAME_DATA_LEN; i++)
			{
				ir_half_data_tim = *(p_ir_data + i);
				if(i%2)
				{
					if((ir_half_data_tim > NEC_MIN_L0_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < NEC_MAX_L0_TIME_US/TIMER_COUNT_PERIOD))
					{
						s_nec_receive_frame_data[s_nec_bit_count++] = 0;
						
						if(s_nec_bit_count == NEC_NOMINAL_READ_FRAME_DATA_LEN)
						{
							infrared_framereceived = NEC_YES;

	//					#ifdef 	IR_DEBUGE			
	//						//print
	//						printf("Infrared reception NEC_YES!\r\n");
	//					#endif
						}
					}
					else if((ir_half_data_tim > NEC_MIN_L1_TIME_US/TIMER_COUNT_PERIOD) && (ir_half_data_tim < NEC_MAX_L1_TIME_US/TIMER_COUNT_PERIOD))
					{
						s_nec_receive_frame_data[s_nec_bit_count++] = 1;
						
						if(s_nec_bit_count == NEC_NOMINAL_READ_FRAME_DATA_LEN)
						{
							infrared_framereceived = NEC_YES;

	//					#ifdef 	IR_DEBUGE			
	//						//print
	//						printf("Infrared reception NEC_YES!\r\n");
	//					#endif
						}
					}
					else
					{
						infrared_framereceived = FAIL;
						
	//				#ifdef 	IR_DEBUGE			
	//					//print
	//					printf("Infrared infrared reception NEC_DATA failure!\r\n");
	//				#endif
					}
				}
			}
		}
		else
		{

			infrared_framereceived = FAIL;
//			#ifdef 	IR_DEBUGE			
//				//print
//				printf("Infrared infrared reception NEC_S2 failure!\r\n");
//			#endif
		}
	}
	else
	{
		infrared_framereceived = FAIL;
//			#ifdef 	IR_DEBUGE			
//				//print
//				printf("Infrared infrared reception S1 failure!\r\n");
//			#endif
	}
}


/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
