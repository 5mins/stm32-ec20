#ifndef __GUI_BASE_H__
#define __GUI_BASE_H__
/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h" 
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];


void GUIBASE_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void Draw_Circle(u16 x0,u16 y0,u16 fc,u8 r);
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s,u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p); //œ‘ æ40*40 QQÕº∆¨
void Gui_DrawbmpWeek(u16 x,u16 y,const unsigned char *p);//œ‘ æ15*11 Õº∆¨
void Gui_DrawbmpSign(u16 x,u16 y,const unsigned char *p);//œ‘ æ10*13 Õº∆¨
void gui_circle(int xc, int yc,u16 c,int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
uint16_t LCD_BGR2RGB(uint16_t c);
void DisplayMovStr(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 displaylen,u8 mode);




#endif

