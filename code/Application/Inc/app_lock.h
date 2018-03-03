/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅 
Description : 锁控接口
-----------------------------------------------------------------------------*/

#ifndef BINGOBIN_APP_LOCK_H_
#define BINGOBIN_APP_LOCK_H_

/* include ANSI C .h file */
#include "main.h"

/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/
//Lock state
#define LOCK_UNLOCKED	1
#define LOCK_LOCKED	0
#define LOCK_ERROR	0xFF


//设置门锁信息
#define LOCK_SET_STATE	0
#define LOCK_SET_TIME	1
//得到门锁信息
#define LOCK_GET	2
/** @defgroup DOOR_GPIO
  * @{
  */
#define DOOR_PIN                         GPIO_PIN_0
#define DOOR_GPIO_PORT                   GPIOB
#define DOOR_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define DOOR_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

#ifndef DOOR_STATE_ADC

#define DOOR_STATE_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define DOOR_STATE_PIN                  GPIO_PIN_0
#define DOOR_STATE_GPIO_PORT            GPIOA
#endif
/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
//锁控相关信息
typedef struct LockInfor
{
	uint8_t state;                  //锁的状态
	uint16_t auto_lock_time;        //自动关锁时间
}
LockInfor_t;

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
Prototype  : void Lock_Init(void);
Parameters : (Nothing)
Return     : (Nothing)
Function   : 初始化和锁相关的信息
-----------------------------------------------------------------------------*/
void Lock_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Lock_Control(uint8_t cmd, LockInfor_t *info)
Parameters : cmd - 要对锁操作的指令
		LOCK_SET_STATE：设置开关锁
		LOCK_SET_TIME ：设置自动关锁时间(总是成功)
		LOCK_GET      ：获取锁信息
	     info - 结构体指针，
		state: LOCK_UNLOCKED和LOCK_LOCKED 
			(如果出错，state会被修改为0xff)
	        auto_lock_time：自动关锁时间，单位：秒
				(5-300s，若超出范围，
                                则出错，state会被修改为0xff)
Return     : (Nothing)
Function   : 控制门锁、获取门锁信息
-----------------------------------------------------------------------------*/
void Lock_Control(uint8_t cmd, LockInfor_t *info);

/*-----------------------------------------------------------------------------
Prototype  : void Lock_Check_Event(void)
Parameters : (Nothing)
Return     : (Nothing)
Function   : 轮训查询锁的状态和要求状态，并执行开关锁操作
	注：放在while(1)循环中调用即可
-----------------------------------------------------------------------------*/
void Lock_CheckEvent(void);

#endif /* End of BINGOBIN_APP_LOCK_H_ */

