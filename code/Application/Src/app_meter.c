/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      :  ��˧
Description :  ��������̾�������ɵ��ʽ��ÿ�ζ����ȡ����ַ
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
* Value Area : ���ļ��ڣ���ȫ����
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
Parameters : probuf - Э��洢�Ļ�����
   	     meter_param - �齨Э���������
	     mtr_p - Meter_t�ṹ��ָ��
Return     : ����Э��ĳ��ȣ�ʧ�ܷ���0(probuf�Ƿ�)
Function   : �齨��ȡ���ܱ���Ϣ��Э��
-----------------------------------------------------------------------------*/
static uint8_t Meter_BuildProtocolReadParam(void *probuf,
				     uint32_t meter_param,
				     Meter_t *mtr_p);
/*-----------------------------------------------------------------------------
Prototype      : Meter_CheckSum(void *sum_buf,uint8_t check_len)
Parameters     : sum_buf - ��У�����ݵ��׵�ַ��check_len - У�����ݵĳ���
Return         : 0 - ʧ�ܣ����򷵻ؼ���Ľ��
Implementation : ��У��ķ�ʽ��ʵ��У�����ݣ�ȡ��255��ģ
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
Parameters : 	buf - ���ݻ�����(Ҫ�㹻��>=24)
		buf_size - buf�Ĵ�С
		mtr_p - �ṹ�� Meter_tָ�룬ֵ�������
Return         : 0ʧ�ܣ��ɹ����ض��ص����ݳ���
Implementation : �ȷ���4�ֽڵ�0xFE���������ߣ�Ȼ���ȡ��ַ
		ע����������buf�ĳ������жϣ������������С�����ܻ������
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
Parameters     : probuf - Э�黺����
Return         : 0 - ʧ��(probuf �Ƿ�);�ɹ������ַ�������
Implementation : ��϶�ȡ��ַ��Э�飬Э��ǰ���������ֽڵĻ�������(0xFE)
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
Parameters     : probuf - Э�黺����
		 meter_param - Ҫ�齨Э�������
		 mtr_p   - Meter_t�ṹ��ָ�룬����ȡ�����ܱ��ַ
Return         : 0 - ʧ��;�ɹ�������Э��֡����
Implementation : ���ݵ��Э���齨
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
Parameters     : ana_buf - ������������
		 len     - �������ݵĳ���
		 mtr_p   - Meter_t�ṹ��ָ�룬������ŵ��ܱ����Ϣ
Return         : 0 - ʧ��;1 - �ɹ�
Implementation : ����ȥ��0xfe(�Է���һ��)��֮�����Э�����
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
	//У��ʹ���
	if(ana_ptr[valid_len-2] != Meter_CheckSum(ana_ptr, valid_len-2))
	{
		return FALSE;
	}
	//����Ӧ��֡
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
	//��վ�쳣Ӧ��
	else if(0xD1 == ana_ptr[8])
	{
		uint8_t error_code = ana_ptr[10] - 0x33;
		
                switch(error_code)
		{
		case 0x02://�������Ӧ������
		case 0x40://��������
                        return TRUE;
		default://�ݲ�֧����������
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
Parameters     : ana_buf - ������������
		 len     - �������ݵĳ���
		 mtr_p   - Meter_t�ṹ��ָ�룬������ŵ��ܱ����Ϣ
Return         : 0 - ʧ��;1 - �ɹ�
Implementation : ����ȥ��0xfe(�Է���һ��)��֮�����Э�����
-----------------------------------------------------------------------------*/
static uint8_t Meter_AnalysisAddr(void *ana_buf, uint8_t len, Meter_t *mtr_p)
{
	if((!ana_buf) || (!mtr_p)){return 0;}
	
        uint8_t *ana_ptr = ana_buf;
	
	while((0xFE == *ana_ptr) && (len--)){++ana_ptr;};
	//��Ϊ��ַ֡��18���ֽڣ����С��18ʱֱ�ӷ���
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
	//У��ʹ���
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
Parameters : 	buf - ���ݻ�����(Ҫ�㹻��>=22)
		buf_size - buf�Ĵ�С
		mtr_p - �ṹ�� Meter_tָ�룬ֵ�������
Return         : (Nothing)
Implementation : �߷���4�ֽڵ�0xFE���������ߣ�Ȼ���ȡ��ַ
		ע����������buf�ĳ������жϣ������������С�����ܻ������
-----------------------------------------------------------------------------*/
static void Meter_ReadAddr(void *buf, uint8_t buf_size, Meter_t *mtr_p)
{
	if((!buf) || (!mtr_p)){return;}
	uint8_t len = 0;
	
	//���ѵ��ܱ�
	memset(buf, 0xFE, buf_size);
	Meter485_Send(buf, 4);
	for(uint8_t i=0; i<3; i++)
	{
		memset(buf, 0, buf_size);
		len = Meter_BuildProtocolReadAddr(buf);
		Meter485_Send(buf, len);
		memset(buf, 0, buf_size);
		//����1s�ȴ���ȡ����
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
Return         : 1 - �ɹ���0 - ʧ��
Implementation : ÿ�ζ���ȥ��ȡ���ܱ�ĵ�ַ
-----------------------------------------------------------------------------*/
uint8_t r_buf[30];
static uint8_t Meter_EventControl(void)
{
	//uint8_t r_buf[30] = {0};
	uint8_t len = 0;

        //�ڴ˺����л����������Σ�ֱ����һ�γɹ�Ϊֹ
        Meter_ReadAddr(r_buf, sizeof(r_buf), &s_meter_param_struct);

        if(FALSE == s_meter_param_struct.meter_exist)
        {
                return 0;
        }
        memset(r_buf, 0, sizeof(r_buf));
        //��������ȡ����
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
Return         : ��������й��ܵ��ܣ���λKWh,
		��ͨѶ���ֹ��ϣ��򷵻�0xFFFFFFFF
Implementation : ֱ��ʡ����С��λ
-----------------------------------------------------------------------------*/
uint32_t Meter_GetCurrCombiActiveTotalEnergy(void)
{
        
	if(0 == Meter_EventControl()){return (0xFFFFFFFF);}
	//ȥ������Ϊ
	uint32_t energy = 
	(s_meter_param_struct.meterCombiActiveEnergy.total_energy &
         0x7FFFFFFF);
	//ȥ��С��λ
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

