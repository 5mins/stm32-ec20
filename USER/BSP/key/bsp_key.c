
#include "BSP/key/bsp_key.h" 
#include "includes.h"



extern STM32_CTR_FLAGS stm32_flat;

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /* 配置中断源：按键1 */
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ;
  /* 配置抢占优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  /* 配置子优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* 配置中断源：按键2，其他使用上面相关配置 */  
//  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
 // NVIC_Init(&NVIC_InitStructure);
}


 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK ,ENABLE);
  
  /* 使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* 配置 NVIC */
  NVIC_Configuration();
  
	/* 选择按键1的引脚 */ 
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
  /* 设置引脚为输入模式 */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
  /* 设置引脚不上拉也不下拉 */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  /* 使用上面的结构体初始化按键 */
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* 连接 EXTI 中断源 到key1引脚 */
  SYSCFG_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE,KEY1_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;
  /* 中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* 下降沿触发 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  /* 使能中断/事件线 */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* 选择按键2的引脚 */ 
 // GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;  
  /* 其他配置与上面相同 */
 // GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      

	/* 连接 EXTI 中断源 到key2 引脚 */
 // SYSCFG_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE,KEY2_INT_EXTI_PINSOURCE);

  /* 选择 EXTI 中断源 */
 // EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* 上升沿触发 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  

}



/**
  * @brief   检测是否有按键按下
  * @param   具体的端口和端口位
  *		@arg GPIOx: x可以是（A...G） 
  *		@arg GPIO_PIN 可以是GPIO_PIN_x（x可以是1...16）
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
//{			
//	/*检测是否有按键按下 */
//	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
//	{	 
//		/*等待按键释放 */
//		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
//		return 	KEY_ON;	 
//	}
//	else
//		return KEY_OFF;
//}
/*********************************************END OF FILE**********************/

/***************************************用户自定义函数***********************************/

//非中断模式

uint8_t GetPinStateOfKey1(void)
{
	if(GPIO_ReadInputDataBit(KEY1_INT_GPIO_PORT,KEY1_INT_GPIO_PIN)==1)
	{
			return 1;
	}
	else
	{
			return 0;
	}
}

/*
************************************************************************************************************************
*                              按键1管脚初始化函数
*
* 函数描述：在按键1使用之前必须先调用
*
*     参数：无
*
*   返回值：无
*
************************************************************************************************************************
*/

void Key1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK,ENABLE);
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN; 
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* 设置引脚速度 */
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  /*使用上面的结构体初始化按键*/
	GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);
}

/*
************************************************************************************************************************
*                              消抖延时函数
*
* 函数描述：用户如果没有定时调用，即没有定义KEY_FIXED_PERIOD，就需要重新根据CPU重写此函数。
*
*     参数：xms        根据不同按键消抖的时间，一般是20ms
*
*   返回值：无
*
*     注意：消抖延时
*
************************************************************************************************************************
*/
#ifndef  KEY_FIXED_PERIOD
void key_delay1ms(uint8_t xms)
{
	OS_ERR  os_err;
	OSTimeDly(xms,OS_OPT_TIME_DLY,&os_err);
}
#endif


/*					以下内容不需要更改，直接使用即可								*/
/*
************************************************************************************************************************
*                              按键创建函数
*
* 函数描述：使用按键前需要创建按键。
*
*     参数：p_Key        指向按键控制变量的指针
*           p_CallBack   用来获取按键电平的回调函数指针，因为不同CPU电平获取的方式不一样，这部分需要用户自己编写
*                        伪代码如下：
*
*                          uint8_t GetPinStateOfKeyXX(void)
*                          {
*                            如果管脚的电平是按键按下时的电平
*                            返回1
*
*                            如果管脚的电平是按键弹起时的电平
*                            返回0
*                          }
*
*   返回值：如果按键处于被按下状态的电平，返回1。
*           如果按键处于弹起状态的电平，返回0。
*
************************************************************************************************************************
*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack)
{
  p_Key->GetStatePtr=p_CallBack;
  p_Key->Times=0;
  p_Key->State=KEY_UP;
  p_Key->StateChange=NOT_CHANGED;
#ifdef KEY_FIXED_PERIOD
	p_Key->Time_ms=0;          //消抖时间初始化为0
#endif
  
#if USER_DATA_EN>0             
	p_Key->User_Data=0;		           //用户变量，可由用户任意使用
#endif
  
}

/*
************************************************************************************************************************
*                              按键状态更新函数
*
* 函数描述：按键驱动函数，只适合STM32单片机库开发，其他类型的单片机可以自行修改。
*
*     参数：p_Key        指向按键控制变量的指针
*
*   返回值：无
*
*     注意：调用频率需要大于20Hz
*
************************************************************************************************************************
*/
void Key_RefreshState(KEY* p_Key)
{
  switch(p_Key->State)
  {
    case KEY_UP:
    {
      if((*(p_Key->GetStatePtr))())//执行回调函数判断按键管脚状态
      {
#ifdef  KEY_FIXED_PERIOD   
        p_Key->Time_ms = 0;
        p_Key->State = KEY_DOWN_WOBBLE;//进行消抖延时
#else
        p_Key->State = KEY_DOWN_WOBBLE;
        key_delay1ms(KEY_WOBBLE_TIME);				
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
#endif
      }
    }break;
    
    #ifdef  KEY_FIXED_PERIOD
    case KEY_DOWN_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >=KEY_WOBBLE_TIME )
      {
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
        else
        {
          p_Key->State = KEY_UP;
        }
      }
    }break;
    #endif

    case KEY_DOWN:
    {
      if( (*(p_Key->GetStatePtr))() == 0 )
      {
#ifdef  KEY_FIXED_PERIOD
        p_Key->Time_ms = 0;
        p_Key->State = KEY_UP_WOBBLE;//进行消抖延时
#else
        key_delay1ms(KEY_WOBBLE_TIME);
        if( (*(p_Key->GetStatePtr))() == 0 )
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//最多允许按下250次没处理
        }
#endif
      }
    }break;

