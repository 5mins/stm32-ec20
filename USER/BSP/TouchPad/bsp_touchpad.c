/**
  ******************************************************************************
  * @file    bsp_touchpad.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ݰ���Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./Bsp/TouchPad/bsp_touchpad.h"
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/systick/bsp_SysTick.h"

//��ʱ��������ֵ
#define TPAD_ARR_MAX_VAL 	0XFFFF	

//����û����ʱ��ʱ������ֵ
volatile uint16_t tpad_default_val=0;

/***********************************
 *
 * ��ʱ�����벶������
 *
 ***********************************/
static void TIMx_CHx_Cap_Init(uint32_t arr,uint16_t psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	//ʹ��TIMʱ��
	RCC_APB1PeriphClockCmd(TPAD_TIM_CLK,ENABLE);  	
	//ʹ��ͨ������ʱ��	
	RCC_AHB1PeriphClockCmd(TPAD_TIM_GPIO_CLK, ENABLE); 	
	//ָ�����Ÿ���
	GPIO_PinAFConfig(TPAD_TIM_CH_PORT,TPAD_TIM_SOURCE,TPAD_TIM_AF); 
	
	//�˿�����
	GPIO_InitStructure.GPIO_Pin = TPAD_TIM_CH_PIN;	
	//���ù���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//���������� 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
	//��ʼ��TIM
	//�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Period = arr;
	//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	 	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TPAD_TIMx, &TIM_TimeBaseStructure); 
	
	//��ʼ��ͨ��
	//ѡ��ʱ������ͨ��
	TIM_ICInitStructure.TIM_Channel = TPAD_TIM_Channel_X; 
	//�����ش���
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
	// ���벶��ѡ��
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
	//���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	
	//���������˲��� ���˲�
	TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInit(TPAD_TIMx, &TIM_ICInitStructure);
	
	//ʹ��TIM
	TIM_Cmd ( TPAD_TIMx, ENABLE ); 
}

/****************************************
 *
 * Ϊ���ݰ����ŵ�
 * �����ʱ����־������
 *
 *****************************************/
static void TPAD_Reset(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	
	//��������Ϊ��ͨ�������
 	GPIO_InitStructure.GPIO_Pin = TPAD_TIM_CH_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 
 	GPIO_Init(TPAD_TIM_CH_PORT, &GPIO_InitStructure);
	
	//����͵�ƽ,�ŵ�
 	GPIO_ResetBits ( TPAD_TIM_CH_PORT, TPAD_TIM_CH_PIN );						 
  //����һС��ʱ��͵�ƽ����֤�ŵ���ȫ
	Delay_ms(5);
	
	//����жϱ�־
	TIM_ClearITPendingBit(TPAD_TIMx, TPAD_TIM_IT_CCx|TIM_IT_Update); 
	//��������0
	TIM_SetCounter(TPAD_TIMx,0);	
	
	//��������Ϊ���ù��ܣ����ϡ�����
	GPIO_InitStructure.GPIO_Pin = TPAD_TIM_CH_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(TPAD_TIM_CH_PORT,&GPIO_InitStructure);
}

/****************************************************
 *
 * �õ���ʱ������ֵ
 * �����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
 *
 *****************************************************/
static uint16_t TPAD_Get_Val(void)
{		
  /* �ȷŵ���ȫ������λ������ */	
	TPAD_Reset();
	//�ȴ�����������
	while(TIM_GetFlagStatus ( TPAD_TIMx, TPAD_TIM_IT_CCx ) == RESET)
	{
		//��ʱ��,ֱ�ӷ���CNT��ֵ
		if(TIM_GetCounter(TPAD_TIMx)>TPAD_ARR_MAX_VAL-500)
			return TIM_GetCounter(TPAD_TIMx);
	};
	/* ���������غ����TIMx_CCRx�Ĵ���ֵ */
	return TPAD_TIM_GetCaptureX(TPAD_TIMx );	  
} 	

/****************************************************
 *
 * ��ȡn��,ȡ���ֵ
 * n��������ȡ�Ĵ���
 * ����ֵ��n�ζ������������������ֵ
 * 
 *****************************************************/
static uint16_t TPAD_Get_MaxVal(uint8_t n)
{
	uint16_t temp=0;
	uint16_t res=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//�õ�һ��ֵ
		if(temp>res)res=temp;
	};
	return res;
}  

/********************************************************
*
* ��ʼ����������
* ��ÿ��ص�ʱ����������ȡֵ.
* ����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
*
*********************************************************/
uint8_t TPAD_Init(void)
{
	uint16_t buf[10];
	uint32_t temp=0;
	uint8_t j,i;
	
	//�趨��ʱ��Ԥ��Ƶ��Ŀ��ʱ��Ϊ��9MHz(180Mhz/20)
	TIMx_CHx_Cap_Init(TPAD_ARR_MAX_VAL,20-1);
	for(i=0;i<10;i++)//������ȡ10��
	{				 
		buf[i]=TPAD_Get_Val();
		Delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	//ȡ�м��6�����ݽ���ƽ��
	for(i=2;i<8;i++)
	{
	  temp+=buf[i];
	}
	
	tpad_default_val=temp/6;	
	/* printf��ӡ��������ʹ�ã�����ȷ����ֵTPAD_GATE_VAL����Ӧ�ù�����Ӧע�͵� */
//	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	
	//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������!
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2)
	{
		return 1;
	}
	
	return 0;		     	    					   
}
 

/*******************************************************************************
*
* ɨ�败������
* mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
* ����ֵ:0,û�а���;1,�а���;			
*
*******************************************************************************/
//��ֵ������ʱ��������(tpad_default_val + TPAD_GATE_VAL),����Ϊ����Ч����.
#define TPAD_GATE_VAL 	100	

uint8_t TPAD_Scan(uint8_t mode)
{
	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	
	static uint8_t keyen=0;
	//ɨ����
	uint8_t res=0;
	//Ĭ�ϲ�������Ϊ3��
	uint8_t sample=3;	
  //����ֵ	
	uint16_t rval;
	
	if(mode)
	{
		//֧��������ʱ�����ò�������Ϊ6��
		sample=6;	
		//֧������	
		keyen=0;	  
	}	
	/* ��ȡ��ǰ����ֵ(���� sample ��ɨ������ֵ) */
	rval=TPAD_Get_MaxVal(sample); 	
	/* printf��ӡ��������ʹ�ã�����ȷ����ֵTPAD_GATE_VAL����Ӧ�ù�����Ӧע�͵� */
//	printf("scan_rval=%d\n",rval);
	
	//����tpad_default_val+TPAD_GATE_VAL,��С��10��tpad_default_val,����Ч
	if(rval>(tpad_default_val+TPAD_GATE_VAL)&&rval<(10*tpad_default_val))
	{			
    //keyen==0,��Ч 		
		if(keyen==0)
		{
			res=1;		 
		}			
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	}
	
	if(keyen)
	{
		keyen--;		
	}		
	return res;
}	 


/*********************************************END OF FILE**********************/
