#include "main.h"
/*---------------------------------------------------------------------------*/
//	Variables
/*---------------------------------------------------------------------------*/
/* Private/Static */
/*
 * Function   : 
 * Value Area : 
 */
 
/*Global*/
uint8_t g_smoke_alarm_status = 0;
/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Prototype      : void Smoke_Alarm_Init(void)
Parameters     : void
Return         : void
Implementation : IO口设置为输入，中断触发方式为下降沿触发
-----------------------------------------------------------------------------*/
void Smoke_Alarm_Init(void)
{
	/*GPIO初始化，设置为输入上拉模式*/
	GPIO_InitTypeDef   GPIO_InitStruct;
	
	/* Enable GPIOA clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = SMOKE_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SMOKE_GPIO, &GPIO_InitStruct);
	
	/* EXTI interrupt init*/	
	HAL_NVIC_SetPriority(SMOKE_EXTI_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(SMOKE_EXTI_IRQn);
}

/*-----------------------------------------------------------------------------
Prototype      : uint16_t Smoke_Alarm_Read()
Parameters     : void
Return         : void
Implementation : 读取烟雾传感器的IO口电平，0为没有烟雾
-----------------------------------------------------------------------------*/
uint16_t Smoke_Alarm_Read()
{
	return (HAL_GPIO_ReadPin(SMOKE_GPIO,SMOKE_GPIO_PIN) == 0)?1:0;
}

//******************************************************************************
//
//This is the PORT1_VECTOR interrupt vector service routine
//
//******************************************************************************
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == SMOKE_GPIO_PIN)
	{
		g_smoke_alarm_status = 
		(HAL_GPIO_ReadPin(SMOKE_GPIO,SMOKE_GPIO_PIN) == 0)?1:0;
	}
}
