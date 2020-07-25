/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
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

#include "BSP/SPI/spi_flash.h"
#include "EmWin_ASCII_Font.h"

#include "string.h"
#include <stddef.h>
#include "EmWin_ASCII_Font.h"
#include "EmWinHZFont.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemwin ������ʾ(FLASH�����ֿ�)   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//��ģ�����ݴ�����,������ģ������ֽ���
//������ģ���Ϊ1024
#define BYTES_PER_FONT	1024	//16*16=256�������ʾ32*32�����ֿ�	
U8 GUI_FontDataBuf[BYTES_PER_FONT];

int ftinfo_f12addr =0X8000;		//12�ֺŵ�ַ
int ftinfo_f16addr =0X95000;		//16�ֺŵ�ַ
//int ftinfo_f24addr =0;	//24�ֺŵ�ַ
int ftinfo_ugbkaddr = 0X151000; //
int ftinfo_ugbksize = 174344;


//��ȡ��ģ����
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c)
{
	unsigned char qh,ql;
	unsigned char i;					  
    unsigned long foffset; 
	unsigned char t;
	unsigned char *mat;
    u8 size,csize;//�����С
    U16 BytesPerFont;
	GUI_FONT EMWINFONT;
	EMWINFONT = *GUI_pContext->pAFont;
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; //ÿ����ģ�������ֽ���
    if (BytesPerFont > BYTES_PER_FONT) BytesPerFont = BYTES_PER_FONT;
	
	//�ж�����Ĵ�С
	if(memcmp(&EMWINFONT,&GUI_FontHZ12,sizeof(GUI_FONT)) == 0) size=12;			//12����
	else if(memcmp(&EMWINFONT,&GUI_FontHZ16,sizeof(GUI_FONT)) == 0) size=16;	//16����
//	else if(memcmp(&EMWINFONT,&GUI_FontHZ24,sizeof(GUI_FONT)) == 0)	size=24;	//24����
		
	csize = (size/8+((size%8)?1:0))*(size);	//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
    if (c < 0x80)	//Ӣ���ַ���ַƫ���㷨
    {
		switch(size)
		{
			case 12:
				for(t=0;t<12;t++) GUI_FontDataBuf[t]=emwin_asc2_1206[c-0x20][t];
				break;
			case 16:
				for(t=0;t<16;t++) GUI_FontDataBuf[t]=emwin_asc2_1608[c-0x20][t];
				break;
//			case 24:
//				for(t=0;t<48;t++) GUI_FontDataBuf[t]=emwin_asc2_2412[c-0x20][t];
//				break;	
		} 
	}
    else                                                                          
    {
		ql=c/256;
		qh=c%256;
		if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�ǳ��ú���
		{   		    
			for(i=0;i<(size*2);i++) *mat++=0x00;//�������
			return; //��������
		}          
		if(ql<0x7f)ql-=0x40; 
		else ql-=0x41;
		qh-=0x81;   
		foffset=((unsigned long)190*qh+ql)*csize;//�õ��ֿ��е��ֽ�ƫ����		  
		switch(size)
		{
			case 12:
				sFLASH_ReadBuffer(GUI_FontDataBuf,foffset+ftinfo_f12addr,csize);
				break;
			case 16:
				sFLASH_ReadBuffer(GUI_FontDataBuf,foffset+ftinfo_f16addr,csize);
				break;
//			case 24:
//				sFLASH_ReadBuffer(GUI_FontDataBuf,foffset+ftinfo_f24addr,csize);
//				break;	
		} 
	}   	
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
void GUIPROP_X_DispChar(U16P c) 
{	
	int BytesPerLine;
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp;
    //������λ�ֿ�������Ϣ 
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last)) break;
    }
    if (pProp)
    {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        GUI_GetDataFromMemory(pProp, c);//ȡ����ģ����
        BytesPerLine = pCharInfo->BytesPerLine;                
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
                       pCharInfo->XSize, GUI_pContext->pAFont->YSize,
                       GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       &GUI_FontDataBuf[0],
                       &LCD_BKCOLORINDEX
                       );
        /* Fill empty pixel lines */
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) 
        {
            int YMag = GUI_pContext->pAFont->YMag;
            int YDist = GUI_pContext->pAFont->YDist * YMag;
            int YSize = GUI_pContext->pAFont->YSize * YMag;
            if (DrawMode != LCD_DRAWMODE_TRANS) 
            {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize, 
                             GUI_pContext->DispPosX + pCharInfo->XSize, 
                             GUI_pContext->DispPosY + YDist);
                GUI_SetColor(OldColor);
            }
        }
        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
		GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
    }
}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int  GUIPROP_X_GetCharDistX(U16P c) 
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;  
    for (; pProp; pProp = pProp->pNext)                                         
    {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

