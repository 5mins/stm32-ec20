/**
  ******************************************************************************
  * @file    bsp_LINKEC20_USART.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重现c库printf函数到usart端口
  ******************************************************************************

  ******************************************************************************
  */

#include "./BSP/usart/bsp_linkec20_usart.h"
#include <string.h>
#include  <includes.h>

uint8_t LINKEC20_USART_RECEIVE_BUF[LINKEC20_USART_RECEIVE_BUF_SIZE]={0};//LINKEC20串口DMA接收缓冲区
type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//当前接收的完整消息
//uint8_t LINKEC20_USART_had_been_Read=1;//LINKEC20消息读取状态，0未读，1已读
//uint8_t LINKEC20_USART_had_been_Sent=0;//LINKEC20当前消息发送状态，0未发，1已发送


//uint8_t LINKEC20_USART_SEND_BUF[LINKEC20_USART_SEND_BUF_SIZE];//LINKEC20串口DMA发送缓冲区
//uint8_t LINKEC20_USART_Use_DMA_Tx_Flag =0;

//uint8_t i =0;



void LINKEC20_USART_ALL_Init(void)
{
	LINKEC20_USART_Config();
	//DMA_Use_LINKEC20_USART_Tx_Init();//发送的数据量不大，不使能DMA发送，改用任务发送，减少系统切换开销。
	DMA_Use_LINKEC20_USART_Rx_Init();
	
}


/**
	* @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
	* @param  无
	* @retval 无
	*/
