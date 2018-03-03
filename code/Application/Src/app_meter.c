/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      :  刘帅
Description :  电表抄收流程精简，做成傻瓜式，每次都会读取电表地址
-----------------------------------------------------------------------------*/

/* include ANSI C .h file */
#include "main.h"

#ifdef BG_485_METER
/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(x[0]))

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
* Function   : void Meter_ReadParam()
* Value Area : 本文件内，做全局用
*/
static Meter_t s_meter_param_struct={0};
static const uint32_t s_meter_cmd_toread[] = {\
		CUR_COMBI_ACT_TOTAL_ENERGY
		//CUR_COMBI_ACT_RATE1_ENERGY,\
		CUR_COMBI_ACT_RATE2_ENERGY,\
		CUR_COMBI_ACT_RATE3_ENERGY,\
		CUR_COMBI_ACT_RATE4_ENERGY,\
			
		//CUR_FORWARD_ACT_TOTAL_ENERGY,\
		CUR_FORWARD_ACT_RATE1_ENERGY,\
		CUR_FORWARD_ACT_RATE2_ENERGY,\
		CUR_FORWARD_ACT_RATE3_ENERGY,\
		CUR_FORWARD_ACT_RATE4_ENERGY,\
			
		//CUR_RESERVE_ACT_TOTAL_ENERGY,\
		CUR_RESERVE_ACT_RATE1_ENERGY,\
		CUR_RESERVE_ACT_RATE2_ENERGY,\
		CUR_RESERVE_ACT_RATE3_ENERGY,\
		CUR_RESERVE_ACT_RATE4_ENERGY,\
			
		//PRE_COMBI_ACT_TOTAL_ENERGY,\
		PRE_COMBI_ACT_RATE1_ENERGY,\
		PRE_COMBI_ACT_RATE2_ENERGY,\
		PRE_COMBI_ACT_RATE3_ENERGY,\
		PRE_COMBI_ACT_RATE4_ENERGY,\
		
		//SWITCH_STATE
};
/* Global */

BOOL g_meter_is_read_flag = FALSE;

/*---------------------------------------------------------------------------*/
//	Extern
/*---------------------------------------------------------------------------*/
/* Variables */

/* Function prototypes */

/*---------------------------------------------------------------------------*/
//	Static Functions
/*---------------------------------------------------------------------------*/
#if 1

/*-----------------------------------------------------------------------------
Prototype  : uint8_t Meter_BuildProtocolReadParam(void *probuf,
				     uint32_t meter_param,
				     Meter_t *mtr_p);
Parameters : probuf - 协议存储的缓冲区
   	     meter_param - 组建协议的命名码
	     mtr_p - Meter_t结构体指针
Return     : 返回协议的长度，失败返回0(probuf非法)
Function   : 组建读取电能表信息的协议
-----------------------------------------------------------------------------*/
static uint8_t Meter_BuildProtocolReadParam(void *probuf,
				     uint32_t meter_param,
				     Meter_t *mtr_p);
