#ifndef __LINKEC20_USART_H
#define	__LINKEC20_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


//#define USART1_DR_Base  0x40013804		// 0x40013800 + 0x04 = 0x40013804
//#define SENDBUFF_SIZE   5000

#define LINKEC20_USART                             USART1
#define LINKEC20_USART_CLK                         RCC_APB2Periph_USART1
#define LINKEC20_USART_BAUDRATE                    115200

#define LINKEC20_USART_RX_GPIO_PORT                GPIOB
#define LINKEC20_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define LINKEC20_USART_RX_PIN                      GPIO_Pin_7
#define LINKEC20_USART_RX_AF                       GPIO_AF_USART1
#define LINKEC20_USART_RX_SOURCE                   GPIO_PinSource7

#define LINKEC20_USART_TX_GPIO_PORT                GPIOB
#define LINKEC20_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define LINKEC20_USART_TX_PIN                      GPIO_Pin_6
#define LINKEC20_USART_TX_AF                       GPIO_AF_USART1
#define LINKEC20_USART_TX_SOURCE                   GPIO_PinSource6

#define LINKEC20_USART_IRQHandler                  USART1_IRQHandler
#define LINKEC20_USART_IRQ                 		     USART1_IRQn

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
void Usart_SendBlock( USART_TypeDef * pUSARTx, uint8_t *Rxbuffer ,uint8_t  Rxcounter);

void USART2_NewIstr(USART_TypeDef * pUSARTx);
void LINKEC20_USART_Config(void);
//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
