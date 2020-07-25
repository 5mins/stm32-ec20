#ifndef __WM8978_H__
#define	__WM8978_H__

#include "stm32f4xx.h"
/*---------------------------------------------------------------------------------------------*/
/*------------------------   I2C����WM8978���ò���  -------------------------------------------*/
/* WM8978 ��Ƶ����ͨ������ѡ��, ����ѡ���·������ MIC_LEFT_ON | LINE_ON */
typedef enum
{
	IN_PATH_OFF		= 0x00,	/* ������ */
	MIC_LEFT_ON 	= 0x01,	/* LIN,LIP�ţ�MIC���������Ӱ�����ͷ��  */
	MIC_RIGHT_ON	= 0x02,	/* RIN,RIP�ţ�MIC���������Ӱ�����ͷ��  */
	LINE_ON			  = 0x04, /* L2,R2 ����������(�Ӱ��ض�������) */
	AUX_ON			  = 0x08,	/* AUXL,AUXR ���������루������û�õ��� */
	DAC_ON			  = 0x10,	/* I2S����DAC (CPU������Ƶ�ź�) */
	ADC_ON			  = 0x20	/* �������Ƶ����WM8978�ڲ�ADC ��I2S¼��) */
}IN_PATH_E;

/* WM8978 ��Ƶ���ͨ������ѡ��, ����ѡ���· */
typedef enum
{
	OUT_PATH_OFF	= 0x00,	/* ����� */
	EAR_LEFT_ON 	= 0x01,	/* LOUT1 ����������(�Ӱ��ض�������) */
	EAR_RIGHT_ON	= 0x02,	/* ROUT1 ����������(�Ӱ��ض�������) */
	SPK_ON			  = 0x04,	/* LOUT2��ROUT2���������������������û�õ���*/
	OUT3_4_ON		  = 0x08,	/* OUT3 �� OUT4 �����������Ƶ��������û�õ���*/
}OUT_PATH_E;

/* ����������� */
#define VOLUME_MAX		                      63		/* ������� */
#define VOLUME_STEP		                       1		/* �������ڲ��� */

/* �������MIC���� */
#define GAIN_MAX		                        63		/* ������� */
#define GAIN_STEP		                         1		/* ���沽�� */

/* STM32 I2C ����ģʽ */
#define WM8978_I2C_Speed                    400000
/* WM8978 I2C�ӻ���ַ */
#define WM8978_SLAVE_ADDRESS                0x34	

/*I2C�ӿ�*/
#define WM8978_I2C                          I2C1
#define WM8978_I2C_CLK                      RCC_APB1Periph_I2C1

#define WM8978_I2C_SCL_PIN                  GPIO_Pin_6                  
#define WM8978_I2C_SCL_GPIO_PORT            GPIOB                       
#define WM8978_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define WM8978_I2C_SCL_SOURCE               GPIO_PinSource6
#define WM8978_I2C_SCL_AF                   GPIO_AF_I2C1

#define WM8978_I2C_SDA_PIN                  GPIO_Pin_7                  
#define WM8978_I2C_SDA_GPIO_PORT            GPIOB                       
#define WM8978_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define WM8978_I2C_SDA_SOURCE               GPIO_PinSource7
#define WM8978_I2C_SDA_AF                   GPIO_AF_I2C1

/*�ȴ���ʱʱ��*/
#define WM8978_I2C_FLAG_TIMEOUT             ((uint32_t)0x4000)
#define WM8978_I2C_LONG_TIMEOUT             ((uint32_t)(10 * WM8978_I2C_FLAG_TIMEOUT))

/* ���ⲿ���õĺ������� */
uint8_t wm8978_Init(void);
uint8_t wm8978_Reset(void);
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void wm8978_OutMute(uint8_t _ucMute);
void wm8978_PowerDown(void);
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void wm8978_SetMicGain(uint8_t _ucGain);
void wm8978_SetLineGain(uint8_t _ucGain);
void wm8978_SetOUT2Volume(uint8_t _ucVolume);
void wm8978_SetOUT1Volume(uint8_t _ucVolume);
uint8_t wm8978_ReadOUT1Volume(void);
uint8_t wm8978_ReadOUT2Volume(void);
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);
																					
