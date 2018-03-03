/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 廖密
Description : 解析网络通信数据和返回相应数据
-----------------------------------------------------------------------------*/
/* include ANSI C .h file */
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "main.h"

#if COMMUNICATION_MODE == COMMUNICATION_MAIN_NOBLCOK
/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/
#define PMM_RST //PMMCTL0 |= PMMSWBOR;  //系统自复位
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

/* Global */
#ifdef BG_DOOR_LOCK
	LockInfor_t s_locki;
        extern uint8_t g_door_auto_lock_send_flag;
#endif

#ifdef BG_SMOKE_ALARM

uint32_t g_smoke_alarm_upload_time = 0;
uint8_t g_smoke_alarm_upload_flag = 0;

#endif
/*---------------------------------------------------------------------------*/
//	Extern
/*---------------------------------------------------------------------------*/

/* Variables */

/* Function prototypes */

/*---------------------------------------------------------------------------*/
//	Static Functions
/*---------------------------------------------------------------------------*/
void Communication_Error_Handle(uint8_t sn)
{	
	close(sn);
	APP_W5500_NetworkInit();
}
/*-----------------------------------------------------------------------------
Prototype      :APP_Communication_Tcps(uint8_t sn, uint8_t* buf, uint16_t port) 
Parameters     : sn用来设置SCOKET是TCP还是UDP模式
		*buf设置网络传输数据的大小字节数
		port是端口的数目
Return         : 返回小于1或者是1，表示通信成果，小于0则关闭scoket
Implementation : 用来网络通信
-----------------------------------------------------------------------------*/
int8_t Communication_Tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
        int8_t ret = 0;
        uint16_t size = 0;
        uint8_t deal_data_return = 0;

#ifdef _COMMUNICATION_DEBUG_
   	uint8_t destip[4];
   	uint16_t destport;
#endif
  	switch(getSn_SR(sn))
   	{
     	 case SOCK_ESTABLISHED :
        	if(getSn_IR(sn) & Sn_IR_CON)
         	{
#ifdef _COMMUNICATION_DEBUG_
				getSn_DIPR(sn, destip);
				destport = getSn_DPORT(sn);


				printf("%d:Connected - %d.%d.%d.%d : %d\r\n", sn,
				destip[0], destip[1], destip[2], destip[3], destport);
#endif
				setSn_IR(sn,Sn_IR_CON);
				/*心跳自动发送的触发条件是进行过一次收或者发,为了保证能触发心跳,需要在连接后发送一个字节数据
				w5500 datasheet中11页,RTR为200ms,RCR为8,经过交换机,会在31.8s触发超时中断,关闭socket.
				*/
				uint8_t keep_alive_value = 0xA0;
				ret = send(sn, &keep_alive_value, 1);
				if(ret < 0)
				{
					close(sn);
				}
         	}
#ifdef BG_SMOKE_ALARM
                /*
                烟雾报警第一次发生g_smoke_alarm_status置为1,
                如果还有烟雾报警g_smoke_alarm_upload_flag每五分钟置为1一次
                */
		if((1 == g_smoke_alarm_status) 
		   || (1 == g_smoke_alarm_upload_flag)) 
		{
                  #ifdef BG_UART_DEBUG
                  	printf("烟雾报警\r\n");
                  #endif
                        g_smoke_alarm_status = 0;
                	g_smoke_alarm_upload_flag = 0;
			ret = send(sn, Communication_SmokeCallInfo(), 8);
                	if(ret < 0)
                	{
                  		close(sn);
                  		return ret;
                	}
		}
#endif
#ifdef BG_DOOR_LOCK
                if(1 == g_door_auto_lock_send_flag)
                {
                        g_door_auto_lock_send_flag = 0;
                        ret = send(sn, Communication_LockCallInfo(), 13);
                        if(ret < 0)
                        {
                                close(sn);
                                return ret;
                        }
                }
#endif
         	if((size = getSn_RX_RSR(sn)) > 0) 
		  // Don't need to check SOCKERR_BUSY 
		  //because it doesn't not occur.
			{

				if(size > DATA_BUF_SIZE) {size = DATA_BUF_SIZE;}
				ret = recv(sn, buf, size);

				if(ret <= 0) {return ret;}
				// check SOCKERR_BUSY & SOCKERR_XXX. 
				//For showing the occurrence of SOCKERR_BUSY.
				deal_data_return = Communication_DealData(buf, size);
							
				printf("deal_data_return=%d\r\n",deal_data_return);
				switch(deal_data_return)
				{
					case COMMUCATION_OK:
						ret = send(sn, buf, buf[6]);
					break;
					case COMMUCATION_MODIFY_IP:
						ret = send(sn, buf, buf[6]);
						//修改IP后,重新初始化网络
							close(sn);
						APP_W5500_NetworkInit();
					break;
					case COMMUCATION_CHECK_ERR:
						#ifdef BG_UART_DEBUG
							  printf("校验错误\r\n");
						#endif
					break;
					case COMMUCATION_START_FLAG_ERR:
						#ifdef BG_UART_DEBUG
							  printf("开始标志错误\r\n");
						#endif
					break;
					case COMMUCATION_LOST_PACKET_ERR:
						#ifdef BG_UART_DEBUG
							  printf("丢包\r\n");
						#endif
					break;
					case COMMUCATION_UNKNOWN_COMMAND:
						#ifdef BG_UART_DEBUG
							  printf("未知命令\r\n");
						#endif
					break;
					default:
					break;
				}				
				if(ret < 0)
				{
					 close(sn);
					 return ret;
				}
         	}
         	break;
      	case SOCK_CLOSE_WAIT :
#ifdef _COMMUNICATION_DEBUG_
         	printf("%d:CloseWait\r\n", sn);
#endif
         	if((ret = disconnect(sn)) != SOCK_OK) {return ret;}
#ifdef _COMMUNICATION_DEBUG_
         	printf("%d:Socket Closed\r\n", sn);
#endif
         	break;
      	case SOCK_INIT :
#ifdef _COMMUNICATION_DEBUG_
    	 	printf("%d:Listen, TCP server \
			loopback, port [%d]\r\n", sn, port);
#endif

         	if((ret = listen(sn)) != SOCK_OK) {return ret;}
         	break;
      	case SOCK_CLOSED:
#ifdef _COMMUNICATION_DEBUG_
         	printf("%d:TCP server loopback start\r\n",sn);
#endif
         	if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) 

                {
                        return ret;
                }
		/*设置心跳自动发送时间为5s,kpalvtr*5,就是进行过一次收或者发的通信后,5s后开始发送心跳*/
		uint8_t kpalvtr = 1;
		if((ret = setsockopt(sn,SO_KEEPALIVEAUTO,&kpalvtr)) != SOCK_OK)
                {
                          close(sn);
                          return ret;
                }
