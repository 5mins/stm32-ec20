/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重现c库printf函数到usart端口
  ******************************************************************************

  ******************************************************************************
  */ 
  
#include "./Bsp/usart/bsp_debug_usart.h"
uint8_t debug_Rxbuffer[Rxbuffer_num];//debug串口接收缓冲区
uint8_t debug_Rxcounter;//debug串口接收字节计数器
uint8_t debug_ReceiveState=0;//debug串口接收状态
//volatile uint8_t debug_Rxbuffer[200];//debug串口接收缓冲区
//volatile uint8_t debug_Rxcounter;//debug串口接收字节计数器
//volatile uint8_t debug_ReceiveState=0;//debug串口接收状态

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration_DEBUG(void)
{
  NVIC_InitTypeDef NVIC_InitStructure_debug;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure_debug.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* 抢断优先级为1 */
  NVIC_InitStructure_debug.NVIC_IRQChannelPreemptionPriority = 0;
  /* 子优先级为1 */
  NVIC_InitStructure_debug.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure_debug.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure_debug);
}

/**
	* @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
	* @param  无
	* @retval 无
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
			
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(DEBUG_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	NVIC_Configuration_DEBUG();
	
	USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);//防止配置完就进入中断
	
	/* 使能串口空闲中断 */
	USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);
	
  //开启串口溢出中断
	//USART_ITConfig(USART2, USART_IT_ORE, ENABLE);
	
	/* 使能串口接收中断 */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
	
  /* 使能串口 */
  USART_Cmd(DEBUG_USART, ENABLE);
	
	//USART_ClearFlag(DEBUG_USART, USART_FLAG_TC);
}



/*********************************************END OF FILE**********************/
