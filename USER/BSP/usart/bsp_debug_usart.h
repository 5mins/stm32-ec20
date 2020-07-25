#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

#define DEBUG_MARK 1


#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK                         RCC_APB1Periph_USART2
#define DEBUG_USART_BAUDRATE                    115200     		//4800 9600 19200 38400 57600 115200  230400 460800  921600 

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_3
#define DEBUG_USART_RX_AF                       GPIO_AF_USART2
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource3

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_2
#define DEBUG_USART_TX_AF                       GPIO_AF_USART2
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource2

#define DEBUG_USART_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART_IRQ                 	      USART2_IRQn

#define DEBUG_USART_DMA_Tx_CHANNEL							DMA_Channel_4
#define DEBUG_USART_DMA_Tx_Sream								DMA1_Stream6
#define DEBUG_USART_DMA_Tx_IRQHandler           DMA1_Stream6_IRQHandler
#define DEBUG_USART_DMA_Tx_IRQ                 	DMA1_Stream6_IRQn
#define DEBUG_USART_DMA_Tx_CLOCK								RCC_AHB1Periph_DMA1
#define DEBUG_USART_DMA_Tx_Flags                DMA_FLAG_TCIF6
#define DEBUG_USART_DMA_Tx_IT_Flags             DMA_IT_TCIF6

#define DEBUG_USART_DMA_Rx_CHANNEL							DMA_Channel_4
#define DEBUG_USART_DMA_Rx_Tream								DMA1_Stream5
#define DEBUG_USART_DMA_Rx_IRQHandler           DMA1_Stream5_IRQHandler
#define DEBUG_USART_DMA_Rx_IRQ                 	DMA1_Stream5_IRQn
#define DEBUG_USART_DMA_Rx_CLOCK								RCC_AHB1Periph_DMA1
#define DEBUG_USART_DMA_Rx_Flags                DMA_FLAG_TCIF5
#define DEBUG_USART_DMA_Rx_IT_Flags             DMA_IT_TCIF5

#define DEBUG_USART_RECEIVE_BUF_SIZE  					170
#define DEBUG_USART_SEND_BUF_SIZE  							170



typedef struct
{
  uint8_t  DEBUG_USART_RxLen;  
	uint8_t  DEBUG_USART_RECEIVE[DEBUG_USART_RECEIVE_BUF_SIZE];
} type_DEBUG_USART_Rx;

void Debug_USART_ALL_Init(void);
static void Debug_USART_Config(void);
static void DMA_Use_DEBUG_USART_Rx_Init(void);
static void DMA_Use_DEBUG_USART_Tx_Init(void);
static void Use_DEBUG_DMA_tx(uint8_t *data,uint16_t size);
void deal_DEBUG_irq_tx_end(void);
uint8_t deal_DEBUG_irq_rx_end(uint8_t *buf);
void Sent_to_DEBUG_with_Block(uint8_t *data,uint16_t size);


//int fputc(int ch, FILE *f);



#endif /* __USART1_H */
