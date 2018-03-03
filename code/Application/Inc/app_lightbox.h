/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅
Description : LED控制接口
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
Function   : 初始化
-----------------------------------------------------------------------------*/
void Ligthbox_Init(void);

/*-----------------------------------------------------------------------------
Prototype  : void Ligthbox_SetLevel(uint8_t level)
Parameters : level ->  灯亮等级(1-5级)
Return     : (Nothing)
Function   : 控制灯亮度等级  
                1->输出25%pwm，2->输出40%pwm,
		3->输出55%pwm,4->输出70%pwm,5->输出100%pwm
-----------------------------------------------------------------------------*/
void Ligthbox_SetLevel(uint8_t lightbox_type,uint8_t level);

void Lightbox_OpenLight(uint8_t lightbox_type);

void Lightbox_CloseLight(uint8_t lightbox_type);

uint8_t Lightbox_GetState(uint8_t lightbox_type);
/*-----------------------------------------------------------------------------
Prototype      : uint8_t Lightbox_Getlevel(void)
Parameters     :(Nothing)
Return         : 灯亮度等级(0-5)
Function       :返回亮度等级
-----------------------------------------------------------------------------*/
uint8_t Lightbox_GetLevel(uint8_t lightbox_type);

#endif /* BINGOBIN_APP_LIGHTBOX_H_ */

