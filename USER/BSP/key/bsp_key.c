
#include "BSP/key/bsp_key.h" 
#include "includes.h"



extern STM32_CTR_FLAGS stm32_flat;

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ;
  /* ������ռ���ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  /* ���������ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* �����ж�Դ������2������ʹ������������� */  
//  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
 // NVIC_Init(&NVIC_InitStructure);
}


 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK ,ENABLE);
  
  /* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* ���� NVIC */
  NVIC_Configuration();
  
	/* ѡ�񰴼�1������ */ 
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
  /* ��������Ϊ����ģʽ */ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
  /* �������Ų�����Ҳ������ */
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  /* ʹ������Ľṹ���ʼ������ */
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ ��key1���� */
  SYSCFG_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE,KEY1_INT_EXTI_PINSOURCE);

  /* ѡ�� EXTI �ж�Դ */
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;
  /* �ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* �½��ش��� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  /* ʹ���ж�/�¼��� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* ѡ�񰴼�2������ */ 
 // GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;  
  /* ����������������ͬ */
 // GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      

	/* ���� EXTI �ж�Դ ��key2 ���� */
 // SYSCFG_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE,KEY2_INT_EXTI_PINSOURCE);

  /* ѡ�� EXTI �ж�Դ */
 // EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  /* �����ش��� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  

}



/**
  * @brief   ����Ƿ��а�������
  * @param   ����Ķ˿ںͶ˿�λ
  *		@arg GPIOx: x�����ǣ�A...G�� 
  *		@arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
//{			
//	/*����Ƿ��а������� */
//	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
//	{	 
//		/*�ȴ������ͷ� */
//		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
//		return 	KEY_ON;	 
//	}
//	else
//		return KEY_OFF;
//}
/*********************************************END OF FILE**********************/

/***************************************�û��Զ��庯��***********************************/

//���ж�ģʽ

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
*                              ����1�ܽų�ʼ������
*
* �����������ڰ���1ʹ��֮ǰ�����ȵ���
*
*     ��������
*
*   ����ֵ����
*
************************************************************************************************************************
*/

