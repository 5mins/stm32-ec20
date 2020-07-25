## ��ԶEC20 +stm32 ���Ž����� ���롣





/*********************************************************************************************/
�ж����ȼ���
debug_usart:
DMA_RX<->�������ȼ�:	�����ȼ���
DMA_TX<->�������ȼ�:2	�����ȼ���1
USART <->�������ȼ�:2	�����ȼ���0

link_ec20:
DMA_RX<->�������ȼ�:	�����ȼ���
DMA_TX<->�������ȼ�:1	�����ȼ���1
USART <->�������ȼ�:1	�����ȼ���0

EC20RIFOOT: PC4
EXIT1
�������ȼ�:3	�����ȼ���2

��ʱ��3�жϣ�
NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3

RTC�����жϣ�
NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
RTC��ʱ�жϣ�
NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//�����ȼ�2

key1:PA0 WAKE UP
�������ȼ�:3	�����ȼ���3
EXIN0



/*********************************************************************************************/

��*�� ���ŷ���
	debug_usart:
	RX<--->PA3
	TX<--->PA2
	
	link_ec20:
	RX<--->PB7
	TX<--->PB6


?	
?	


KEY:
��������Ӳ��ȥ�������µ�ʱ���Ϊ�ߵ�ƽ������ʱΪ�͵�ƽ��
	KEY1<--->PA0
	
//     LCDģ�� 					      STM32��Ƭ�� 
//       LED         ��          PC7          //Һ������������źţ��������Ҫ���ƣ���5V��3.3V
//       SCK         ��          PB13          //Һ����SPI����ʱ���ź�
//       A0          ��          PC6          //Һ��������/��������ź�
//       RESET       ��          PB14          //Һ������λ�����ź�
//       CS          ��          PB12          //Һ����Ƭѡ�����ź�
//      PB15  SPI SDI

����ttp226

RST 	PB0
CLK   PB1
DO  //PC5

(SPI_FLASH)SPI1
PA4 NS
PA5 SCK
PA6  MISO
PA7  MOSI


//��ѹ�ɼ� pc3
	
/*********************************************************************************************/
(STM32F40_41xxx)
SystemCoreClock = 168000000;


��*�� ʱ��

A.����
-�ⲿ���پ���8MHz
-RTC����32.768KHz

/*********************************************************************************************/
W25Q16  SPI_FLASH ��С��2MB ��1����4096�ֽڣ�
��ַ��Χ:0k -- 2048k(0--512*4k)
ÿҳ 256 �ֽڣ��ܹ� 8,192 ҳ��ɡ�ÿҳ�� 256 �ֽ���һ��ҳ
���ָ�����ɡ�ÿ�β��� 16 ҳ��1��������128 ҳ��32KB �飩��256 ҳ��64KB �飩��ȫƬ������
W25Q16 �� 512 ���ɲ��������� 32���ɲ����顣��С 4KB �������������Ӧ��ȥҪ�����ݺͲ�������

/*�����ֿ�洢��FLASH����ʼ��ַ*/


GBK12.DZK:0X8000 	size:574,560 �ֽ�
GBK16.DZK:0X9500   	size:766,080 �ֽ�
UNIGBK.BIN:0X151000  size:174,344 �ֽ�

ʣ��ռ䣺0x17B990-0X1FFFFF

