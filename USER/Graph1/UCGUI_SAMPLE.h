
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

//#define MY_MESSAGE_Operation_Bus 	(WM_USER + 0) 	//��Ӫ����Ϣ��ʶ�����й��ƶ�����
//#define MY_MESSAGE_UpData_Date 		(WM_USER + 1) 	//�ػ�������Ϣ��ʶ
//#define MY_MESSAGE_USER_LEFT_ID 	(WM_USER + 2) 	//�Զ�����̿��Ʊ�ʶ

#define MY_WIDGET_LISTAPP_ID_0 										(GUI_ID_USER + 0x00) //0x800 +0
#define MY_WIDGET_LISTSMSWINO_ID_0 									(GUI_ID_USER + 0x01) //0x800 +1
#define MY_WIDGET_INBOX_MULTIEDIT_Handle_ID_0         (GUI_ID_USER + 0x02)
#define MY_WIDGET_INBOX_EDIT_Handle_ID_0        		 (GUI_ID_USER + 0x03)
#define MY_WIDGET_LISTSMSINBOXSELETE_ID_0 					(GUI_ID_USER + 0x04) //
#define MY_WIDGET_LISTCLOCKWIN_ID_0 									(GUI_ID_USER + 0x05) //

/* define ------------------------------------------------------------------*/

#define ICONVIEW_Width      20  /* �ؼ�ICONVIEW�Ŀ�                         */  
#define ICONVIEW_Height     20  /* �ؼ�ICONVIEW�ĸߣ�����ͼ���Y������    */  
#define ICONVIEW_XSpace     10  /* �ؼ�ICONVIEW��X������                  */ 
#define ICONVIEW_YSpace     10  /* �ؼ�ICONVIEW��Y������                  */ 
#define ICONVIEW_XPos       2  /* �ؼ�ICONVIEW��X�����������              */
#define BOTWIN_YSpace       10  /* ��Ļ�·����ִ���Y������                */
#define T0PWIN_Time_Width       	 30  //  	/* ״̬��ʱ����             */
#define TOPWIN_Hight      	14 	 						/* ��Ļ�·����ִ���Y������                */
#define MIDWIN_yPos         TOPWIN_Hight  	/* �м䴰��Y������ʼλ�ü��������ֿ��ø߶�  */
#define MIDWIN_xPos          0  						/* �м䴰��Y������ʼλ��                    */

#define TOPWINTIME_xPos          1  										/* ״̬��ʱ����ʼλ�� x                   */
#define TOPWINTIME_yPos          0  										/* ״̬��ʱ����ʼλ�� y                   */
#define TopWindow_Week_xPos          TOPWINTIME_xPos+T0PWIN_Time_Width+4+12  						/* ״̬����ͼ����ʼλ�� x                   */
#define TopWindow_Week_yPos          0  											/* ״̬����ͼ����ʼλ�� y                   */
#define TopWindow_Signa_xPos          TopWindow_Week_xPos+8+15  						/* ״̬���ź�ͼ����ʼλ�� x                   */
#define TopWindow_Signa_yPos          0  											/* ״̬���ź�ͼ����ʼλ�� y                   */
#define TopWindow_SignaWord_xPos         TopWindow_Signa_xPos+15  						/* ״̬���ź�ͼ��������ʼλ�� x                   */
#define TopWindow_SignaWord_yPos          0  									/* ״̬���ź�ͼ��������ʼλ�� y                   */
#define TopWindow_voltage_xPos         TopWindow_SignaWord_xPos+16  						/* ״̬���ź�ͼ��������ʼλ�� x                   */
#define TopWindow_voltage_yPos          0  									/* ״̬���ź�ͼ��������ʼλ�� y                   */
#define T0PWIN_voltage_Width       	 20  //  	/* ״̬����ѹ���             */

#undef  LCD_NUM_LAYERS
#define LCD_NUM_LAYERS    1               //������ʾ����

#if   LCD_NUM_LAYERS == 1                 //�������һ��ʹ��WM_HBKWINΪ����ʾ��
  #define HDTWIN      WM_HBKWIN
#elif LCD_NUM_LAYERS == 2
  #define HDTWIN      WM_GetDesktopWindowEx(1)
#else
  #error LCD_NUM_LAYERS must be set to 1 or 2.
#endif

/* Exported types ------------------------------------------------------------*/
	
typedef struct {
	uint8_t mem:2;//GUI��ǰ������ʾ��SMS��������������men:1 from SM  2:form ME
	int16_t index;//GUI��ǰ������ʾ��SMS����������
	uint8_t wait:1;//GUI��ǰ�Ƿ����ڵȴ�SMS��ȡ���񷵻�
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