#ifdef _COMMUNICATION_DEBUG_
         	printf("%d:Socket opened\r\n", sn);
#endif
         	break;
      	default:
         	break;
   	}
   	return 1;
}
/*-----------------------------------------------------------------------------
Prototype      : int8_t Communication_DealData(uint8_t* buf, uint16_t size)
Parameters     :uint8_t* buf 接收到的字符串
		uint16_t size接收到的字符串长度	   
Return         :根据解析返回对应的要发送的字符串 
Implementation :解析接收到字符串,如果碰到校验错误,首字符不对,接收到的个数不对,
                返回相应的错误码,不进行任何数据处理,如果接收的包的个数大于数据
                长度,进行拆包
-----------------------------------------------------------------------------*/

int8_t Communication_DealData(uint8_t* buf, uint16_t size)
{
#ifdef BG_UART_DEBUG
	printf("recv data:");
	uint8_t i = 0;
	for(i = 0;i < size;i++)
	{
		printf("buf[%d]=%X ",i,buf[i]);
	}
	printf("\r\n");
#endif
	
	uint8_t data_size = (buf[5]<<8)|buf[6];
	//拆包只拆分两个包的时候
	if(data_size < size)
	{
		memcpy(buf,buf+data_size,size-data_size);
		size = size - data_size;
		
#ifdef BG_UART_DEBUG
			printf("split data:");
			uint8_t j = 0;
			for(j = 0;j < size;j++)
			{
				printf("buf[%d]=%X ",j,buf[j]);
			}
			printf("\r\n");
#endif
			
	}
    else if(((buf[5]<<8)|buf[6]) > size)
	{
		return COMMUCATION_LOST_PACKET_ERR;
	}
	
	uint8_t check_value = Communication_BytesChecker(buf, size);
	if(buf[size - 1] != check_value)
	{
		return COMMUCATION_CHECK_ERR;
	}
	if('$' != buf[0])
	{
		return COMMUCATION_START_FLAG_ERR;
	}

	uint8_t *setformation;	
        if(memcmp(&buf[1], "CXSB", 4) == 0)  
        {	
                setformation = Communication_GetDevInfo();
                for(int i = 0; i < setformation[6]; i++)
                {
                        *buf=setformation[i];
                        buf++;
                }
        }
        else if(memcmp(&buf[1], "HEAT", 4) == 0) 
        {
                setformation = Communication_GetHeartBeatInfo();
                for(int i = 0; i < setformation[6]; i++)
                {
                        *buf = setformation[i];
                        buf++;
                }	
        } 
		
#ifdef BG_LED
        else if(memcmp(&buf[1], "DKLD", 4) == 0)
        {
		if((LIGHTBOX1 == buf[7]) || (LIGHTBOX2 == buf[7]))
		{
			setformation=Communication_OpenLight(buf[7]);
			for(int i = 0; i < setformation[6]; i++)
			{
				*buf = setformation[i];
				buf++;
			}
		}
        }
        else if(memcmp(&buf[1], "GBLD", 4) == 0)
        {
		if((LIGHTBOX1 == buf[7]) || (LIGHTBOX2 == buf[7]))
		{
			setformation=Communication_CloseLight(buf[7]);
			for(int i = 0; i < setformation[6]; i++)
			{
				*buf = setformation[i];
				buf++;
			}
		}
        } 
        else if(memcmp(&buf[1], "CXLD", 4) == 0)
        {		
		if((LIGHTBOX1 == buf[7]) || (LIGHTBOX2 == buf[7]))
		{
			setformation=Communication_GetLedInfo(buf[7]);
			for(int i = 0; i < setformation[6]; i++)
			{
				*buf = setformation[i];
				buf++;
			}
		}
        }
        else if(memcmp(&buf[1], "SZLD", 4) == 0)
        {		
		if((LIGHTBOX1 == buf[7]) || (LIGHTBOX2 == buf[7]))
		{
			setformation = Communication_SetLedClassInfo(buf);
			for(int i = 0; i < setformation[6]; i++)
			{
				*buf = setformation[i];
				buf++;
			}
		}
        }      
#endif 

#ifdef BG_DOOR_LOCK	  
        else if(memcmp(&buf[1], "DKMS", 4) == 0)
        {
                setformation = Communication_OpenLock();
                for(int i = 0; i < setformation[6]; i++)
                {				
                        *buf = setformation[i];
                        buf++;
                }
        }	  
        else if(memcmp(&buf[1], "GBMS", 4) == 0)
        {			
                setformation = Communication_CloseLock();
                for(int i = 0; i < setformation[6]; i++)
                {				
                        *buf = setformation[i];
                        buf++;
                }
        }	  
        else if(memcmp(&buf[1], "CXMS", 4) == 0)
        {			
                setformation = Communication_GetLockInfo();
                for(int i = 0;i < setformation[6]; i++)
                {		
                        *buf = setformation[i];
                        buf++;
                }
        }
        else if(memcmp(&buf[1], "SZMS", 4) == 0)
        {	
                setformation = Communication_SetLockTime(buf);
                for(int i = 0; i < setformation[6]; i++)
                {
                        *buf = setformation[i];
                        buf++;
                }
        } 
#endif 

#ifdef BG_485_METER    
        else if(memcmp(&buf[1], "CXDB", 4) == 0)
        {
                setformation = Communication_GetElectricEnergyInfo();
                for(int i = 0; i <setformation[6]; i++)
                {
                        *buf = setformation[i];
                        buf++;
                }
        }
#endif

#ifdef BG_INFO  
        else if(memcmp(&buf[1], "XGIP", 4) == 0)
        {
			setformation = Communication_ChangIp(buf);
			for(int i = 0; i < setformation[6]; i++)
			{
					*buf = setformation[i];
					buf++;
			}
			return COMMUCATION_MODIFY_IP;
        }	
#endif
		
#ifdef BG_INFRARED  
        else if(memcmp(&buf[1], "HWXX", 4) == 0)
        {
			setformation = Communication_IRLearn(buf[7]);
			for(int i = 0; i < setformation[6]; i++)
			{
					*buf = setformation[i];
					buf++;
			}	
        }
		else if(memcmp(&buf[1], "HWZX", 4) == 0)
        {
			setformation = Communication_IRPerform(buf[7]);
			for(int i = 0; i < setformation[6]; i++)
			{
					*buf = setformation[i];
					buf++;
			}
        }
		else if(memcmp(&buf[1], "HWDR", 4) == 0)
        {
			setformation = Communication_IRImportData(buf);
			for(int i = 0; i < setformation[6]; i++)
			{
					*buf = setformation[i];
					buf++;
			}
        }
		else if(memcmp(&buf[1], "HWDC", 4) == 0)
        {
			setformation = Communication_IRExportData(buf);
        }
#endif
        else 
        {
                return COMMUCATION_UNKNOWN_COMMAND;
        }
	return COMMUCATION_OK;
}


