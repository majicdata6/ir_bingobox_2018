/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 廖密
Description : 解析网络通信数据和返回相应数据
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
Parameters     : sn用来设置SCOKET是TCP还是UDP模式
		*buf设置网络传输数据的大小字节数
		port是端口的数目
Return         : 返回小于1或者是1，表示通信成果，小于0则关闭scoket
Implementation : 用来网络通信
-----------------------------------------------------------------------------*/
int8_t Communication_Tcps(uint8_t sn, uint8_t* buf, uint16_t port);

/*-----------------------------------------------------------------------------
Prototype      :int8_t Communication_DealData( uint8_t* buf, uint16_t size);
Parameters     :uint8_t* buf 接收到的字符串
		uint16_t size接收到的字符串长度	   
Return         :根据解析返回对应的要发送的字符串 
Implementation : 解析接收到字符串
-----------------------------------------------------------------------------*/
int8_t Communication_DealData( uint8_t* buf, uint16_t size);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Communication_BytesChecker(
			                           uint8_t* buf,
			                           uint16_t size );
Parameters     : uint8_t* buf接收到的字符串
		 uint16_t size接收到的字符串长度
Return         : 校验位的取得，按照抑或进行校验
Implementation : 得到校验位
-----------------------------------------------------------------------------*/
uint8_t Communication_BytesChecker(uint8_t* buf,uint16_t size );

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetDevInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到设备信息的字符串
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetDevInfo(void);//得到设备信息

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetHeartBeatInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到心跳信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetHeartBeatInfo(void);//得到心跳

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_OpenLight(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 开灯
-----------------------------------------------------------------------------*/
uint8_t *Communication_OpenLight(uint8_t lightbox_type);//开灯

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLight(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 关灯
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLight(uint8_t lightbox_type);//关灯

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetLedInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到灯的信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLedInfo(uint8_t lightbox_type);//得到灯的信息

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLedClassInfo(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 返回字符串的首地址
Implementation : 设置led灯等级信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_SetLedClassInfo(uint8_t* buf);//得到led灯等级

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetLockInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到锁的信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLockInfo(void);//得到锁的信息

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_OpenLock(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 开锁
-----------------------------------------------------------------------------*/
uint8_t *Communication_OpenLock(void);//开锁

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLock(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 关锁
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLock(void);//关锁

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLockTime(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 返回字符串的首地址
Implementation : 设置关锁的时间
-----------------------------------------------------------------------------*/
uint8_t *Communication_SetLockTime(uint8_t* buf);//设置锁的时间
/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetElectricEnergyInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到电量信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetElectricEnergyInfo(void);//得到电量

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SmokeCallInfo(void);
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 烟雾报警
-----------------------------------------------------------------------------*/
uint8_t *Communication_SmokeCallInfo(void);//烟雾报警

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_LockCallInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 门锁自动关闭返回
-----------------------------------------------------------------------------*/
uint8_t *Communication_LockCallInfo(void);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_ChangIp(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 返回字符串的首地址
Implementation : 修改ip
-----------------------------------------------------------------------------*/
uint8_t *Communication_ChangIp(uint8_t* buf);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CheckFail(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 当数据非法时返回的函数
Implementation : 当数据非法时执行的函数
-----------------------------------------------------------------------------*/
uint8_t *Communication_IllegalData(uint8_t* buf);
uint8_t *Communication_LED_IllegalData(uint8_t lightbox_type,uint8_t* buf);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRPerform(uint8_t ir_command)
Parameters     : uint8_t ir_command    红外执行指令
Return         : 返回字符串的首地址
Implementation : 红外码执行，   "HWZX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRPerform(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRLearn(uint8_t ir_command)
Parameters     : uint8_t ir_command 红外学习指令
Return         : 返回字符串的首地址
Implementation : 红外码学习   "HWXX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRLearn(uint8_t ir_command);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRImportData(uint8_t* import_buf_addr)
Parameters     : uint8_t* data_buf_addr 导入首地址
Return         : 返回字符串的首地址
Implementation : 红外码导入  "HWDR"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRImportData(uint8_t* import_buf_addr);

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRExportData(uint8_t* export_buf_addr)
Parameters     : uint8_t* export_buf_addr 导出首地址
Return         : 返回字符串的首地址
Implementation : 红外码导出  "HWDC"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRExportData(uint8_t* export_buf_addr);

#endif /* End of <PROJECT>_<PATH>_<FILE>_H_ */
