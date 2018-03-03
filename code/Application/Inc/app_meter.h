/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧ 
Description : ���ӿ�
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
//Ϊ���涨���ÿ����Ҫ�������¼�����
#define METER_EVENT_EACH_GROUP_NUM	(1)
#define METER_SAME_EVENT_NUM		METER_EVENT_EACH_GROUP_NUM*1

//-----------��ȡ���ܱ����ݱ�ʶ----------------DI3 DI2 DI1 DI0---
//��ǰ����й��ܵ��ܺͷ��ʵ���
#define CUR_COMBI_ACT_TOTAL_ENERGY		0x00000000

/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
typedef struct MeterEnergy
{
	//�ܵ���
	uint32_t total_energy;
}
MeterEnergy_t;

typedef struct MeterData
{
	//����й��ܵ���
	MeterEnergy_t meterCombiActiveEnergy;
	//���ܱ���ڱ�־ TRUE�����ڣ�FALSE��������
	uint8_t meter_exist;
	//���ܱ��ַ,LSB
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
Return     : ��������й��ܵ��ܣ���λKWh
Function   : �õ�����й��ܵ���,���λ�Ƿ���λ��(0:������1:����)
-----------------------------------------------------------------------------*/
uint32_t Meter_GetCurrCombiActiveTotalEnergy(void);

#endif /* End of BINGOBIN_APP_METER485_H_ */

