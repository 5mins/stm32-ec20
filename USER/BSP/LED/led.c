#include "./BSP/LED/led.h" 
#include  <includes.h>
//////////////////////////////////////////////////////////////////////////////////	 			  
////////////////////////////////////////////////////////////////////////////////// 	 

//��ʼ��PC13�����.��ʹ��ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);//ʹ��led1(GPIOC)ʱ��

  //GPIO��ʼ������
  GPIO_InitStructure.GPIO_Pin = LED1_PIN ;//LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);//��ʼ��GPIO
	
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);//���øߣ�����
	//GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);

}
//led1 ��˸
void LED_BLINK(void){
OS_ERR    err;
LED1(ON);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
LED1(OFF);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
LED1(ON);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
LED1(OFF);	

}








