/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./systick/bsp_SysTick.h"

//__IO uint32_t TimeOut = 0x0;   //�趨����0��ֵ�����״���ʱʱ��


 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
//void SysTick_Init(void)
//{
//	/* SystemFrequency / 1000    1ms�ж�һ��
//	 * SystemFrequency / 100000	 10us�ж�һ��
//	 * SystemFrequency / 1000000 1us�ж�һ��
//	 */
//	if (SysTick_Config(SystemCoreClock / 1000))
//	{ 
//		/* Capture error */ 
//		while (1);
//	}	
//	  /* Configure the SysTick handler priority */
//  NVIC_SetPriority(SysTick_IRQn, 0x0);
//}


/*********************************************END OF FILE**********************/