/*-----------------------------------------------------------------------------
Prototype      : uint8_t Communication_BytesChecker(uint8_t* buf, uint16_t size)
{
Parameters     : uint8_t* buf接收到的字符串
		 uint16_t size接收到的字符串长度
Return         : 校验位的取得，按照抑或进行校验
Implementation : 得到校验位
-----------------------------------------------------------------------------*/
uint8_t Communication_BytesChecker(uint8_t* buf, uint16_t size)
{
        uint8_t check = buf[0];
        for(int i = 1; i < size-1; i++)
        {
                check = check ^ buf[i];
        }
        return check;  
} 

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetDevInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到设备信息的字符串
-----------------------------------------------------------------------------*/

uint8_t *Communication_GetDevInfo(void)
{
  	uint8_t hard_info[5];
	uint8_t soft_info[5];
	static uint8_t device_info[16] = {'$','S','B','X','X',
		0x00,0x10,0x01,0x00,0x01,0x00,0x01,0x01,0x01,0x00,0x24};  
	Info_ReadHardVer((char *)hard_info);
	Info_ReadSoftVer((char *)soft_info);
		printf_array(hard_info,5);
		printf_array(soft_info,5);
	device_info[10] = hard_info[0];
	device_info[9] = hard_info[1];
	device_info[8] = hard_info[2];
	device_info[7] = hard_info[3];
	device_info[14] = soft_info[0];
	device_info[13] = soft_info[1];
	device_info[12] = soft_info[2];
	device_info[11] = soft_info[3];	  
        device_info[15] =
	Communication_BytesChecker(device_info, 16);
        return device_info;
}
/*-----------------------------------------------------------------------------

Prototype      : uint8_t *Communication_GetHeartBeatInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到心跳信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetHeartBeatInfo(void)
{
	static uint8_t heart_beat_info[8] = {0x24,0x48,0x41,
					0x43,0x4b,0x00,0x08,0x2d};
	heart_beat_info[7] =
	Communication_BytesChecker(heart_beat_info, 8);
 	return heart_beat_info;
}
/*-----------------------------------------------------------------------------

Prototype      : uint8_t *Communication_OpenLight(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 开灯
-----------------------------------------------------------------------------*/

