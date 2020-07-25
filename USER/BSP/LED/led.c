#include "./BSP/LED/led.h" 
#include  <includes.h>
//////////////////////////////////////////////////////////////////////////////////	 			  
////////////////////////////////////////////////////////////////////////////////// 	 

//初始化PC13输出口.并使能时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);//使能led1(GPIOC)时钟

  //GPIO初始化设置
  GPIO_InitStructure.GPIO_Pin = LED1_PIN ;//LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);//设置高，灯灭
	//GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN);

}
//led1 闪烁
void LED_BLINK(void){
OS_ERR    err;
LED1(ON);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
LED1(OFF);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
LED1(ON);
OSTimeDly(500,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
LED1(OFF);	

}








