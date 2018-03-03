/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : WXF
Description : 
-----------------------------------------------------------------------------*/

/* include ANSI C .h file */
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "app_infrared.h"
#include "main.h"

/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/
//红外码地址暂定
#define IR_COMMAND_ADDRESS_START    ((uint32_t)0x0800F000)       /*Infrared control instruction address.*/
#define IR_COMMAND_ADDRESS_END      ((uint32_t)0x0800F400)       /*Infrared control instruction address.*/
#define FLASH_ONE_PAGES_SIZE        FLASH_PAGE_SIZE              /*FLASH_ONE_PAGES_SIZE.*/

#define IR_EXE_OK                   0x00        //0成功，E2 设备未响应，E5 超时，E6 无此命令
#define IR_EXE_NO_RESPONSE          0xE2        //0成功，E2 设备未响应，E5 超时，E6 无此命令
#define IR_EXE_BUSY                 0xE3        //0成功，E2 设备未响应，E5 超时，E6 无此命令
#define IR_EXE_DATA_ERR             0xE4        //0成功，E2 设备未响应，E5 超时，E6 无此命令
#define IR_EXE_TIME_OUT             0xE5        //0成功，E2 设备未响应，E5 超时，E6 无此命令
#define IR_EXE_NOT_COMMAND          0xE6        //0成功，E2 设备未响应，E5 超时，E6 无此命令

/*---------------------------------------------------------------------------*/
//	Types and Enums
/*---------------------------------------------------------------------------*/
IRFrameStatus_e rc5_framereceived = NO;        /* Will be used by the user to check if an RC5 has been received or not */
IRFrameStatus_e nec_framereceived = NO;        /* Will be used by the user to check if an NEC has been received or not */
IRFrameStatus_e infrared_framereceived = NO;   /* Will be used by the user to check if an infrared has been received or not */

//TIM_HandleTypeDef *ir_send_time_compare = NULL;
//TIM_HandleTypeDef *ir_receive_time_compare = NULL;
TIM_HandleTypeDef *ir_send_time_pwm = NULL;
TIM_HandleTypeDef *ir_time_compare = NULL;

//P_IR_Timer_Back P_IR_Send_Timer_Compare_Back = NULL;
//P_IR_Timer_Back P_IR_Send_Timer_PWM_Back = NULL;
//P_IR_Timer_Back P_IR_Receive_Timer_Compare_Back = NULL;
P_IR_Timer_Back P_IR_Timer_Compare_Back = NULL;
IRAir_t ir_air_command;
IRCommand_u ir_command_read;

/*---------------------------------------------------------------------------*/
//	Constants
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Variables
/*---------------------------------------------------------------------------*/
/* Private/Static */
/*
 * Function   : 
 * Value Area : 
 */

/* Global */


/*---------------------------------------------------------------------------*/
//	Extern
/*---------------------------------------------------------------------------*/
/* Variables */

/* Function prototypes */

static  void Infrared_Flash_Pages_Erase(uint32_t Infrared_Flash_start, uint32_t Infrared_Flash_end);
static void Infrared_Flash_Write_Byte(uint32_t store_addr , uint8_t *p_data , uint16_t byte_num);
static void Infrared_Command_Store(void);
static void Load_Infrared_Command(void);
static uint32_t *Infrared_Command_Parsing(uint8_t ir_command);

/*---------------------------------------------------------------------------*/
//	Static Functions
/*---------------------------------------------------------------------------*/
#if 1

/*-----------------------------------------------------------------------------
Prototype      : static  void Infrared_Flash_Pages_Erase(uint32_t Infrared_Flash_start, uint32_t Infrared_Flash_end)
Parameters     : uint32_t Infrared_Flash_start
Parameters     : uint32_t Infrared_Flash_end
Return         : none
Implementation : 
-----------------------------------------------------------------------------*/
static void Infrared_Flash_Pages_Erase(uint32_t Infrared_Flash_start, uint32_t Infrared_Flash_end)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t page_err = 0;
	
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = Infrared_Flash_start;
	EraseInitStruct.NbPages = (Infrared_Flash_end - Infrared_Flash_start)/FLASH_ONE_PAGES_SIZE;
	
	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&EraseInitStruct, &page_err) != HAL_OK)
	{
		//
	}
	HAL_FLASH_Lock();
}

