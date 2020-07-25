

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



/*******************中文********** 在显示屏上显示的字符大小 ***************************/
#define      macWIDTH_CH_CHAR		                16	    //中文字符宽度 
#define      macHEIGHT_CH_CHAR		              16		  //中文字符高度 

//0表示使用SD卡字模，非零表示FLASH字模,由于SD卡字模有文件系统，速度慢很多。

#define GBKCODE_FLASH 						1

#if GBKCODE_FLASH
/*使用FLASH字模*/
/*中文字库存储在FLASH的起始地址*/
/*FLASH*/
#define GBKCODE_START_ADDRESS   32768


/*获取字库的函数*/
//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）
#define      macGetGBKCode( ucBuffer, usChar )  GetGBKCode_from_EXFlash( ucBuffer, usChar )  
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint8_t *c);

#else
/*使用SD字模*/


/*SD卡字模路径*/
#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"


/*获取字库的函数*/
//定义获取中文字符字模数组的函数名，ucBuffer为存放字模数组名，usChar为中文字符（国标码）

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
