/**
  ******************************************************************************
  * @file    bsp_LINKEC20_USART.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����c��printf������usart�˿�
  ******************************************************************************

  ******************************************************************************
  */

#include "./BSP/usart/bsp_linkec20_usart.h"
#include <string.h>
#include  <includes.h>

uint8_t LINKEC20_USART_RECEIVE_BUF[LINKEC20_USART_RECEIVE_BUF_SIZE]={0};//LINKEC20����DMA���ջ�����
type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//��ǰ���յ�������Ϣ
//uint8_t LINKEC20_USART_had_been_Read=1;//LINKEC20��Ϣ��ȡ״̬��0δ����1�Ѷ�
//uint8_t LINKEC20_USART_had_been_Sent=0;//LINKEC20��ǰ��Ϣ����״̬��0δ����1�ѷ���


//uint8_t LINKEC20_USART_SEND_BUF[LINKEC20_USART_SEND_BUF_SIZE];//LINKEC20����DMA���ͻ�����
//uint8_t LINKEC20_USART_Use_DMA_Tx_Flag =0;

//uint8_t i =0;



void LINKEC20_USART_ALL_Init(void)
{
	LINKEC20_USART_Config();
	//DMA_Use_LINKEC20_USART_Tx_Init();//���͵����������󣬲�ʹ��DMA���ͣ����������ͣ�����ϵͳ�л�������
	DMA_Use_LINKEC20_USART_Rx_Init();
	
}


/**
	* @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
	* @param  ��
	* @retval ��
	*/
