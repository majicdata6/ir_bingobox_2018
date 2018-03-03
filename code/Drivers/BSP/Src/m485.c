/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅
Description : 485相关
-----------------------------------------------------------------------------*/

/* include ANSI C .h file */
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "main.h"

/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/
#define CONFIG_485_RX_MODE HAL_GPIO_WritePin(M485REDE_GPIO_PORT, M485REDE_PIN, GPIO_PIN_RESET)
#define CONFIG_485_TX_MODE HAL_GPIO_WritePin(M485REDE_GPIO_PORT, M485REDE_PIN, GPIO_PIN_SET)

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

/*---------------------------------------------------------------------------*/
//	Extern
/*---------------------------------------------------------------------------*/
/* Variables */
extern uint32_t g_485_rx_wait_time;


uint8_t g_uart_rx_finish_b = FALSE;

/* Function prototypes */
extern void UART_ResetRxPointer(void); 
/* UART handler declaration */
UART_HandleTypeDef Uart1Handle;
__IO ITStatus Uart1Ready = RESET;

void USART1_Init(void)
{
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
	- Word Length = 8 Bits
	- Stop Bit = One Stop bit
	- Parity = None
	- BaudRate = 9600 baud
	- Hardware flow control disabled (RTS and CTS signals) */
	Uart1Handle.Instance        = USART1x;

	Uart1Handle.Init.BaudRate   = 9600;
	Uart1Handle.Init.WordLength = UART_WORDLENGTH_9B;
	Uart1Handle.Init.StopBits   = UART_STOPBITS_1;
	Uart1Handle.Init.Parity     = UART_PARITY_EVEN;
	Uart1Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	Uart1Handle.Init.Mode       = UART_MODE_TX_RX;
	Uart1Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&Uart1Handle) != HAL_OK)
	{
		while(1);
	}
}
/*---------------------------------------------------------------------------*/
//	Static Functions
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype      : void Meter485_Init(void)
Parameters     : (Nothing)
Return         : (Nothing)
Implementation : 根据datasheet初始化引脚
-----------------------------------------------------------------------------*/
void Meter485_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO_LED clock */
	M485REDE_GPIO_CLK_ENABLE();  

	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = M485REDE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(M485REDE_GPIO_PORT, &GPIO_InitStruct);
	//待确定是高电平还是低电平
	HAL_GPIO_WritePin(M485REDE_GPIO_PORT, M485REDE_PIN, GPIO_PIN_SET); 
	USART1_Init();
}

/*-----------------------------------------------------------------------------
Prototype      : void Meter485_Send(U8 *str,U16 len)
Parameters     : str - 数据缓冲区首地址
		 len - 发送数据长度
Return         : (Nothing)
Implementation : 根据485手册和原理图，需要切换发送和接收功能脚
-----------------------------------------------------------------------------*/
void Meter485_Send(uint8_t *str, uint16_t len)
{        
#ifdef BG_485_METER
	//切换485发送
        CONFIG_485_TX_MODE;
	if(HAL_UART_Transmit_IT(&Uart1Handle, str,len)!= HAL_OK)
	{
		Error_Handler();
	}
	while (Uart1Ready != SET)
	{
	}
	Uart1Ready = RESET;
	
	CONFIG_485_RX_MODE;
#endif
}


/*-----------------------------------------------------------------------------
Prototype      : uint8_t Meter485_ReceiveData(void *rec_buf,uint32_t msec)
Parameters     : rec_buf - 存放接收数据的缓冲区
		 msec - 阻塞等待的时间(ms),大于0
Return         : 返回读回数据的长度，返回0则表示失败或者数据没有准备好
Implementation : 在msec时间内，循环读取接收缓冲区的数据，直到数据就绪，或者超时
-----------------------------------------------------------------------------*/
uint8_t Meter485_ReceiveData(void *rec_buf, uint32_t msec)
{
        uint8_t len=0;
#ifdef BG_485_METER
	if((!rec_buf) || (0 == msec)){return 0;}
        
	g_485_rx_wait_time = msec;
        if(HAL_UART_Receive_IT(&Uart1Handle, rec_buf,1024)!= HAL_OK)
	{
		Error_Handler();
	}
        
	while(g_485_rx_wait_time)
	{
                if(g_uart_rx_finish_b)
                {
                        g_uart_rx_finish_b = FALSE;
                        break;
                }
                
		
	}
        len = Uart1Handle.RxXferSize - Uart1Handle.RxXferCount;
        CLEAR_BIT(Uart1Handle.Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));

        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        CLEAR_BIT(Uart1Handle.Instance->CR3, USART_CR3_EIE);

        /* Rx process is completed, restore huart->RxState to Ready */
        Uart1Handle.RxState = HAL_UART_STATE_READY;
        
        Uart1Handle.RxXferCount = 0;
#endif
	//返回读回的数据长度
	return len;
}

