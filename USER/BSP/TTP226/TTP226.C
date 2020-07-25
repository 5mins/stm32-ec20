#include "TTP226.h"
#include "includes.h"

//DV   	PB1
//RST 	PB0
//CLK   PB2
//DO  PC5

void ttp226init(void){
	
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
 
	GPIO_InitStructure.GPIO_Pin = TTP226_RST_GPIO_PIN|TTP226_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//
	GPIO_Init(TTP226_RST_GPIO_PORT, &GPIO_InitStructure);
	
//		GPIO_InitStructure.GPIO_Pin = TTP226_DV_GPIO_PIN;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//		GPIO_Init(TTP226_DV_GPIO_PORT, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = TTP226_DO_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(TTP226_DO_GPIO_PORT, &GPIO_InitStructure);
	
}

//void delay_user(unsigned int n)
//{
//	while(n--);
//}

unsigned char ttp226_read(void)
{
	OS_ERR err;
	unsigned char i;
	unsigned char temp=0;
	TTP226_CLK_CLR;//scl=0;

	TTP226_RST_SET;
	//delay_user(2000);
	OSTimeDly(16,OS_OPT_TIME_DLY,&err);
	TTP226_RST_CLR;
	for(i=0;i<8;i++)
	{
	temp>>=1;
		TTP226_CLK_SET;//scl=1;
		//delay_user(1000);
		OSTimeDly(8,OS_OPT_TIME_DLY,&err);
		if(!TTP226_DO_READ){}else{
		temp|=0x80;
		}
		//delay_user(1000);
		OSTimeDly(8,OS_OPT_TIME_DLY,&err);
		TTP226_CLK_CLR;//scl=0;	
		
		//delay_user(2000);
		OSTimeDly(16,OS_OPT_TIME_DLY,&err);
		
	}
	TTP226_RST_CLR;
	
return temp;
}





