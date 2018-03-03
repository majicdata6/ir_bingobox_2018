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
//û���ڴ�����ʼ������Ϊ����Ҫ��flash�ж�ȡ
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
Implementation : ���ݲ�ͬ�ĺ꣬ѡ��ͬ�Ĺ�������
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
Implementation :����
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
Implementation :����ԭ��ͼ��ʼ������״̬����Ϊ����
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
Return         : GPIO_PinState���ŵ�ƽ״̬
                        GPIO_PIN_RESET:���ŵ�ƽ��
                        GPIO_PIN_SET:���ŵ�ƽ��
Implementation :��ȡ����״̬���ŵĵ�ƽ״̬
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
Implementation : ���ݲ�ͬ�ĺ�ʵ�ֲ�ͬ�����ط�ʽ������ADC��ֵ�õ�����״̬
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
Implementation : ��ʼ�����ƽţ���ʼ��ADC�Լ�һЩ���ò���
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
	//��ȡflash�д洢���Զ�����ʱ��
	uint16_t auto_time;
	Info_ReadTimer(&auto_time);
	s_lock_information.auto_lock_time = auto_time;
	printf("RLock_time: %d\r\n", s_lock_information.auto_lock_time);
	//��������������������⣬��Ĭ��Ϊ30s
	if((s_lock_information.auto_lock_time > 300) ||
	   (s_lock_information.auto_lock_time < 5))
	{
		s_lock_information.auto_lock_time = 30;
	}
        //�ϵ��Զ�����
	s_lock_information.state = LOCK_LOCKED;
#endif
}

/*-----------------------------------------------------------------------------
Prototype      : void Lock_Init(void)
Parameters     : cmd - Ҫ����������ָ��
			LOCK_SET_STATE�����ÿ�����
			LOCK_SET_TIME �������Զ�����ʱ��(���ǳɹ�)
			LOCK_GET      ����ȡ����Ϣ
	     	info - �ṹ��ָ�룬
			state: LOCK_UNLOCKED��LOCK_LOCKED 
				(�������state�ᱻ�޸�Ϊ0xff)
			auto_lock_time���Զ�����ʱ�䣬��λ����
				(5-300s����������Χ��
                                �����state�ᱻ�޸�Ϊ0xff)
Return         : (Nothing)
Implementation : ������������ȡ������Ϣ������������״̬��
                ���ó�ʱʱ��ȣ��˺���ֻ�ı��ڴ��е�ֵ��
                �������ʵ�ʵĶ�����������������������ʵ��
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
Implementation : ʵ�������Ŀ����������͵�ʱ�Զ��������Ͷ�ʱ����ϣ�ʵ���Զ�������
		��������ʵʱ��ѵ����ѯ����״̬������״̬�����ж���
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
                        //��״̬����  ADC������
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