/*-----------------------------------------------------------------------------
Prototype      : static void Infrared_Flash_Write_Byte(uint32_t store_addr , uint8_t *p_data , uint16_t byte_num)
Parameters     : uint32_t store_addr	//Store address
Parameters     : uint8_t *p_data        //Data address
Parameters     : uuint16_t Byte_Num     //The number of bytes must be even.If it's not an even number, the last byte fills 1.
Return         : none
Implementation : 
-----------------------------------------------------------------------------*/
static void Infrared_Flash_Write_Byte(uint32_t store_addr , uint8_t *p_data , uint16_t byte_num)
{
	
#ifdef BG_INFRARED
	uint32_t page_err = 0;
	uint32_t half_word = 0;
	uint16_t half_word_num = 0;
	half_word_num = byte_num/2;
	
	HAL_FLASH_Unlock();
	while(half_word_num --)
	{
		half_word  = *(p_data++);
		half_word |= *(p_data++)<<8;
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, store_addr, half_word) != HAL_OK)
		{
			//
		}
		store_addr += 2;  
	}
	if(byte_num%2)
	{
		half_word  = *(p_data++);
		half_word |= 0xff;
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, store_addr, half_word) != HAL_OK)
		{
			//
		}
	}
	HAL_FLASH_Lock();
#endif
	
}
/*-----------------------------------------------------------------------------
Prototype      : static void Load_Infrared_Command(void)
Parameters     : none
Return         : none
Implementation : 
-----------------------------------------------------------------------------*/
static void Load_Infrared_Command(void)
{
	
#ifdef BG_INFRARED
	uint8_t *p_ir_air_command_start = (uint8_t *)IR_COMMAND_ADDRESS_START;
	uint8_t *p_ir_air_command_load = (uint8_t *)&ir_air_command;
	
	for(uint16_t i=0; i<sizeof(IRAir_t)/sizeof(IRCommand_t); i++)
	{
		if((p_ir_air_command_start[i*IR_COMMAND_BYTE_LEN] == IR_TYPE_NEC) || (p_ir_air_command_start[i*IR_COMMAND_BYTE_LEN] == IR_TYPE_RC5))
		{
			for(uint16_t j=0; j<IR_COMMAND_BYTE_LEN; j++)
			{
				p_ir_air_command_load[i*IR_COMMAND_BYTE_LEN + j] = p_ir_air_command_start[i*IR_COMMAND_BYTE_LEN + j];
			}
		}
		else
		{
		#ifdef 	IR_DEBUGE			
			//print
			printf("p_ir_air_command_load[i] command_num=%d,fail!\r\n",i);
		#endif
		}
	}
#endif
	
}

/*-----------------------------------------------------------------------------
Prototype      : static uint32_t *Infrared_Command_Parsing(uint8_t ir_command);
Parameters     : uint8_t ir_command
Return         : int32_t *
Implementation : 解析红外指令，返回发送指令地址
-----------------------------------------------------------------------------*/
static uint32_t *Infrared_Command_Parsing(uint8_t ir_command)
{
	uint32_t *infrared_address = NULL;
	
#ifdef BG_INFRARED
	switch(ir_command)
	{
		case IR_SET_OPEN:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.open;
		}
		break;
		
		case IR_SET_CLOSE:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.close;
		}
		break;
		
		case IR_SET_COOL:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.cool;
		}
		break;
		
		case IR_SET_HOT:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.hot;
		}
		break;
		
		case IR_SET_TEMP_5:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_5;
		}
		break;
		case IR_SET_TEMP_6:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_6;
		}
		break;
		case IR_SET_TEMP_7:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_7;
		}
		break;
		case IR_SET_TEMP_8:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_8;
		}
		break;
		case IR_SET_TEMP_9:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_9;
		}
		break;
		case IR_SET_TEMP_10:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_10;
		}
		break;
		case IR_SET_TEMP_11:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_11;
		}
		break;
		case IR_SET_TEMP_12:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_12;
		}
		break;
		case IR_SET_TEMP_13:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_13;
		}
		break;
		case IR_SET_TEMP_14:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_14;
		}
		break;
		case IR_SET_TEMP_15:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_15;
		}
		break;
		
		case IR_SET_TEMP_16:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_16;
		}
		break;
		
		case IR_SET_TEMP_17:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_17;
		}
		break;
		
		case IR_SET_TEMP_18:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_18;
		}
		break;
		
		case IR_SET_TEMP_19:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_19;
		}
		break;
		
		case IR_SET_TEMP_20:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_20;
		}
		break;
		
		case IR_SET_TEMP_21:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_21;
		}
		break;
		
		case IR_SET_TEMP_22:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_22;
		}
		break;
		
		case IR_SET_TEMP_23:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_23;
		}
		break;
		
		case IR_SET_TEMP_24:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_24;
		}
		break;
		
		case IR_SET_TEMP_25:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_25;
		}
		break;
		
		case IR_SET_TEMP_26:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_26;
		}
		break;
		
		case IR_SET_TEMP_27:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_27;
		}
		break;
		
		case IR_SET_TEMP_28:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_28;
		}
		break;
		
		case IR_SET_TEMP_29:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_29;
		}
		break;
		
		case IR_SET_TEMP_30:
		{
			//user code  infrared_address = infrared_command_address
			infrared_address = (uint32_t *)&ir_air_command.temp_set_30;
		}
		break;
		
		default :
		{
			infrared_address = NULL;
		}
		break;
	}
