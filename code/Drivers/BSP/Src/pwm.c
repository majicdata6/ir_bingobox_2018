#include "main.h"
/* Timer handler declaration */
TIM_HandleTypeDef    PwmTimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef PwmSConfig;

PWM_ChannelTypeDef pwm_channel1;
PWM_ChannelTypeDef pwm_channel2;


/*-----------------------------------------------------------------------------
Prototype      : void Pwm_GetChangeRate(PWM_ChannelTypeDef * Ptr)
Parameters     : (PWM_ChannelTypeDef * Ptr
Return         : void
Implementation : Get pwm change rate. (float)
-----------------------------------------------------------------------------*/
static void Pwm_GetChangeRate( PWM_ChannelTypeDef * Ptr)
{
        if(NULL == Ptr){return;}
        PWM_ChannelTypeDef *PWM_ChannelPtr = Ptr;
        
        uint32_t tmp = 0;
        float f_temp=0.0;
        if(PWM_ChannelPtr->current_pwm_value > PWM_ChannelPtr->pwm_value)
        {
                tmp = PWM_ChannelPtr->current_pwm_value - PWM_ChannelPtr->pwm_value;
        }
        else
        {
                 tmp = PWM_ChannelPtr->pwm_value - PWM_ChannelPtr->current_pwm_value;
        }
        
        f_temp = tmp / (PWM_CHANGE_OF_EACH_TIME*1000.0);
        
        PWM_ChannelPtr->pwm_change_rate = f_temp /PWM_CHANGE_TIMEROUT;
}

