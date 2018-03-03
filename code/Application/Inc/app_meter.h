/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅 
Description : 电表接口
-----------------------------------------------------------------------------*/

#ifndef BINGOBIN_APP_METER_H_
#define BINGOBIN_APP_METER_H_

/* include ANSI C .h file */
#include <stdint.h>
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */

/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/
//为下面定义的每组需要操作的事件数量
#define METER_EVENT_EACH_GROUP_NUM	(1)
#define METER_SAME_EVENT_NUM		METER_EVENT_EACH_GROUP_NUM*1

//-----------读取电能表数据标识----------------DI3 DI2 DI1 DI0---
//当前组合有功总电能和费率电能
#define CUR_COMBI_ACT_TOTAL_ENERGY		0x00000000

/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
typedef struct MeterEnergy
{
	//总电能
	uint32_t total_energy;
}
MeterEnergy_t;

typedef struct MeterData
{
	//组合有功总电能
	MeterEnergy_t meterCombiActiveEnergy;
	//电能表存在标志 TRUE：存在；FALSE：不存在
	uint8_t meter_exist;
	//电能表地址,LSB
	uint8_t meter_addr[6];
}
Meter_t;

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
Prototype  : uint32_t Meter_GetCurrCombiActiveTotalEnergy(void)
Parameters : (Nothing)
Return     : 返回组合有功总电能，单位KWh
Function   : 得到组合有功总电能,最高位是符号位，(0:正数，1:负数)
-----------------------------------------------------------------------------*/
uint32_t Meter_GetCurrCombiActiveTotalEnergy(void);

#endif /* End of BINGOBIN_APP_METER485_H_ */

