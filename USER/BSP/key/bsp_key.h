#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

//���Ŷ���
/*******************************************************/
#define KEY1_INT_GPIO_PORT                GPIOA
#define KEY1_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define KEY1_INT_GPIO_PIN                 GPIO_Pin_0
#define KEY1_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOA
#define KEY1_INT_EXTI_PINSOURCE           EXTI_PinSource0
#define KEY1_INT_EXTI_LINE                EXTI_Line0
#define KEY1_INT_EXTI_IRQ                 EXTI0_IRQn

#define KEY1_IRQHandler                   EXTI0_IRQHandler

//#define KEY2_INT_GPIO_PORT                GPIOC
//#define KEY2_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOC
//#define KEY2_INT_GPIO_PIN                 GPIO_Pin_13
//#define KEY2_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOC
//#define KEY2_INT_EXTI_PINSOURCE           EXTI_PinSource13
//#define KEY2_INT_EXTI_LINE                EXTI_Line13
//#define KEY2_INT_EXTI_IRQ                 EXTI15_10_IRQn

//#define KEY2_IRQHandler                   EXTI15_10_IRQHandler



/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0



//#define digitalHi(p,i) {p->BSRRL=i;} //����Ϊ�ߵ�ƽ
//#define digitalLo(p,i) {p->BSRRH=i;} //����͵�ƽ

void EXTI_Key_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);
void Key1_GPIO_Config(void);
uint8_t GetPinStateOfKey1(void);


void mySTART_STM32STOPMODE(void);


/*****************************************�û�������������************************************************/

/*ʹ���û����ݣ�ÿһ��������һ���������Թ��û�����ʹ�ã��������
 *��������������ڸĳ�0���ɣ����Ҫ�þ͸ĳ�1����������˷�һ����
 *�ڵ��ڴ�
 */
#define USER_DATA_EN			0

#define KEY_WOBBLE_TIME		10      //��������ʱ�䡣Ҳ��������ʱ��,��λms
 
#define KEY_FIXED_PERIOD	 10     //�̶�Ƶ�ʵ��ð���״̬���º���,������Ϊ�������ڣ����ڵ�λΪms

//����ص���������
typedef  uint8_t    (*KEY_CALLBACK_PTR)(void);

#define KEY_TIMES_MAX (0XFF)
typedef enum{
	KEY_ACCESS_READ = 0,   //Ĭ���Ƕ�ȡ
	KEY_ACCESS_WRITE_CLEAR = 0x01
}ACCESS_TYPE;

/************�������ݾ�����Ҫ���ģ�ֱ��ʹ�ü���*******************************/
typedef enum
{
	KEY_DOWN 				= 1,
	KEY_UP	 				= 2,
	KEY_UP_WOBBLE 	= 3,//ȷ�ϵ�������״̬
	KEY_DOWN_WOBBLE = 4 //ȷ�ϰ�������״̬
}KEY_STATE;

typedef enum
{
	CHANGED 				    = 1,
	NOT_CHANGED	 				= 2,
}KEY_STATE_CHAGE;

typedef struct
{
	KEY_CALLBACK_PTR    GetStatePtr;	//���ڻ�ȡ����״̬�ĺ��� 
	uint8_t     				Times;				//���²��������1��ʹ�ú���Ӧ�ó����1
  KEY_STATE		        State;
  KEY_STATE_CHAGE     StateChange;                      
#ifdef KEY_FIXED_PERIOD
	unsigned char				Time_ms;			//���ڹ̶����ڵ���״̬���º����ļ�ʱ
#endif
  
#if USER_DATA_EN>0             
	uint8_t				      User_Data;		//�û������������û�����ʹ��
#endif
  
}KEY;

/*			����������	*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack);
void Key_RefreshState(KEY* theKey);
uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt);
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State);

#endif /* __EXTI_H */

