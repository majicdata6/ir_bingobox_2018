#include "main.h" 
/**************************************************************************//**
 * @brief Default Network Inforamtion
 *****************************************************************************/
wiz_NetInfo gWIZNETINFO = {
#ifdef  BG_LED
			    .mac = {0x00, 0x08, 0xdc, 0x63, 0xab, 0xcd},
                            .ip = {192, 168, 1, 63},
#elif   defined(BG_DOOR_LOCK)
			    .mac = {0x00, 0x08, 0xdc, 0x64, 0xab, 0xcd},
                            .ip = {192, 168, 1, 64},
#elif   defined(BG_485_METER)
			    .mac = {0x00, 0x08, 0xdc, 0x65, 0xab, 0xcd},
                            .ip = {192, 168, 1, 65},
#elif   defined(BG_SMOKE_ALARM)
			    .mac = {0x00, 0x08, 0xdc, 0x66, 0xab, 0xcd},
                            .ip = {192, 168, 1, 66},
#elif	defined(BG_INFRARED)
               .mac = {0x00, 0x08, 0xdc, 0x66, 0xab, 0xcd},
                            .ip = {192,168,3,67},
#else
			    .mac = {0x00, 0x08, 0xdc, 0x80, 0xab, 0xcd},
                            .ip = {192, 168, 1, 80},
#endif
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 3, 1},
                            .dns = {0, 0, 0, 0},
                            .dhcp = NETINFO_STATIC };
/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype      : void APP_W5500_NetworkInit(void)
Parameters     : void
Return         : void
Implementation : 对硬件进行网络信息的设置
-----------------------------------------------------------------------------*/
void APP_W5500_NetworkInit(void)
{
#ifdef BG_INFO
        uint8_t ip[5];
        Info_ReadIp(ip);
        #ifdef BG_UART_DEBUG
            printf("IP: %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
	#endif
        if((ip[0]==192)&&(ip[1]==168)&&(ip[2]==1))
        {
            memcpy(gWIZNETINFO.ip,ip,4);
         
        #ifdef BG_UART_DEBUG
            printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], \
                      gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);	
	#endif
        }
#endif

#ifdef USE_UNIQUE_CHIP_IP_SET_MAC	
	/*
	芯片全球唯一ID为三个字,96bit
	(0x1FFFF7AC);  
	(0x1FFFF7B0);  
	(0x1FFFF7B4);  
	下面通过芯片中间ID来初始化mac地址,根据单播规定,mac地址的首位为0x00
	*/
	uint8_t ChipUniqueID[6];
	uint32_t CpuID[3];
		
	CpuID[0]=*(__IO uint32_t*)(0x1FFFF7AC);
	CpuID[1]=*(__IO uint32_t*)(0x1FFFF7B0);
	CpuID[2]=*(__IO uint32_t*)(0x1FFFF7B4);
	printf("rn########### 芯片唯一ID为: %X-%X-%X \r\n",
	CpuID[0],CpuID[1],CpuID[2]);  
	ChipUniqueID[0] = 0x00;
	ChipUniqueID[1]=(*(uint32_t*)(0x1FFFF7AC))&(0xFF);
	ChipUniqueID[2]=((*(uint32_t*)(0x1FFFF7AC))>>8)&(0xFF);
	ChipUniqueID[3]=((*(uint32_t*)(0x1FFFF7AC))>>16)&(0xFF);
	ChipUniqueID[4]=((*(uint32_t*)(0x1FFFF7AC))>>24)&(0xFF);
	ChipUniqueID[5]=(*(uint32_t*)(0x1FFFF7B0))&(0xFF);
	
	printf("rn########### 截取ID为: %X-%X-%X-%X-%X \r\n",
	ChipUniqueID[1],ChipUniqueID[2],ChipUniqueID[3],ChipUniqueID[4],ChipUniqueID[5]);  
	
	memcpy(gWIZNETINFO.mac,ChipUniqueID,6);
#endif
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
#ifdef BG_UART_DEBUG	
	uint8_t tmpstr[6];
        ctlwizchip(CW_GET_ID,(void*)tmpstr);
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],\
                  gWIZNETINFO.mac[1], gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],\
                  gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1],\
                  gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1],\
                  gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1],\
                  gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1],\
                  gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
	printf("======================\r\n");
#endif
}
/*-----------------------------------------------------------------------------
Prototype      : void APP_W5500_Init(void)
Parameters     : void
Return         : void
Implementation : 先初始化SPI,再初始化W5500
-----------------------------------------------------------------------------*/
void APP_W5500_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
  	SPI_Init();
	/* 
        First of all, Should register SPI callback functions implemented 
        by user for accessing WIZCHIP
        */ 
    	/* Critical section callback */
    	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   
    	/* Chip selection call back */
#if     _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    	reg_wizchip_cs_cbfunc(SPI_CsSelect, SPI_CsDeselect);
#elif   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	// CS must be tried with LOW.
    	reg_wizchip_cs_cbfunc(SPI_CsSelect, SPI_CsDeselect);  
#else
   	#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      		#error "Unknown _WIZCHIP_IO_MODE_"
   	#else
      	reg_wizchip_cs_cbfunc(SPI_CsSelect, SPI_CsDeselect);
   	#endif
#endif
    	/* SPI Read & Write callback function */
    	reg_wizchip_spi_cbfunc(SPI_Recv,SPI_Send); 
	W5500_RST_GPIO_CLK_ENABLE();
	/* Configure the GPIO_RST pin */
	GPIO_InitStruct.Pin = W5500_RST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(W5500_RST_GPIO_PORT, &GPIO_InitStruct);
		
        APP_W5500_Reset();
	/* WIZCHIP SOCKET Buffer initialize */
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{	
		//printf("WIZCHIP Initialized fail.\r\n");
     		while(1);
	}
}
/*-----------------------------------------------------------------------------
Prototype      : void APP_W5500_Reset(void)
Parameters     : void
Return         : void
Implementation : 复位W5500
-----------------------------------------------------------------------------*/
void APP_W5500_Reset(void)
{
	HAL_GPIO_WritePin(W5500_RST_GPIO_PORT, W5500_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(W5500_RST_GPIO_PORT, W5500_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(10);	
}
