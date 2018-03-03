/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧ 
Description : ���ؽӿ�
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


//����������Ϣ
#define LOCK_SET_STATE	0
#define LOCK_SET_TIME	1
//�õ�������Ϣ
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
//���������Ϣ
typedef struct LockInfor
{
	uint8_t state;                  //����״̬
	uint16_t auto_lock_time;        //�Զ�����ʱ��
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
Function   : ��ʼ��������ص���Ϣ
-----------------------------------------------------------------------------*/
void Lock_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Lock_Control(uint8_t cmd, LockInfor_t *info)
Parameters : cmd - Ҫ����������ָ��
		LOCK_SET_STATE�����ÿ�����
		LOCK_SET_TIME �������Զ�����ʱ��(���ǳɹ�)
		LOCK_GET      ����ȡ����Ϣ
	     info - �ṹ��ָ�룬
		state: LOCK_UNLOCKED��LOCK_LOCKED 
			(�������state�ᱻ�޸�Ϊ0xff)
	        auto_lock_time���Զ�����ʱ�䣬��λ����
				(5-300s����������Χ��
                                �����state�ᱻ�޸�Ϊ0xff)
Return     : (Nothing)
Function   : ������������ȡ������Ϣ
-----------------------------------------------------------------------------*/
void Lock_Control(uint8_t cmd, LockInfor_t *info);

/*-----------------------------------------------------------------------------
Prototype  : void Lock_Check_Event(void)
Parameters : (Nothing)
Return     : (Nothing)
Function   : ��ѵ��ѯ����״̬��Ҫ��״̬����ִ�п���������
	ע������while(1)ѭ���е��ü���
-----------------------------------------------------------------------------*/
void Lock_CheckEvent(void);

#endif /* End of BINGOBIN_APP_LOCK_H_ */