static void LINKEC20_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd( LINKEC20_USART_RX_GPIO_CLK|LINKEC20_USART_TX_GPIO_CLK, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(LINKEC20_USART_CLK, ENABLE);

    	/* 管脚映射 Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(LINKEC20_USART_RX_GPIO_PORT,LINKEC20_USART_RX_SOURCE, LINKEC20_USART_RX_AF);
    GPIO_PinAFConfig(LINKEC20_USART_TX_GPIO_PORT,LINKEC20_USART_TX_SOURCE,LINKEC20_USART_TX_AF);

  	/* 配置串口管脚 */
    /* Configure USART Tx as alternate function  */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              	//选择输出类型（推挽或开漏）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                	//选择上拉/下拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                	//选择 I/O 方向（输入、输出、AF、模拟）
    GPIO_InitStructure.GPIO_Pin = LINKEC20_USART_TX_PIN  ;      	//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;          	//速度 
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
	
			
    /* 配置串_USART 模式 */
    USART_InitStructure.USART_BaudRate = LINKEC20_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		 /* When using Parity the word length must be configured to 9 bits */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(LINKEC20_USART, &USART_InitStructure);

    /* 嵌套向量中断控制器NVIC配置 */
		NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = LINKEC20_USART_IRQ;		/* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				/* 子优先级为1 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* 使能中断 */
    NVIC_Init(&NVIC_InitStructure);								 					/* 初始化配置NVIC */


	/* 配置串口中断 */
	//USART_ITConfig(LINKEC20_USART, USART_IT_ORE, ENABLE);//开启串口溢出中断
	//USART_ITConfig(LINKEC20_USART, USART_IT_RXNE, ENABLE);/* 使能串口接收中断 */
	USART_ITConfig(LINKEC20_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(LINKEC20_USART, USART_IT_TC,   DISABLE);
  USART_ITConfig(LINKEC20_USART, USART_IT_TXE,  DISABLE);  
  USART_ITConfig(LINKEC20_USART, USART_IT_IDLE, DISABLE);/* 使能IDLE中断 *///改用rtos系统任务循环读取消息，不使能空闲中断

	
  /* 使能串口 */
  USART_Cmd(LINKEC20_USART, ENABLE);

    
}

    /* 复位串口状态，慎用，
		PE：奇偶校验错误 (Parity error)
		FE：帧错误 (Framing error)
		ORE：上溢错误 (Overrun error)
		USART_IT_RXNE：读取数据寄存器不为空 中断
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
//        USART_ReceiveData(pUSARTx);////这里可以设置一个变量来获取receivedata的值，防止数据丢失。
//    }
//}


/* DMA发送数据初始化 */
//static void DMA_Use_LINKEC20_USART_Tx_Init(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    DMA_InitTypeDef DMA_InitStructure;


//    /* 使能DMA时钟 */
//    RCC_AHB1PeriphClockCmd(LINKEC20_USART_DMA_Tx_CLOCK,ENABLE);

//    /* 配置使用DMA发送数据 */
//    DMA_DeInit(DMA1_Stream6); 

//    DMA_InitStructure.DMA_Channel             = LINKEC20_USART_DMA_Tx_CHANNEL;               /* 配置DMA通道 */
//    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(LINKEC20_USART->DR));   /* 目的 */
//    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)LINKEC20_USART_SEND_BUF;             /* 源 */
//    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;    /* 方向 */
//    DMA_InitStructure.DMA_BufferSize          = LINKEC20_USART_SEND_BUF_SIZE;      /* 长度 */                  
//    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* 外设地址是否自增 */
//    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* 内存地址是否自增 */
//    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* 目的数据带宽 */
//    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* 源数据宽度 */
//    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* 单次传输模式/循环传输模式 */
//    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;             /* DMA优先级 */
//    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFO模式/直接模式 */
//    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO大小 */
//    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* 单次传输 */
//    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

//    /* 配置DMA */
//    DMA_Init(LINKEC20_USART_DMA_Tx_Sream, &DMA_InitStructure);

//    /*使能DMA中断 */
//		DMA_ClearFlag(LINKEC20_USART_DMA_Tx_Sream,LINKEC20_USART_DMA_Tx_Flags);  //防止立即产生中断
//    DMA_ITConfig(LINKEC20_USART_DMA_Tx_Sream, DMA_IT_TC, ENABLE);//test

//    /* 使能串口的DMA发送接口 */
//    USART_DMACmd(LINKEC20_USART, USART_DMAReq_Tx, ENABLE);

//    /* 配置DMA中断优先级 */
//    NVIC_InitStructure.NVIC_IRQChannel                   = LINKEC20_USART_DMA_Tx_IRQ;           
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    /* 不使能DMA */                  
//    DMA_Cmd(LINKEC20_USART_DMA_Tx_Sream, DISABLE);
//}



/* DMA接收数据初始化 */
static void DMA_Use_LINKEC20_USART_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    LINKEC20_USART_Rx.LINKEC20_USART_RxLen = 0;
	
    /* 使能DMA1时钟 */
    RCC_AHB1PeriphClockCmd(LINKEC20_USART_DMA_Rx_CLOCK, ENABLE);

    /* 配置使用DMA接收数据 */
    DMA_DeInit(LINKEC20_USART_DMA_Rx_Tream); 

    DMA_InitStructure.DMA_Channel             = LINKEC20_USART_DMA_Rx_CHANNEL;               /* 配置DMA通道 */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(LINKEC20_USART->DR));   /* 源 */
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)LINKEC20_USART_RECEIVE_BUF;   /* 目的 */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* 方向 */
    DMA_InitStructure.DMA_BufferSize          = LINKEC20_USART_RECEIVE_BUF_SIZE;    /* 长度 */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* 外设地址是否自增 */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* 内存地址是否自增 */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* 目的数据带宽 */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* 源数据宽度 */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* 单次传输模式/循环传输模式 */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_VeryHigh;        /* DMA优先级 */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFO模式/直接模式 */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull;    /* FIFO大小 */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* 单次传输 */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /*  配置DMA */
    DMA_Init(LINKEC20_USART_DMA_Rx_Tream, &DMA_InitStructure);

    /* 由于接收不需要DMA中断，故不设置DMA中断 */

    /* 使能串口的DMA接收 */
    USART_DMACmd(LINKEC20_USART,USART_DMAReq_Rx,ENABLE);

    /* 由于接收不需要DMA中断，故不能配置DMA中断优先级 */

    /* 使能DMA */ 
    DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE);
}


/* 在串口接收完成中断中最终会调用deal_irq_rx_end函数，
			在该函数中清除中断标识，并将接收到的数据拷贝到tUART1_Rx.UART1RxBuf中。 */
//uint8_t deal_LINKEC20_irq_rx_end(uint8_t *buf)  
//{     
//		
//    uint16_t len = 0; 

