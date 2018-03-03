/*-----------------------------------------------------------------------------
  Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ??
Description : ???????info memory
-----------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/*---------------------------------------------------------------------------*/
	//Defines and Macros 
/*---------------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define SIZE_USER_FLASH_FIRMWARE_VER 	4
#define SIZE_USER_FLASH_HARDWARE_VER 	4
#define SIZE_USER_FLASH_W5500_IP 	4
#define SIZE_USER_FLASH_AUTO_LOCK_TIME 	1
#define SIZE_USER_FLASH_LIGHT1_LEVEL 	1
#define SIZE_USER_FLASH_LIGHT2_LEVEL 	1
/*---------------------------------------------------------------------------*/
	//Types and Enums
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
	//Constants
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
	//Variables
/*---------------------------------------------------------------------------*/
/* Private/Static */
FLASH_UserInfoTypeDef UserInfoStruct;
/*
 * Function   :
 * Value Area :
 */

/* Global */
/*---------------------------------------------------------------------------*/
	//Extern
/*---------------------------------------------------------------------------*/
/* Variables */
extern wiz_NetInfo gWIZNETINFO;


/* Function prototypes */

/*---------------------------------------------------------------------------*/
	//Static Functions
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype      :
Parameters     :
Return         :
Implementation :
-----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
	//Public Functions
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype      : printf_array()
Parameters     : array数组指针,size打印的长度
Return         : void
Implementation : 打印指定的长度的数组
-----------------------------------------------------------------------------*/
void printf_array(uint8_t *array,uint8_t size)
{
	for(uint8_t i=0;i<size;i++)
	{
		printf("0x%X ",array[i]);
	}
	printf("\r\n");
}
/*-----------------------------------------------------------------------------
Prototype      : Info_MemoryAllWrite()
Parameters     : void
Return         : void
Implementation : 把用户参数全部写入进去
-----------------------------------------------------------------------------*/
void Info_MemoryAllWrite(void)
{
	uint8_t i = 0;
	uint32_t  Address = FLASH_USER_START_ADDR;
	/*写操作和擦除操作都先需要解锁,而且写操作前需要擦除flash*/
	HAL_FLASH_Unlock();
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.falsh_is_init_flag));
	Flash_User_Write_One_Word(Address,(uint32_t)UserInfoStruct.falsh_is_init_flag);
	Address = Address + 4;
	
	for(i = 0;i < SIZE_USER_FLASH_FIRMWARE_VER;i++)
	{
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_firmware_ver[i]));
		Flash_User_Write_One_Word(Address,(uint32_t)(UserInfoStruct.flash_firmware_ver[i]));
		Address = Address + 4;
	}
	for(i = 0;i < SIZE_USER_FLASH_HARDWARE_VER;i++)
	{
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_hardware_ver[i]));
		Flash_User_Write_One_Word(Address,(uint32_t)(UserInfoStruct.flash_hardware_ver[i]));
		Address = Address + 4;
	}
	for(i = 0;i < SIZE_USER_FLASH_W5500_IP;i++)
	{
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_w5500_ip[i]));
		Flash_User_Write_One_Word(Address,(uint32_t)(UserInfoStruct.flash_w5500_ip[i]));
		Address = Address + 4;
	}
	
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_audo_lock_time));
	Flash_User_Write_One_Word(Address,(uint32_t)UserInfoStruct.flash_audo_lock_time);
	Address = Address + 4;
	
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_light1_level));
	Flash_User_Write_One_Word(Address,(uint32_t)UserInfoStruct.flash_light1_level);
	Address = Address + 4;
	
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_light2_level));
	Flash_User_Write_One_Word(Address,(uint32_t)UserInfoStruct.flash_light2_level);
	HAL_FLASH_Lock();
}
/*-----------------------------------------------------------------------------
Prototype      : Info_MemoryAllRead()
Parameters     : void
Return         : void
Implementation : 读取全部的用户参数
-----------------------------------------------------------------------------*/
void Info_MemoryAllRead(void)
{
	uint8_t i = 0;
	uint32_t  Address = FLASH_USER_DATA_ADDR;
	for(i = 0;i < SIZE_USER_FLASH_FIRMWARE_VER;i++)
	{
		UserInfoStruct.flash_firmware_ver[i] = (uint8_t)Flash_User_Read_One_Word(Address);
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_firmware_ver[i]));
		Address = Address + 4;
	}
	for(i = 0;i < SIZE_USER_FLASH_HARDWARE_VER;i++)
	{
		UserInfoStruct.flash_hardware_ver[i] = (uint8_t)Flash_User_Read_One_Word(Address);
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_hardware_ver[i]));
		Address = Address + 4;
	}
	for(i = 0;i < SIZE_USER_FLASH_W5500_IP;i++)
	{
		UserInfoStruct.flash_w5500_ip[i] = (uint8_t)Flash_User_Read_One_Word(Address);
		//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_w5500_ip[i]));
		Address = Address + 4;
	}
	
	UserInfoStruct.flash_audo_lock_time = (uint16_t)Flash_User_Read_One_Word(Address);
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_audo_lock_time));
	Address = Address + 4;
	
	UserInfoStruct.flash_light1_level = (uint8_t)Flash_User_Read_One_Word(Address);
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_light1_level));
        Address = Address + 4;
	
	UserInfoStruct.flash_light2_level = (uint8_t)Flash_User_Read_One_Word(Address);
	//printf("addr:0x%X,0x%X\r\n",Address,(uint32_t)(UserInfoStruct.flash_light2_level));
}
/*-----------------------------------------------------------------------------
Prototype      : Info_MemoryInit()
Parameters     : void
Return         : void
Implementation : info memory?????
-----------------------------------------------------------------------------*/
void Info_MemoryInit()
{
	uint32_t  Address = FLASH_USER_FLAG_ADDR;
        //char *ptr = (char *)FLASH_USER_START_ADDR;
	Flash_User_Init();
	uint32_t falsh_is_init_flag = 0xfefefefe;

	#ifdef BG_485_METER
		uint8_t firmware_ver[4]={1,1,1,0};//1.0.10
	#endif
	#ifdef BG_LED 
		uint8_t firmware_ver[4]={1,2,1,0};//1.0.10
	#endif
	#ifdef BG_DOOR_LOCK
		uint8_t firmware_ver[4]={1,3,1,0};//1.0.10
	#endif
	#ifdef BG_SMOKE_ALARM
		uint8_t firmware_ver[4]={1,4,1,0};//1.0.10
	#endif
	#ifdef BG_INFRARED
        uint8_t firmware_ver[4]={1,5,0,0};//1.0.00
	#endif
	uint8_t hardware_ver[4]={1,0,1,0};//1.0.10
	
	memcpy((UserInfoStruct.flash_firmware_ver),firmware_ver,4);
	memcpy((UserInfoStruct.flash_hardware_ver),hardware_ver,4);
	memcpy((UserInfoStruct.flash_w5500_ip),gWIZNETINFO.ip,4);
	UserInfoStruct.flash_audo_lock_time = 5;
	UserInfoStruct.flash_light1_level = 3;
	UserInfoStruct.flash_light2_level = 3;
	UserInfoStruct.falsh_is_init_flag = 0xfefefefe;

//	Flash_User_Erase();
        if(falsh_is_init_flag != Flash_User_Read_One_Word(Address))
        {
		printf("第一次初始化[%s][%d]\r\n",__FUNCTION__,__LINE__);
		Flash_User_Erase();
		Info_MemoryAllWrite();	
        }
	Info_MemoryAllRead();
}

