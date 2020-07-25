#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
//#include <stdio.h>
#include "./Bsp/usart/bsp_linkec20_usart.h"


//#define USART1_DR_Base  0x40013804		// 0x40013800 + 0x04 = 0x40013804
//#define SENDBUFF_SIZE   5000

#define DEBUG_USART                             USART2
#define DEBUG_USART_CLK                         RCC_APB1Periph_USART2
#define DEBUG_USART_BAUDRATE                    115200

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

#define Rxbuffer_num  											   100

void Debug_USART_Config(void);
//int fputc(int ch, FILE *f);



#endif /* __USART1_H */