void Key1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(KEY1_INT_GPIO_CLK,ENABLE);
  /*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN; 
  /*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  /*�������Ų�����Ҳ������*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* ���������ٶ� */
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  /*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);
}

/*
************************************************************************************************************************
*                              ������ʱ����
*
* �����������û����û�ж�ʱ���ã���û�ж���KEY_FIXED_PERIOD������Ҫ���¸���CPU��д�˺�����
*
*     ������xms        ���ݲ�ͬ����������ʱ�䣬һ����20ms
*
*   ����ֵ����
*
*     ע�⣺������ʱ
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


/*					�������ݲ���Ҫ���ģ�ֱ��ʹ�ü���								*/
/*
************************************************************************************************************************
*                              ������������
*
* ����������ʹ�ð���ǰ��Ҫ����������
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*           p_CallBack   ������ȡ������ƽ�Ļص�����ָ�룬��Ϊ��ͬCPU��ƽ��ȡ�ķ�ʽ��һ�����ⲿ����Ҫ�û��Լ���д
*                        α�������£�
*
*                          uint8_t GetPinStateOfKeyXX(void)
*                          {
*                            ����ܽŵĵ�ƽ�ǰ�������ʱ�ĵ�ƽ
*                            ����1
*
*                            ����ܽŵĵ�ƽ�ǰ�������ʱ�ĵ�ƽ
*                            ����0
*                          }
*
*   ����ֵ������������ڱ�����״̬�ĵ�ƽ������1��
*           ����������ڵ���״̬�ĵ�ƽ������0��
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
	p_Key->Time_ms=0;          //����ʱ���ʼ��Ϊ0
#endif
  
#if USER_DATA_EN>0             
	p_Key->User_Data=0;		           //�û������������û�����ʹ��
#endif
  
}

/*
************************************************************************************************************************
*                              ����״̬���º���
*
* ������������������������ֻ�ʺ�STM32��Ƭ���⿪�����������͵ĵ�Ƭ�����������޸ġ�
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*
*   ����ֵ����
*
*     ע�⣺����Ƶ����Ҫ����20Hz
*
************************************************************************************************************************
*/
void Key_RefreshState(KEY* p_Key)
{
  switch(p_Key->State)
  {
    case KEY_UP:
    {
      if((*(p_Key->GetStatePtr))())//ִ�лص������жϰ����ܽ�״̬
      {
#ifdef  KEY_FIXED_PERIOD   
        p_Key->Time_ms = 0;
        p_Key->State = KEY_DOWN_WOBBLE;//����������ʱ
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
        p_Key->State = KEY_UP_WOBBLE;//����������ʱ
#else
        key_delay1ms(KEY_WOBBLE_TIME);
        if( (*(p_Key->GetStatePtr))() == 0 )
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//���������250��û����
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
            p_Key->Times = 250;//���������250��û����
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
*                              ���������´������ʺ���
*
* �����������԰��������´����ķ���
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*           option	     ѡ����ڰ�������һ��ֻ�õ�һ�����ֲ��������������ľ�û�а�������
*                        KEY_ACCESS_READ         ��ȡ���������µĴ���
*                        KEY_ACCESS_WRITE_CLEAR  ���㰴�������µĴ���
*   ����ֵ��->times��ֵ��
*
*     ע�⣺���ʹ���û�����ź��������������ͬһ����������µ���Key_AccessTimes��Key_RefreshState
*           ��û��ʲô��ϵ�ģ�����ϵͳ������������������������о�Ҫ�����ٽ�α����ˣ�������������
*           һ�㶼Ҫ��һ��������ʹ�ã���Ϊÿ�ε���Key_RefreshState��Times���п��ܱ����£����ں���
*           ��ѯ���
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
*                              ����״̬���ʺ���
*
* �����������԰���������״̬�ķ���
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��

*   ����ֵ��->State��ֵ��
*
*     ע�⣺1.���ʹ���û�����ź��������������ͬһ����������µ���Key_AccessTimes��Key_RefreshState
*           ��û��ʲô��ϵ�ģ�����ϵͳ������������������������о�Ҫ�����ٽ�α����ˣ�������������
*           һ�㶼Ҫ��һ��������ʹ�ã���Ϊÿ�ε���Key_RefreshState��Times���п��ܱ����£����ں���
*           ��ѯ���
************************************************************************************************************************
*/
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State)
{
  uint8_t StateChange=p_Key->StateChange;
  //��ȡ״̬
  *p_State = p_Key->State;
  //��ȡ�����״̬�Ƿ񱻸ı�
  p_Key->StateChange=NOT_CHANGED;
  //����״̬�Ƿ�ı�
  return StateChange;
}


/*
*********************************************************************************************************
*                   ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL
*        					 ����ѡ��PLL��Ϊϵͳʱ��.
*********************************************************************************************************

*/
static void SYSCLKConfig_STOP(void)
{
	
  /* After wake-up from STOP reconfigure the system clock */
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* �ȴ� HSE ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* ʹ�� PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* �ȴ� PLL ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
	
	KEEP_LCD_ON_FOR_A_WHILES();//��������60s
	LED1(OFF);
}
/*
*********************************************************************************************************
*                 ����stm32ֹͣģʽ
*********************************************************************************************************

*/

void mySTART_STM32STOPMODE(void){
	if(stm32_flat.STM32isAllowStopMode_MASTER==1){
		
			if(stm32_flat.STM32isAllowStopMode==1){
			stm32_flat.StopModeState=1;
			//LED1(ON);
				/*����ֹͣģʽʱ��FLASH�������״̬*/
			PWR_FlashPowerDownCmd (ENABLE);
			/* ����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��жϻ��� */
			PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
			
			//��ֹͣģʽ�±����Ѻ�ʹ�õ���HSIʱ�ӣ��˴�����HSEʱ��,ʹ��PLLCLK
			SYSCLKConfig_STOP();
			//SystemInit();
			
		}
	
	
	}



}
