#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 

//���Ŷ���
/*******************************************************/
//LED1
#define LED1_PIN                  GPIO_Pin_8                 
#define LED1_GPIO_PORT            GPIOA                      
#define LED1_GPIO_CLK             RCC_AHB1Periph_GPIOA

 /* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//����Ϊ�ߵ�ƽ
#define digitalLo(p,i)			 {p->BSRRH=i;}		//����͵�ƽ
#define digitalToggle(p,i)	 {p->ODR ^=i;}		//�����ת״̬

/* �������IO�ĺ� */
#define LED1_TOGGLE		digitalToggle(LED1_GPIO_PORT,LED1_PIN)

/** ����LED������ĺ꣬
	* LED�͵�ƽ��������ON=0��OFF=1
	* ��LED�ߵ�ƽ�����Ѻ����ó�ON=1 ��OFF=0 ����
	*/
#define ON  0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)
					
					

					

void LED_Init(void);//��ʼ��		
void LED_BLINK(void);					

#endif
