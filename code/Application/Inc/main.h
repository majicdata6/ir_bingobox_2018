/**
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_ComDMA/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "led.h"
#include "config.h"
#include "spi.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "flash.h"
#include "pwm.h"
#include "smoke.h"
#include "m485.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "w5500.h"
#include "wizchip_conf.h"
#include "app_communication.h"
#include "app_w5500.h"
#include "app_info.h"
#include "app_lock.h"
#include "app_meter.h"
#include "app_lightbox.h"
#include "iwdg.h"
#include "app_infrared.h"
/* Exported types ------------------------------------------------------------*/
#define BOOL		int
#define TRUE		1
#define FALSE		0

#if defined(BG_DOOR_LOCK)
//locking power state
#define POWER_ON	0
#define POWER_OFF       1

// Lock is locked when power on
#define LOCKED_WHEN_POWER_STATE		POWER_ON


//Lock state
#define LOCK_UNLOCKED	1
#define LOCK_LOCKED	0
#define LOCK_ERROR	0xFF


#endif
/* Exported constants --------------------------------------------------------*/

extern uint16_t g_485uart_rx_end_time;

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