/*-----------------------------------------------------------------------------
Prototype      : Meter_CheckSum(void *sum_buf,uint8_t check_len)
Parameters     : sum_buf - 待校验数据的首地址；check_len - 校验数据的长度
Return         : 0 - 失败；否则返回检验的结果
Implementation : 和校验的方式，实现校验数据，取和255的模
-----------------------------------------------------------------------------*/
static uint8_t Meter_CheckSum(void *sum_buf, uint8_t check_len)
{
	if(!sum_buf){return 0;}
	
        uint8_t *ptr = sum_buf;
	uint8_t sum = 0;

	for(uint8_t i = 0; i < check_len; i++)
	{
		sum += ptr[i] & 0xFF;
	}
	
	return (sum & 0xFF);
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Meter_ReadParam(void *buf,
					uint8_t buf_size,
					uint8_t cmd_index)
Parameters : 	buf - 数据缓冲区(要足够大>=24)
		buf_size - buf的大小
		mtr_p - 结构体 Meter_t指针，值结果参数
Return         : 0失败，成功返回读回的数据长度
Implementation : 先发送4字节的0xFE，唤醒总线，然后读取地址
		注：函数不对buf的长度做判断，如果缓冲区过小，可能会出问题
-----------------------------------------------------------------------------*/
static uint8_t Meter_ReadParam(void *buf, uint8_t buf_size, uint8_t cmd_index)
{
	if(!buf){return 0;}
	
	uint8_t len = 0;

	memset(buf, 0xFE, buf_size);
	Meter485_Send(buf, 4);
	memset(buf, 0, buf_size);
	len = Meter_BuildProtocolReadParam(buf,
					   s_meter_cmd_toread[cmd_index],
					   &s_meter_param_struct);
	Meter485_Send(buf, len);
	return Meter485_ReceiveData(buf, 1000);
}



/*-----------------------------------------------------------------------------
Prototype      : Meter_BuildProtocalReadAddr(void *probuf)
Parameters     : probuf - 协议缓冲区
Return         : 0 - 失败(probuf 非法);成功返回字符串长度
Implementation : 组合读取地址的协议，协议前加了两个字节的唤醒数据(0xFE)
-----------------------------------------------------------------------------*/
static uint8_t Meter_BuildProtocolReadAddr(void *probuf)
{
	if(!probuf){return 0;}
	
        uint8_t *ptr = probuf;
	
        *ptr++ = 0x68;
	
	*ptr++ = 0xAA;
	*ptr++ = 0xAA;
	*ptr++ = 0xAA;
	*ptr++ = 0xAA;
	*ptr++ = 0xAA;
	*ptr++ = 0xAA;
	
	*ptr++ = 0x68;
	//control code 
	*ptr++ = 0x13;
	//data length
	*ptr++ = 0x00;
	//begin with 0x68(first)
	*ptr++ = Meter_CheckSum((uint8_t*)probuf, 10);
	*ptr++ = 0x16;
	return 12;
}

/*-----------------------------------------------------------------------------
Prototype      : Meter_BuildProtocolReadParam(void *probuf,
				     uint32_t meter_param,
				     Meter_t *mtr_p)
Parameters     : probuf - 协议缓冲区
		 meter_param - 要组建协议的命令
		 mtr_p   - Meter_t结构体指针，从中取出电能表地址
Return         : 0 - 失败;成功，返回协议帧长度
Implementation : 根据电表协议组建
-----------------------------------------------------------------------------*/
static uint8_t Meter_BuildProtocolReadParam(void *probuf,
				     uint32_t meter_param,
				     Meter_t *mtr_p)
{
	if((!probuf) || (!mtr_p)){return 0;}
	
        uint8_t *ptr = probuf;
	
        *ptr++ = 0x68;
	
	*ptr++ = mtr_p->meter_addr[0];
	*ptr++ = mtr_p->meter_addr[1];
	*ptr++ = mtr_p->meter_addr[2];
	*ptr++ = mtr_p->meter_addr[3];
	*ptr++ = mtr_p->meter_addr[4];
	*ptr++ = mtr_p->meter_addr[5];
	
	*ptr++ = 0x68;
	//control code 
	*ptr++ = 0x11;
	//data length
	*ptr++ = 0x04;
	//data
	*ptr++ = (meter_param & 0xff) + 0x33;
	*ptr++ = ((meter_param >> 8) & 0xff) + 0x33;
	*ptr++ = ((meter_param >> 16) & 0xff) + 0x33;
	*ptr++ = ((meter_param >> 24) & 0xff) + 0x33;
	//begin with 0x68(first)
	*ptr++ = Meter_CheckSum((uint8_t*)probuf, 14);
	*ptr++ = 0x16;
	return 16;
}

/*-----------------------------------------------------------------------------
Prototype      : Meter_AnalysisAddr(void *ana_buf, uint8_t len, Meter_t *mtr_p)
Parameters     : ana_buf - 待解析的数据
		 len     - 解析数据的长度
		 mtr_p   - Meter_t结构体指针，用来存放电能表的信息
Return         : 0 - 失败;1 - 成功
Implementation : 首先去除0xfe(以防万一有)，之后根据协议解析
-----------------------------------------------------------------------------*/
static uint8_t Meter_AnalysisEnergyParam(void *ana_buf, uint8_t len, 
					 uint8_t cmd_index, Meter_t *mtr_p)
{
	if((!ana_buf) || (!mtr_p)){return 0;}
	
        uint8_t *ana_ptr = ana_buf;
	
	while((0xFE == *ana_ptr) && (len--)){++ana_ptr;};
	
        uint8_t valid_len = ana_ptr[9] + 12;
	
        if((0x68 != ana_ptr[0]) ||
	   (0x68 != ana_ptr[7]) ||
	   (0x16 != ana_ptr[valid_len-1]))
	{
		return FALSE;
	}
	//校验和错误
	if(ana_ptr[valid_len-2] != Meter_CheckSum(ana_ptr, valid_len-2))
	{
		return FALSE;
	}
	//正常应答帧
	if((0x91 == ana_ptr[8]) || (0x98 == ana_ptr[8]))
	{
		uint32_t tmp = 0;
		uint32_t DI_code = 0;
		
                DI_code =  (ana_ptr[13] - 0x33);
		DI_code = DI_code << 8;
		DI_code |= (ana_ptr[12] - 0x33);
		DI_code = DI_code << 8;
		DI_code |= (ana_ptr[11] - 0x33);
		DI_code = DI_code << 8;
		DI_code |= (ana_ptr[10] - 0x33);
		
		if(s_meter_cmd_toread[cmd_index] == DI_code)
                {
                        tmp =  (ana_ptr[17] - 0x33);
                        tmp = tmp << 8;
                        tmp |= (ana_ptr[16] - 0x33);
                        tmp = tmp << 8;
                        tmp |= (ana_ptr[15] - 0x33);
                        tmp = tmp << 8;
                        tmp |= (ana_ptr[14] - 0x33);
			
                        MeterEnergy_t *ptr = (MeterEnergy_t *)mtr_p;
                        ptr += cmd_index / METER_EVENT_EACH_GROUP_NUM;
                        uint32_t *pdata = (uint32_t *)ptr;
                        memcpy(pdata + cmd_index % METER_EVENT_EACH_GROUP_NUM,
                                (uint8_t*)&tmp, 4);
			return TRUE;
		}
	}
	//从站异常应答
	else if(0xD1 == ana_ptr[8])
	{
		uint8_t error_code = ana_ptr[10] - 0x33;
		
                switch(error_code)
		{
		case 0x02://电表无响应的数据
		case 0x40://费率数超
                        return TRUE;
		default://暂不支持其他错误
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	return FALSE;
}

/*-----------------------------------------------------------------------------
Prototype      : Meter_AnalysisAddr(void *ana_buf, uint8_t len, Meter_t *mtr_p)
Parameters     : ana_buf - 待解析的数据
		 len     - 解析数据的长度
		 mtr_p   - Meter_t结构体指针，用来存放电能表的信息
Return         : 0 - 失败;1 - 成功
Implementation : 首先去除0xfe(以防万一有)，之后根据协议解析
-----------------------------------------------------------------------------*/
static uint8_t Meter_AnalysisAddr(void *ana_buf, uint8_t len, Meter_t *mtr_p)
{
	if((!ana_buf) || (!mtr_p)){return 0;}
	
        uint8_t *ana_ptr = ana_buf;
	
	while((0xFE == *ana_ptr) && (len--)){++ana_ptr;};
	//因为地址帧是18个字节，因此小于18时直接返回
	if(len<18)
	{
		return 0;
	}
	
        uint8_t valid_len = ana_ptr[9] + 12;
	
        if((0x68 != ana_ptr[0]) ||
	   (0x68 != ana_ptr[7]) ||
	   (0x93 != ana_ptr[8]) ||
	   (0x16 != ana_ptr[valid_len-1]))
	{
		return 0;
	}
	//校验和错误
	if(ana_ptr[valid_len-2] != Meter_CheckSum(ana_ptr, valid_len-2))
	{
		return 0;
	}
	
        for(uint8_t i=0; i < 6; i++)
	{
		mtr_p->meter_addr[i] = ana_ptr[10+i] - 0x33;
	}

	return 1;
}

static uint8_t Meter_BCDToDecimal(uint8_t bcd)
{
	uint8_t dec = bcd >> 4;
	
        return (dec * 10 + (bcd & 0x0F));
}

/*-----------------------------------------------------------------------------
Prototype      : void Meter_ReadAddr(void *buf,
                                     uint8_t buf_size,
                                     Meter_t *mtr_p)
Parameters : 	buf - 数据缓冲区(要足够大>=22)
		buf_size - buf的大小
		mtr_p - 结构体 Meter_t指针，值结果参数
Return         : (Nothing)
Implementation : 线发送4字节的0xFE，唤醒总线，然后读取地址
		注：函数不对buf的长度做判断，如果缓冲区过小，可能会出问题
-----------------------------------------------------------------------------*/
static void Meter_ReadAddr(void *buf, uint8_t buf_size, Meter_t *mtr_p)
{
	if((!buf) || (!mtr_p)){return;}
	uint8_t len = 0;
	
	//唤醒电能表
	memset(buf, 0xFE, buf_size);
	Meter485_Send(buf, 4);
	for(uint8_t i=0; i<3; i++)
	{
		memset(buf, 0, buf_size);
		len = Meter_BuildProtocolReadAddr(buf);
		Meter485_Send(buf, len);
		memset(buf, 0, buf_size);
		//阻塞1s等待读取数据
		if(0 == (len = Meter485_ReceiveData(buf, 1000)))
		{
			mtr_p->meter_exist = FALSE;
			continue;
		}
		if(0==Meter_AnalysisAddr(buf, len, mtr_p))
		{
			mtr_p->meter_exist = FALSE;
			continue;
		}
		mtr_p->meter_exist = TRUE;
		break;
	}
}
/*-----------------------------------------------------------------------------
Prototype      : static uint8_t Meter_EventControl(void)
Parameters     :(Nothing)
Return         : 1 - 成功，0 - 失败
Implementation : 每次都会去读取电能表的地址
-----------------------------------------------------------------------------*/
uint8_t r_buf[30];
static uint8_t Meter_EventControl(void)
{
	//uint8_t r_buf[30] = {0};
	uint8_t len = 0;

        //在此函数中会连续读三次，直到有一次成功为止
        Meter_ReadAddr(r_buf, sizeof(r_buf), &s_meter_param_struct);

        if(FALSE == s_meter_param_struct.meter_exist)
        {
                return 0;
        }
        memset(r_buf, 0, sizeof(r_buf));
        //会阻塞读取数据
        len = Meter_ReadParam(r_buf, sizeof(r_buf), 0);
        if(FALSE == Meter_AnalysisEnergyParam(r_buf, len, 0,
                                &s_meter_param_struct))
        {
                return 0;
        }
        return 1;
}

#endif /* End of 1, Just a tag, ignore it */

/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
Prototype      : uint32_t Meter_GetCurrCombiActiveTotalEnergy(void)
Parameters     :(Nothing)
Return         : 返回组合有功总电能，单位KWh,
		若通讯出现故障，则返回0xFFFFFFFF
Implementation : 直接省略了小数位
-----------------------------------------------------------------------------*/
uint32_t Meter_GetCurrCombiActiveTotalEnergy(void)
{
        
	if(0 == Meter_EventControl()){return (0xFFFFFFFF);}
	//去除符号为
	uint32_t energy = 
	(s_meter_param_struct.meterCombiActiveEnergy.total_energy &
         0x7FFFFFFF);
	//去除小数位
	energy >>= 8;
	//bcd->int
	uint32_t ener_int =
                (uint32_t)Meter_BCDToDecimal((energy >> 24) & 0xFF);
        ener_int *= 100;
	ener_int += (uint32_t)Meter_BCDToDecimal((energy >> 16) & 0xFF);
        ener_int *= 100;
	ener_int += (uint32_t)Meter_BCDToDecimal((energy >> 8) & 0xFF);
        ener_int *= 100;
	ener_int += (uint32_t)Meter_BCDToDecimal((energy & 0xFF));

	return ener_int;
}

#endif //(defined(BG_485_METER))

