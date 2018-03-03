/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧
Description : LED���ƽӿ�
-----------------------------------------------------------------------------*/
#ifndef BINGOBIN_APP_LIGHTBOX_H_
#define BINGOBIN_APP_LIGHTBOX_H_

/* include ANSI C .h file */
#include <stdint.h>
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "config.h"

/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
typedef enum
{
   LIGHTBOX1 = 1,    ///< Static IP configuration by manually.
   LIGHTBOX2           ///< Dynamic IP configruation from a DHCP sever
}lightbox_type;
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
Prototype  : void Ligthbox_Init
Parameters : (Nothing)
Return     : (Nothing)
Function   : ��ʼ��
-----------------------------------------------------------------------------*/
void Ligthbox_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Ligthbox_SetLevel(uint8_t level)
Parameters : level ->  �����ȼ�(1-5��)
Return     : (Nothing)
Function   : ���Ƶ����ȵȼ�  
                1->���25%pwm��2->���40%pwm,
		3->���55%pwm,4->���70%pwm,5->���100%pwm
-----------------------------------------------------------------------------*/
void Ligthbox_SetLevel(uint8_t lightbox_type,uint8_t level);

void Lightbox_OpenLight(uint8_t lightbox_type);

void Lightbox_CloseLight(uint8_t lightbox_type);

uint8_t Lightbox_GetState(uint8_t lightbox_type);
/*-----------------------------------------------------------------------------
Prototype      : uint8_t Lightbox_Getlevel(void)
Parameters     :(Nothing)
Return         : �����ȵȼ�(0-5)
Function       :�������ȵȼ�
-----------------------------------------------------------------------------*/
uint8_t Lightbox_GetLevel(uint8_t lightbox_type);

#endif /* BINGOBIN_APP_LIGHTBOX_H_ */

