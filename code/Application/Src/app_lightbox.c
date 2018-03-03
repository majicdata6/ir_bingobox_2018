/*-----------------------------------------------------------------------------
Copyright 2017 - 2020, BingoBox Inc.
License     : Internal Use.
Author      : ��˧ 
Description : ���Ƶ������ع��ܺ���
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
Parameters     : level - �����ȼ� 0-5��
Return         : (Nothing)
Implementation : 1->���25%pwm��2->���40%pwm,
		3->���55%pwm,4->���70%pwm,5->���100%pwm
-----------------------------------------------------------------------------*/
void Ligthbox_SetLevel(uint8_t lightbox_type,uint8_t level)
{
#ifdef BG_LED
	printf("%s,%d,%d\r\n",__FUNCTION__,lightbox_type,level);
        if(level > 5 || level < 1){return;}
        
	if(LIGHTBOX1 == lightbox_type)
	{
                s_light1_level = level;
                //�ص��²�������
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
                //�ص��²�������
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
Return         : �����ȵȼ�(0-5)
Implementation :�������ȵȼ�
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
Implementation :���ƣ������һ�ο��ƣ��ȼ�Ϊ0,Ĭ����3���򿪵�,
                ���info������Ϣ���棬��ÿ�ζ���info�ж�ȡ��
                ����ÿ�ζ���3����
-----------------------------------------------------------------------------*/
void Lightbox_OpenLight(uint8_t lightbox_type)
{
#ifdef BG_LED
        uint8_t level = 0;
        
#ifdef BG_INFO
        //ÿ�δ�info����ȡ�ȼ� 
	Info_ReadLevel(lightbox_type,&level);
#endif
        //����򿪵�ʱ���ȼ���0������δ���õȼ�������3����
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
Implementation :�ص�
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
Implementation :��ʼ��������Ϣ
-----------------------------------------------------------------------------*/
void Ligthbox_Init(void)
{
#ifdef BG_LED
        Info_ReadLevel(1,&s_light1_level);
        Info_ReadLevel(2,&s_light2_level);
	PWM_Init();
#endif
}

