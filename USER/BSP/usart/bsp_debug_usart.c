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
  
#include "./BSP/usart/bsp_debug_usart.h"
#include "./BSP/usart/bsp_linkec20_usart.h"
#include <string.h>
#include "includes.h"

uint8_t DEBUG_USART_RECEIVE_BUF[DEBUG_USART_RECEIVE_BUF_SIZE];//debug����DMA���ջ�����
type_DEBUG_USART_Rx  DEBUG_USART_Rx;//��ǰ���յ�������Ϣ
//uint8_t DEBUG_USART_had_been_Read=1;//debug��Ϣ��ȡ״̬��0δ����1�Ѷ�
//uint8_t DEBUG_USART_had_been_Sent=0;//debug��ǰ��Ϣ����״̬��0δ����1�ѷ���


uint8_t DEBUG_USART_SEND_BUF[DEBUG_USART_SEND_BUF_SIZE];//debug����DMA���ͻ�����
uint8_t DEBUG_USART_Use_DMA_Tx_Flag =0;



void Debug_USART_ALL_Init(void)
{
	Debug_USART_Config();
	//DMA_Use_DEBUG_USART_Tx_Init();
	DMA_Use_DEBUG_USART_Rx_Init();
	
}

/**
	* @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
	* @param  ��
	* @retval ��
	*/
static void Debug_USART_Config(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* ʹ��PA��ʱ�� */	
  RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK, ENABLE);

  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  
	/* �ܽ�ӳ�� */
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);/* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF); /* Connect PXx to USARTx_Rx*/

	/* ���ô��ڹܽ� */
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//ѡ��������ͣ������©��
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//ѡ������/����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//ѡ�� I/O �������롢�����AF��ģ�⣩
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;	//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	//�ٶ� 
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
	//GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; 
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	
	 /* Enable the USART OverSampling by 8 */
   //USART_OverSampling8Cmd(DEBUG_USART, ENABLE); 
  
  /* USARTx configured as follows:
        - BaudRate = 5250000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
		     is: (USART APB Clock / 8) 
			 Example: 
			    - (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
			    - (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
		     is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
			 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */ 
			
  /* ���ô��ڹ���ģʽ */ 
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;							/* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;							/* �ֳ�(����λ+У��λ)��8 */
    /* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;									/* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_Parity = USART_Parity_No;											/* У��λѡ�񣺲�ʹ��У�� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						/* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */  
  USART_Init(DEBUG_USART, &USART_InitStructure); 														/* ���USART��ʼ������ */
	
	/* �����ж� */
	NVIC_InitTypeDef NVIC_InitStructure_debug;
  NVIC_InitStructure_debug.NVIC_IRQChannel = DEBUG_USART_IRQ;/* ����USARTΪ�ж�Դ */
  NVIC_InitStructure_debug.NVIC_IRQChannelPreemptionPriority = 2;/* �������ȼ� */
  NVIC_InitStructure_debug.NVIC_IRQChannelSubPriority = 0; /* �����ȼ� */
  NVIC_InitStructure_debug.NVIC_IRQChannelCmd = ENABLE;/* ʹ���ж� */
  NVIC_Init(&NVIC_InitStructure_debug);/* ��ʼ������NVIC */
	USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);//��ֹ������ͽ����ж�
	
	
	/* ���ô����ж� */
	//USART_ITConfig(DEBUG_USART, USART_IT_ORE, ENABLE);//������������ж�
	//USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(DEBUG_USART, USART_IT_TC,   DISABLE);
  USART_ITConfig(DEBUG_USART, USART_IT_TXE,  DISABLE);  
  USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);/* ʹ��IDLE�ж� */

	
  /* ʹ�ܴ��� */
  USART_Cmd(DEBUG_USART, ENABLE);
	
}


///* DMA�������ݳ�ʼ�� */
//static void DMA_Use_DEBUG_USART_Tx_Init(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    DMA_InitTypeDef DMA_InitStructure;


//    /* ʹ��DMAʱ�� */
//    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_Tx_CLOCK,ENABLE);

//    /* ����ʹ��DMA�������� */
//    DMA_DeInit(DMA1_Stream6); 

//    DMA_InitStructure.DMA_Channel             = DEBUG_USART_DMA_Tx_CHANNEL;               /* ����DMAͨ�� */
//    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(DEBUG_USART->DR));   /* Ŀ�� */
//    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)DEBUG_USART_SEND_BUF;             /* Դ */
//    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;    /* ���� */
//    DMA_InitStructure.DMA_BufferSize          = DEBUG_USART_SEND_BUF_SIZE;      /* ���� */                  
//    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* �����ַ�Ƿ����� */
//    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ƿ����� */
//    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* Ŀ�����ݴ��� */
//    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* Դ���ݿ�� */
//    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* ���δ���ģʽ/ѭ������ģʽ */
//    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;             /* DMA���ȼ� */
//    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFOģʽ/ֱ��ģʽ */
//    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO��С */
//    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* ���δ��� */
//    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

//    /* ����DMA */
//    DMA_Init(DEBUG_USART_DMA_Tx_Sream, &DMA_InitStructure);

//    /*ʹ��DMA�ж� */
//		DMA_ClearFlag(DEBUG_USART_DMA_Tx_Sream,DEBUG_USART_DMA_Tx_Flags);  //��ֹ���������ж�
//    DMA_ITConfig(DEBUG_USART_DMA_Tx_Sream, DMA_IT_TC, ENABLE);

//    /* ʹ�ܴ��ڵ�DMA���ͽӿ� */
//    USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);

