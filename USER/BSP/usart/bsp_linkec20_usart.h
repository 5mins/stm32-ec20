#ifndef __LINKEC20_USART_H
#define	__LINKEC20_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


//#define USART1_DR_Base  0x40013804		// 0x40013800 + 0x04 = 0x40013804
//#define SENDBUFF_SIZE   5000

//#define LINKEC20_USART                             USART1
//#define LINKEC20_USART_CLK                         RCC_APB2Periph_USART1
//#define LINKEC20_USART_BAUDRATE                    921600     		//ec20 支持 auto detectable <rate>s (4800,9600,19200,38400,57600,115200),supported fixed-only <rate>(9600,19200,38400,57600,115200,230400,460800,921600,2900000,3000000,3200000,3686400,4000000) 
//#define LINKEC20_USART_RX_GPIO_PORT                GPIOB
//#define LINKEC20_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB		//
//#define LINKEC20_USART_RX_PIN                      GPIO_Pin_7						//
//#define LINKEC20_USART_RX_AF                       GPIO_AF_USART1				//
//#define LINKEC20_USART_RX_SOURCE                   GPIO_PinSource7				

//#define LINKEC20_USART_TX_GPIO_PORT                GPIOB// GPIOB
//#define LINKEC20_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB				
//#define LINKEC20_USART_TX_PIN                      GPIO_Pin_6        				
//#define LINKEC20_USART_TX_AF                       GPIO_AF_USART1				
//#define LINKEC20_USART_TX_SOURCE                   GPIO_PinSource6				

//#define LINKEC20_USART_IRQHandler                  USART1_IRQHandler		
//#define LINKEC20_USART_IRQ                 		     USART1_IRQn

//#define LINKEC20_USART_DMA_Tx_CHANNEL							 DMA_Channel_4			
//#define LINKEC20_USART_DMA_Tx_Sream								 DMA2_Stream7							
//#define LINKEC20_USART_DMA_Tx_IRQHandler           DMA2_Stream7_IRQHandler		
//#define LINKEC20_USART_DMA_Tx_IRQ                  DMA2_Stream7_IRQn				
//#define LINKEC20_USART_DMA_Tx_CLOCK								 RCC_AHB1Periph_DMA2			
//#define LINKEC20_USART_DMA_Tx_Flags                DMA_FLAG_TCIF7					
//#define LINKEC20_USART_DMA_Tx_IT_Flags             DMA_IT_TCIF7					

//#define LINKEC20_USART_DMA_Rx_CHANNEL							 DMA_Channel_4
//#define LINKEC20_USART_DMA_Rx_Tream								 DMA2_Stream2
//#define LINKEC20_USART_DMA_Rx_IRQHandler           DMA2_Stream2_IRQHandler
//#define LINKEC20_USART_DMA_Rx_IRQ                  DMA2_Stream2_IRQn
//#define LINKEC20_USART_DMA_Rx_CLOCK								 RCC_AHB1Periph_DMA2
//#define LINKEC20_USART_DMA_Rx_Flags                DMA_FLAG_TCIF2
//#define LINKEC20_USART_DMA_Rx_IT_Flags             DMA_IT_TCIF2
////////////
#define LINKEC20_USART                             USART1
#define LINKEC20_USART_CLK                         RCC_APB2Periph_USART1
#define LINKEC20_USART_BAUDRATE                    115200     		//ec20 支持 auto detectable <rate>s (4800,9600,19200,38400,57600,115200),supported fixed-only <rate>(9600,19200,38400,57600,115200,230400,460800,921600,2900000,3000000,3200000,3686400,4000000) 
#define LINKEC20_USART_RX_GPIO_PORT                GPIOB
#define LINKEC20_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB		//
#define LINKEC20_USART_RX_PIN                      GPIO_Pin_7						//
#define LINKEC20_USART_RX_AF                       GPIO_AF_USART1				//
#define LINKEC20_USART_RX_SOURCE                   GPIO_PinSource7				

#define LINKEC20_USART_TX_GPIO_PORT                GPIOB// GPIOB
#define LINKEC20_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB				
#define LINKEC20_USART_TX_PIN                      GPIO_Pin_6        				
#define LINKEC20_USART_TX_AF                       GPIO_AF_USART1				
#define LINKEC20_USART_TX_SOURCE                   GPIO_PinSource6				

#define LINKEC20_USART_IRQHandler                  USART1_IRQHandler		
#define LINKEC20_USART_IRQ                 		     USART1_IRQn

#define LINKEC20_USART_DMA_Tx_CHANNEL							 DMA_Channel_4			
#define LINKEC20_USART_DMA_Tx_Sream								 DMA2_Stream7							
#define LINKEC20_USART_DMA_Tx_IRQHandler           DMA2_Stream7_IRQHandler		
#define LINKEC20_USART_DMA_Tx_IRQ                  DMA2_Stream7_IRQn				
#define LINKEC20_USART_DMA_Tx_CLOCK								 RCC_AHB1Periph_DMA2			
#define LINKEC20_USART_DMA_Tx_Flags                DMA_FLAG_TCIF7					
#define LINKEC20_USART_DMA_Tx_IT_Flags             DMA_IT_TCIF7					

#define LINKEC20_USART_DMA_Rx_CHANNEL							 DMA_Channel_4
#define LINKEC20_USART_DMA_Rx_Tream								 DMA2_Stream2
#define LINKEC20_USART_DMA_Rx_IRQHandler           DMA2_Stream2_IRQHandler
#define LINKEC20_USART_DMA_Rx_IRQ                  DMA2_Stream2_IRQn
#define LINKEC20_USART_DMA_Rx_CLOCK								 RCC_AHB1Periph_DMA2
#define LINKEC20_USART_DMA_Rx_Flags                DMA_FLAG_TCIF2
#define LINKEC20_USART_DMA_Rx_IT_Flags             DMA_IT_TCIF2

#define LINKEC20_USART_RECEIVE_BUF_SIZE  					 400
//#define LINKEC20_USART_SEND_BUF_SIZE  						 256

typedef struct
{
  uint8_t  LINKEC20_USART_RxLen;  
	uint8_t  LINKEC20_USART_RECEIVE[LINKEC20_USART_RECEIVE_BUF_SIZE];
} type_LINKEC20_USART_Rx;


void LINKEC20_USART_ALL_Init(void);
static void LINKEC20_USART_Config(void);
static void DMA_Use_LINKEC20_USART_Rx_Init(void);
static void DMA_Use_LINKEC20_USART_Tx_Init(void);
static void Use_LINKEC20_DMA_tx(uint8_t *data,uint16_t size);
void deal_LINKEC20_irq_tx_end(void);
uint8_t deal_LINKEC20_irq_rx_end(uint8_t *buf);
void Sent_to_EC20_with_Block(uint8_t *data,uint16_t size);
void Use_LINKEC20_DMA_TX_String(char *str);
void USART_NewIstr(USART_TypeDef * pUSARTx);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void EC20_SendCMD(char *str);//发送命令

#endif /* __USART1_H */
