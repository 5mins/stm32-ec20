/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/usart/bsp_linkec20_usart.h"
#include "./systick/bsp_SysTick.h"
#include "bsp/key/bsp_key.h" 
#include "./Bsp/LED/led.h" 
#include "Graph1/menu.h"
#include  <includes.h>
#include "app_cfg.h"
//#define TaskMailCenterTCB GetTaskMailCenterTCB()
//extern OS_SEM SemOfKey;          //��־KEY1�Ƿ񱻵����Ķ�ֵ�ź���
//extern OS_TCB   AppTaskMailCenterTCB;
extern  OS_TCB AppTaskDisposeKeySignTCB;
//extern OS_SEM SemOfRec_A_EC20_NEWS;          //��־EC20��������Ϣ�Ķ�ֵ�ź���
//extern uint8_t DEBUG_USART_had_been_Read;//debug��Ϣ��ȡ״̬��0δ����1�Ѷ�
//extern uint8_t DEBUG_USART_had_been_Sent;//debug��ǰ��Ϣ����״̬��0δ����1�ѷ���
extern type_DEBUG_USART_Rx  DEBUG_USART_Rx;//��ǰ���յ�������Ϣ

//extern uint8_t LINKEC20_USART_had_been_Read;//LINKEC20��Ϣ��ȡ״̬��0δ����1�Ѷ�
//extern uint8_t LINKEC20_USART_had_been_Sent;//LINKEC20��ǰ��Ϣ����״̬��0δ����1�ѷ���
//extern type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//��ǰ���յ�������Ϣ




extern STM32_CTR_FLAGS stm32_flat;


/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

//__asm void wait()
//{
//      BX lr
//}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
//	 wait();
	  if (CoreDebug->DHCSR & 1) {  //check C_DEBUGEN == 1 -> Debugger Connected  
      __breakpoint(0);  // halt program execution here        
				printf("__breakpoint0!!!\n");
		} 
	printf("hardfault!!!\n");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
// 

//	
//  /* Decrement the timeout value */
////  if (TimeOut != 0x0)
////  {
////    TimeOut--;
////  }
////    
////  if (ubCounter < 100)   //100����Լ0.5����ʱ  
////  {
////    ubCounter++;
////  }
////  else
////  {
////    ubCounter = 0x00;
////    LED1_TOGGLE;
////		//printf("SYSTICK_handler\r\n");//debug_mark
////  }	
////	
//	
//}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

void DEBUG_USART_IRQHandler(void)
{
	OSIntEnter();				//�����ж�     
	OS_ERR err;

		if(USART_GetITStatus(DEBUG_USART, USART_IT_IDLE) != RESET)
		{
			
			DEBUG_USART->SR;  /* ��USART_IT_IDLE��־ */
       DEBUG_USART->DR; /* ��USART_IT_IDLE��־ */

						//���������ź��� ֪ͨ��Ϣ�ѵ���
			OSTaskSemPost ((OS_TCB *)&AppTaskDisposeKeySignTCB,
											(OS_OPT) OS_OPT_POST_NO_SCHED,
											(OS_ERR *)err);
			
			
			
	       /* ��������жϴ��� */
				//DEBUG_USART_Rx.DEBUG_USART_RxLen = deal_DEBUG_irq_rx_end(DEBUG_USART_Rx.DEBUG_USART_RECEIVE); 
				
				//PostKeySign(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,DEBUG_USART_Rx.DEBUG_USART_RxLen);
			
						//����
		
				//Sent_to_EC20_with_Block(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,DEBUG_USART_Rx.DEBUG_USART_RxLen);

//				Sent_to_DEBUG_with_Block(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,DEBUG_USART_Rx.DEBUG_USART_RxLen);//debug_mark
					
			
		}
		else
		{
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_PE     ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_TXE    ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_TC     ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_RXNE   ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_ORE_RX ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_IDLE   ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_LBD    ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_CTS    ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_ERR    ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_ORE_ER ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_NE     ));//debug_mark
//			Usart_SendByte(DEBUG_USART,USART_GetITStatus(DEBUG_USART, USART_IT_FE     ));//debug_mark
	}

		
		
 

 OSIntExit();        //���������л����ж�  		

	

}