//    /* ����DMA�ж����ȼ� */
//    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_USART_DMA_Tx_IRQ;           
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;          
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    /* ��ʹ��DMA */                  
//    DMA_Cmd(DEBUG_USART_DMA_Tx_Sream, DISABLE);
//}


/* DMA�������ݳ�ʼ�� */
static void DMA_Use_DEBUG_USART_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DEBUG_USART_Rx.DEBUG_USART_RxLen = 0;
	
    /* ʹ��DMA1ʱ�� */
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_Rx_CLOCK, ENABLE);

    /* ����ʹ��DMA�������� */
    DMA_DeInit(DEBUG_USART_DMA_Rx_Tream); 

    DMA_InitStructure.DMA_Channel             = DEBUG_USART_DMA_Rx_CHANNEL;               /* ����DMAͨ�� */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(DEBUG_USART->DR));   /* Դ */
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)DEBUG_USART_RECEIVE_BUF;   /* Ŀ�� */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* ���� */
    DMA_InitStructure.DMA_BufferSize          = DEBUG_USART_RECEIVE_BUF_SIZE;    /* ���� */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* �����ַ�Ƿ����� */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ƿ����� */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* Ŀ�����ݴ��� */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* Դ���ݿ�� */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* ���δ���ģʽ/ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;        /* DMA���ȼ� */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFOģʽ/ֱ��ģʽ */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO��С */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* ���δ��� */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /*  ����DMA */
    DMA_Init(DEBUG_USART_DMA_Rx_Tream, &DMA_InitStructure);

    /* ���ڽ��ղ���ҪDMA�жϣ��ʲ�����DMA�ж� */

    /* ʹ�ܴ��ڵ�DMA���� */
    USART_DMACmd(DEBUG_USART,USART_DMAReq_Rx,ENABLE);

    /* ���ڽ��ղ���ҪDMA�жϣ��ʲ�������DMA�ж����ȼ� */

    /* ʹ��DMA */ 
    DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);
}


/* �ڴ��ڽ�������ж������ջ����deal_irq_rx_end������
			�ڸú���������жϱ�ʶ���������յ������ݿ�����tUART1_Rx.UART1RxBuf�С� */
//uint8_t deal_DEBUG_irq_rx_end(uint8_t *buf)  
//{     
//		    DEBUG_USART->SR;  /* ��USART_IT_IDLE��־ */
//        DEBUG_USART->DR; /* ��USART_IT_IDLE��־ */
//	
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
//			uint16_t len = 0;  
//    /* ��������ж� */
//      /* �ȴ�������� δ�Ӽ�������������ѭ�� Waiting the end of Data transfer */
//			//while (USART_GetFlagStatus(DEBUG_USART,USART_FLAG_TC)==RESET){};  
//        /* �����־λ Clear USART Transfer Complete Flags */
////				USART_ClearFlag(DEBUG_USART,USART_FLAG_TC);
//				
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
//			//while (DMA_GetFlagStatus(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags)==RESET){};
//         

//  

//        /* �رս���DMA  */
//        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,DISABLE);  
//        /* �����־λ Clear DMA Transfer Complete Flags */
//        DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags);  
//				//DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_IT_Flags); //test

//        /* ��ý���֡֡�� */
//        len = DEBUG_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream);  
//								//����ǰ�������ԭ��������
//				memset(buf,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(buf));
//        memcpy(buf,DEBUG_USART_RECEIVE_BUF,len);  

//			 //DEBUG_USART_had_been_Read=0;//���δ��״̬
//				//DEBUG_USART_had_been_Sent=0;//
//			
//        /* �������ô������ݳ��� */
//        DMA_SetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_RECEIVE_BUF_SIZE);  
//        /* ��DMA */
//        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);  

//        return len;  
//       

//    
//}  






// void deal_DEBUG_irq_tx_end(void)  	 
//{  
//    //if(USART_GetITStatus(DEBUG_USART, USART_IT_TXE) == RESET)  //RESET OR SET ??
//    {  
//        /* �رշ�������ж� ����ͣ�ô��жϣ� */ 
//       // USART_ITConfig(DEBUG_USART,USART_IT_TC,DISABLE);  
//        /* �������  */
//        DEBUG_USART_Use_DMA_Tx_Flag = 0;  
//    }    
//} 


///* ʹ�ú��� Use_DMA_tx ����DMA�������ݡ� */
//static void Use_DEBUG_DMA_tx(uint8_t *data,uint16_t size)  
//{  
//    /* �ȴ����� */
//    while (DEBUG_USART_Use_DMA_Tx_Flag);
//    DEBUG_USART_Use_DMA_Tx_Flag = 1;  
//    /* �������� */
//    memcpy(DEBUG_USART_SEND_BUF,data,size);  
//    /* ���ô������ݳ��� */  
//    DMA_SetCurrDataCounter(DEBUG_USART_DMA_Tx_Sream,size);  
//    /* ��DMA,��ʼ���� */  
//    DMA_Cmd(DEBUG_USART_DMA_Tx_Sream,ENABLE);  
//} 

///* ʹ�ú��� Use_DEBUG_DMA_tx �������� */
//void Sent_to_DEBUG_with_Block(uint8_t *data,uint16_t size)
//{
//	Use_DEBUG_DMA_tx(data,size);
//}


//�ض��� c �⺯�� printf �����ڣ��ض�����ʹ�� printf ����
int fputc(int ch, FILE *f)
 {
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USART, (uint8_t) ch);

		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);

		return (ch);
 }
 
 
 

/*********************************************END OF FILE**********************/
