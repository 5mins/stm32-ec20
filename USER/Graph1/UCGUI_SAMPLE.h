
 /* @attention
  *

**************************************************************************************************/	
#ifndef __UCGUISAMPLE_H__
#define __UCGUISAMPLE_H__

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include  <includes.h>
#include "GUI.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"
#include  <os.h>

/* define ------------------------------------------------------------------*/
#define m_BC_color           WHITE
#define m_TimeLine_BC_color  BLACK
#define m_TimeLine_FC_color  WHITE

//#define MY_MESSAGE_Operation_Bus 	(WM_USER + 0) 	//运营商消息标识，“中国移动”等
//#define MY_MESSAGE_UpData_Date 		(WM_USER + 1) 	//重绘日期消息标识
//#define MY_MESSAGE_USER_LEFT_ID 	(WM_USER + 2) 	//自定义键盘控制标识

#define MY_WIDGET_LISTAPP_ID_0 										(GUI_ID_USER + 0x00) //0x800 +0
#define MY_WIDGET_LISTSMSWINO_ID_0 									(GUI_ID_USER + 0x01) //0x800 +1
#define MY_WIDGET_INBOX_MULTIEDIT_Handle_ID_0         (GUI_ID_USER + 0x02)
#define MY_WIDGET_INBOX_EDIT_Handle_ID_0        		 (GUI_ID_USER + 0x03)
#define MY_WIDGET_LISTSMSINBOXSELETE_ID_0 					(GUI_ID_USER + 0x04) //
#define MY_WIDGET_LISTCLOCKWIN_ID_0 									(GUI_ID_USER + 0x05) //

/* define ------------------------------------------------------------------*/

#define ICONVIEW_Width      20  /* 控件ICONVIEW的宽                         */  
#define ICONVIEW_Height     20  /* 控件ICONVIEW的高，包括图标的Y方向间距    */  
#define ICONVIEW_XSpace     10  /* 控件ICONVIEW的X方向间距                  */ 
#define ICONVIEW_YSpace     10  /* 控件ICONVIEW的Y方向间距                  */ 
#define ICONVIEW_XPos       2  /* 控件ICONVIEW的X方向调整距离              */
#define BOTWIN_YSpace       10  /* 屏幕下方部分窗口Y方向间距                */
#define T0PWIN_Time_Width       	 30  //  	/* 状态栏时间宽度             */
#define TOPWIN_Hight      	14 	 						/* 屏幕下方部分窗口Y方向间距                */
#define MIDWIN_yPos         TOPWIN_Hight  	/* 中间窗口Y方向起始位置即顶部文字可用高度  */
#define MIDWIN_xPos          0  						/* 中间窗口Y方向起始位置                    */

#define TOPWINTIME_xPos          1  										/* 状态栏时间起始位置 x                   */
#define TOPWINTIME_yPos          0  										/* 状态栏时间起始位置 y                   */
#define TopWindow_Week_xPos          TOPWINTIME_xPos+T0PWIN_Time_Width+4+12  						/* 状态栏周图标起始位置 x                   */
#define TopWindow_Week_yPos          0  											/* 状态栏周图标起始位置 y                   */
#define TopWindow_Signa_xPos          TopWindow_Week_xPos+8+15  						/* 状态栏信号图标起始位置 x                   */
#define TopWindow_Signa_yPos          0  											/* 状态栏信号图标起始位置 y                   */
#define TopWindow_SignaWord_xPos         TopWindow_Signa_xPos+15  						/* 状态栏信号图标文字起始位置 x                   */
#define TopWindow_SignaWord_yPos          0  									/* 状态栏信号图标文字起始位置 y                   */
#define TopWindow_voltage_xPos         TopWindow_SignaWord_xPos+16  						/* 状态栏信号图标文字起始位置 x                   */
#define TopWindow_voltage_yPos          0  									/* 状态栏信号图标文字起始位置 y                   */
#define T0PWIN_voltage_Width       	 20  //  	/* 状态栏电压宽度             */

#undef  LCD_NUM_LAYERS
#define LCD_NUM_LAYERS    1               //设置显示层数

#if   LCD_NUM_LAYERS == 1                 //如果仅有一次使用WM_HBKWIN为主显示层
  #define HDTWIN      WM_HBKWIN
#elif LCD_NUM_LAYERS == 2
  #define HDTWIN      WM_GetDesktopWindowEx(1)
#else
  #error LCD_NUM_LAYERS must be set to 1 or 2.
#endif

/* Exported types ------------------------------------------------------------*/
	
typedef struct {
	uint8_t mem:2;//GUI当前正在显示的SMS短信信箱索引。men:1 from SM  2:form ME
	int16_t index;//GUI当前正在显示的SMS短信索引。
	uint8_t wait:1;//GUI当前是否正在等待SMS读取任务返回
}SMS_SHOW;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int  show_color(void);
void ucGUIMainApp(void);
void ucGUIMainAppinit(void);
static void CreateSMSWIN0(void);
static void CreateMidWin1(void);
static void CreateSMSWIN(void);
void UpdataMidWin0(void);
void UpData_Date(u8 forceUpdata) ;
void GUI_Show_MYmsg1(const char*text,const char*title) ;
void ResetLCDBKtime(u32 times);
void KEEP_LCD_ON_FOR_A_WHILES(void);

#endif