/*-----------------------------------------------------------------------------
Prototype      : void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
Parameters     : TIM_HandleTypeDef *htim
Return         : void
Implementation : Initializes the TIM PWM MSP.
-----------------------------------------------------------------------------*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef   GPIO_InitStruct;
	/*##-1- Enable peripherals and GPIO Clocks ##*/
	/* TIMx Peripheral clock enable */
	PWM_TIMx_CLK_ENABLE();

	/* Enable GPIO Channels Clock */
	TIMx_CHANNEL_GPIO_PORT();

	/* Configure PB.04 (TIM3_Channel1), PB.05 (TIM3_Channel2), PB.00 (TIM3_Channel3),
	PB.01 (TIM3_Channel4) in output, push-pull, alternate function mode
	*/
	/* Common configuration for all channels */
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL1;
	GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
	HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

	GPIO_InitStruct.Alternate = TIMx_GPIO_AF_CHANNEL2;
	GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL2;
	HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

}
/*-----------------------------------------------------------------------------
Prototype      : void ChangePwm()
Parameters     : uint32_t percent
Return         : void
Implementation : 改变PWM频率前的初始化
-----------------------------------------------------------------------------*/
void Pwm_Change_Init(uint8_t lightbox_type,uint32_t percent)
{
	if(LIGHTBOX1 == lightbox_type)
	{
		pwm_channel1.pwm_value = (uint32_t)(PERIOD_VALUE*percent/100);
		pwm_channel1.pwm_channel = TIM_CHANNEL_1;
		pwm_channel1.current_pwm_value = __HAL_TIM_GET_COMPARE(&PwmTimHandle, pwm_channel1.pwm_channel);
		pwm_channel1.pwm_flag = 1;
		pwm_channel1.pwm_timer_count = 0;
                

                Pwm_GetChangeRate(&pwm_channel1);
                
		if(pwm_channel1.current_pwm_value <= pwm_channel1.pwm_value)
		{
			pwm_channel1.pwm_up_down_flag = PWM_VALUE_UP;
		}
		else
		{
			pwm_channel1.pwm_up_down_flag = PWM_VALUE_DOWN;

		}
	}
	if(LIGHTBOX2 == lightbox_type)
	{
		pwm_channel2.pwm_value = (uint32_t)(PERIOD_VALUE*percent/100);
		pwm_channel2.pwm_channel = TIM_CHANNEL_2;
		pwm_channel2.current_pwm_value = __HAL_TIM_GET_COMPARE(&PwmTimHandle, pwm_channel2.pwm_channel);
		pwm_channel2.pwm_flag = 1;
		pwm_channel2.pwm_timer_count = 0;
                
                Pwm_GetChangeRate(&pwm_channel2);
                
		if(pwm_channel2.current_pwm_value <= pwm_channel2.pwm_value)
		{
			pwm_channel2.pwm_up_down_flag = PWM_VALUE_UP;
		}
		else
		{
			pwm_channel2.pwm_up_down_flag = PWM_VALUE_DOWN;

		}
	}

}
/*-----------------------------------------------------------------------------
Prototype      : void PWM_Change_ValueOfChannel(uint32_t pwm_channel,uint32_t pwm_value)
Parameters     : pwm_channel通道,pwm_value通道pwm值
Return         : void
Implementation : 修改某个通道的pwm值
-----------------------------------------------------------------------------*/
void PWM_Change_ValueOfChannel(uint32_t pwm_channel,uint32_t pwm_value)
{	
	(*(__IO uint32_t *)(&((&PwmTimHandle)->Instance->CCR1) + ((pwm_channel) >> 2U)) = (pwm_value));
	//_HAL_TIM_SET_COMPARE(&PwmTimHandle, pwm_channel, pwm_value);
}
/*-----------------------------------------------------------------------------
Prototype      : void PWM_Change_Use_Timer(void)
Parameters     : void
Return         : void
Implementation : 通过1ms定时器渐变式改变pwm,一直达到所修改的pwm占空比
-----------------------------------------------------------------------------*/
void PWM_Change_Use_Timer(void)
{
	if(pwm_channel1.pwm_flag == 1)
	{
		pwm_channel1.pwm_timer_count ++;
		if(pwm_channel1.pwm_timer_count >= PWM_CHANGE_TIMEROUT)
		{
			pwm_channel1.pwm_timer_count = 0;
			if(1 == pwm_channel1.pwm_up_down_flag)
			{
				if(pwm_channel1.current_pwm_value < pwm_channel1.pwm_value)
				{
                                        pwm_channel1.current_pwm_value_f += pwm_channel1.pwm_change_rate;
                                        pwm_channel1.current_pwm_value = (uint32_t)pwm_channel1.current_pwm_value_f;
					PWM_Change_ValueOfChannel(pwm_channel1.pwm_channel,pwm_channel1.current_pwm_value);			
				}
				else
				{      
					pwm_channel1.pwm_flag = 0;
					pwm_channel1.pwm_up_down_flag = PWM_VALUE_UP_DOWN_INIT;
				}	
			}
			else if(2 == pwm_channel1.pwm_up_down_flag)
			{
				if(pwm_channel1.current_pwm_value > pwm_channel1.pwm_value)
				{
                                        pwm_channel1.current_pwm_value_f -= pwm_channel1.pwm_change_rate;
                                        if(pwm_channel1.current_pwm_value_f < 0)
                                        {
                                                pwm_channel1.current_pwm_value_f = 0;
                                        }
                                        pwm_channel1.current_pwm_value = (uint32_t)pwm_channel1.current_pwm_value_f;
					PWM_Change_ValueOfChannel(pwm_channel1.pwm_channel,pwm_channel1.current_pwm_value);						
				}
				else
				{      
					pwm_channel1.pwm_flag = 0;
					pwm_channel1.pwm_up_down_flag = PWM_VALUE_UP_DOWN_INIT;
				}	
			}			
		}
	}
	if(pwm_channel2.pwm_flag == 1)
	{
		pwm_channel2.pwm_timer_count ++;
		if(pwm_channel2.pwm_timer_count >= PWM_CHANGE_TIMEROUT)
		{
			pwm_channel2.pwm_timer_count = 0;
			if(1 == pwm_channel2.pwm_up_down_flag)
			{
				if(pwm_channel2.current_pwm_value <= pwm_channel2.pwm_value)
				{
                                        pwm_channel2.current_pwm_value_f += pwm_channel2.pwm_change_rate;
                                        pwm_channel2.current_pwm_value = (uint32_t)pwm_channel2.current_pwm_value_f;
					PWM_Change_ValueOfChannel(pwm_channel2.pwm_channel,pwm_channel2.current_pwm_value);	
					//pwm_channel2.current_pwm_value++; 
				}
				else
				{
					pwm_channel2.pwm_flag = 0;
					pwm_channel2.pwm_up_down_flag = PWM_VALUE_UP_DOWN_INIT;
				}	
			}
			else if(2 == pwm_channel2.pwm_up_down_flag)
			{
				if(pwm_channel2.current_pwm_value > pwm_channel2.pwm_value)
				{
                                        pwm_channel2.current_pwm_value_f -= pwm_channel2.pwm_change_rate;
                                        if(pwm_channel2.current_pwm_value_f < 0)
                                        {
                                                pwm_channel2.current_pwm_value_f = 0;
                                        }
                                        pwm_channel2.current_pwm_value = (uint32_t)pwm_channel2.current_pwm_value_f;
                                        
					PWM_Change_ValueOfChannel(pwm_channel2.pwm_channel,pwm_channel2.current_pwm_value);		
					//pwm_channel2.current_pwm_value --;	
				}
				else
				{
					pwm_channel2.pwm_flag = 0;
					pwm_channel2.pwm_up_down_flag = PWM_VALUE_UP_DOWN_INIT;
				}	
			}			
		}
	}
}
/*-----------------------------------------------------------------------------
Prototype      : void PWM_Channel_Value_Init(PWM_ChannelTypeDef pwm_channel)
Parameters     : PWM_ChannelTypeDef pwm_channel
Return         : void
Implementation : 初始化pwm通道
-----------------------------------------------------------------------------*/
void PWM_Channel_Value_Init(PWM_ChannelTypeDef pwm_channel)
{
	pwm_channel.current_pwm_value = 0;
	pwm_channel.pwm_channel = 0;
	pwm_channel.pwm_flag = 0;
	pwm_channel.pwm_up_down_flag = PWM_VALUE_UP_DOWN_INIT;
	pwm_channel.pwm_value = 0;
	pwm_channel.pwm_timer_count = 0;
}
/*-----------------------------------------------------------------------------
Prototype      : void PWM_Init(void)
Parameters     : void
Return         : void
Implementation : 初始化PWM
-----------------------------------------------------------------------------*/
void PWM_Init(void)
{
	/* Counter Prescaler value */
	uint32_t uhPrescalerValue = 0;
	uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;
	PwmTimHandle.Instance = PWM_TIMx;

	PwmTimHandle.Init.Prescaler         = uhPrescalerValue;
	PwmTimHandle.Init.Period            = PERIOD_VALUE;
	PwmTimHandle.Init.ClockDivision     = 0;
	PwmTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	PwmTimHandle.Init.RepetitionCounter = 0;
	PwmTimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&PwmTimHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
	PwmSConfig.OCMode       = TIM_OCMODE_PWM1;
	PwmSConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	PwmSConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	PwmSConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	PwmSConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	PwmSConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* Set the pulse value for channel 1 */
	PwmSConfig.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &PwmSConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}
	/* Set the pulse value for channel 2 */
	PwmSConfig.Pulse = 0;
	if (HAL_TIM_PWM_ConfigChannel(&PwmTimHandle, &PwmSConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}	
	/* Start channel 1 */
	if (HAL_TIM_PWM_Start(&PwmTimHandle, TIM_CHANNEL_1) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	/* Start channel 2 */
	if (HAL_TIM_PWM_Start(&PwmTimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
		/* PWM Generation Error */
		Error_Handler();
	}
	PWM_Channel_Value_Init(pwm_channel1);
	PWM_Channel_Value_Init(pwm_channel2);
}
/*-----------------------------------------------------------------------------
Prototype      : void PWM_Start(uint8_t lightbox_type,uint8_t duty_cycle)
Parameters     : uint8_t lightbox_type(LED的类型 1,2),uint8_t duty_cycle(占空比,范围为0-100)
Return         : void
Implementation : 开启某个LED的占空比
-----------------------------------------------------------------------------*/
void PWM_Start(uint8_t lightbox_type,uint8_t duty_cycle)
{	
	if(duty_cycle > 100)
	{
		duty_cycle = 100;
	}
	Pwm_Change_Init(lightbox_type,duty_cycle);
}
