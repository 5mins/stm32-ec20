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
  
#include "./BSP/usart/bsp_debug_usart.h"
#include "./BSP/usart/bsp_linkec20_usart.h"
#include <string.h>
#include "includes.h"

uint8_t DEBUG_USART_RECEIVE_BUF[DEBUG_USART_RECEIVE_BUF_SIZE];//debug串口DMA接收缓冲区
type_DEBUG_USART_Rx  DEBUG_USART_Rx;//当前接收的完整消息
//uint8_t DEBUG_USART_had_been_Read=1;//debug消息读取状态，0未读，1已读
//uint8_t DEBUG_USART_had_been_Sent=0;//debug当前消息发送状态，0未发，1已发送


uint8_t DEBUG_USART_SEND_BUF[DEBUG_USART_SEND_BUF_SIZE];//debug串口DMA发送缓冲区
uint8_t DEBUG_USART_Use_DMA_Tx_Flag =0;



void Debug_USART_ALL_Init(void)
{
	Debug_USART_Config();
	//DMA_Use_DEBUG_USART_Tx_Init();
	DMA_Use_DEBUG_USART_Rx_Init();
	
}

/**
	* @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
	* @param  无
	* @retval 无
	*/
static void Debug_USART_Config(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	
	/* 使能PA口时钟 */	
  RCC_AHB1PeriphClockCmd( DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK, ENABLE);

  /* Enable UART clock */
  RCC_APB1PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  
	/* 管脚映射 */
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE, DEBUG_USART_RX_AF);/* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF); /* Connect PXx to USARTx_Rx*/

	/* 配置串口管脚 */
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//选择输出类型（推挽或开漏）
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//选择上拉/下拉
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//选择 I/O 方向（输入、输出、AF、模拟）
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;	//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	//速度 
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
			
  /* 配置串口工作模式 */ 
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;							/* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;							/* 字长(数据位+校验位)：8 */
    /* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;									/* 停止位：1个停止位 */
  USART_InitStructure.USART_Parity = USART_Parity_No;											/* 校验位选择：不使用校验 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;/* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						/* USART模式控制：同时使能接收和发送 */  
  USART_Init(DEBUG_USART, &USART_InitStructure); 														/* 完成USART初始化配置 */
	
	/* 设置中断 */
	NVIC_InitTypeDef NVIC_InitStructure_debug;
  NVIC_InitStructure_debug.NVIC_IRQChannel = DEBUG_USART_IRQ;/* 配置USART为中断源 */
  NVIC_InitStructure_debug.NVIC_IRQChannelPreemptionPriority = 2;/* 抢断优先级 */
  NVIC_InitStructure_debug.NVIC_IRQChannelSubPriority = 0; /* 子优先级 */
  NVIC_InitStructure_debug.NVIC_IRQChannelCmd = ENABLE;/* 使能中断 */
  NVIC_Init(&NVIC_InitStructure_debug);/* 初始化配置NVIC */
	USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);//防止配置完就进入中断
	
	
	/* 配置串口中断 */
	//USART_ITConfig(DEBUG_USART, USART_IT_ORE, ENABLE);//开启串口溢出中断
	//USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);/* 使能串口接收中断 */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(DEBUG_USART, USART_IT_TC,   DISABLE);
  USART_ITConfig(DEBUG_USART, USART_IT_TXE,  DISABLE);  
  USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);/* 使能IDLE中断 */

	
  /* 使能串口 */
  USART_Cmd(DEBUG_USART, ENABLE);
	
}


///* DMA发送数据初始化 */
//static void DMA_Use_DEBUG_USART_Tx_Init(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    DMA_InitTypeDef DMA_InitStructure;


//    /* 使能DMA时钟 */
//    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_Tx_CLOCK,ENABLE);

//    /* 配置使用DMA发送数据 */
//    DMA_DeInit(DMA1_Stream6); 

//    DMA_InitStructure.DMA_Channel             = DEBUG_USART_DMA_Tx_CHANNEL;               /* 配置DMA通道 */
//    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(DEBUG_USART->DR));   /* 目的 */
//    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)DEBUG_USART_SEND_BUF;             /* 源 */
//    DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;    /* 方向 */
//    DMA_InitStructure.DMA_BufferSize          = DEBUG_USART_SEND_BUF_SIZE;      /* 长度 */                  
//    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* 外设地址是否自增 */
//    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* 内存地址是否自增 */
//    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* 目的数据带宽 */
//    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* 源数据宽度 */
//    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* 单次传输模式/循环传输模式 */
//    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;             /* DMA优先级 */
//    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFO模式/直接模式 */
//    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO大小 */
//    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* 单次传输 */
//    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

//    /* 配置DMA */
//    DMA_Init(DEBUG_USART_DMA_Tx_Sream, &DMA_InitStructure);

//    /*使能DMA中断 */
//		DMA_ClearFlag(DEBUG_USART_DMA_Tx_Sream,DEBUG_USART_DMA_Tx_Flags);  //防止立即产生中断
//    DMA_ITConfig(DEBUG_USART_DMA_Tx_Sream, DMA_IT_TC, ENABLE);

//    /* 使能串口的DMA发送接口 */
//    USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);

//    /* 配置DMA中断优先级 */
//    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_USART_DMA_Tx_IRQ;           
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;          
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; 
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);

//    /* 不使能DMA */                  
//    DMA_Cmd(DEBUG_USART_DMA_Tx_Sream, DISABLE);
//}


