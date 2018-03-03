#include "main.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
UART_HandleTypeDef Uart2Handle;
__IO ITStatus Uart2Ready = RESET;

void USART2_Init(void)
{
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
	- Word Length = 8 Bits
	- Stop Bit = One Stop bit
	- Parity = None
	- BaudRate = 9600 baud
	- Hardware flow control disabled (RTS and CTS signals) */
	Uart2Handle.Instance        = USART2x;

	Uart2Handle.Init.BaudRate   = 9600;
	Uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
	Uart2Handle.Init.StopBits   = UART_STOPBITS_1;
	Uart2Handle.Init.Parity     = UART_PARITY_NONE;
	Uart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	Uart2Handle.Init.Mode       = UART_MODE_TX_RX;
	Uart2Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&Uart2Handle) != HAL_OK)
	{
		while(1);
	}
}
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */  
	if(HAL_UART_Transmit_IT(&Uart2Handle, (uint8_t *)&ch,1)!= HAL_OK)
	{
		Error_Handler();
	}
	while (Uart2Ready != SET)
	{
	}
	Uart2Ready = RESET;
	return (ch);
}


