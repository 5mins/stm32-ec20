/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUICharP.C
Purpose     : Implementation of Proportional fonts
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
//#include "BSP/SPI/spi_flash.h"
/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

//字模数据的暂存数组,以单个字模的最大字节数为设定值 
//#define BYTES_PER_FONT 16*16/8   //最大支持16*16
//static U8 GUI_FontDataBuf[BYTES_PER_FONT];

/*********************************************************************
*
*       GUIPROP_FindChar
*/
static const GUI_FONT_PROP GUI_UNI_PTR * GUIPROP_FindChar(const GUI_FONT_PROP GUI_UNI_PTR* pProp, U16P c) {
  for (; pProp; pProp = pProp->pNext) {
    if ((c>=pProp->First) && (c<=pProp->Last))
      break;
  }
  return pProp;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIPROP_DispChar
*
* Purpose:
*   This is the routine that displays a character. It is used by all
*   other routines which display characters as a subroutine.
*/
void GUIPROP_DispChar(U16P c) {
  int BytesPerLine;
  GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  if (pProp) {
    GUI_DRAWMODE OldDrawMode;
    const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo+(c-pProp->First);
    BytesPerLine = pCharInfo->BytesPerLine;
    OldDrawMode  = LCD_SetDrawMode(DrawMode);
    LCD_DrawBitmap( GUI_Context.DispPosX, GUI_Context.DispPosY,
                       pCharInfo->XSize,
											 GUI_Context.pAFont->YSize,
                       GUI_Context.pAFont->XMag,
											 GUI_Context.pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       pCharInfo->pData,
                       &LCD_BKCOLORINDEX
                       );
    /* Fill empty pixel lines */
    if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) {
      int YMag = GUI_Context.pAFont->YMag;
      int YDist = GUI_Context.pAFont->YDist * YMag;
      int YSize = GUI_Context.pAFont->YSize * YMag;
      if (DrawMode != LCD_DRAWMODE_TRANS) {
        LCD_COLOR OldColor = GUI_GetColor();
        GUI_SetColor(GUI_GetBkColor());
        LCD_FillRect(GUI_Context.DispPosX, 
                     GUI_Context.DispPosY + YSize, 
                     GUI_Context.DispPosX + pCharInfo->XSize, 
                     GUI_Context.DispPosY + YDist);
        GUI_SetColor(OldColor);
      }
    }
    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
    GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
  }
}

//https://www.cnblogs.com/liu_xf/archive/2011/04/14/2015789.html

//void GUIPROP_DispChar(U16P c) {
//  int BytesPerLine;
//  U8 BytesPerFont;  //一个字的字节数
//  U32 base,oft;     //字库的起始地址和偏移量
//		
//  GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
//  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
//  if (pProp) {
//    GUI_DRAWMODE OldDrawMode;
//	const GUI_CHARINFO GUI_UNI_PTR * pCharInfo;
//        //支持2种字体==
//	if((GUI_Context.pAFont == &GUI_FontHZ12)||(GUI_Context.pAFont == &GUI_FontHZ16))
//	{
//		pCharInfo = pProp->paCharInfo;

//		base = (U32)pProp->paCharInfo->pData;
//    	BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine; //每个字模的数据字节数 
//    	if (BytesPerFont > BYTES_PER_FONT)
//    	{
//    		BytesPerFont = BYTES_PER_FONT;
//    	} 
//    	if (c < 0x80) //英文字符地址偏移算法 
//    	{ 
//    		oft = base + (c - 0x20) * BytesPerFont; //计算出字码在flash中的偏移地址
//    	}
//    	else //中文字符地址偏移算法 
//    	{
//    		//if((c>>8) >= 0xb0)
//			//	oft = base + (((c>>8) - 0xa1 - 6) * 94 + ((c&0xff) - 0xa1)) * BytesPerFont - (32*20); 
//			//else
//    			oft = base + (((c>>8) - 0xa1) * 94 + ((c&0xff) - 0xa1)) * BytesPerFont; 

//    	} 		
//		sFLASH_ReadBuffer(GUI_FontDataBuf, oft, BytesPerFont);//取出字模数据 

//		BytesPerLine = pCharInfo->BytesPerLine;
//		OldDrawMode  = LCD_SetDrawMode(DrawMode);		

//		LCD_DrawBitmap( GUI_Context.DispPosX, 
//						GUI_Context.DispPosY,
//						pCharInfo->XSize,
//						GUI_Context.pAFont->YSize,
//						GUI_Context.pAFont->XMag,
//						GUI_Context.pAFont->YMag,
//						1,     /* Bits per Pixel */
//						BytesPerLine,
//						GUI_FontDataBuf,
//						&LCD_BKCOLORINDEX
//						);
//	}
//        //--
//	else
//	{
//		pCharInfo = pProp->paCharInfo+(c-pProp->First);
//		BytesPerLine = pCharInfo->BytesPerLine;
//		OldDrawMode  = LCD_SetDrawMode(DrawMode);		
//		LCD_DrawBitmap( GUI_Context.DispPosX, 
//						GUI_Context.DispPosY,
//						pCharInfo->XSize,
//						GUI_Context.pAFont->YSize,
//						GUI_Context.pAFont->XMag,
//						GUI_Context.pAFont->YMag,
//						1,     /* Bits per Pixel */
//						BytesPerLine,
//						pCharInfo->pData,
//						&LCD_BKCOLORINDEX
//						);
//	}


