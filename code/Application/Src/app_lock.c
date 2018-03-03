/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Types and Enums
/*---------------------------------------------------------------------------*/
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
#ifdef BG_DOOR_LOCK
//没有在此做初始化，因为后面要从flash中读取
static LockInfor_t s_lock_information;

#endif

/* Global */

#ifdef BG_DOOR_LOCK

uint8_t g_door_auto_lock_flag = 0; 

uint8_t g_door_auto_lock_send_flag = 0;

#endif

/*---------------------------------------------------------------------------*/
//	Extern
/*---------------------------------------------------------------------------*/
/* Variables */
#ifdef BG_DOOR_LOCK

extern uint32_t g_door_lock_auto_time;

#endif
/* Function prototypes */

/*---------------------------------------------------------------------------*/
//	Static Functions
/*---------------------------------------------------------------------------*/
#if 1

/*-----------------------------------------------------------------------------
Prototype      : static void Lock_Locked(void);
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : 根据不同的宏，选择不同的关锁方法
-----------------------------------------------------------------------------*/
#ifdef BG_DOOR_LOCK
static void Lock_Locked(void)
{
	HAL_GPIO_WritePin(DOOR_GPIO_PORT, DOOR_PIN, GPIO_PIN_SET); 
}
#endif

/*-----------------------------------------------------------------------------
Prototype      : static void Lock_Unlocked(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation :开锁
-----------------------------------------------------------------------------*/
#ifdef BG_DOOR_LOCK
static void Lock_Unlocked(void)
{
	HAL_GPIO_WritePin(DOOR_GPIO_PORT, DOOR_PIN, GPIO_PIN_RESET); 
}
#endif

/*-----------------------------------------------------------------------------
Prototype      : static void Lock_StatePinInit(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation :根据原理图初始化门锁状态引脚为输入
-----------------------------------------------------------------------------*/
#ifndef DOOR_STATE_ADC
static void Lock_StatePinInit(void)
{
        GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO_STATE_DOOR clock */
	
        DOOR_STATE_GPIO_CLK_ENABLE(); 

	/* Configure the GPIO_DOOR pin */
	GPIO_InitStruct.Pin = DOOR_STATE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(DOOR_STATE_GPIO_PORT, &GPIO_InitStruct);
}

/*-----------------------------------------------------------------------------
Prototype      : static GPIO_PinState Lock_GetStatePinValue(void)
Parameters     : (Nothing)
Return         : GPIO_PinState引脚电平状态
                        GPIO_PIN_RESET:引脚电平低
                        GPIO_PIN_SET:引脚电平高
Implementation :获取门锁状态引脚的电平状态
-----------------------------------------------------------------------------*/
static GPIO_PinState Lock_GetStatePinValue(void)
{
        return HAL_GPIO_ReadPin(DOOR_STATE_GPIO_PORT, DOOR_STATE_PIN);
}

#endif // end of DOOR_STATE_ADC

/*-----------------------------------------------------------------------------
Prototype      : static uint8_t Lock_GetLockState(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : 根据不同的宏实现不同的锁控方式，根据ADC的值得到锁的状态
-----------------------------------------------------------------------------*/
#if defined(BG_DOOR_LOCK)
static int8_t Lock_GetLockState(void)
{
        #ifdef DOOR_STATE_ADC
        int32_t adc_value = ADC_GetValue();
	if( adc_value>= 890)
	{
		return LOCK_LOCKED;
	}
	else if(adc_value>= 0)
	{
		return LOCK_UNLOCKED;
	}
        else
        {
                return LOCK_ERROR;
        }
        #else
        GPIO_PinState state = Lock_GetStatePinValue();
        if(GPIO_PIN_RESET == state)
        {
                return LOCK_UNLOCKED;
        }
        else
        {
                return LOCK_LOCKED;
        }
        #endif
}



#endif

#endif /* End of 1, Just a tag, ignore it */

/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/
	 