/*---------------------------------------------------------------------------------------------*/
/*------------------------   I2S�������ݴ��䲿��  ---------------------------------------------*/																					
/**
	* I2S���ߴ�����Ƶ���ݿ���
	* WM8978_LRC    -> PB12/I2S2_WS
	* WM8978_BCLK   -> PD3/I2S2_CK
	* WM8978_ADCDAT -> PC2/I2S2ext_SD
	* WM8978_DACDAT -> PI3/I2S2_SD
	* WM8978_MCLK   -> PC6/I2S2_MCK
	*/
#define WM8978_CLK                     RCC_APB1Periph_SPI2
#define WM8978_I2Sx_SPI                SPI2
#define WM8978_I2Sx_ext                I2S2ext

#define WM8978_LRC_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define WM8978_LRC_PORT            	   GPIOB
#define WM8978_LRC_PIN             	   GPIO_Pin_12
#define WM8978_LRC_AF                  GPIO_AF_SPI2
#define WM8978_LRC_SOURCE              GPIO_PinSource12

#define WM8978_BCLK_GPIO_CLK           RCC_AHB1Periph_GPIOD
#define WM8978_BCLK_PORT            	 GPIOD
#define WM8978_BCLK_PIN             	 GPIO_Pin_3
#define WM8978_BCLK_AF                 GPIO_AF_SPI2
#define WM8978_BCLK_SOURCE             GPIO_PinSource3

#define WM8978_ADCDAT_GPIO_CLK         RCC_AHB1Periph_GPIOC
#define WM8978_ADCDAT_PORT             GPIOC
#define WM8978_ADCDAT_PIN              GPIO_Pin_2
#define WM8978_ADCDAT_AF               GPIO_AF_SPI3    //���� GPIO_AF6_SPI2 -> AF6
#define WM8978_ADCDAT_SOURCE           GPIO_PinSource2

#define WM8978_DACDAT_GPIO_CLK         RCC_AHB1Periph_GPIOI
#define WM8978_DACDAT_PORT             GPIOI
#define WM8978_DACDAT_PIN              GPIO_Pin_3
#define WM8978_DACDAT_AF               GPIO_AF_SPI2
#define WM8978_DACDAT_SOURCE           GPIO_PinSource3

#define WM8978_MCLK_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define WM8978_MCLK_PORT            	 GPIOC
#define WM8978_MCLK_PIN             	 GPIO_Pin_6
#define WM8978_MCLK_AF                 GPIO_AF_SPI2
#define WM8978_MCLK_SOURCE             GPIO_PinSource6

#define I2Sx_DMA                       DMA1
#define I2Sx_DMA_CLK                   RCC_AHB1Periph_DMA1
#define I2Sx_TX_DMA_CHANNEL            DMA_Channel_0
#define I2Sx_TX_DMA_STREAM             DMA1_Stream4
#define I2Sx_TX_DMA_IT_TCIF            DMA_IT_TCIF4
#define I2Sx_TX_DMA_STREAM_IRQn        DMA1_Stream4_IRQn 
#define I2Sx_TX_DMA_STREAM_IRQFUN			 DMA1_Stream4_IRQHandler

#define I2Sxext_RX_DMA_CHANNEL         DMA_Channel_3
#define I2Sxext_RX_DMA_STREAM          DMA1_Stream3
#define I2Sxext_RX_DMA_IT_TCIF         DMA_IT_TCIF3
#define I2Sxext_RX_DMA_STREAM_IRQn     DMA1_Stream3_IRQn 
#define I2Sxext_RX_DMA_STREAM_IRQFUN	 DMA1_Stream3_IRQHandler

extern void (*I2S_DMA_TX_Callback)(void);		//I2S DMA TX�ص�����ָ��  
extern void (*I2S_DMA_RX_Callback)(void);	  //I2S DMA RX�ص�����

void I2S_GPIO_Config(void);
void I2S_Stop(void);
void I2Sx_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq);
void I2Sx_TX_DMA_Init(const uint16_t *buffer0,const uint16_t *buffer1,const uint32_t num);
void I2S_Play_Start(void);
void I2S_Play_Stop(void);

void I2Sxext_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq);
void I2Sxext_RX_DMA_Init(const uint16_t *buffer0,const uint16_t *buffer1,const uint32_t num);
void I2Sxext_Recorde_Start(void);
void I2Sxext_Recorde_Stop(void);
#endif /* __WM8978_H__ */