//    /* Fill empty pixel lines */
//    if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) {
//      int YMag = GUI_Context.pAFont->YMag;
//      int YDist = GUI_Context.pAFont->YDist * YMag;
//      int YSize = GUI_Context.pAFont->YSize * YMag;
//      if (DrawMode != LCD_DRAWMODE_TRANS) {
//        LCD_COLOR OldColor = GUI_GetColor();
//        GUI_SetColor(GUI_GetBkColor());
//        LCD_FillRect(GUI_Context.DispPosX, 
//                     GUI_Context.DispPosY + YSize, 
//                     GUI_Context.DispPosX + pCharInfo->XSize, 
//                     GUI_Context.DispPosY + YDist);
//        GUI_SetColor(OldColor);
//      }
//    }
//    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
//    GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
//  }
//}


//http://www.cnblogs.com/hiker-blogs/archive/2013/01/04/2843538.html
//void GUIPROP_DispChar(U16P c) {
//  int BytesPerLine;
//  U8 BytesPerFont; //一个字的字节数
//  U32 base,oft; //字库的起始地址和偏移量


//  
//  GUI_DRAWMODE DrawMode = GUI_Context.TextMode;
//  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
//  if (pProp) {
//    GUI_DRAWMODE OldDrawMode;
//    const GUI_CHARINFO GUI_UNI_PTR * pCharInfo;

//    
//    //支持的字体==
//    if((c > 0x7F) && ((GUI_Context.pAFont == &GUI_FontHZ12) 
//                || (GUI_Context.pAFont == &GUI_FontHZ16) 
//                || (GUI_Context.pAFont == &GUI_FontHZ24)
//                 || (GUI_Context.pAFont == &GUI_FontHZ32)))
//    {
//        pCharInfo = pProp->paCharInfo;

//        base = (U32)pProp->paCharInfo->pData;
//        BytesPerFont = GUI_Context.pAFont->YSize * pProp->paCharInfo->BytesPerLine; //每个字模的数据字节数
//        if (BytesPerFont > BYTES_PER_FONT)
//        {
//        BytesPerFont = BYTES_PER_FONT;
//        }

//        oft = base + (((c>>8) - 0xa1) * 94 + ((c&0xff) - 0xa1)) * BytesPerFont;

//        SPI_Flash_Read(GUI_FontDataBuf, oft, BytesPerFont);//取出字模数据    
//        BytesPerLine = pCharInfo->BytesPerLine;
//        OldDrawMode = LCD_SetDrawMode(DrawMode);
//        if (GUI_MoveRTL) {
//              GUI_Context.DispPosX -= pCharInfo->XDist * GUI_Context.pAFont->XMag;
//            }

//        LCD_DrawBitmap(     GUI_Context.DispPosX,
//                            GUI_Context.DispPosY,
//                            pCharInfo->XSize,
//                            GUI_Context.pAFont->YSize,
//                            GUI_Context.pAFont->XMag,
//                            GUI_Context.pAFont->YMag,
//                            1, /* Bits per Pixel */
//                            BytesPerLine,
//                            GUI_FontDataBuf,
//                            &LCD_BKCOLORINDEX
//                            );
//    }
//    else
//    {

//     pCharInfo = pProp->paCharInfo+(c-pProp->First);
//     
//    BytesPerLine = pCharInfo->BytesPerLine;
//    OldDrawMode  = LCD_SetDrawMode(DrawMode);
//    if (GUI_MoveRTL) {
//      GUI_Context.DispPosX -= pCharInfo->XDist * GUI_Context.pAFont->XMag;
//    }
//    LCD_DrawBitmap(     GUI_Context.DispPosX, 
//                        GUI_Context.DispPosY,
//                        pCharInfo->XSize,
//                        GUI_Context.pAFont->YSize,
//                        GUI_Context.pAFont->XMag,
//                        GUI_Context.pAFont->YMag,
//                        1,     /* Bits per Pixel */
//                        BytesPerLine,
//                        pCharInfo->pData,
//                        &LCD_BKCOLORINDEX
//                        );
//     }
//    /* Fill empty pixel lines */
//    if (GUI_Context.pAFont->YDist > GUI_Context.pAFont->YSize) {
//      int YMag = GUI_Context.pAFont->YMag;
//      int YDist = GUI_Context.pAFont->YDist * YMag;
//      int YSize = GUI_Context.pAFont->YSize * YMag;
//      if (DrawMode != LCD_DRAWMODE_TRANS) {
//        LCD_COLOR OldColor = GUI_GetColor();
//        GUI_SetColor(GUI_GetBkColor());
//        LCD_FillRect(GUI_Context.DispPosX, 
//                     GUI_Context.DispPosY + YSize, 
//                     GUI_Context.DispPosX + pCharInfo->XSize, 
//                     GUI_Context.DispPosY + YDist);
//        GUI_SetColor(OldColor);
//      }
//    }
//    LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
//    if (!GUI_MoveRTL) {
//      GUI_Context.DispPosX += pCharInfo->XDist * GUI_Context.pAFont->XMag;
//    }
//  }
//}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int GUIPROP_GetCharDistX(U16P c) {
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(GUI_Context.pAFont->p.pProp, c);
  return (pProp) ? (pProp->paCharInfo+(c-pProp->First))->XSize * GUI_Context.pAFont->XMag : 0;
}

/*********************************************************************
*
*       GUIPROP_GetFontInfo
*/
void GUIPROP_GetFontInfo(const GUI_FONT GUI_UNI_PTR * pFont, GUI_FONTINFO* pfi) {
  GUI_USE_PARA(pFont);
  pfi->Flags = GUI_FONTINFO_FLAG_PROP;
}

/*********************************************************************
*
*       GUIPROP_IsInFont
*/
char GUIPROP_IsInFont(const GUI_FONT GUI_UNI_PTR * pFont, U16 c) {
  const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUIPROP_FindChar(pFont->p.pProp, c);
  return (pProp==NULL) ? 0 : 1;
}

/*************************** End of file ****************************/