uint8_t *Communication_OpenLight(uint8_t lightbox_type)
{
	static uint8_t open_light[14] = {0x24,0x46,0x4b,0x58,0x58,
				0x00,0x0e,0x44,0x4b,0x4c,0x44,0x00,0x00,0x00};    
	open_light[11] = lightbox_type;	
#ifdef BG_LED
        Lightbox_OpenLight(lightbox_type);
#endif	
	open_light[13] = Communication_BytesChecker(open_light, 14);
 	return open_light;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLight(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 关灯
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLight(uint8_t lightbox_type)
{
	static uint8_t close_light[14] = {0x24,0x46,0x4b,0x58,0x58,
				0x00,0x0e,0x47,0x42,0x4c,0x44,0x00,0x00,0x00};
	close_light[11] = lightbox_type;	
#ifdef BG_LED
        Lightbox_CloseLight(lightbox_type);
#endif
	close_light[13] = Communication_BytesChecker(close_light, 14);
 	return close_light;
}
/*-----------------------------------------------------------------------------

Prototype      : uint8_t *Communication_GetLedInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到灯的信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLedInfo(uint8_t lightbox_type)
{
	static uint8_t led_info[11] = {0x24,0x4c,0x44,0x58,
					0x58,0x00,0x0b,0x00,0x00,0x05,0x00};
	led_info[7] = lightbox_type;
#ifdef BG_LED
        led_info[8] = Lightbox_GetState(lightbox_type);
	led_info[9] = Lightbox_GetLevel(lightbox_type);
#endif
	led_info[10] = Communication_BytesChecker(led_info, 11);
 	return led_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLedClassInfo(uint8_t* buf)
Parameters     : uint8_t* buf接收到的字符串
Return         : 返回字符串的首地址
Implementation : 设置等级
-----------------------------------------------------------------------------*/
			   
uint8_t *Communication_SetLedClassInfo(uint8_t* buf)
{	
	static uint8_t led_class_info[11] = {0x24,0x4c,0x44,0x58,
					0x58,0x00,0x0b,0x00,0x01,0x05,0x23};
	uint8_t lightbox_type = buf[7];
#ifdef BG_LED
        signed char level_led = (signed char)buf[8];
	if((level_led < 6)&& (level_led > 0))
	{
                led_class_info[9] = level_led;
		Ligthbox_SetLevel(lightbox_type,level_led);
                #ifdef BG_INFO
                Info_WriteLightLevel(buf[7],(char *)&led_class_info[9]);
                #endif
	}
	else
	{
		return Communication_LED_IllegalData(lightbox_type,buf);
	}
#endif
        led_class_info[7] = buf[7];
        led_class_info[8] = Lightbox_GetState(lightbox_type);
	led_class_info[10] =
	Communication_BytesChecker(led_class_info, 11);
 	return led_class_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetLockInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到锁的信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetLockInfo(void)
{ 
    	static uint8_t lock_info[11] = {0x24,0x4d,0x53,0x58,
				0x58,0x00,0x0b,0x00,0x01,0x2c,0x1c};
#ifdef BG_DOOR_LOCK
 	Lock_Control(LOCK_GET, &s_locki);
	
	lock_info[7] = s_locki.state;
	lock_info[8] = s_locki.auto_lock_time >> 8;
	lock_info[9] = s_locki.auto_lock_time;
#endif  
  	lock_info[10] = Communication_BytesChecker(lock_info, 11);
  	return lock_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_OpenLock(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 开锁
-----------------------------------------------------------------------------*/
uint8_t *Communication_OpenLock(void)
{	
	static uint8_t open_lock[13] = {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0d,0x44,0x4b,0x4d,0x53,0x00,0x35};
#ifdef BG_DOOR_LOCK
	s_locki.state = LOCK_UNLOCKED;
	
	Lock_Control(LOCK_SET_STATE, &s_locki);
#endif	
	open_lock[12] = Communication_BytesChecker(open_lock, 13);	
 	return open_lock;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CloseLock(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 关锁
-----------------------------------------------------------------------------*/
uint8_t *Communication_CloseLock(void)
{
	static uint8_t close_lock[13] = {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0d,0x47,0x42,0x4d,0x53,0x00,0x3f};	
#ifdef BG_DOOR_LOCK 
	s_locki.state = LOCK_LOCKED;
	
	Lock_Control(LOCK_SET_STATE, &s_locki);
#endif
	close_lock[12] = Communication_BytesChecker(close_lock, 13);
 	return close_lock;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SetLockTime(uint8_t* buf)
Parameters     : uint8_t* buf接收到的字符串
Return         : 返回字符串的首地址
Implementation : 设置关锁的时间
-----------------------------------------------------------------------------*/
uint8_t *Communication_SetLockTime(uint8_t* buf)
{
	static uint8_t set_lock_time[11] = {0x24,0x4d,0x53,0x58,0x58,
				0x00,0x0b,0x00,0x01,0x2c,0x1c};
#ifdef BG_DOOR_LOCK
	set_lock_time[8] = buf[7];
	set_lock_time[9] = buf[8];
	s_locki.auto_lock_time = (buf[7] << 8) | buf[8];
	
	if((int) s_locki.auto_lock_time > 300)
	{
	  	return Communication_IllegalData(buf);	
	}
	else if((int) s_locki.auto_lock_time < 5 )
	{
		return Communication_IllegalData(buf);		
	}
	   
	printf("Lock_time: %d\r\n", s_locki.auto_lock_time);
#ifdef BG_INFO
	Info_WriteLockTimer(&s_locki.auto_lock_time);
#endif
	set_lock_time[7] = s_locki.state;	
	Lock_Control(LOCK_SET_TIME, &s_locki);
#endif	
	set_lock_time[10] = Communication_BytesChecker(set_lock_time, 11);
 	return set_lock_time;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_GetElectricEnergyInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 得到电量信息
-----------------------------------------------------------------------------*/
uint8_t *Communication_GetElectricEnergyInfo(void)
{	
#ifdef BG_485_METER
	uint32_t Energynum;
	
	static uint8_t electric_energy_info[12] = {0x24,0x44,0x42,0x58,
				0x58,0x00,0x0c,0x00,0x00,0x02,0x58,0x74};
	static uint8_t electric_energy_info1[13] = {0x24,0x46,0x4b,0x58,
				0x58,0x00,0x0d,0x44,0x42,0x58,0x58,0xe2,0x77};
	Energynum = Meter_GetCurrCombiActiveTotalEnergy();
	if(0xFFFFFFFF == Energynum)
	{	//缺少结果码  0xE2
		electric_energy_info1[12] =
	  	Communication_BytesChecker(electric_energy_info1, 13);
		return electric_energy_info1;
	}
	else
	{		
		printf("Energynum: %ld\r\n", Energynum);
		
		electric_energy_info[7] = Energynum >> 24;
		electric_energy_info[8] = Energynum >> 16;
		electric_energy_info[9] = Energynum >> 8;
		electric_energy_info[10] = Energynum & 0xff;
		electric_energy_info[11]=
	  	Communication_BytesChecker(electric_energy_info, 12);	
		return electric_energy_info;
	}
#endif
	return NULL;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_LockCallInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 门锁自动关闭返回
-----------------------------------------------------------------------------*/
uint8_t *Communication_LockCallInfo(void)
{
	static uint8_t lock_info[13] =  {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0d,0x47,0x42,0x4d,0x53,0x00,0x3f};
	lock_info[12] =
	Communication_BytesChecker(lock_info, 13);	
 	return lock_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_SmokeCallInfo(void)
Parameters     : 无
Return         : 返回字符串的首地址
Implementation : 烟雾报警
-----------------------------------------------------------------------------*/
uint8_t *Communication_SmokeCallInfo(void)
{
	static uint8_t smoke_info[8] = {0x24,0x59,0x57,
				0x42,0x4a,0x00,0x08,0x34};
	smoke_info[7] =
	Communication_BytesChecker(smoke_info, 8);	
 	return smoke_info;
}
/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_ChangIp(uint8_t* buf)
Parameters     : uint8_t* buf接收到的字符串
        Return         : 返回字符串的首地址
Implementation : 修改ip
-----------------------------------------------------------------------------*/
uint8_t *Communication_ChangIp(uint8_t* buf)
{                  
  	uint8_t ip[4];
  	static uint8_t ip_info[13] = {0x24,0x46,0x4B,0x58,0x58,0x00,0x0D,0x58,
			0x47,0x49,0x50,0x00,0x34};
	ip[0] = buf[7];
	ip[1] = buf[8];
	ip[2] = buf[9];
	ip[3] = buf[10];

#ifdef BG_INFO
	Info_WriteIp(ip);		
#endif
	ip_info[12] = Communication_BytesChecker(ip_info, 12);
 	return ip_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CheckFail(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 当校验不通过时返回的函数
Implementation : 当校验不通过时执行的函数
-----------------------------------------------------------------------------*/
uint8_t *Communication_CheckFail(uint8_t* buf)
{
	uint8_t copy_buf[5];
	static uint8_t check_fail_info[13] = {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0d,0x47,0x42,0x4d,0x53,0xe1,0x3f};
	
	memcpy(copy_buf, buf, 5);
	
	check_fail_info[7] = copy_buf[1];
	check_fail_info[8] = copy_buf[2];
	check_fail_info[9] = copy_buf[3];
	check_fail_info[10] = copy_buf[4];
	
	check_fail_info[12] = Communication_BytesChecker(check_fail_info, 13);
	return check_fail_info;
}
			   
/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_CheckFail(uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 当数据非法时返回的函数
Implementation : 当数据非法时执行的函数
-----------------------------------------------------------------------------*/
uint8_t *Communication_IllegalData(uint8_t* buf)
{
	uint8_t copy_buf[5];
	static uint8_t illegal_data_info[13] = {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0d,0x47,0x42,0x4d,0x53,0xe4,0x3f};
	
	memcpy(copy_buf, buf, 5);
	
	illegal_data_info[7] = copy_buf[1];
	illegal_data_info[8] = copy_buf[2];
	illegal_data_info[9] = copy_buf[3];
	illegal_data_info[10] = copy_buf[4];
	
	illegal_data_info[12] = Communication_BytesChecker(illegal_data_info, 13);
	return illegal_data_info;
}
/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_LED_IllegalData(uint8_t lightbox_type,uint8_t* buf);
Parameters     : uint8_t* buf接收到的字符串
Return         : 当数据非法时返回的函数
Implementation : ,LED通信错误时数据非法时执行的函数
-----------------------------------------------------------------------------*/
uint8_t *Communication_LED_IllegalData(uint8_t lightbox_type,uint8_t* buf)
{
	uint8_t copy_buf[5];
	static uint8_t illegal_data_info[14] = {0x24,0x46,0x4b,0x58,0x58,
			0x00,0x0e,0x00,0x47,0x42,0x4d,0x53,0xe4,0x3f};
	
	memcpy(copy_buf, buf, 5);
	illegal_data_info[7] = lightbox_type;
	illegal_data_info[8] = copy_buf[1];
	illegal_data_info[9] = copy_buf[2];
	illegal_data_info[10] = copy_buf[3];
	illegal_data_info[11] = copy_buf[4];
	
	illegal_data_info[13] = Communication_BytesChecker(illegal_data_info, 14);
	return illegal_data_info;
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRPerform(uint8_t ir_command)
Parameters     : uint8_t ir_command    红外执行指令
Return         : 返回字符串的首地址
Implementation : 红外码执行，   "HWZX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRPerform(uint8_t ir_command)
{
	static uint8_t infrared_info[13] = {'$','F','K','X','X',
				0,13,'H','W','Z','X',0,0};

#ifdef	BG_INFRARED
	infrared_info[11] = APP_Infrared_Send(ir_command);
	infrared_info[12] = Communication_BytesChecker(infrared_info, 13);
#endif
				
	return(infrared_info);
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRLearn(uint8_t ir_command)
Parameters     : uint8_t ir_command 红外学习指令
Return         : 返回字符串的首地址
Implementation : 红外码学习   "HWXX"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRLearn(uint8_t ir_command)
{
	static uint8_t infrared_info[13] = {'$','F','K','X','X',
				0,13,'H','W','X','X',0,0};

#ifdef	BG_INFRARED
	infrared_info[11] = APP_Infrared_Receive(ir_command);
	infrared_info[12] = Communication_BytesChecker(infrared_info, 13);
#endif
				
	return(infrared_info);			
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRImportData(uint8_t* import_buf_addr)
Parameters     : uint8_t* data_buf_addr 导入首地址
Return         : 返回字符串的首地址
Implementation : 红外码导入  "HWDR"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRImportData(uint8_t* import_buf_addr)
{
	uint16_t data_size = 0;
	static uint8_t infrared_info[13] = {'$','F','K','X','X',
				0,13,'H','W','D','R',0,0};

#ifdef	BG_INFRARED
	data_size = import_buf_addr[5];
	data_size = (data_size<<8) | import_buf_addr[6];
	infrared_info[11] = APP_Infrared_Download(import_buf_addr+7,data_size);
	infrared_info[12] = Communication_BytesChecker(infrared_info, 13);
#endif
				
	return(infrared_info);
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t *Communication_IRExportData(uint8_t* export_buf_addr)
Parameters     : uint8_t* export_buf_addr 导出首地址
Return         : 返回字符串的首地址
Implementation : 红外码导出  "HWZL"
-----------------------------------------------------------------------------*/
uint8_t *Communication_IRExportData(uint8_t* export_buf_addr)
{
	
#ifdef	BG_INFRARED
	uint16_t i = 0;
	uint8_t* p_buf = export_buf_addr;
	static uint8_t infrared_info_yes[7] = {'$','H','W','Z','L',
				0,158};
	for(i = 0; i<7; i++)
	{
		p_buf[i] = infrared_info_yes[i];
	}
	APP_Infrared_Export(p_buf+i);
	p_buf[157] = Communication_BytesChecker(p_buf, 158);
#endif
	
	return(export_buf_addr);
}

#endif /* End of 1, Just a tag, ignore it */