/* DMA接收数据初始化 */
static void DMA_Use_DEBUG_USART_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DEBUG_USART_Rx.DEBUG_USART_RxLen = 0;
	
    /* 使能DMA1时钟 */
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_Rx_CLOCK, ENABLE);

    /* 配置使用DMA接收数据 */
    DMA_DeInit(DEBUG_USART_DMA_Rx_Tream); 

    DMA_InitStructure.DMA_Channel             = DEBUG_USART_DMA_Rx_CHANNEL;               /* 配置DMA通道 */
    DMA_InitStructure.DMA_PeripheralBaseAddr  = (uint32_t)(&(DEBUG_USART->DR));   /* 源 */
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)DEBUG_USART_RECEIVE_BUF;   /* 目的 */
    DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;    /* 方向 */
    DMA_InitStructure.DMA_BufferSize          = DEBUG_USART_RECEIVE_BUF_SIZE;    /* 长度 */                  
    DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;    /* 外设地址是否自增 */
    DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;         /* 内存地址是否自增 */
    DMA_InitStructure.DMA_PeripheralDataSize  = DMA_MemoryDataSize_Byte;      /* 目的数据带宽 */
    DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;      /* 源数据宽度 */
    DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;              /* 单次传输模式/循环传输模式 */
    DMA_InitStructure.DMA_Priority            = DMA_Priority_High;        /* DMA优先级 */
    DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;          /* FIFO模式/直接模式 */
    DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_HalfFull; /* FIFO大小 */
    DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_Single;       /* 单次传输 */
    DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_Single;

    /*  配置DMA */
    DMA_Init(DEBUG_USART_DMA_Rx_Tream, &DMA_InitStructure);

    /* 由于接收不需要DMA中断，故不设置DMA中断 */

    /* 使能串口的DMA接收 */
    USART_DMACmd(DEBUG_USART,USART_DMAReq_Rx,ENABLE);

    /* 由于接收不需要DMA中断，故不能配置DMA中断优先级 */

    /* 使能DMA */ 
    DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);
}


/* 在串口接收完成中断中最终会调用deal_irq_rx_end函数，
			在该函数中清除中断标识，并将接收到的数据拷贝到tUART1_Rx.UART1RxBuf中。 */
//uint8_t deal_DEBUG_irq_rx_end(uint8_t *buf)  
//{     
//		    DEBUG_USART->SR;  /* 清USART_IT_IDLE标志 */
//        DEBUG_USART->DR; /* 清USART_IT_IDLE标志 */
//	
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x21);//debug_mark
//			uint16_t len = 0;  
//    /* 接收完成中断 */
//      /* 等待接收完成 未加计数器可能有死循环 Waiting the end of Data transfer */
//			//while (USART_GetFlagStatus(DEBUG_USART,USART_FLAG_TC)==RESET){};  
//        /* 清除标志位 Clear USART Transfer Complete Flags */
////				USART_ClearFlag(DEBUG_USART,USART_FLAG_TC);
//				
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
////			Usart_SendByte(LINKEC20_USART,0x22);//debug_mark
//			//while (DMA_GetFlagStatus(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags)==RESET){};
//         

//  

//        /* 关闭接收DMA  */
//        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,DISABLE);  
//        /* 清除标志位 Clear DMA Transfer Complete Flags */
//        DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags);  
//				//DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_IT_Flags); //test

//        /* 获得接收帧帧长 */
//        len = DEBUG_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream);  
//								//拷贝前，先清空原缓存数据
//				memset(buf,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(buf));
//        memcpy(buf,DEBUG_USART_RECEIVE_BUF,len);  

//			 //DEBUG_USART_had_been_Read=0;//标记未读状态
//				//DEBUG_USART_had_been_Sent=0;//
//			
//        /* 重新设置传输数据长度 */
//        DMA_SetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_RECEIVE_BUF_SIZE);  
//        /* 打开DMA */
//        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);  

//        return len;  
//       

//    
//}  






// void deal_DEBUG_irq_tx_end(void)  	 
//{  
//    //if(USART_GetITStatus(DEBUG_USART, USART_IT_TXE) == RESET)  //RESET OR SET ??
//    {  
//        /* 关闭发送完成中断 （已停用此中断） */ 
//       // USART_ITConfig(DEBUG_USART,USART_IT_TC,DISABLE);  
//        /* 发送完成  */
//        DEBUG_USART_Use_DMA_Tx_Flag = 0;  
//    }    
//} 


///* 使用函数 Use_DMA_tx 启动DMA发送数据。 */
//static void Use_DEBUG_DMA_tx(uint8_t *data,uint16_t size)  
//{  
//    /* 等待空闲 */
//    while (DEBUG_USART_Use_DMA_Tx_Flag);
//    DEBUG_USART_Use_DMA_Tx_Flag = 1;  
//    /* 复制数据 */
//    memcpy(DEBUG_USART_SEND_BUF,data,size);  
//    /* 设置传输数据长度 */  
//    DMA_SetCurrDataCounter(DEBUG_USART_DMA_Tx_Sream,size);  
//    /* 打开DMA,开始发送 */  
//    DMA_Cmd(DEBUG_USART_DMA_Tx_Sream,ENABLE);  
//} 

///* 使用函数 Use_DEBUG_DMA_tx 发送数组 */
//void Sent_to_DEBUG_with_Block(uint8_t *data,uint16_t size)
//{
//	Use_DEBUG_DMA_tx(data,size);
//}


//重定向 c 库函数 printf 到串口，重定向后可使用 printf 函数
int fputc(int ch, FILE *f)
 {
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USART, (uint8_t) ch);

		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);

		return (ch);
 }
 
 
 

/*********************************************END OF FILE**********************/