#endif
		
	return(infrared_address);
}

/*-----------------------------------------------------------------------------
Prototype      : static void Infrared_Command_Store(void)
Parameters     : none
Return         : none
Implementation : Infrared instruction storage
-----------------------------------------------------------------------------*/
static void Infrared_Command_Store(void)
{

#ifdef BG_INFRARED
	Infrared_Flash_Pages_Erase(IR_COMMAND_ADDRESS_START, IR_COMMAND_ADDRESS_END);
	Infrared_Flash_Write_Byte(IR_COMMAND_ADDRESS_START, (uint8_t*)&ir_air_command, sizeof(IRAir_t)/sizeof(IRCommand_t)*IR_COMMAND_BYTE_LEN);
#endif
	
}

#endif /* End of 1, Just a tag, ignore it */


/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Prototype      : Infrared_TIM_IRQHandler(TIM_HandleTypeDef *htim)
Parameters     : TIM_HandleTypeDef *htim
Return         : none
Implementation : Private timer callback function.
-----------------------------------------------------------------------------*/
void Infrared_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
#ifdef BG_INFRARED
	/* Capture compare 1 event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC1) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC1) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC1);
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_1;

			/* Input capture event */
			if((htim->Instance->CCMR1 & TIM_CCMR1_CC1S) != 0x00U)
			{
				HAL_TIM_IC_CaptureCallback(htim);
			}
			/* Output compare event */
			else
			{
//				HAL_TIM_OC_DelayElapsedCallback(htim);
				(*P_IR_Timer_Compare_Back)(htim);
				HAL_TIM_PWM_PulseFinishedCallback(htim);
			}
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
		}
	}
	/* Capture compare 2 event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC2) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC2) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC2);
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_2;
			/* Input capture event */
			if((htim->Instance->CCMR1 & TIM_CCMR1_CC2S) != 0x00U)
			{
				HAL_TIM_IC_CaptureCallback(htim);
			}
			/* Output compare event */
			else
			{
//				HAL_TIM_OC_DelayElapsedCallback(htim);
				(*P_IR_Timer_Compare_Back)(htim);
				HAL_TIM_PWM_PulseFinishedCallback(htim);
			}
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
		}
	}
	/* Capture compare 3 event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC3) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC3) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC3);
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_3;
			/* Input capture event */
			if((htim->Instance->CCMR2 & TIM_CCMR2_CC3S) != 0x00U)
			{
				HAL_TIM_IC_CaptureCallback(htim);
			}
			/* Output compare event */
			else
			{
//				HAL_TIM_OC_DelayElapsedCallback(htim);
				(*P_IR_Timer_Compare_Back)(htim);
				HAL_TIM_PWM_PulseFinishedCallback(htim);
			}
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
		}
	}
	/* Capture compare 4 event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC4) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_CC4) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC4);
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_4;
			/* Input capture event */
			if((htim->Instance->CCMR2 & TIM_CCMR2_CC4S) != 0x00U)
			{
				HAL_TIM_IC_CaptureCallback(htim);
			}
			/* Output compare event */
			else
			{
//				HAL_TIM_OC_DelayElapsedCallback(htim);
				(*P_IR_Timer_Compare_Back)(htim);
				HAL_TIM_PWM_PulseFinishedCallback(htim);
			}
			htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
		}
	}
	/* TIM Update event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
//			HAL_TIM_PeriodElapsedCallback(htim);
			(*P_IR_Timer_Compare_Back)(htim);
		}
	}
	/* TIM Break input event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_BREAK) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_BREAK);
			HAL_TIMEx_BreakCallback(htim);
		}
	}
	/* TIM Trigger detection event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_TRIGGER) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_TRIGGER) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_TRIGGER);
			HAL_TIM_TriggerCallback(htim);
		}
	}
	/* TIM commutation event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_COM) != RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_COM) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_FLAG_COM);
			HAL_TIMEx_CommutationCallback(htim);
		}
	}
#endif
	
}


/*-----------------------------------------------------------------------------
Prototype      : uint8_t APP_Infrared_Send(uint8_t ir_command)
Parameters     : uint8_t ir_command
Return         : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Implementation : 
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Send(uint8_t ir_command)
{
	uint8_t return_flag = IR_EXE_NO_RESPONSE;
#ifdef BG_INFRARED
	IRCommand_t *p_ir_air_command = NULL;
	p_ir_air_command = (IRCommand_t *)Infrared_Command_Parsing(ir_command);
	if(p_ir_air_command == NULL)
	{
		return(IR_EXE_NOT_COMMAND);
	}
	
	switch(p_ir_air_command->type)
	{
		case IR_TYPE_NEC:
		{
			ir_send_time_pwm = &nec_38kPWM_tim;
			ir_time_compare = &nec_compare_tim;
			P_IR_Timer_Compare_Back = NEC_Send_Time_Out_Back;
			
			NEC_Send_Init();
			for(uint8_t i=0; i<4; i++)
			{
				p_ir_air_command->command.command_array[i] = ir_command_read.array[i];
			}
			NEC_Data_Decod(ir_command_read.command);
			return_flag = IR_EXE_OK;
		}
		break;
		
		case IR_TYPE_RC5:
		{
			ir_send_time_pwm = &rc5_36kPWM_tim;
			ir_time_compare = &rc5_compare_tim;
			P_IR_Timer_Compare_Back = RC5_Send_Time_Out_Back;
			
			RC5_Send_Init();
			for(uint8_t i=0; i<4; i++)
			{
				p_ir_air_command->command.command_array[i] = ir_command_read.array[i];
			}
			RC5_Data_Decod(ir_command_read.command);
			return_flag = IR_EXE_OK;
		}
		break;
		
		default : break;
	}
#endif
	return(return_flag);
}


/*-----------------------------------------------------------------------------
Prototype      : uint8_t APP_Infrared_Receive(uint8_t ir_command)
Parameters     : uint8_t ir_command
Return         : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Implementation : 
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Receive(uint8_t ir_command)
{
	uint8_t return_flag = IR_EXE_NO_RESPONSE;
#ifdef BG_INFRARED
	uint16_t tim_count = 0;
	IRCommand_t *p_ir_air_command = NULL;
	
	
//	rc5_framereceived = NO;
//	nec_framereceived = NO;
	infrared_framereceived = NO;
	p_ir_air_command = (IRCommand_t *)Infrared_Command_Parsing(ir_command);
	if(p_ir_air_command==NULL)
	{
		return(IR_EXE_NOT_COMMAND);
	}
	
	//start Infrared receive
	ir_time_compare = &receive_tim;
	P_IR_Timer_Compare_Back = IR_Receiver_Time_Out_Back;
	
	//	extern uint16_t s_receive_data[];
	//	extern uint16_t s_receive_data_len; 
	for(uint16_t i=0; i<100; i++)
	{
		s_receive_data[i++] = 0;
	}
	s_receive_data_len = 0;
	
	IR_Receiver_Init();
	IR_Receiver_EXTI_Start();

	HAL_Delay(1000);
	while(1)
	{
		tim_count ++;
		if(NEC_YES == infrared_framereceived)
		{
			p_ir_air_command->type = IR_TYPE_NEC;
			ir_command_read.command = (uint32_t)NEC_IR_Receiver_Turn();
			for(uint8_t i=0; i<4; i++)
			{
				p_ir_air_command->command.command_array[i] = ir_command_read.array[i];
			}
			infrared_framereceived = NO;
			
			Infrared_Command_Store();
			return_flag = IR_EXE_OK;
			
		#ifdef 	IR_DEBUGE			
			//print
			printf("Infrared PC_command=%d, ir_command=%lx, sizeof=%d reception NEC_YES!\r\n",ir_command,ir_command_read.command,sizeof(IRCommand_t));
		#endif
			break;
		}
		else if(RC5_YES == infrared_framereceived)
		{
			p_ir_air_command->type = IR_TYPE_RC5;
			ir_command_read.command = (uint32_t)RC5_IR_Receiver_Turn();
			for(uint8_t i=0; i<4; i++)
			{
				p_ir_air_command->command.command_array[i] = ir_command_read.array[i];
			}
			infrared_framereceived = NO;
			
			Infrared_Command_Store();
			return_flag = IR_EXE_OK;
			
		#ifdef 	IR_DEBUGE			
			//print
			printf("Infrared PC_command=%d, ir_command=%lx reception RC5_YES!\r\n",ir_command,ir_command_read.command );
		#endif
			break;
		}
		else if(FAIL == infrared_framereceived)
		{
			return_flag = IR_EXE_NO_RESPONSE;
		#ifdef 	IR_DEBUGE			
			//print
			printf("Infrared ir_command=%d reception FAIL!\r\n",ir_command);
		#endif
			break;
		}

		if(tim_count >= 50)
		{	
			return_flag = IR_EXE_TIME_OUT;
			IR_Receiver_EXTI_Stop();
			NEC_Timer_Start();
//			HAL_TIM_Base_Stop_IT(&receive_tim);
			
//		#ifdef 	IR_DEBUGE			
//			//print
//			printf("Infrared ir_command=%d timer out failure!\r\n",ir_command);
//			for(uint16_t i = 0; i<100; i++)
//			printf("fail ir_bit_data%d=%ld !\r\n",i,s_receive_data[i]);
//		#endif
			break;
		}
		
		HAL_Delay(100);
	}
#endif
	return(return_flag);
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t APP_Infrared_Download(uint8_t* p_ir_command,uint16_t len)
Parameters     : uint8_t* p_ir_command
Parameters     : uint16_t len
Return         : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Implementation : Infrared data import function.
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Download(uint8_t* p_ir_command,uint16_t len)
{
	uint8_t return_flag = IR_EXE_NO_RESPONSE;
#ifdef BG_INFRARED
	uint8_t* p_ir_command_download = (uint8_t*)&ir_air_command;
	for(uint16_t i=0; i<len; i++)
	{
		p_ir_command_download[i] = p_ir_command[i];
	}
	//store function
	Infrared_Command_Store();
	return_flag = IR_EXE_OK;
#endif
	return(return_flag);
}

/*-----------------------------------------------------------------------------
Prototype      : APP_Infrared_Export(uint8_t* p_ir_command)
Parameters     : uint8_t* p_ir_command   //Infrared Export memory address
Return         : uint16_t                //Infrared command byte len
Implementation : Infrared data export function.
-----------------------------------------------------------------------------*/
uint16_t APP_Infrared_Export(uint8_t* p_ir_command)
{	
	
#ifdef BG_INFRARED
	uint8_t* p_ir_command_Export = (uint8_t*)&ir_air_command;
	for(uint16_t i=0; i<sizeof(ir_air_command); i++)
	{
		p_ir_command[i] = p_ir_command_Export[i];
	}
#endif
	return (sizeof(ir_air_command));

}

/*-----------------------------------------------------------------------------
Prototype      : void APP_Infrared_Init(void)
Parameters     : none
Return         : none
Implementation : Infrared data init function.
-----------------------------------------------------------------------------*/
void APP_Infrared_Init(void)
{
#ifdef BG_INFRARED
	//load Infrared command from flash address ..
	Load_Infrared_Command();

#endif
	
}