static void LINKEC20_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd( LINKEC20_USART_RX_GPIO_CLK|LINKEC20_USART_TX_GPIO_CLK, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(LINKEC20_USART_CLK, ENABLE);

    	/* �ܽ�ӳ�� Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(LINKEC20_USART_RX_GPIO_PORT,LINKEC20_USART_RX_SOURCE, LINKEC20_USART_RX_AF);
    GPIO_PinAFConfig(LINKEC20_USART_TX_GPIO_PORT,LINKEC20_USART_TX_SOURCE,LINKEC20_USART_TX_AF);

  	/* ���ô��ڹܽ� */
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              	//ѡ��������ͣ������©��
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                	//ѡ������/����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                	//ѡ�� I/O �������롢�����AF��ģ�⣩
    GPIO_InitStructure.GPIO_Pin = LINKEC20_USART_TX_PIN  ;      	//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;          	//�ٶ� 
    GPIO_Init(LINKEC20_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function  */
    GPIO_InitStructure.GPIO_Pin = LINKEC20_USART_RX_PIN;
    GPIO_Init(LINKEC20_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	 /* Enable the USART OverSampling by 8 */
    USART_OverSampling8Cmd(LINKEC20_USART, ENABLE); 
	 
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
	
			
    /* ���ô�_USART ģʽ */
    USART_InitStructure.USART_BaudRate = LINKEC20_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		 /* When using Parity the word length must be configured to 9 bits */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(LINKEC20_USART, &USART_InitStructure);

    /* Ƕ�������жϿ�����NVIC���� */
		NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = LINKEC20_USART_IRQ;		/* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				/* �����ȼ�Ϊ1 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* ʹ���ж� */
    NVIC_Init(&NVIC_InitStructure);								 					/* ��ʼ������NVIC */


	/* ���ô����ж� */
	//USART_ITConfig(LINKEC20_USART, USART_IT_ORE, ENABLE);//������������ж�
	//USART_ITConfig(LINKEC20_USART, USART_IT_RXNE, ENABLE);/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(LINKEC20_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(LINKEC20_USART, USART_IT_TC,   DISABLE);
  USART_ITConfig(LINKEC20_USART, USART_IT_TXE,  DISABLE);  
  USART_ITConfig(LINKEC20_USART, USART_IT_IDLE, DISABLE);/* ʹ��IDLE�ж� *///����rtosϵͳ����ѭ����ȡ��Ϣ����ʹ�ܿ����ж�

	
  /* ʹ�ܴ��� */
  USART_Cmd(LINKEC20_USART, ENABLE);

    
}

    /* ��λ����״̬�����ã�
		PE����żУ����� (Parity error)
		FE��֡���� (Framing error)
		ORE��������� (Overrun error)
		USART_IT_RXNE����ȡ���ݼĴ�����Ϊ�� �ж�
		*/
//void USART_NewIstr(USART_TypeDef * pUSARTx)
//{
//    //uint8_t Data;
//    if (USART_GetFlagStatus(pUSARTx, USART_FLAG_PE) != RESET)
//    {
//        USART_ReceiveData(pUSARTx);
//        USART_ClearFlag(pUSARTx, USART_FLAG_PE);
//    }

//    if (USART_GetFlagStatus(pUSARTx, USART_FLAG_ORE) != RESET)
//    {
//        USART_ReceiveData(pUSARTx);
//        USART_ClearFlag(pUSARTx, USART_FLAG_ORE);
//    }

//    if (USART_GetFlagStatus(pUSARTx, USART_FLAG_FE) != RESET)
//    {
//        USART_ReceiveData(pUSARTx);
//        USART_ClearFlag(pUSARTx, USART_FLAG_FE);
//    }

//    if(USART_GetITStatus(pUSARTx, USART_IT_RXNE) != RESET)
//    {
//        USART_ClearFlag(pUSARTx, USART_FLAG_RXNE);
//        USART_ClearITPendingBit(pUSARTx, USART_IT_RXNE);
//        USART_ReceiveData(pUSARTx);////�����������һ����������ȡreceivedata��ֵ����ֹ���ݶ�ʧ��
//    }
//}


/* DMA�������ݳ�ʼ�� */
//static void DMA_Use_LINKEC20_USART_Tx_Init(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    DMA_InitTypeDef DMA_InitStructure;


//    /* ʹ��DMAʱ�� */
//    RCC_AHB1PeriphClockCmd(LINKEC20_USART_DMA_Tx_CLOCK,ENABLE);

//    /* ����ʹ��DMA�������� */
//    DMA_DeInit(DMA1_Stream6); 

//    DMA_InitStructure.DMA_Channel             = LINKEC20_USART_DMA_Tx_CHANNEL;               /* ����DMAͨ�� */
//    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(LINKEC20_USART->DR));   /* Ŀ�� */
//    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)LINKEC20_USART_SEND_BUF;             /* Դ */
//    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;    /* ���� */
//    DMA_InitStructure.DMA_BufferSize          = LINKEC20_USART_SEND_BUF_SIZE;      /* ���� */                  
//    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* �����ַ�Ƿ����� */
//    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ƿ����� */
//    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* Ŀ�����ݴ��� */
//    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* Դ���ݿ�� */
//    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* ���δ���ģʽ/ѭ������ģʽ */
//    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;             /* DMA���ȼ� */
//    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFOģʽ/ֱ��ģʽ */
//    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO��С */
//    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* ���δ��� */
//    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

//    /* ����DMA */
//    DMA_Init(LINKEC20_USART_DMA_Tx_Sream, &DMA_InitStructure);

//    /*ʹ��DMA�ж� */
//		DMA_ClearFlag(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_Flags);  //��ֹ���������ж�
//    DMA_ITConfig(LINKEC20_USART_DMA_Tx_Sream, DMA_IT_TC, ENABLE);//test

//    /* ʹ�ܴ��ڵ�DMA���ͽӿ� */
//    USART_DMACmd(LINKEC20_USART, USART_DMAReq_Tx, ENABLE);

//    /* ����DMA�ж����ȼ� */
//    NVIC_InitStructure.NVIC_IRQChannel                   = LINKEC20_USART_DMA_Tx_IRQ;           
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    /* ��ʹ��DMA */                  
//    DMA_Cmd(LINKEC20_USART_DMA_Tx_Sream, DISABLE);
//}



/* DMA�������ݳ�ʼ�� */
static void DMA_Use_LINKEC20_USART_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    LINKEC20_USART_Rx.LINKEC20_USART_RxLen = 0;
	
    /* ʹ��DMA1ʱ�� */
    RCC_AHB1PeriphClockCmd(LINKEC20_USART_DMA_Rx_CLOCK, ENABLE);

    /* ����ʹ��DMA�������� */
    DMA_DeInit(LINKEC20_USART_DMA_Rx_Tream); 

    DMA_InitStructure.DMA_Channel             = LINKEC20_USART_DMA_Rx_CHANNEL;               /* ����DMAͨ�� */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(LINKEC20_USART->DR));   /* Դ */
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)LINKEC20_USART_RECEIVE_BUF;   /* Ŀ�� */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* ���� */
    DMA_InitStructure.DMA_BufferSize          = LINKEC20_USART_RECEIVE_BUF_SIZE;    /* ���� */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* �����ַ�Ƿ����� */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* �ڴ��ַ�Ƿ����� */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* Ŀ�����ݴ��� */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* Դ���ݿ�� */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* ���δ���ģʽ/ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;        /* DMA���ȼ� */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFOģʽ/ֱ��ģʽ */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull;    /* FIFO��С */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* ���δ��� */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /*  ����DMA */
    DMA_Init(LINKEC20_USART_DMA_Rx_Tream, &DMA_InitStructure);

    /* ���ڽ��ղ���ҪDMA�жϣ��ʲ�����DMA�ж� */

    /* ʹ�ܴ��ڵ�DMA���� */
    USART_DMACmd(LINKEC20_USART,USART_DMAReq_Rx,ENABLE);

    /* ���ڽ��ղ���ҪDMA�жϣ��ʲ�������DMA�ж����ȼ� */

    /* ʹ��DMA */ 
    DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE);
}


