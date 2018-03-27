#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
void initall_PWM(void);


void TIM14_PWM_Init(u32 arr,u32 psc);


void TIM9_ch1_PWM_Init(u32 arr,u32 psc);
void	TIM9_ch2_PWM_Init(u32 arr,u32 psc);
void 	TIM10_ch1_PWM_Init(u32 arr,u32 psc);
void 	TIM11_ch1_PWM_Init(u32 arr,u32 psc);
	
void 	TIM13_ch1_PWM_Init(u32 arr,u32 psc);
void 	TIM12_ch1_PWM_Init(u32 arr,u32 psc);
void 	TIM14_PWM_Init(u32 arr,u32 psc);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.  
void 	TIM5_ch2_PWM_Init(u32 arr,u32 psc);
void 	TIM4_ch1_PWM_Init(u32 arr,u32 psc);
void 	TIM4_ch2_PWM_Init(u32 arr,u32 psc);
void 	TIM3_ch1_PWM_Init(u32 arr,u32 psc);
void 	TIM3_ch2_PWM_Init(u32 arr,u32 psc);
void 	TIM3_ch3_PWM_Init(u32 arr,u32 psc);
void 	TIM3_ch4_PWM_Init(u32 arr,u32 psc);
	
	
void 	TIM4_ch3_PWM_Init(u32 arr,u32 psc);//I2C
void 	TIM4_ch4_PWM_Init(u32 arr,u32 psc);//I2C



#endif