#ifdef  KEY_FIXED_PERIOD
    case KEY_UP_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >= KEY_WOBBLE_TIME )
      {
        if( (*(p_Key->GetStatePtr))()==0)
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//最多允许按下250次没处理
        }
        else
        {
          p_Key->State = KEY_DOWN;
        }
      }
    }break;
#endif
  }
}

/*
************************************************************************************************************************
*                              按键被按下次数访问函数
*
* 函数描述：对按键被按下次数的访问
*
*     参数：p_Key        指向按键控制变量的指针
*           option	     选项，由于按键操作一般只用到一下两种操作，所以其他的就没有包括进来
*                        KEY_ACCESS_READ         读取按键被按下的次数
*                        KEY_ACCESS_WRITE_CLEAR  清零按键被按下的次数
*   返回值：->times的值。
*
*     注意：访问过程没有用信号量保护，如果在同一个任务的上下调用Key_AccessTimes和Key_RefreshState
*           是没有什么关系的，而在系统中如果两个函数在两个任务当中就要加以临界段保护了，不过两个函数
*           一般都要在一个任务中使用，因为每次调用Key_RefreshState，Times就有可能被更新，跟在后面
*           查询最好
*
************************************************************************************************************************
*/

uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt)
{
	uint8_t times_temp;
	
	if((opt&KEY_ACCESS_WRITE_CLEAR) == KEY_ACCESS_WRITE_CLEAR)
	{
			p_Key->Times = 0;					
	}
		
	times_temp = p_Key->Times;
	
	return times_temp;
}

/*
************************************************************************************************************************
*                              按键状态访问函数
*
* 函数描述：对按键被按下状态的访问
*
*     参数：p_Key        指向按键控制变量的指针

*   返回值：->State的值。
*
*     注意：1.访问过程没有用信号量保护，如果在同一个任务的上下调用Key_AccessTimes和Key_RefreshState
*           是没有什么关系的，而在系统中如果两个函数在两个任务当中就要加以临界段保护了，不过两个函数
*           一般都要在一个任务中使用，因为每次调用Key_RefreshState，Times就有可能被更新，跟在后面
*           查询最好
************************************************************************************************************************
*/
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State)
{
  uint8_t StateChange=p_Key->StateChange;
  //读取状态
  *p_State = p_Key->State;
  //读取后更新状态是否被改变
  p_Key->StateChange=NOT_CHANGED;
  //返回状态是否改变
  return StateChange;
}


/*
*********************************************************************************************************
*                   停机唤醒后配置系统时钟: 使能 HSE, PLL
*        					 并且选择PLL作为系统时钟.
*********************************************************************************************************

*/
static void SYSCLKConfig_STOP(void)
{
	
  /* After wake-up from STOP reconfigure the system clock */
  /* 使能 HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* 等待 HSE 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* 使能 PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* 等待 PLL 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* 选择PLL作为系统时钟源 */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* 等待PLL被选择为系统时钟源 */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
	
	KEEP_LCD_ON_FOR_A_WHILES();//开启背光60s
	LED1(OFF);
}
/*
*********************************************************************************************************
*                 启动stm32停止模式
*********************************************************************************************************

*/

void mySTART_STM32STOPMODE(void){
	if(stm32_flat.STM32isAllowStopMode_MASTER==1){
		
			if(stm32_flat.STM32isAllowStopMode==1){
			stm32_flat.StopModeState=1;
			//LED1(ON);
				/*设置停止模式时，FLASH进入掉电状态*/
			PWR_FlashPowerDownCmd (ENABLE);
			/* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 */
			PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
			
			//从停止模式下被唤醒后使用的是HSI时钟，此处重启HSE时钟,使用PLLCLK
			SYSCLKConfig_STOP();
			//SystemInit();
			
		}
	
	
	}



}