/* �ڴ��ڽ�������ж������ջ����deal_irq_rx_end������
			�ڸú���������жϱ�ʶ���������յ������ݿ�����tUART1_Rx.UART1RxBuf�С� */
//uint8_t deal_LINKEC20_irq_rx_end(uint8_t *buf)  
//{     
//		
//    uint16_t len = 0; 

//		    LINKEC20_USART->SR;  /* ��USART_IT_IDLE��־ */
//        LINKEC20_USART->DR; /* ��USART_IT_IDLE��־ */
//				
//			/*�ȴ�������� δ�Ӽ�������������ѭ�� Waiting the end of Data transfer */
//			//while (USART_GetFlagStatus(LINKEC20_USART,USART_FLAG_TC)==RESET){};  
//			
//				/* �����־λ Clear USART Transfer Complete Flags */
//			//USART_ClearFlag(LINKEC20_USART,USART_FLAG_TC);
//				
//			//while (DMA_GetFlagStatus(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags)==RESET){};

//			
//        /* �رս���DMA  */
//        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,DISABLE);  
//			/* �����־λ Clear DMA Transfer Complete Flags */
//        DMA_ClearFlag(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags);  

//        /* ��ý���֡֡�� */
//        len = LINKEC20_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream);  
//				//����ǰ�������ԭ��������
//				memset(buf,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));
//        memcpy(buf,LINKEC20_USART_RECEIVE_BUF,len);  

//			  //LINKEC20_USART_had_been_Read=0;//���δ��״̬
//			  //LINKEC20_USART_had_been_Sent=0;
//			
//        /* �������ô������ݳ��� */
//        DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_RECEIVE_BUF_SIZE);  
//        /* ��DMA */
//        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE);  

//        return len;  

//}  






// void deal_LINKEC20_irq_tx_end(void)  	 
//{  
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
//	
//   /* �رշ�������ж�  */ 
//		//USART_NewIstr( LINKEC20_USART);  //�����־ //����
//   //USART_ITConfig(LINKEC20_USART,USART_IT_TC,DISABLE);  
//   /* �������  */
//   LINKEC20_USART_Use_DMA_Tx_Flag = 0;  
//        
//} 



///* ʹ�ú��� Use_DMA_tx ����DMA�������ݡ� */
//static void Use_LINKEC20_DMA_tx(uint8_t *data,uint16_t size)  
//{  
//	

//    /* �ȴ����� */
//    while (LINKEC20_USART_Use_DMA_Tx_Flag); 
//    LINKEC20_USART_Use_DMA_Tx_Flag = 1;  
//    /* �������� */
//    memcpy(LINKEC20_USART_SEND_BUF,data,size);  
//		
//	
//    /* ���ô������ݳ��� */  
//    DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Tx_Sream,size);  
//    /* ��DMA,��ʼ���� */  
//    DMA_Cmd(LINKEC20_USART_DMA_Tx_Sream,ENABLE);  
//} 



///* ʹ�ú��� Use_DMA_tx �������� */
//void Sent_to_EC20_with_Block(uint8_t *data,uint16_t size)
//{
//	Use_LINKEC20_DMA_tx(data,size);
//}



///* ʹ�ú��� Use_DMA_tx �����ַ����� */
//void Use_LINKEC20_DMA_TX_String(char *str)
//{
//	uint16_t size;
//	size = sizeof(str);
//	Use_LINKEC20_DMA_tx((uint8_t *)str,size) ;
//	
//}








/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* ����һ���ֽ����ݵ�USART */
    USART_SendData(pUSARTx,ch);

    /* �ȴ��������ݼĴ���Ϊ�� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}




/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k=0;
    do
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');

    /* �ȴ�������� */
    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
    {}
       //USART_ClearITPendingBit(LINKEC20_USART,USART_IT_RXNE);
}
/*****************  �������EC20 **********************/
void EC20_SendCMD(char *str)
{
 Usart_SendString(LINKEC20_USART,str);
	
}

/*****************  �������� **********************/
//void Usart_SendBlock( USART_TypeDef * pUSARTx, uint8_t *Rxbuffer ,uint8_t  Rxcounter)
//{
//    i=0;
//    while(Rxcounter--)
//    {
//        USART_SendData(pUSARTx,Rxbuffer[i++]);
//    }
//    /* �ȴ�������� */
//    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
//    {}
//    Rxcounter=0;
//}


/*****************  ����һ��16λ�� **********************/
//void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    /* ȡ���߰�λ */
//    temp_h = (ch&0XFF00)>>8;
//    /* ȡ���Ͱ�λ */
//    temp_l = ch&0XFF;

//    /* ���͸߰�λ */
//    USART_SendData(pUSARTx,temp_h);
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

//    /* ���͵Ͱ�λ */
//    USART_SendData(pUSARTx,temp_l);
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
//}