//void LINKEC20_USART_IRQHandler (void)
//{
//	OSIntEnter();				//�����ж�   
//	OS_ERR  err;
//		
//		if(USART_GetITStatus(LINKEC20_USART, USART_IT_IDLE) != RESET)
//		{
//			
//	    /* ��������жϴ��� */
//			//LINKEC20_USART_Rx.LINKEC20_USART_RxLen= deal_LINKEC20_irq_rx_end(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE); 
//			//ת������
//			//Sent_to_DEBUG_with_Block(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,LINKEC20_USART_Rx.LINKEC20_USART_RxLen);
//			
//			
//		    LINKEC20_USART->SR;  /* ��USART_IT_IDLE��־ */
//        LINKEC20_USART->DR; /* ��USART_IT_IDLE��־ */
//			//���������ź��� ֪ͨ��Ϣ�ѵ���
//			OSTaskSemPost ((OS_TCB *)&AppTaskMailCenterTCB,
//											(OS_OPT) OS_OPT_POST_NO_SCHED,
//											(OS_ERR *)err);
//	
//		}
//		else
//		{
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_PE     ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_TXE    ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_TC     ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_RXNE   ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_ORE_RX ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_IDLE   ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_LBD    ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_CTS    ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_ERR    ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_ORE_ER ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_NE     ));//debug_mark
////			Usart_SendByte(DEBUG_USART,USART_GetITStatus(LINKEC20_USART, USART_IT_FE     ));//debug_mark
//	}

// OSIntExit();        //���������л����ж�  

//}


void KEY1_IRQHandler(void)
{
	OSIntEnter();				//�����ж�             
   
//		OS_ERR      err;
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);   //����жϱ�־λ  
		
//		
//		 OSSemPost((OS_SEM  *)&SemOfKey,                                        //����SemOfKey
//							 (OS_OPT   )OS_OPT_POST_ALL,                                   //���������еȴ�����
//							 (OS_ERR  *)&err);                                             //���ش�������
		// LED1 ȡ��		
		LED1_TOGGLE;

		if(stm32_flat.StopModeState==1){
			stm32_flat.StopModeState=0;
			
		}
		
    
		
	}

 OSIntExit();        //���������л����ж�  
}


//void KEY2_IRQHandler(void)
//{
//  //ȷ���Ƿ������EXTI Line�ж�
//	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
//	{
//		// LED2 ȡ��		
//		LED1_TOGGLE;
//    //����жϱ�־λ
//		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
//	}  
//}

//void EC20RIFOOT_IRQHandler(void)
//{
//	OSIntEnter();				//�����ж�             
//   
////		OS_ERR      err;
//  //ȷ���Ƿ������EXTI Line�ж�
//	if(EXTI_GetITStatus(EC20RIFOOT_INT_EXTI_LINE) != RESET) 
//	{
//		
//		EXTI_ClearITPendingBit(EC20RIFOOT_INT_EXTI_LINE);   //����жϱ�־λ  
//		LED1_TOGGLE;


//		if(stm32_flat.StopModeState==1){
//			stm32_flat.StopModeState=0;
//			
//		}
//		
//    
//		
//	}

// OSIntExit();        //���������л����ж�  
//}



//void DEBUG_USART_DMA_Rx_IRQHandler(void)
//{

//}


