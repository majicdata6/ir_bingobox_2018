/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : WXF
Description : 
-----------------------------------------------------------------------------*/

#ifndef BINGOBIN_APP_INFRARED_H_
#define BINGOBIN_APP_INFRARED_H_

/* include ANSI C .h file */
/* include Other Lib .h file */
/* include Project .h file */
#include "stm32f0xx_hal.h"

/* include Local .h file */
//#ifdef BG_INFRARED
#include "ir_nec_send.h"
#include "ir_rc5_send.h"
#include "ir_receiver.h"


/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/
#define IR_DEBUGE

#define IR_COMMAND_BYTE_LEN 5

#define IR_TYPE_NEC        1     //infrared type NEC
#define IR_TYPE_RC5        2     //infrared type RC5


#define IR_SET_OPEN        1     //infrared command SET_OPEN 
#define IR_SET_CLOSE       2     //infrared command SET_CLOSE
#define IR_SET_COOL        3     //infrared command SET_COOL 
#define IR_SET_HOT         4     //infrared command SET_HOT
#define IR_SET_TEMP_5      5     //infrared command SET_TEMP
#define IR_SET_TEMP_6      6     //infrared command SET_TEMP
#define IR_SET_TEMP_7      7     //infrared command SET_TEMP
#define IR_SET_TEMP_8      8     //infrared command SET_TEMP
#define IR_SET_TEMP_9      9     //infrared command SET_TEMP
#define IR_SET_TEMP_10     10    //infrared command SET_TEMP
#define IR_SET_TEMP_11     11    //infrared command SET_TEMP
#define IR_SET_TEMP_12     12    //infrared command SET_TEMP
#define IR_SET_TEMP_13     13    //infrared command SET_TEMP
#define IR_SET_TEMP_14     14    //infrared command SET_TEMP
#define IR_SET_TEMP_15     15    //infrared command SET_TEMP
#define IR_SET_TEMP_16     16    //infrared command SET_TEMP
#define IR_SET_TEMP_17     17    //infrared command SET_TEMP
#define IR_SET_TEMP_18     18    //infrared command SET_TEMP
#define IR_SET_TEMP_19     19    //infrared command SET_TEMP
#define IR_SET_TEMP_20     20    //infrared command SET_TEMP
#define IR_SET_TEMP_21     21    //infrared command SET_TEMP
#define IR_SET_TEMP_22     22    //infrared command SET_TEMP
#define IR_SET_TEMP_23     23    //infrared command SET_TEMP
#define IR_SET_TEMP_24     24    //infrared command SET_TEMP
#define IR_SET_TEMP_25     25    //infrared command SET_TEMP
#define IR_SET_TEMP_26     26    //infrared command SET_TEMP
#define IR_SET_TEMP_27     27    //infrared command SET_TEMP
#define IR_SET_TEMP_28     28    //infrared command SET_TEMP
#define IR_SET_TEMP_29     29    //infrared command SET_TEMP
#define IR_SET_TEMP_30     30    //infrared command SET_TEMP

//#endif
/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
typedef void (*P_IR_Timer_Back)(TIM_HandleTypeDef *htim);

typedef enum 
{ 
	NO = 0, 
	RC5_YES,
	NEC_YES,
	FAIL
}IRFrameStatus_e;

typedef struct
{
	uint8_t 	add1;
	uint8_t 	add2;
	uint8_t 	com1;
	uint8_t 	com2;
}IRNECCommand_t;

typedef union
{
	uint8_t command_array[4];
	IRNECCommand_t nec_command;
}IRTypeCommand_u;

typedef union
{
	uint8_t array[4];
	uint32_t command;
}IRCommand_u;

typedef struct
{
	uint8_t 	type;
	IRTypeCommand_u command;
}IRCommand_t;

//typedef struct
//{
//	uint8_t 	type;
//	uint32_t 	command;
//}IRCommand_t;

typedef struct
{
	IRCommand_t open;
	IRCommand_t close;
	IRCommand_t cool;
	IRCommand_t hot;
	IRCommand_t temp_set_5;
	IRCommand_t temp_set_6;
	IRCommand_t temp_set_7;
	IRCommand_t temp_set_8;
	IRCommand_t temp_set_9;
	IRCommand_t temp_set_10;
	IRCommand_t temp_set_11;
	IRCommand_t temp_set_12;
	IRCommand_t temp_set_13;
	IRCommand_t temp_set_14;
	IRCommand_t temp_set_15;
	IRCommand_t temp_set_16;
	IRCommand_t temp_set_17;
	IRCommand_t temp_set_18;
	IRCommand_t temp_set_19;
	IRCommand_t temp_set_20;
	IRCommand_t temp_set_21;
	IRCommand_t temp_set_22;
	IRCommand_t temp_set_23;
	IRCommand_t temp_set_24;
	IRCommand_t temp_set_25;
	IRCommand_t temp_set_26;
	IRCommand_t temp_set_27;
	IRCommand_t temp_set_28;
	IRCommand_t temp_set_29;
	IRCommand_t temp_set_30;
}IRAir_t;

extern IRFrameStatus_e rc5_framereceived;
extern IRFrameStatus_e nec_framereceived;  
extern IRFrameStatus_e infrared_framereceived;


extern TIM_HandleTypeDef *ir_send_time_pwm;
extern TIM_HandleTypeDef *ir_time_compare;


//extern P_IR_Timer_Back P_IR_Send_Timer_PWM_Back;
extern P_IR_Timer_Back P_IR_Timer_Compare_Back;
/*---------------------------------------------------------------------------*/
//	Global Constants
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Variables
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Function prototypes
/*---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Prototype      : Infrared_TIM_IRQHandler(TIM_HandleTypeDef *htim)
Parameters     : TIM_HandleTypeDef *htim
Return         : none
Implementation : Private timer callback function.
-----------------------------------------------------------------------------*/
void Infrared_TIM_IRQHandler(TIM_HandleTypeDef *htim);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t APP_Infrared_Send(uint8_t ir_command)
Parameters     : uint8_t ir_command
Return         : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Implementation : 
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Send(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype  : uint8_t APP_Infrared_Receive(uint8_t ir_command)
Parameters : uint8_t ir_command
Return     : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Function   : Infrared receive
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Receive(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t APP_Infrared_Download(uint8_t* p_ir_command,uint16_t len)
Parameters     : uint8_t* p_ir_command
Parameters     : uint16_t len
Return         : uint8_t 0成功，E2 设备未响应，E5 学习超时，E6 无此命令
Implementation : Infrared data import function.
-----------------------------------------------------------------------------*/
uint8_t APP_Infrared_Download(uint8_t* p_ir_command,uint16_t len);
	
/*-----------------------------------------------------------------------------
Prototype      : APP_Infrared_Export(uint8_t* p_ir_command)
Parameters     : uint8_t* p_ir_command   //Infrared Export memory address
Return         : uint16_t                //Infrared command byte len
Implementation : Infrared data export function.
-----------------------------------------------------------------------------*/
uint16_t APP_Infrared_Export(uint8_t* p_ir_command);

/*-----------------------------------------------------------------------------
Prototype      : void APP_Infrared_Init(void)
Parameters     : none
Return         : none
Implementation : Infrared data init function.
-----------------------------------------------------------------------------*/
void APP_Infrared_Init(void);
	
#endif /* End of BINGOBIN_APP_INFRARED_H_ */
