/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧
Description : 485���
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
Implementation : ����datasheet��ʼ������
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
	//��ȷ���Ǹߵ�ƽ���ǵ͵�ƽ
	HAL_GPIO_WritePin(M485REDE_GPIO_PORT, M485REDE_PIN, GPIO_PIN_SET); 
	USART1_Init();
}

/*-----------------------------------------------------------------------------
Prototype      : void Meter485_Send(U8 *str,U16 len)
Parameters     : str - ���ݻ������׵�ַ
		 len - �������ݳ���
Return         : (Nothing)
Implementation : ����485�ֲ��ԭ��ͼ����Ҫ�л����ͺͽ��չ��ܽ�
-----------------------------------------------------------------------------*/
void Meter485_Send(uint8_t *str, uint16_t len)
{        
#ifdef BG_485_METER
	//�л�485����
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
Parameters     : rec_buf - ��Ž������ݵĻ�����
		 msec - �����ȴ���ʱ��(ms),����0
Return         : ���ض������ݵĳ��ȣ�����0���ʾʧ�ܻ�������û��׼����
Implementation : ��msecʱ���ڣ�ѭ����ȡ���ջ����������ݣ�ֱ�����ݾ��������߳�ʱ
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
	//���ض��ص����ݳ���
	return len;
}

