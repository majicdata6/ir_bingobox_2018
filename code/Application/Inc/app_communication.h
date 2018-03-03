/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ����
Description : ��������ͨ�����ݺͷ�����Ӧ����
-----------------------------------------------------------------------------*/

#ifndef BINGOBIN_APP_COMMUNICATION_H_
#define BINGOBIN_APP_COMMUNICATION_H_

/* include ANSI C .h file */
#include <stdint.h>
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "config.h"
/* COMMUNICATION test debug message printout enable */

#ifdef BG_UART_DEBUG
#define	_COMMUNICATION_DEBUG_
#endif

#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE			64
#endif

/************************/
/* Select COMMUNICATION_MODE */
/************************/
#define COMMUNICATION_MAIN_NOBLOCK    0
#define COMMUNICATION_MODE   COMMUNICATION_MAIN_NOBLOCK

/*---------------------------------------------------------------------------*/
//	Global Defines and Macros
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Types and Enums
/*---------------------------------------------------------------------------*/
typedef enum COMMUNICATION_RET
{
   COMMUCATION_OK = 0,
   COMMUCATION_MODIFY_IP,
   COMMUCATION_CHECK_ERR,       
   COMMUCATION_START_FLAG_ERR,  
   COMMUCATION_LOST_PACKET_ERR,
   COMMUCATION_UNKNOWN_COMMAND,
}Commucation_ret_e;
/*---------------------------------------------------------------------------*/
//	Global Constants
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Variables
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Global Function prototypes
/*---------------------------------------------------------------------------*/
void Communication_Error_Handle(uint8_t sn);
/*-----------------------------------------------------------------------------
Prototype      :int8_t Communication_Tcps(uint8_t sn,
                                          uint8_t* buf,
                                          uint16_t port);
Parameters     : sn��������SCOKET��TCP����UDPģʽ
		*buf�������紫�����ݵĴ�С�ֽ���
		port�Ƕ˿ڵ���Ŀ
Return         : ����С��1������1����ʾͨ�ųɹ���С��0��ر�scoket
Implementation : ��������ͨ��
-----------------------------------------------------------------------------*/
int8_t Communication_Tcps(uint8_t sn, uint8_t* buf, uint16_t port);

/*-----------------------------------------------------------------------------
Prototype      :int8_t Communication_DealData( uint8_t* buf, uint16_t size);
Parameters     :uint8_t* buf ���յ����ַ���
		uint16_t size���յ����ַ�������	   
Return         :���ݽ������ض�Ӧ��Ҫ���͵��ַ��� 
Implementation : �������յ��ַ���
-----------------------------------------------------------------------------*/
int8_t Communication_DealData( uint8_t* buf, uint16_t size);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Communication_BytesChecker(
			                           uint8_t* buf,
			                           uint16_t size );
Parameters     : uint8_t* buf���յ����ַ���
		 uint16_t size���յ����ַ�������
Return         : У��λ��ȡ�ã������ֻ����У��
Implementation : �õ�У��λ
-----------------------------------------------------------------------------*/
uint8_t Communication_BytesChecker(uint8_t* buf,uint16_t size );

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetDevInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �õ��豸��Ϣ���ַ���
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetDevInfo(void);//�õ��豸��Ϣ

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetHeartBeatInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �õ�������Ϣ
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetHeartBeatInfo(void);//�õ�����

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_OpenLight(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : ����
-----------------------------------------------------------------------------*/
uint8_t *Communication_OpenLight(uint8_t lightbox_type);//����

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLight(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �ص�
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLight(uint8_t lightbox_type);//�ص�

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetLedInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �õ��Ƶ���Ϣ
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLedInfo(uint8_t lightbox_type);//�õ��Ƶ���Ϣ

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLedClassInfo(uint8_t* buf);
Parameters     : uint8_t* buf���յ����ַ���
Return         : �����ַ������׵�ַ
Implementation : ����led�Ƶȼ���Ϣ
-----------------------------------------------------------------------------*/
uint8_t *Communication_SetLedClassInfo(uint8_t* buf);//�õ�led�Ƶȼ�

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetLockInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �õ�������Ϣ
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLockInfo(void);//�õ�������Ϣ

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_OpenLock(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : ����
-----------------------------------------------------------------------------*/
uint8_t *Communication_OpenLock(void);//����

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLock(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : ����
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLock(void);//����

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLockTime(uint8_t* buf);
Parameters     : uint8_t* buf���յ����ַ���
Return         : �����ַ������׵�ַ
Implementation : ���ù�����ʱ��
-----------------------------------------------------------------------------*/
uint8_t *Communication_SetLockTime(uint8_t* buf);//��������ʱ��
/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetElectricEnergyInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �õ�������Ϣ
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetElectricEnergyInfo(void);//�õ�����

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SmokeCallInfo(void);
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : ������
-----------------------------------------------------------------------------*/
uint8_t *Communication_SmokeCallInfo(void);//������

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_LockCallInfo(void)
Parameters     : ��
Return         : �����ַ������׵�ַ
Implementation : �����Զ��رշ���
-----------------------------------------------------------------------------*/
uint8_t *Communication_LockCallInfo(void);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_ChangIp(uint8_t* buf);
Parameters     : uint8_t* buf���յ����ַ���
Return         : �����ַ������׵�ַ
Implementation : �޸�ip
-----------------------------------------------------------------------------*/
uint8_t *Communication_ChangIp(uint8_t* buf);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CheckFail(uint8_t* buf);
Parameters     : uint8_t* buf���յ����ַ���
Return         : �����ݷǷ�ʱ���صĺ���
Implementation : �����ݷǷ�ʱִ�еĺ���
-----------------------------------------------------------------------------*/
uint8_t *Communication_IllegalData(uint8_t* buf);
uint8_t *Communication_LED_IllegalData(uint8_t lightbox_type,uint8_t* buf);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRPerform(uint8_t ir_command)
Parameters     : uint8_t ir_command    ����ִ��ָ��
Return         : �����ַ������׵�ַ
Implementation : ������ִ�У�   "HWZX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRPerform(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRLearn(uint8_t ir_command)
Parameters     : uint8_t ir_command ����ѧϰָ��
Return         : �����ַ������׵�ַ
Implementation : ������ѧϰ   "HWXX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRLearn(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRImportData(uint8_t* import_buf_addr)
Parameters     : uint8_t* data_buf_addr �����׵�ַ
Return         : �����ַ������׵�ַ
Implementation : �����뵼��  "HWDR"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRImportData(uint8_t* import_buf_addr);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRExportData(uint8_t* export_buf_addr)
Parameters     : uint8_t* export_buf_addr �����׵�ַ
Return         : �����ַ������׵�ַ
Implementation : �����뵼��  "HWDC"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRExportData(uint8_t* export_buf_addr);

#endif /* End of <PROJECT>_<PATH>_<FILE>_H_ */
