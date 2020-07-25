

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>


typedef struct _tFont
{    
  const uint16_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;

extern sFONT Font16x16;


/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Constants
  * @{
  */ 
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))



/*******************����********** ����ʾ������ʾ���ַ���С ***************************/
#define      macWIDTH_CH_CHAR		                16	    //�����ַ���� 
#define      macHEIGHT_CH_CHAR		              16		  //�����ַ��߶� 

//0��ʾʹ��SD����ģ�������ʾFLASH��ģ,����SD����ģ���ļ�ϵͳ���ٶ����ܶࡣ

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
/*ʹ��FLASH��ģ*/
/*�����ֿ�洢��FLASH����ʼ��ַ*/
/*FLASH*/
#define GBKCODE_START_ADDRESS   32768


/*��ȡ�ֿ�ĺ���*/
//�����ȡ�����ַ���ģ����ĺ�������ucBufferΪ�����ģ��������usCharΪ�����ַ��������룩
#define      macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint8_t *c);

#else
/*ʹ��SD��ģ*/


/*SD����ģ·��*/
#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"


/*��ȡ�ֿ�ĺ���*/
//�����ȡ�����ַ���ģ����ĺ�������ucBufferΪ�����ģ��������usCharΪ�����ַ��������룩

#define macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_sd( ucBuffer, usChar )
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t *c);

#endif

/******************************************************************************/

/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Functions
  * @{
  */ 
/**
  * @}
  */

#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_H */
 
/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
