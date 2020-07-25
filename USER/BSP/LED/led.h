#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 

//引脚定义
/*******************************************************/
//LED1
#define LED1_PIN                  GPIO_Pin_8                 
#define LED1_GPIO_PORT            GPIOA                      
#define LED1_GPIO_CLK             RCC_AHB1Periph_GPIOA

 /* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			 {p->BSRRL=i;}		//设置为高电平
#define digitalLo(p,i)			 {p->BSRRH=i;}		//输出低电平
#define digitalToggle(p,i)	 {p->ODR ^=i;}		//输出反转状态

/* 定义控制IO的宏 */
#define LED1_TOGGLE		digitalToggle(LED1_GPIO_PORT,LED1_PIN)

/** 控制LED灯亮灭的宏，
	* LED低电平亮，设置ON=0，OFF=1
	* 若LED高电平亮，把宏设置成ON=1 ，OFF=0 即可
	*/
#define ON  0
#define OFF 1

/* 带参宏，可以像内联函数一样使用 */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)
					
					

					

void LED_Init(void);//初始化		
void LED_BLINK(void);					

#endif
