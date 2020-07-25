## 移远EC20 +stm32 短信接收器 代码。





/*********************************************************************************************/
中断优先级：
debug_usart:
DMA_RX<->抢断优先级:	子优先级：
DMA_TX<->抢断优先级:2	子优先级：1
USART <->抢断优先级:2	子优先级：0

link_ec20:
DMA_RX<->抢断优先级:	子优先级：
DMA_TX<->抢断优先级:1	子优先级：1
USART <->抢断优先级:1	子优先级：0

EC20RIFOOT: PC4
EXIT1
抢断优先级:3	子优先级：2

定时器3中断：
NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3

RTC闹钟中断：
NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
RTC计时中断：
NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2

key1:PA0 WAKE UP
抢断优先级:3	子优先级：3
EXIN0



/*********************************************************************************************/

【*】 引脚分配
	debug_usart:
	RX<--->PA3
	TX<--->PA2
	
	link_ec20:
	RX<--->PB7
	TX<--->PB6


?	
?	


KEY:
按键均有硬件去抖，按下的时候均为高电平，不按时为低电平。
	KEY1<--->PA0
	
//     LCD模块 					      STM32单片机 
//       LED         接          PC7          //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK         接          PB13          //液晶屏SPI总线时钟信号
//       A0          接          PC6          //液晶屏数据/命令控制信号
//       RESET       接          PB14          //液晶屏复位控制信号
//       CS          接          PB12          //液晶屏片选控制信号
//      PB15  SPI SDI

按键ttp226

RST 	PB0
CLK   PB1
DO  //PC5

(SPI_FLASH)SPI1
PA4 NS
PA5 SCK
PA6  MISO
PA7  MOSI


//电压采集 pc3
	
/*********************************************************************************************/
(STM32F40_41xxx)
SystemCoreClock = 168000000;


【*】 时钟

A.晶振：
-外部高速晶振：8MHz
-RTC晶振：32.768KHz

/*********************************************************************************************/
W25Q16  SPI_FLASH 大小：2MB （1扇区4096字节）
地址范围:0k -- 2048k(0--512*4k)
每页 256 字节，总共 8,192 页组成。每页的 256 字节用一次页
编程指令即可完成。每次擦除 16 页（1扇区）、128 页（32KB 块）、256 页（64KB 块）和全片擦除。
W25Q16 有 512 个可擦除扇区和 32个可擦除块。最小 4KB 扇区允许更灵活的应用去要求数据和参数保存

/*中文字库存储在FLASH的起始地址*/


GBK12.DZK:0X8000 	size:574,560 字节
GBK16.DZK:0X9500   	size:766,080 字节
UNIGBK.BIN:0X151000  size:174,344 字节

剩余空间：0x17B990-0X1FFFFF

