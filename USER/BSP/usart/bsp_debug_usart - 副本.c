/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����c��printf������usart�˿�
  ******************************************************************************

  ******************************************************************************
  */ 
  
#include "./Bsp/usart/bsp_debug_usart.h"
uint8_t debug_Rxbuffer[Rxbuffer_num];//debug���ڽ��ջ�����
uint8_t debug_Rxcounter;//debug���ڽ����ֽڼ�����
uint8_t debug_ReceiveState=0;//debug���ڽ���״̬
//volatile uint8_t debug_Rxbuffer[200];//debug���ڽ��ջ�����
//volatile uint8_t debug_Rxcounter;//debug���ڽ����ֽڼ�����
//volatile uint8_t debug_ReceiveState=0;//debug���ڽ���״̬

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration_DEBUG(void)
{
  NVIC_InitTypeDef NVIC_InitStructure_debug;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure_debug.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* �������ȼ�Ϊ1 */
  NVIC_InitStructure_debug.NVIC_IRQChannelPreemptionPriority = 0;
  /* �����ȼ�Ϊ1 */
  NVIC_InitStructure_debug.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure_debug.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure_debug);
}

/**
	* @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
	* @param  ��
	* @retval ��
	*/
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK, ENABLE);

  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ�񣺲�ʹ��У�� */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(DEBUG_USART, &USART_InitStructure); 
	
  /* Ƕ�������жϿ�����NVIC���� */
	NVIC_Configuration_DEBUG();
	
	USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);//��ֹ������ͽ����ж�
	
	/* ʹ�ܴ��ڿ����ж� */
	USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);
	
  //������������ж�
	//USART_ITConfig(USART2, USART_IT_ORE, ENABLE);
	
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
	
  /* ʹ�ܴ��� */
  USART_Cmd(DEBUG_USART, ENABLE);
	
	//USART_ClearFlag(DEBUG_USART, USART_FLAG_TC);
}



/*********************************************END OF FILE**********************/
