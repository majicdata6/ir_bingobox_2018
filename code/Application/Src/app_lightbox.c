/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : 刘帅 
Description : 控制灯箱的相关功能函数
-----------------------------------------------------------------------------*/

/* include ANSI C .h file */
/* include Other Lib .h file */
/* include Project .h file */
/* include Local .h file */
#include "main.h"

/*---------------------------------------------------------------------------*/
//	Defines and Macros
/*---------------------------------------------------------------------------*/

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
#ifdef BG_LED
static uint8_t s_light1_level=0;
static uint8_t s_light2_level=0;
static uint8_t s_light1_isopen = FALSE;
static uint8_t s_light2_isopen = FALSE;
#endif
/* Global */

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
Prototype      : 
Parameters     : 
Return         : 
Implementation : 
-----------------------------------------------------------------------------*/

#endif /* End of 1, Just a tag, ignore it */

/*---------------------------------------------------------------------------*/
//	Public Functions
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Prototype      : void Ligthbox_SetLevel(uint8_t level)
Parameters     : level - 灯亮等级 0-5级
Return         : (Nothing)
Implementation : 1->输出25%pwm，2->输出40%pwm,
		3->输出55%pwm,4->输出70%pwm,5->输出100%pwm
-----------------------------------------------------------------------------*/
void Ligthbox_SetLevel(uint8_t lightbox_type,uint8_t level)
{
#ifdef BG_LED
	printf("%s,%d,%d\r\n",__FUNCTION__,lightbox_type,level);
        if(level > 5 || level < 1){return;}
        
	if(LIGHTBOX1 == lightbox_type)
	{
                s_light1_level = level;
                //关灯下不做操作
                if(FALSE==s_light1_isopen){return;}
                
		switch(level)
		{
		case 1:
			PWM_Start(lightbox_type,5);
			break;
		case 2:
			PWM_Start(lightbox_type,15);
			break;
		case 3:
			PWM_Start(lightbox_type,25);
			break;
		case 4:
			PWM_Start(lightbox_type,50);
			break;
		case 5:
			PWM_Start(lightbox_type,100);
		default:
			break;
		}
	}
	else if(LIGHTBOX2 == lightbox_type)
	{
                s_light2_level = level;
                //关灯下不做操作
                if(FALSE==s_light2_isopen){return;}
                
		switch(level)
		{
		case 1:
			PWM_Start(lightbox_type,5);
			break;
		case 2:
			PWM_Start(lightbox_type,15);
			break;
		case 3:
			PWM_Start(lightbox_type,25);
			break;
		case 4:
			PWM_Start(lightbox_type,50);
			break;
		case 5:
			PWM_Start(lightbox_type,100);
		default:
			break;
		}	
	}

#endif
}

/*-----------------------------------------------------------------------------
Prototype      : uint8_t Lightbox_Getlevel(void)
Parameters     :(Nothing)
Return         : 灯亮度等级(0-5)
Implementation :返回亮度等级
-----------------------------------------------------------------------------*/
uint8_t Lightbox_GetLevel(uint8_t lightbox_type)
{
#ifdef BG_LED
	if(LIGHTBOX1 == lightbox_type)
	{
		return s_light1_level;	
	}
	else if(LIGHTBOX2 == lightbox_type)
	{
		return s_light2_level;
	}
#endif
}

uint8_t Lightbox_GetState(uint8_t lightbox_type)
{
#ifdef BG_LED
	if(LIGHTBOX1 == lightbox_type)
	{
		return s_light1_isopen;	
	}
	else if(LIGHTBOX2 == lightbox_type)
	{
		return s_light2_isopen;
	}
#endif
}

/*-----------------------------------------------------------------------------
Prototype      : void Lightbox_OpenLight(void)
Parameters     :(Nothing)
Return         :(Nothing)
Implementation :开灯，如果第一次开灯，等级为0,默认以3级打开灯,
                如果info区有信息保存，则每次都从info中读取，
                否则每次都以3级打开
-----------------------------------------------------------------------------*/
void Lightbox_OpenLight(uint8_t lightbox_type)
{
#ifdef BG_LED
        uint8_t level = 0;
        
#ifdef BG_INFO
        //每次从info区读取等级 
	Info_ReadLevel(lightbox_type,&level);
#endif
        //如果打开灯时，等级是0，即还未设置等级，就以3级打卡
        if(0 == level)
        {
                level = 3;
        }
        
        if(LIGHTBOX1 == lightbox_type)
        {
              s_light1_isopen = TRUE;  
        }
        else if(LIGHTBOX2 == lightbox_type)
        {
                s_light2_isopen = TRUE;
        }
        
        Ligthbox_SetLevel(lightbox_type,level);
#endif
}

/*-----------------------------------------------------------------------------
Prototype      :void Lightbox_CloseLight(void)
Parameters     :(Nothing)
Return         :(Nothing)
Implementation :关灯
-----------------------------------------------------------------------------*/
void Lightbox_CloseLight(uint8_t lightbox_type)
{
#ifdef BG_LED
        if(LIGHTBOX1 == lightbox_type)
        {
              s_light1_isopen = FALSE;  
        }
        else if(LIGHTBOX2 == lightbox_type)
        {
                s_light2_isopen = FALSE;
        }
        
	PWM_Start(lightbox_type,0);
#endif
}
/*-----------------------------------------------------------------------------
Prototype      : void Ligthbox_Init(void)
Parameters     :(Nothing)
Return         :(Nothing)
Implementation :初始化灯箱信息
-----------------------------------------------------------------------------*/
void Ligthbox_Init(void)
{
#ifdef BG_LED
        Info_ReadLevel(1,&s_light1_level);
        Info_ReadLevel(2,&s_light2_level);
	PWM_Init();
#endif
}

