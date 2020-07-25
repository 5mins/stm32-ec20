#include "Graph1/menu.h"
#include "./BSP/RTC/rtc.h"
#include "Graph1/GUI_BASE.h"
#include "./BSP/1.44TFT/LCD_drive_hard.h"
#include "./BSP/usart/bsp_debug_usart.h"
#include "./Public_Func/delay.h"
#include "./Graph1/photo.h"
//////////////////////////////////////////////////////////////////////////////////	 
//首页菜单
//						  
//********************************************************************************
//修改说明
//
//
////////////////////////////////////////////////////////////////////////////////// 

//========================variable==========================//
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
u8 tbuf[40];
u8 UpdateMenu_sign=1;//是否允许刷新菜单
//=====================end of variable======================//

////////////////////////////////////////////////////////////////////////////////// 

/*****************************************************************************
 * @name       :void GUI_DrawMenuBackground(void)
 * @date       :2018-08-24 
 * @function   :Drawing menu background
 * @parameters :None
 * @retvalue   :None
******************************************************************************/ 

void Menu_init(void)
{
	
	GUI_DrawMenuBackground();
	GUI_DrawTopColumn_Time(m_TimeLine_FC_color);
	
}

//更新顶部状态栏
void UpdataMenu1(void)
{
	//LCD_Fill(0,0,lcddev.width,14,m_TimeLine_BC_color);//背景
	GUI_DrawTopColumn_Time(m_TimeLine_FC_color);
	GUI_DrawTopColumn_Signal(m_TimeLine_FC_color);
	
}
//画背景
void GUI_DrawMenuBackground(void)
{
	//绘制固定栏up
	LCD_Clear(m_BC_color);
	LCD_Fill(0,0,lcddev.width,14,m_TimeLine_BC_color);
		//过渡线条
	POINT_COLOR=GRAY;
	LCD_DrawLine(0, 14, lcddev.width,14); //H
	//绘制固定栏down
	LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.width,m_TimeLine_BC_color);
	//POINT_COLOR=WHITE;
	//Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//居中显示
}

//更新时间
void GUI_DrawTopColumn_Time(u16 FC_color)
{
	
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	sprintf((char*)tbuf,"%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes); 
	Show_Str(4,0,FC_color,m_TimeLine_BC_color,tbuf,12,0);
	
//	switch (RTC_DateStruct.RTC_WeekDay){

//		case 1:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou1);
//			break;
//		case 2:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou2);
//			break;
//		case 3:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou3);
//			break;
//		case 4:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou4);
//			break;
//		case 5:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou5);
//			break;
//		case 6:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou6);
//			break;
//		case 7:
//			Gui_DrawbmpWeek(4+33,0,gImage_zhou7);
//			break;
//		default :
//			break;
//	}
	//Gui_DrawbmpWeek(4+33,0,gImage_zhou0);
	
	//sprintf((char*)tbuf,"%d",RTC_DateStruct.RTC_WeekDay); 
	//Show_Str(37+10,0,FC_color,m_TimeLine_BC_color,tbuf,12,0);
	//printf("%s",tbuf);//debug_mark
	

}

//更新信号值
void GUI_DrawTopColumn_Signal(uint16_t FC_color)
{
	
	//Gui_DrawbmpSign(58,0,gImage_sign3);
	char *temp ="5G";
	Show_Str(58+15,0,FC_color,m_TimeLine_BC_color,(uint8_t *)temp,12,0);
	//Show_Str(58+14,0,FC_color,m_TimeLine_BC_color,(uint8_t *)temp,12,0);
	


	
	


}