/*-----------------------------------------------------------------------------
Prototype      : Info_WriteIp()
Parameters     : void
Return         : void
Implementation : ??IP??
-----------------------------------------------------------------------------*/
void Info_WriteIp(uint8_t *ip_info)
{
	memcpy((UserInfoStruct.flash_w5500_ip),ip_info,4);
	Flash_User_Erase();
	Info_MemoryAllWrite();
}

/*-----------------------------------------------------------------------------
Prototype      : Info_WriteLockTimer()
Parameters     : void
Return         : void
Implementation : ???????
-----------------------------------------------------------------------------*/
void Info_WriteLockTimer(uint16_t *timer_info)
{
	UserInfoStruct.flash_audo_lock_time = *timer_info;
	printf("[%s][%d]:%d\r\n",__FUNCTION__,__LINE__,*timer_info);
	Flash_User_Erase();
	Info_MemoryAllWrite();
}

/*-----------------------------------------------------------------------------
Prototype      : Info_WriteLightLevel()
Parameters     : void
Return         : void
Implementation : ????????
-----------------------------------------------------------------------------*/
void Info_WriteLightLevel(uint8_t lightbox_type,char *level_info)
{	
	if(LIGHTBOX1 == lightbox_type)
	{
		UserInfoStruct.flash_light1_level = *level_info;
	}
	else if(LIGHTBOX2 == lightbox_type)
	{
		UserInfoStruct.flash_light2_level = *level_info;
	}
	Flash_User_Erase();
	Info_MemoryAllWrite();
}