//void DEBUG_USART_DMA_Tx_IRQHandler(void)
//{
//		OSIntEnter();				//�����ж�    
//	  /* Waiting the end of Data transfer */
//		while (USART_GetFlagStatus(DEBUG_USART,USART_FLAG_TC)==RESET);    
//		while (DMA_GetFlagStatus(DEBUG_USART_DMA_Tx_Sream,DEBUG_USART_DMA_Tx_Flags)==RESET);
// 
//     /* �����־λ */
//		DMA_ClearFlag(DEBUG_USART_DMA_Tx_Sream,DEBUG_USART_DMA_Tx_Flags);/* Clear DMA Transfer Complete Flags */
//		USART_ClearFlag(DEBUG_USART,USART_FLAG_TC);  /* Clear USART Transfer Complete Flags */
//	
//       /* �ر�DMA */
//      DMA_Cmd(DEBUG_USART_DMA_Tx_Sream,DISABLE);
//	
//		 /* ������ɺ������� */
//			deal_DEBUG_irq_tx_end();   
//	
////        /* �򿪷�������ж�,ȷ�����һ���ֽڷ��ͳɹ� (��ͣ�ô��жϣ����������whileѭ���ȴ��������)*/
////      USART_ITConfig(DEBUG_USART,USART_IT_TC,ENABLE);  
//    OSIntExit();        //���������л����ж�     
//}


//void LINKEC20_USART_DMA_Rx_IRQHandler(void)
//{

//	
//}

//void LINKEC20_USART_DMA_Tx_IRQHandler(void)
//{
//	OSIntEnter();				//�����ж�   ucos
//	
////			Usart_SendByte(LINKEC20_USART,0x04);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x04);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x04);//debug_mark
//	  /* Waiting the end of Data transfer */
//  while (USART_GetFlagStatus(LINKEC20_USART,USART_FLAG_TC)==RESET);    
//	
////			Usart_SendByte(LINKEC20_USART,0x41);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x41);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x41);//debug_mark
//  while (DMA_GetFlagStatus(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_Flags)==RESET);
//	
////			Usart_SendByte(LINKEC20_USART,0x42);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x42);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x42);//debug_mark
//	
////	 Usart_SendByte(LINKEC20_USART,DMA_GetITStatus(LINKEC20_USART_DMA_Tx_Sream,DMA_IT_TCIF7));//debug_mark
////	 Usart_SendByte(LINKEC20_USART,DMA_GetITStatus(LINKEC20_USART_DMA_Tx_Sream,DMA_FLAG_TCIF7));//debug_mark
//	
//		//if(DMA_GetITStatus(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_flags) == SET)  
//	/*TCIFx�������� x ��������жϱ�־ 
//	(Stream x transfer complete interrupt flag) (x = 3..0)
//	��λ����Ӳ���� 1����������㣬���ֻ�轫 1 д�� DMA_LIFCR �Ĵ�������Ӧλ��
//	0�������� x ���޴�������¼�
//	1�������� x �Ϸ�����������¼�
//	TCIFx: Stream x transfer complete interrupt flag (x = 3..0)
//	This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the
//	DMA_LIFCR register.
//	0: No transfer complete event on stream x
//	1: A transfer complete event occurred on stream x
//																																																	*/
//      
//			
//	

//   /* �����־λ */
//	 //DMA_ClearITPendingBit(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_IT_Flags); //test
//	 DMA_ClearFlag(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_Flags);/* Clear DMA Transfer Complete Flags */
//   USART_ClearFlag(LINKEC20_USART,USART_FLAG_TC);  /* Clear USART Transfer Complete Flags */		  
//   /* �ر�DMA */
//   DMA_Cmd(LINKEC20_USART_DMA_Tx_Sream,DISABLE);
//	 
//	 /* ��������жϺ������� */
//		deal_LINKEC20_irq_tx_end();
//			
//   /* �򿪷�������ж�,ȷ�����һ���ֽڷ��ͳɹ�(��ͣ�ô��жϣ����������whileѭ���ȴ��������) */
////   USART_ITConfig(LINKEC20_USART,USART_IT_TC,ENABLE);  
//	OSIntExit();        //ucos���������л����ж�  
//}



/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