//		    LINKEC20_USART->SR;  /* 清USART_IT_IDLE标志 */
//        LINKEC20_USART->DR; /* 清USART_IT_IDLE标志 */
//				
//			/*等待接收完成 未加计数器可能有死循环 Waiting the end of Data transfer */
//			//while (USART_GetFlagStatus(LINKEC20_USART,USART_FLAG_TC)==RESET){};  
//			
//				/* 清除标志位 Clear USART Transfer Complete Flags */
//			//USART_ClearFlag(LINKEC20_USART,USART_FLAG_TC);
//				
//			//while (DMA_GetFlagStatus(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags)==RESET){};

//			
//        /* 关闭接收DMA  */
//        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,DISABLE);  
//			/* 清除标志位 Clear DMA Transfer Complete Flags */
//        DMA_ClearFlag(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags);  

//        /* 获得接收帧帧长 */
//        len = LINKEC20_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream);  
//				//拷贝前，先清空原缓存数据
//				memset(buf,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));
//        memcpy(buf,LINKEC20_USART_RECEIVE_BUF,len);  

//			  //LINKEC20_USART_had_been_Read=0;//标记未读状态
//			  //LINKEC20_USART_had_been_Sent=0;
//			
//        /* 重新设置传输数据长度 */
//        DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_RECEIVE_BUF_SIZE);  
//        /* 打开DMA */
//        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE);  

//        return len;  

//}  






// void deal_LINKEC20_irq_tx_end(void)  	 
//{  
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x43);//debug_mark
//	
//   /* 关闭发送完成中断  */ 
//		//USART_NewIstr( LINKEC20_USART);  //清除标志 //测试
//   //USART_ITConfig(LINKEC20_USART,USART_IT_TC,DISABLE);  
//   /* 发送完成  */
//   LINKEC20_USART_Use_DMA_Tx_Flag = 0;  
//        
//} 



///* 使用函数 Use_DMA_tx 启动DMA发送数据。 */
//static void Use_LINKEC20_DMA_tx(uint8_t *data,uint16_t size)  
//{  
//	

//    /* 等待空闲 */
//    while (LINKEC20_USART_Use_DMA_Tx_Flag); 
//    LINKEC20_USART_Use_DMA_Tx_Flag = 1;  
//    /* 复制数据 */
//    memcpy(LINKEC20_USART_SEND_BUF,data,size);  
//		
//	
//    /* 设置传输数据长度 */  
//    DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Tx_Sream,size);  
//    /* 打开DMA,开始发送 */  
//    DMA_Cmd(LINKEC20_USART_DMA_Tx_Sream,ENABLE);  
//} 



///* 使用函数 Use_DMA_tx 发送数组 */
//void Sent_to_EC20_with_Block(uint8_t *data,uint16_t size)
//{
//	Use_LINKEC20_DMA_tx(data,size);
//}



///* 使用函数 Use_DMA_tx 发送字符串。 */
//void Use_LINKEC20_DMA_TX_String(char *str)
//{
//	uint16_t size;
//	size = sizeof(str);
//	Use_LINKEC20_DMA_tx((uint8_t *)str,size) ;
//	
//}








/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
    /* 发送一个字节数据到USART */
    USART_SendData(pUSARTx,ch);

    /* 等待发送数据寄存器为空 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}




/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k=0;
    do
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');

    /* 等待发送完成 */
    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
    {}
       //USART_ClearITPendingBit(LINKEC20_USART,USART_IT_RXNE);
}
/*****************  发送命令到EC20 **********************/
void EC20_SendCMD(char *str)
{
 Usart_SendString(LINKEC20_USART,str);
	
}

/*****************  发送数组 **********************/
//void Usart_SendBlock( USART_TypeDef * pUSARTx, uint8_t *Rxbuffer ,uint8_t  Rxcounter)
//{
//    i=0;
//    while(Rxcounter--)
//    {
//        USART_SendData(pUSARTx,Rxbuffer[i++]);
//    }
//    /* 等待发送完成 */
//    while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
//    {}
//    Rxcounter=0;
//}


/*****************  发送一个16位数 **********************/
//void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
//{
//    uint8_t temp_h, temp_l;

//    /* 取出高八位 */
//    temp_h = (ch&0XFF00)>>8;
//    /* 取出低八位 */
//    temp_l = ch&0XFF;

//    /* 发送高八位 */
//    USART_SendData(pUSARTx,temp_h);
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

//    /* 发送低八位 */
//    USART_SendData(pUSARTx,temp_l);
//    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
//}