/*-----------------------------------------------------------------------------
Prototype      : Info_ReadHardVer()
Parameters     : char *hard_info
Return         : void
Implementation : ??info memory?????
-----------------------------------------------------------------------------*/
void Info_ReadHardVer(char *hard_info)
{

        snprintf(hard_info,5,"%c%c%c%c",UserInfoStruct.flash_hardware_ver[0],UserInfoStruct.flash_hardware_ver[1],\
					UserInfoStruct.flash_hardware_ver[2],UserInfoStruct.flash_hardware_ver[3]);
}

/*-----------------------------------------------------------------------------
Prototype      : Info_ReadSoftVer()
Parameters     : char *soft_info
Return         : void
Implementation : ??info memory?????
-----------------------------------------------------------------------------*/
void Info_ReadSoftVer(char *soft_info)
{

        snprintf(soft_info,5,"%c%c%c%c",UserInfoStruct.flash_firmware_ver[0],UserInfoStruct.flash_firmware_ver[1],\
					UserInfoStruct.flash_firmware_ver[2],UserInfoStruct.flash_firmware_ver[3]);
}

/*-----------------------------------------------------------------------------
Prototype      : Info_ReadIp()
Parameters     : uint8_t *ip_info
Return         : void
Implementation : ??info memory?IP??
-----------------------------------------------------------------------------*/
void Info_ReadIp(uint8_t *ip_info)
{
        snprintf((char *)ip_info,5,"%c%c%c%c%c",UserInfoStruct.flash_w5500_ip[0],UserInfoStruct.flash_w5500_ip[1],\
					UserInfoStruct.flash_w5500_ip[2],UserInfoStruct.flash_w5500_ip[3],0x00);
}

/*-----------------------------------------------------------------------------
Prototype      : Info_ReadTimer()
Parameters     : uint16_t *timer_info
Return         : void
Implementation : ??info memory?????
-----------------------------------------------------------------------------*/
void Info_ReadTimer(uint16_t *timer_info)
{
	*timer_info = UserInfoStruct.flash_audo_lock_time;
}

/*-----------------------------------------------------------------------------
Prototype      : Info_ReadLevel()
Parameters     : uint8_t *timer_info
Return         : void
Implementation : ??info memory?????
-----------------------------------------------------------------------------*/
void Info_ReadLevel(uint8_t lightbox_type,uint8_t *level_info)
{
	if(LIGHTBOX1 == lightbox_type)
	{
		*level_info = UserInfoStruct.flash_light1_level;
	}
	else if(LIGHTBOX2 == lightbox_type)
	{
		*level_info = UserInfoStruct.flash_light2_level;
	}	
}

