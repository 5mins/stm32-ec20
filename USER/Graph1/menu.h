 /* @attention
  *

**************************************************************************************************/	
#ifndef __Menu_H__
#define __Menu_H__

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>


/* define ------------------------------------------------------------------*/
#define m_BC_color           WHITE
#define m_TimeLine_BC_color  BLACK
#define m_TimeLine_FC_color  WHITE

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void GUI_DrawMenuBackground(void);
void GUI_DrawTopColumn_Time(uint16_t FC_color);
void Menu_init(void);
void UpdataMenu1(void);//¸üÐÂ¶¥²¿×´Ì¬À¸
void GUI_DrawTopColumn_Signal(uint16_t FC_color);

#endif
