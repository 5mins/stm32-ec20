#ifndef  BSP_TTP226
#define  BSP_TTP226

#include "./Public_Func/sys.h" 
#include "./BSP/LED/led.h" 


//RST 	PB0
//CLK   PB2
//DO  PB1

//#define RST        0	
////#define DO        1	
//#define CLK        2	

//#define TTP226_RST  PBout(0)
//#define TTP226_DO   PBin(1)
//#define TTP226_CLK  PBout(2)

//控制口置1操作语句宏定义

//#define	TTP226_RST_SET  	TTP226_RST=1    
//#define	TTP226_CLK_SET  	TTP226_CLK=1  
//#define	TTP226_DO_SET  	TTP226_DO=1  


//控制口置0操作语句宏定义

//#define	TTP226_RST_CLR  	TTP226_RST=0  
//#define	TTP226_CLK_CLR  	TTP226_CLK=0 
//#define	TTP226_DO_CLR  	TTP226_DO=0 


//#define TTP226_DV_GPIO_PIN                 	 GPIO_Pin_2   
//#define TTP226_DV_GPIO_PORT          			 		GPIOB    
#define TTP226_RST_GPIO_PIN           			 GPIO_Pin_0    
#define TTP226_RST_GPIO_PORT          			 GPIOB                      
#define TTP226_CLK_GPIO_PIN            			 GPIO_Pin_1
#define TTP226_CLK_GPIO_PORT          			 GPIOB
#define TTP226_DO_GPIO_PIN             				GPIO_Pin_5
#define TTP226_DO_GPIO_PORT          					 GPIOC   


#define	TTP226_DO_READ 	GPIO_ReadInputDataBit(TTP226_DO_GPIO_PORT,TTP226_DO_GPIO_PIN) 
//#define	TTP226_DV_READ 	GPIO_ReadInputDataBit(TTP226_DV_GPIO_PORT,TTP226_DV_GPIO_PIN) 
//#define	TTP226_RST_SET 	GPIO_SetBits(TTP226_RST_GPIO_PORT,TTP226_RST__GPIO_PIN,Bit_SET)
//#define	TTP226_RST_CLR 	GPIO_SetBits(TTP226_RST_GPIO_PORT,TTP226_RST__GPIO_PIN,Bit_RESET)

#define	TTP226_RST_SET 	digitalHi(TTP226_RST_GPIO_PORT,TTP226_RST_GPIO_PIN)
#define	TTP226_RST_CLR  digitalLo(TTP226_RST_GPIO_PORT,TTP226_RST_GPIO_PIN)
#define	TTP226_CLK_SET 	digitalHi(TTP226_CLK_GPIO_PORT,TTP226_CLK_GPIO_PIN)
#define	TTP226_CLK_CLR  digitalLo(TTP226_CLK_GPIO_PORT,TTP226_CLK_GPIO_PIN)
#define	TTP226_DO_SET  	digitalHi(TTP226_DO_GPIO_PORT,TTP226_DO_GPIO_PIN)
#define	TTP226_DO_CLR  	digitalLo(TTP226_DO_GPIO_PORT,TTP226_DO_GPIO_PIN)

unsigned char ttp226_read(void);
void ttp226init(void);

#endif