/*-----------------------------------------------------------------------------
Prototype      : void Lock_Init(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : 初始化控制脚，初始化ADC以及一些自用参数
-----------------------------------------------------------------------------*/	 
void Lock_Init(void)
{
#ifdef BG_DOOR_LOCK

	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO_DOOR clock */
	DOOR_GPIO_CLK_ENABLE();  

	/* Configure the GPIO_DOOR pin */
	GPIO_InitStruct.Pin = DOOR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(DOOR_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(DOOR_GPIO_PORT, DOOR_PIN, GPIO_PIN_RESET); 

        #ifdef DOOR_STATE_ADC
	ADC_Init();
	#else
        Lock_StatePinInit();
        #endif
	//读取flash中存储的自动关锁时间
	uint16_t auto_time;
	Info_ReadTimer(&auto_time);
	s_lock_information.auto_lock_time = auto_time;
	printf("RLock_time: %d\r\n", s_lock_information.auto_lock_time);
	//如果读出来的数据有问题，则默认为30s
	if((s_lock_information.auto_lock_time > 300) ||
	   (s_lock_information.auto_lock_time < 5))
	{
		s_lock_information.auto_lock_time = 30;
	}
        //上电自动锁门
	s_lock_information.state = LOCK_LOCKED;
#endif
}

/*-----------------------------------------------------------------------------
Prototype      : void Lock_Init(void)
Parameters     : cmd - 要对锁操作的指令
			LOCK_SET_STATE：设置开关锁
			LOCK_SET_TIME ：设置自动关锁时间(总是成功)
			LOCK_GET      ：获取锁信息
	     	info - 结构体指针，
			state: LOCK_UNLOCKED和LOCK_LOCKED 
				(如果出错，state会被修改为0xff)
			auto_lock_time：自动关锁时间，单位：秒
				(5-300s，若超出范围，
                                则出错，state会被修改为0xff)
Return         : (Nothing)
Implementation : 操作门锁，读取门锁信息或者设置门锁状态，
                设置超时时间等，此函数只改变内存中的值，
                不会产生实际的动作，动作函数在其他函数实现
-----------------------------------------------------------------------------*/
void Lock_Control(uint8_t cmd, LockInfor_t *info)
{
#ifdef BG_DOOR_LOCK
	if(!info){return;}
	
        switch(cmd)
	{
	case LOCK_SET_STATE:
		if(info->state != LOCK_UNLOCKED && info->state != LOCK_LOCKED)
		{
			info->state = LOCK_ERROR;
			return;
		}
		s_lock_information.state = Lock_GetLockState();
                if(LOCK_ERROR == s_lock_information.state)
                {
                        return;
                }
		if(info->state != s_lock_information.state)
		{
			s_lock_information.state = info->state;
		}
		break;
	case LOCK_SET_TIME:
		if((info->auto_lock_time >= 5) &&
			(info->auto_lock_time <= 300))
		{
			s_lock_information.auto_lock_time =
                                info->auto_lock_time;
		}
		break;
	case LOCK_GET:
		s_lock_information.state = Lock_GetLockState();
		info->auto_lock_time = s_lock_information.auto_lock_time;
		info->state = s_lock_information.state;
		break;
	default:
		info->state = 0xFF;
		break;
	}
#endif
}

/*-----------------------------------------------------------------------------
Prototype      : void Lock_Check_Event(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : 实现门锁的开锁、关锁和到时自动关锁，和定时器结合，实现自动关锁，
		在主函数实时轮训，查询门锁状态和设置状态，进行动作
-----------------------------------------------------------------------------*/
void Lock_CheckEvent(void)
{
#ifdef BG_DOOR_LOCK

	if(s_lock_information.state != Lock_GetLockState())     
	{
		if(LOCK_UNLOCKED == s_lock_information.state)
		{
			Lock_Unlocked();
			g_door_lock_auto_time = 
				s_lock_information.auto_lock_time * 1000;
		}
		else if(LOCK_LOCKED == s_lock_information.state)
		{
			Lock_Locked();
			g_door_lock_auto_time = 0;
		}
                else
                {
                        //锁状态错误  ADC出问题
                }
	}
	if(1 == g_door_auto_lock_flag)
	{
		g_door_auto_lock_flag = 0 ;
		Lock_Locked();
		s_lock_information.state = LOCK_LOCKED;
                g_door_auto_lock_send_flag = 1;

	}
#endif
}

