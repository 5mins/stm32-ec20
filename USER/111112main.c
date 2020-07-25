/**
  ******************************************************************************

  ******************************************************************************

  *
  ******************************************************************************
  */
/**
  ******************************************************************************
  *                              ͷ�ļ�
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "my_malloc_in.h" 
#include "./BSP/usart/bsp_debug_usart.h"
#include "./BSP/usart/bsp_linkec20_usart.h"
#include "systick/bsp_SysTick.h"
#include "./Public_Func/delay.h"
#include "./Public_Func/sys.h"
#include "./BSP/key/bsp_key.h" 
#include "./BSP/LED/led.h" 
#include "./BSP/SPI/spi_flash.h"
#include "./BSP/1.44TFT/LCD_SPI_hard.h"
#include "./BSP/1.44TFT/LCD_drive_hard.h"
//#include "./Graph1/GUI_BASE.h"
#include "Graph1/UCGUI_SAMPLE.h"
#include "./Graph1/menu.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/TIMER/timer.h"
#include "GUI.H"
#include <stdio.h>
#include <string.h>



#define  FLASH_WRITE_ADDRESS      800000
//#define  FLASH_Font_ADDRESS       
#define  FLASH_READ_ADDRESS       FLASH_WRITE_ADDRESS
#define  FLASH_SECTOR_TO_ERASE    FLASH_WRITE_ADDRESS

#define  BufferSize (countof(Tx_Buffer1)-1)
/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
extern uint8_t DEBUG_USART_had_been_Read;//debug��Ϣ��ȡ״̬��0δ����1�Ѷ�
extern uint8_t DEBUG_USART_had_been_Sent;//debug��ǰ��Ϣ����״̬��0δ����1�ѷ���
extern type_DEBUG_USART_Rx  DEBUG_USART_Rx;//��ǰ���յ�������Ϣ

extern uint8_t LINKEC20_USART_had_been_Read;//LINKEC20��Ϣ��ȡ״̬��0δ����1�Ѷ�
extern uint8_t LINKEC20_USART_had_been_Sent;//LINKEC20��ǰ��Ϣ����״̬��0δ����1�ѷ���
extern type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//��ǰ���յ�������Ϣ


uint8_t Tx_Buffer1[] = "STM32F4xx SPI Firmware Library Example: communication with an M25P SPI FLASH";
extern uint8_t  Rx_Buffer[sFlash_BufferSize];
extern __IO uint8_t Index ;
extern __IO uint32_t FlashID;





/**
  ******************************************************************************
  *                                ������
  ******************************************************************************
  */
	
static int initialize(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);/* Ƕ�������жϿ�������ѡ�� */
	/* ��ʼ�� */
	delay_init(168);     //��ʼ����ʱ����
	//SysTick_Init();
	EXTI_Key_Config();
	LED_Init();
	My_RTC_Init();		 		//��ʼ��RTC
	LINKEC20_USART_ALL_Init();
	Debug_USART_ALL_Init(); //���Դ��ڳ�ʼ��
	sFLASH_Init();
	//my_LCD_Init();	   //Һ������ʼ��
	//Menu_init();//���˵���ʼ��
	GUI_Init();//UCGUI ��ʼ��
	TIM3_Int_Init(1000-1,8400-1);	// ����ˢ����Ļ������1000Ϊ0.1�룬��ʱ��ʱ��84M(168/2)����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms 
  /* ��ʼ�� */

	return 0;
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	initialize();//��ʼ��



	//RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	
		
	//RTC_Set_Date(18,9,22,6);
	//RTC_Set_Time(13,38,50,1);

	  /* Get SPI Flash ID */
 // FlashID = sFLASH_ReadID();
	//printf("FlashIDΪ:  %d\r\n",FlashID);//debug_mark
//	 if (FlashID == sFLASH_W25Q16_ID)
//  {
//		
//		
//	}

		GUI_SetBkColor(GUI_BLACK);
//		GUI_SetColor(GUI_BROWN);
//		GUI_Clear();
//		GUI_DrawCircle(50,45,30);//��Բ
//		//GUI_SetFont();
//		GUI_DispString("Hello world\n");
		
		//show_color();
		


	
	while(1)
	{

//		LED1(ON);
//		delay_ms(100);
//		LED1(OFF);
		delay_ms(500);
		GUI_SetColor(GUI_BLACK);
		GUI_Clear();
		//delay_ms(500);
		show_color();
		delay_ms(500);
//		if(DEBUG_USART_had_been_Read==0)
//		{
//			DEBUG_USART_had_been_Read=1;
//			//Usart_SendByte(LINKEC20_USART,0x01);
//			if(DEBUG_USART_had_been_Sent==0)
//				{
//				//Usart_SendByte(LINKEC20_USART,0x02);
//				LED1_TOGGLE;
//				DEBUG_USART_had_been_Sent=1;
//				//Sent_to_EC20_with_Block(DEBUG_USART_Rx.DEBUG_USART_RECEIVE, DEBUG_USART_Rx.DEBUG_USART_RxLen);
//				}

//		}

	}

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
