#include "includes.h"
#include "Graph1/UCGUI_SAMPLE.h"
//#include "Graph1/GUI_BASE.h"
//#include "./Graph1/photo.h"
#include "ec20.h"
#include "pyinput.h"
/*
*********************************************************************************************************
*                                         宏定义 
*********************************************************************************************************
*/






/*
*********************************************************************************************************
状态栏：时间，信号，闹钟，电量,
00-默认页面：
10-短信：收件箱，发件箱，草稿箱，关键字拦截，设置
20-电话：电话本，通话记录，黑白名单，设置
30-GPS:
40-小工具：闹钟，记事本，提醒
50-设置：时间，声音，
*********************************************************************************************************
*/ 

/*
*********************************************************************************************************
*                                      变量
*********************************************************************************************************
*/ 
extern OS_MEM INTERNAL_MEM; 
//星期图标	
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou0;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou1;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou2;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou3;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou4;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou5;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou6;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou7;
extern GUI_CONST_STORAGE GUI_BITMAP bmAIR_MODE;//飞行模式图标
extern GUI_CONST_STORAGE GUI_BITMAP bmnosign;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign1;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign2;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign3;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign4;

extern enum EC20_SENT_CTR SENTCTR;//EC20发送控制指令标记
extern EC20_SENT_CTR_Parameters SENTCTR_PARA;////发送控制指令传入参数
extern OS_TCB   AppTaskEC20CtrTCB;//EC20消息处理任务任务
//uint8_t UserApp_Flag=0;
extern RTC_TIME rtc_time;
///////窗口标志
GUI_HWIN TEMP_WIN;//用于记录最近的窗口，用于BACK返回键。
GUI_HWIN TOPWIN;//顶部状态栏
uint8_t TOP_IS_INIT=0;//标记状态栏图标是否完成初始化
uint8_t  DATE_IS_CHANGE=1;//标记日期是否完成改变
//uint8_t  WEEK_IS_CHANGE=1;//标记WEEK是否改变
//PROGBAR_Handle PowerProgBar; /* 电量显示进度条句柄 */
//TEXT_Handle voltage_TextHandle;/* 状态栏时间显示文本句柄 */
TEXT_Handle Time_TextHandle;/* 状态栏时间显示文本句柄 */

RTC_DateTypeDef RTC_OldDate;//  用来判断是否需要刷新屏幕上的日期
char date_text_buffer[12]={0};//2018-11-11/0 日期缓存变量
char time_text_buffer[6]={0};//02:02:01/0 如果显示秒，数组大小设定为9；

char voltage_text_buffer[6]={0};//20%

/////////////
GUI_HWIN MIDWIN[2];
//MIDWIN0
TEXT_Handle MIDWIN0_Operation_Bus_handle;
TEXT_Handle MIDWIN0_Date_handle;
//struct Operation_Bus{
//int sign;//标记下面的运营商标识数组是否有效，1有效
//char COPS[16] ;//存储发送过来的运营商标识
//}Operation1;
extern NET_FLAGS net_flags;//网络状态
extern Operation_Bus  Operation ;
extern volatile int rssi;//信号强度变量
extern volatile int ber;//误码率
extern EC20TmrCTR_FLAGS ec20Tmrctr_flags;

//MIDWIN1

LISTBOX_Handle LISTAPP_Handle ;
//SMSWIN
GUI_HWIN SMSWIN;
LISTBOX_Handle SMSWIN_Handle;
GUI_HWIN SMSWIN_INBOX;
EDIT_Handle SMSWIN_INBOX_EDIT_Handle;//显示消息inxex句柄
MULTIEDIT_HANDLE SMSWIN_INBOX_MULTIEDIT_HANDLE;//显示消息详情页面句柄
LISTBOX_Handle LISTSMSWIN_INBOX_SELETE_Handle ;//显示消息页面选项，删除，回复等
extern TEMP_Message_Storage tmp_sms[1];////临时存储信息，供显示使用
extern Message_BOX_COUNT SMSBOX;//短消息信箱计数器2 供全局变量使用

SMS_SHOW sms_show;
//CLOCKWIN
GUI_HWIN CLOCKWIN;
LISTBOX_Handle CLOCKWIN_Handle;
/////
typedef struct WIN_PARA{			//窗口使用到的用户定义参数，方便在回调函数中使用
	int xSizeLCD, ySizeLCD;			//LCD屏幕尺寸
	int xPosWin,  yPosWin;			//窗口的起始位置
	int xSizeWin, ySizeWin;			//窗口尺寸	
	int xSizeBM,  ySizeBM;		//图标宽度，高度	
}WIN_PARA;

WIN_PARA midWinPara;			//用户使用的窗口额外数据


//int32_t LCD_TURNOFF_TIME =30;//LCD屏幕背光倒计时 单位秒
//uint32_t LCD_TURNOFF_TIME_TEMP=30;

extern STM32_CTR_FLAGS stm32_flat;
//u8 LCD_KEEP_ON =0;//LCD屏幕背光常亮

/* 用于桌面列表的创建 */
 GUI_ConstString applistItem[] = 
{
	"SMS","Notepad","Weather","Clock","Safari", "Phone","Picture","Calculator",NULL
};
/* 二级菜单 */
 GUI_ConstString SMSItem[] = 
{
	"Inbox","Send SMS","Outbox","Drafts","Keyword","Intercept", "Settings",NULL
};

 GUI_ConstString PhoneItem[] = 
{
	"Phone Book","Call History","Black List","White List", "Settings",NULL
};
 GUI_ConstString IBSELETEItem[] = 
{
	"Quick reply","Del SMS","Del ALL READ SMS","Del ALL SMS","Block number",NULL
};

 GUI_ConstString CLOCKItem[] = 
{
	"NTP SYNC CLOCK",NULL
};

static void CreateSMSWIN_INBOX(void);
static void CreateCLOCKWIN(void);

/*********************************************************************************************************
*	函 数 名: ucGUI_UpDataTopWindow_Time
*	功能说明: 定时器 更新状态栏时间和日期
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void ucGUI_UpDataTopWindow_Time(void)
{
	if(!(bsp_result&BSP_RTC))
		{

			RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
			RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);

			sprintf(time_text_buffer,"%02d:%02d",\
			rtc_time.RTC_Time.RTC_Hours,\
			rtc_time.RTC_Time.RTC_Minutes\
			);
			
			if(RTC_OldDate.RTC_Date!=rtc_time.RTC_Date.RTC_Date||\
				RTC_OldDate.RTC_Month!=rtc_time.RTC_Date.RTC_Month||\
				RTC_OldDate.RTC_Year!=rtc_time.RTC_Date.RTC_Year||\
				RTC_OldDate.RTC_WeekDay!=rtc_time.RTC_Date.RTC_WeekDay)//如果不等
					{
						
					RTC_OldDate.RTC_Year	= rtc_time.RTC_Date.RTC_Year;
					RTC_OldDate.RTC_Month= rtc_time.RTC_Date.RTC_Month;
					RTC_OldDate.RTC_Date	= rtc_time.RTC_Date.RTC_Date;
					RTC_OldDate.RTC_WeekDay =	rtc_time.RTC_Date.RTC_WeekDay;
					sprintf(date_text_buffer,"20%02d-%02d-%02d",rtc_time.RTC_Date.RTC_Year,\
																											rtc_time.RTC_Date.RTC_Month,\
																											rtc_time.RTC_Date.RTC_Date);
						
							switch (rtc_time.RTC_Date.RTC_WeekDay){

									case 1:
										GUI_DrawBitmap(&bmzhou1,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 2:
										GUI_DrawBitmap(&bmzhou2,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 3:
										GUI_DrawBitmap(&bmzhou3,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 4:
										GUI_DrawBitmap(&bmzhou4,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 5:
										GUI_DrawBitmap(&bmzhou5,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 6:
										GUI_DrawBitmap(&bmzhou6,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									case 7:
										GUI_DrawBitmap(&bmzhou0,TopWindow_Week_xPos,TopWindow_Week_yPos);
										break;
									default :
										break;
								}//end [switch]
					DATE_IS_CHANGE=1;
					UpdataMidWin0();
					
					
					}
			
			
			
			TEXT_SetText(Time_TextHandle,time_text_buffer)  ;		//设置指定 TEXT 控件的文本。
					
						GUI_SetColor(GUI_BLACK);//反显
						GUI_SetBkColor(GUI_WHITE);
						sprintf(time_text_buffer,"%02d",(u8)rtc_time.RTC_Time.RTC_Seconds);
						GUI_DispStringAt(time_text_buffer,TOPWINTIME_xPos+32,TOPWINTIME_yPos);//
				
		}
}

/*********************************************************************************************************
*	函 数 名: ucGUI_UpDataTopWindow_Signal
*	功能说明: 定时器 更新状态栏电压值
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/

void ucGUI_UpDataTopWindow_voltage(void)
{
	
	float stm32inputvoltage = GetSTM32INPUTVOLTAGE();//stm32vcc输入端电压
//	float pers = (stm32inputvoltage/4.1f)*100;
	//sprintf(voltage_text_buffer,"%d%%",(u8)pers);
	u8 int_part =(u8)stm32inputvoltage;
	float dec_part =(stm32inputvoltage - int_part)*100;
	GUI_SetColor(GUI_BLACK);
	GUI_SetBkColor(GUI_WHITE);
	sprintf(voltage_text_buffer,"%d",int_part);
	GUI_DispStringAt(voltage_text_buffer,TopWindow_voltage_xPos,TopWindow_voltage_yPos);//
	
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_BLACK);
	sprintf(voltage_text_buffer,"%02d",(u8)dec_part);
//	if(int_part<10){
	GUI_DispStringAt(voltage_text_buffer,TopWindow_voltage_xPos+6,TopWindow_voltage_yPos);//
//	}
	
	
	//TEXT_SetText(voltage_TextHandle,voltage_text_buffer)  ;		//设置指定 TEXT 控件的文本。
	

}
/*********************************************************************************************************
*	函 数 名: ucGUI_UpDataTopWindow_Signal
*	功能说明: 定时器 更新状态栏信号值
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/

void ucGUI_UpDataTopWindow_Signal(void )
{

	
	if(net_flags.EC20_AIR_MODE==1||net_flags.IS_EC20_OFF==1||net_flags.EC20_INIT==0){
		
		GUI_DrawBitmap(&bmAIR_MODE,TopWindow_Signa_xPos,0);//飞行模式图标

		
	}else{
		
	if(Operation.sign||net_flags.EC20_CSQ){
			
			if((rssi>=2&&rssi<=30)||(rssi>=102&&rssi<=190)){ //-109dBm...-53dBm 信号适中 3格
			//Gui_DrawbmpSign(58,0,gImage_sign3);
			GUI_DrawBitmap(&bmsign3,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
				
			}else if(rssi==31||rssi==191){ //-51dBm or greater 信号良好 4格
			//Gui_DrawbmpSign(58,0,gImage_sign4);
				GUI_DrawBitmap(&bmsign4,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==1||rssi==101){ //-111dBm  信号微弱 2格
			//Gui_DrawbmpSign(58,0,gImage_sign2);
				GUI_DrawBitmap(&bmsign2,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==0||rssi==100){//-113dBm or less 信号极微弱 1格
				//Gui_DrawbmpSign(58,0,gImage_sign1);
				GUI_DrawBitmap(&bmsign1,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==99||rssi==199){ //Not known or not detectable 无信号
			//Gui_DrawbmpSign(58,0,gImage_sign0);
				GUI_DrawBitmap(&bmnosign,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else{
			//Gui_DrawbmpSign(58,0,gImage_sign0);//未知情况
				GUI_DrawBitmap(&bmnosign,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}
			GUI_SetFont(&GUI_FontHZ12);
			GUI_SetColor(GUI_WHITE);
			GUI_SetBkColor(GUI_BLACK);
			GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
			
			
			
			switch(Operation.Act){
						case 0:
							GUI_DispStringAt("2G",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//GSM
							break;
						case 100:
							GUI_DispStringAt("CD",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//CDMA
							break;
						case 2:
							GUI_DispStringAt("3G",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//UTRAN
							break;
						case 3:
							GUI_DispStringAt("E ",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//GSM W/EGPRS
							break;
						case 4:
							GUI_DispStringAt("3G",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//UTRAN W/HSDPA
							break;
						case 5:
							GUI_DispStringAt("H+",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//UTRAN W/HSUPA
							break;
						case 6:
							GUI_DispStringAt("3G",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//UTRAN W/HSDPA and HSUPA
							break;
						case 7:
							GUI_DispStringAt("4G",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//E-UTRAN
							break;
						case 11:default:
							GUI_DispStringAt("??",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//未知
							break;

			
			}
			

			

		}else{
			GUI_DrawBitmap(&bmnosign,TopWindow_Signa_xPos,TopWindow_Signa_yPos);//未知情况
		GUI_DispStringAt("??",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//未知
		}
	
	
	
	
	
	}


		
}



/*********************************************************************************************************
*	函 数 名: UpDataTopWindow
*	功能说明: 更新顶部状态栏
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
//void UpDataTopWindow() 
//{
//	if(!topwin_hide){
//	//printf("UpDataTopWindow\n");//debug_mark1

//	//LCD_Fill(0,0,lcddev.width,14,m_TimeLine_BC_color);//背景
//	ucGUI_UpDataTopWindow_Time(GUI_WHITE);
//	ucGUI_UpDataTopWindow_Signal(GUI_WHITE);
//		}

//}
/*
*********************************************************************************************************
*	函 数 名: _cbTOPWin
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbTOPWin(WM_MESSAGE * pMsg) 
{
	//printf("_cbTOPWin_ID:%x\n",pMsg->MsgId);
	switch (pMsg->MsgId) 
	{
		
	
		case WM_TIMER:
					ucGUI_UpDataTopWindow_Time();//更新状态栏时间
					ucGUI_UpDataTopWindow_Signal();//更新状态栏信号值
					ucGUI_UpDataTopWindow_voltage();//更新电压
					//WM_InvalidateWindow(TOPWIN);//使指定窗口无效。
					WM_BringToBottom(TOPWIN) ;
					WM_RestartTimer(pMsg->Data.v, 1000);  
		break;
		
		/* 重绘消息*/
		case WM_PAINT:
			{
			
				//printf("_cbTOPWin重绘消息\n");
					GUI_SetColor(GUI_BLACK);
					GUI_Clear();

					}	
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}


/*********************************************************************************************************
*	函 数 名: CreatTopWindow
*	功能说明: 创建顶部状态栏
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void CreatTopWindow() 
{
	TOPWIN = WM_CreateWindow(TOPWINTIME_xPos,TOPWINTIME_yPos,T0PWIN_Time_Width,TOPWIN_Hight,WM_CF_SHOW|WM_CF_MEMDEV,_cbTOPWin,0);//0,0,34,TOPWIN_Hight,WM_CF_SHOW|WM_CF_MEMDEV,_cbTOPWin,0
	///电量显示
//	timeProgBar = PROGBAR_CreateAsChild(0,2,40,14,TOPWIN,1,WM_CF_SHOW|WM_CF_MEMDEV);
//	PROGBAR_SetFont(timeProgBar,&GUI_FontHZ12);
//	//PROGBAR_SetText(timeProgBar,"11");
//	PROGBAR_SetMinMax(timeProgBar,0,60);
//	voltage_TextHandle = TEXT_CreateEx(TopWindow_voltage_xPos,TopWindow_voltage_yPos,T0PWIN_voltage_Width,TOPWIN_Hight,TOPWIN,WM_CF_SHOW|WM_CF_MEMDEV,TEXT_CF_LEFT|TEXT_CF_VCENTER,0,"3.7v");
//	TEXT_SetFont(voltage_TextHandle,&GUI_FontHZ12) ;			//设置用于指定 TEXT 控件的字体。
//	TEXT_SetTextColor(voltage_TextHandle,GUI_WHITE)  ;				//设置指定 TEXT 控件的文本颜色
	
	//////时间显示
	Time_TextHandle = TEXT_CreateEx(TOPWINTIME_xPos,TOPWINTIME_yPos,T0PWIN_Time_Width,TOPWIN_Hight,TOPWIN,WM_CF_SHOW|WM_CF_MEMDEV,TEXT_CF_LEFT|TEXT_CF_TOP,0,"20:20");
	TEXT_SetFont(Time_TextHandle,&GUI_FontHZ12) ;			//设置用于指定 TEXT 控件的字体。
	//TEXT_SetText(Time_TextHandle,)  ;									//设置指定 TEXT 控件的文本。
	//TEXT_SetTextAlign(Time_TextHandle,TEXT_CF_LEFT|TEXT_CF_VCENTER)  ;				//设置指定 TEXT 控件的文本对齐方式。
	TEXT_SetTextColor(Time_TextHandle,GUI_WHITE)  ;				//设置指定 TEXT 控件的文本颜色
	
	WM_CreateTimer(TOPWIN,0, 1000, 0);//设置定时器更新状态栏
}



/*
*********************************************************************************************************
*	函 数 名: Update_Operation_Bus
*	功能说明: 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Update_Operation_Bus(void) 
{

		if(MIDWIN0_Operation_Bus_handle){
			
			
			if(net_flags.IS_EC20_OFF==1){	
				
				TEXT_SetTextColor(MIDWIN0_Operation_Bus_handle,GUI_WHITE);
				TEXT_SetTextAlign(MIDWIN0_Operation_Bus_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetFont(MIDWIN0_Operation_Bus_handle,&GUI_FontHZ16);
				TEXT_SetText(MIDWIN0_Operation_Bus_handle,"EC20 OFF");
			}else if(net_flags.EC20_INIT==0){	
				
				TEXT_SetTextColor(MIDWIN0_Operation_Bus_handle,GUI_WHITE);
				TEXT_SetTextAlign(MIDWIN0_Operation_Bus_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetFont(MIDWIN0_Operation_Bus_handle,&GUI_FontHZ16);
				TEXT_SetText(MIDWIN0_Operation_Bus_handle,"INIT==0");
			}else if(net_flags.EC20_AIR_MODE==1){	
				
				TEXT_SetTextColor(MIDWIN0_Operation_Bus_handle,GUI_WHITE);
				TEXT_SetTextAlign(MIDWIN0_Operation_Bus_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetFont(MIDWIN0_Operation_Bus_handle,&GUI_FontHZ16);
				TEXT_SetText(MIDWIN0_Operation_Bus_handle,"AIR MODE");
			}else{
							if(Operation.sign){
									TEXT_SetTextColor(MIDWIN0_Operation_Bus_handle,GUI_WHITE);
									TEXT_SetTextAlign(MIDWIN0_Operation_Bus_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
									TEXT_SetFont(MIDWIN0_Operation_Bus_handle,&GUI_FontHZ16);
									TEXT_SetText(MIDWIN0_Operation_Bus_handle,Operation.COPS);
							}else{
									
									TEXT_SetTextColor(MIDWIN0_Operation_Bus_handle,GUI_LIGHTGRAY);
									TEXT_SetTextAlign(MIDWIN0_Operation_Bus_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
									TEXT_SetFont(MIDWIN0_Operation_Bus_handle,&GUI_FontHZ16);
									TEXT_SetText(MIDWIN0_Operation_Bus_handle,"OFF LINE");
								}
						}
			
		
	}	
}

/*
*********************************************************************************************************
*	函 数 名: UpData_Date
*	功能说明: 
*	形    参：forceUpdata 1:强制更新时间，
*	返 回 值: 无
*********************************************************************************************************
*/
void UpData_Date(u8 forceUpdata) 
{
	if(MIDWIN0_Date_handle){

	      if(!(bsp_result&BSP_RTC))
      {
				if(forceUpdata){
						//更新RTC时间
						RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
						RTC_OldDate.RTC_Year	=	 rtc_time.RTC_Date.RTC_Year;
						RTC_OldDate.RTC_Month  = rtc_time.RTC_Date.RTC_Month;
						RTC_OldDate.RTC_Date	 =	 rtc_time.RTC_Date.RTC_Date;
						RTC_OldDate.RTC_WeekDay = rtc_time.RTC_Date.RTC_WeekDay;
						sprintf(date_text_buffer,"20%02d-%02d-%02d",rtc_time.RTC_Date.RTC_Year,\
																												rtc_time.RTC_Date.RTC_Month,\
																												rtc_time.RTC_Date.RTC_Date);

						
						DATE_IS_CHANGE=1;
				}else{
					if(DATE_IS_CHANGE){

						//绘制中间日期
						TEXT_SetTextColor(MIDWIN0_Date_handle,GUI_WHITE);
						TEXT_SetTextAlign(MIDWIN0_Date_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
						TEXT_SetFont(MIDWIN0_Date_handle,&GUI_FontHZ12);
						TEXT_SetText(MIDWIN0_Date_handle,date_text_buffer);
						
						DATE_IS_CHANGE=0;
					}
				}

				

				
			}else{

				TEXT_SetTextColor(MIDWIN0_Date_handle,GUI_WHITE);
				TEXT_SetTextAlign(MIDWIN0_Date_handle,GUI_TA_HCENTER | GUI_TA_VCENTER);
				TEXT_SetFont(MIDWIN0_Date_handle,&GUI_FontHZ12);
				TEXT_SetText(MIDWIN0_Date_handle,"!RTC");
				return;//强制退出函数
			
			}


	}	
}


/*
*********************************************************************************************************
*	函 数 名: _cbMidWin0
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbMidWin0(WM_MESSAGE * pMsg) 
{
//	printf("WIN0_MESSAGE_ID:%d\n",pMsg->MsgId);
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			WM_SetFocus(MIDWIN[0]);
			break;
		case WM_NOTIFY_OWNER_KEY:
		{
//			printf("0Wkeyesc\n");
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
							{

							case GUI_KEY_ENTER:
								
								break;
							case GUI_KEY_ESCAPE:
								
								break;
							default:
								break;
						
							}
			}
			break;
		case WM_KEY:
		{

					//int kkey = GUI_GetKey();//返回按键缓冲器中的内容。
					//printf("WIN0kkey:%d\n",kkey);//debug_mark
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
							case GUI_KEY_ENTER:
							break;
						case 0 :
						case GUI_KEY_ESCAPE:
									TEMP_WIN=MIDWIN[0];
									WM_HideWindow(MIDWIN[0]);
									WM_ShowWindow(MIDWIN[1]);
									//WM_BringToBottom(MIDWIN[1]) ;
									WM_BringToTop(MIDWIN[1]);
									//WM_SetFocus(MIDWIN[1]);
									WM_SetFocus(LISTAPP_Handle);
							break;
						case GUI_KEY_LEFT:
									TEMP_WIN=MIDWIN[0];
									WM_HideWindow(MIDWIN[0]);
									CreateSMSWIN_INBOX();
							break;
						case GUI_KEY_RIGHT:
									TEMP_WIN=MIDWIN[0];
									WM_HideWindow(MIDWIN[0]);
									WM_ShowWindow(MIDWIN[1]);
									//WM_BringToBottom(MIDWIN[0][1]) ;
									WM_BringToTop(MIDWIN[1]);
									//WM_SetFocus(MIDWIN[1]);
									WM_SetFocus(LISTAPP_Handle);
							break;
						case GUI_KEY_UP:
							break;
						case GUI_KEY_DOWN:
							break;
						default:
							break;
						
					}
//					
//					//GUI_ClearKeyBuffer();//清除按键缓冲器
//				
				}
			break;
		
			
		/* 重绘消息*/
		case WM_PAINT:
			{
			
					GUI_SetColor(GUI_BLACK);
					GUI_Clear();
					Update_Operation_Bus();//运营商标识
					UpData_Date(0) ;//日期
				  //WM_SetFocus(MIDWIN[0]);//测试是否需要设定焦点
					}	
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}



/**
*********************************************************************************************************
  * @brief  CreateMidWin，创建中间的窗口0
  * @param  none
  * @retval none
*********************************************************************************************************
  */
static void CreateMidWin0(void)
{
	MIDWIN[0] = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW,_cbMidWin0,0);//中间窗体默认页面	hFrame[0] _cbMidWin0
	MIDWIN0_Operation_Bus_handle = TEXT_CreateEx(0,24,midWinPara.xSizeWin,16,MIDWIN[0],WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,0,"cmcc");
	MIDWIN0_Date_handle = TEXT_CreateEx(0,44,midWinPara.xSizeWin,12,MIDWIN[0],WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,0,"2018-10-09");
	WM_SetFocus(MIDWIN[0]);
	TEMP_WIN=MIDWIN[0];
	//WM_SetCallback(MIDWIN[0],_cbMidWin0);
}

void UpdataMidWin0(void){
WM_InvalidateWindow(MIDWIN[0]);//使指定窗口无效。

}


/*
*********************************************************************************************************
*	函 数 名: _cbMidWin1
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbMidWin1(WM_MESSAGE * pMsg) 
	{
//#ifdef DEBUG_MARK
//CPU_SR_ALLOC();
//OS_CRITICAL_ENTER();
////printf("_cbMidWin1_ID:%d\n",pMsg->MsgId);
//OS_CRITICAL_EXIT();
//#endif
			switch (pMsg->MsgId) 
		{
			
		case WM_CREATE:
			WM_SetFocus(MIDWIN[1]);
			break;
		case WM_NOTIFY_OWNER_KEY:
			{
				//	int kkey = GUI_GetKey();//返回按键缓冲器中的内容。
				switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
					{
					case GUI_KEY_ENTER:
						{
							switch(LISTBOX_GetSel(LISTAPP_Handle)){
								
								case 0:
									TEMP_WIN=MIDWIN[1];
									WM_HideWindow(MIDWIN[1]);
									CreateSMSWIN();
									break;
								case 1://notepad
									break;
								case 2://weather
									break;
								case 3://Clock
									TEMP_WIN=MIDWIN[1];
									WM_HideWindow(MIDWIN[1]);
									CreateCLOCKWIN();
									break;
								case 4://safari
									break;
								case 5://phone
									break;
								case 6:
									break;
								case 7:
									break;
								default:
									break;
							}
																
						}
						break;
					case GUI_KEY_ESCAPE:
						//printf("1OWkeyesc\n");
									TEMP_WIN=MIDWIN[1];
								  WM_HideWindow(MIDWIN[1]);
									WM_ShowWindow(MIDWIN[0]);
//									//WM_BringToBottom(MIDWIN[1]) ;
									WM_BringToTop(MIDWIN[0]);
									WM_SetFocus(MIDWIN[0]);
						break;
					default:
						break;
				
					}
				}	
			break;
				
		case WM_KEY:
				{
					//WM_SetFocus(LISTAPP_Handle);
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
//						case GUI_KEY_ENTER:
//						{
//							switch(LISTBOX_GetSel(LISTAPP_Handle)){
//							
//								case 0:
//									WM_HideWindow(MIDWIN[1]);
//									
//									CreateSMSWIN();
//									break;
//								case 1:
//									break;
//								case 2:
//									break;
//								case 3:
//									break;
//								case 4:
//									break;
//								case 5:
//									break;
//								case 6:
//									break;
//								case 7:
//									break;
//								default:
//									break;
//							}
//																

//						}
//							break;
						case 0 :
						case GUI_KEY_ESCAPE:
							//printf("keyesc\n");
//									WM_HideWindow(MIDWIN[1]);
//									WM_ShowWindow(MIDWIN[0]);
//									//WM_BringToBottom(MIDWIN[1]) ;
//									WM_BringToTop(MIDWIN[0]);
//									WM_SetFocus(MIDWIN[0]);
							break;
						case GUI_KEY_LEFT:
							break;
						case GUI_KEY_RIGHT:
							break;
						case GUI_KEY_UP:
							break;
						case GUI_KEY_DOWN:
							break;
						default:
							break;
						
					}
					
//					printf("kkey:%d ,key:%d\n",kkey,keystate.Key);
//					printf("keypress:%d\n",keystate.Pressed);
					GUI_ClearKeyBuffer();//清除按键缓冲器
				
				}
			break;
				
		
		/* 重绘消息*/
			case WM_PAINT:
				{
					//printf("MidWin1重绘消息\n");
						//WM_SetFocus(LISTAPP_Handle);
						GUI_SetBkColor(GUI_BLACK);
						GUI_Clear();
				}			
			break;

		 default:
			WM_DefaultProc(pMsg);
			break;
		}

}


/*
*********************************************************************************************************
*	函 数 名: CreateMidWin1
*	功能说明: 创建功能菜单
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateMidWin1(void)
{
//		uint8_t i=0;
		MIDWIN[1] = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_HIDE|WM_CF_FGND|WM_CF_MEMDEV,_cbMidWin1,0);//中间窗体默认页面	hFrame[0] _cbMidWin0

		LISTAPP_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,MIDWIN[1],WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTAPP_ID_0,applistItem);	//
//		LISTBOX_SetBackColor(LISTAPP_Handle,0,GUI_BLACK);	   //设置列表框未被选中的背景颜色
//		LISTBOX_SetBackColor(LISTAPP_Handle,1,GUI_LIGHTGRAY);	   //设置列表框被选中的背景颜色
//		LISTBOX_SetFont(LISTAPP_Handle,&GUI_FontHZ12);        //设置列表框中文本字体
//		LISTBOX_SetTextColor(LISTAPP_Handle,0,GUI_LIGHTGRAY);	   //设置列表框未被选中的文本颜色
//		LISTBOX_SetTextColor(LISTAPP_Handle,1,GUI_BLACK);	   //设置列表框被选中的文本颜色
		//LISTBOX_SetItemSel(LISTAPP_Handle,3,1);//设置第一栏选定状态
		LISTBOX_EnableWrapMode(LISTAPP_Handle,1);//启用自动换行模式（wrap moade），即：从结尾立即滚动到开始或反之 1 启用自动换行模式，0 (默认)禁用。
		//WIDGET_SetFocusable(LISTAPP_Handle,1);//设置控件能接收输入焦点
		WM_SetFocus(LISTAPP_Handle);
		
		//printf("WM_GetParent(MIDWIN[0]):%ld \nMIDWIN[0]:%ld  \n WM_GetParent(MIDWIN[0][1]):%ld\n MIDWIN[0][1]:%ld\n",WM_GetParent(MIDWIN[0][0]),MIDWIN[0][0],WM_GetParent(MIDWIN[0][1]),MIDWIN[0][1]);
		
}



/*
*********************************************************************************************************
*	函 数 名: GUI_SHOW1SMS
*	功能说明: 填充一条SMS消息到 SMSWIN_INBOX_MULTIEDIT_HANDLE 
*	形    参：men:1 from SM  2:form ME  
*	返 回 值: 无
*********************************************************************************************************
*/

void GUI_SHOW1SMS(uint8_t mem,uint16_t index,int16_t show_index){
	OS_ERR err;
	OS_ERR os_err;
	
	if(SMSBOX.SELETED_used>0){//先检查信箱消息数是否为0
			sms_show.wait=1;//标记GUI正在等待消息数据返回

				SENTCTR =sent_EC20_Read1SMS;  //设定发送任务
				SENTCTR_PARA.Para1=mem;
				SENTCTR_PARA.Para3=index;
				OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
				uint8_t times =0;//最长等待1500ms 
				while(((tmp_sms[0].alive==0)||(tmp_sms[0].index!=index))&&(times<15)){	
				OSTimeDly(100,OS_OPT_TIME_DLY,&os_err);times++;}//debug_mark GUI_Delay(100);
					
						char  * p_mem_blk;
					CPU_SR_ALLOC();
					OS_CRITICAL_ENTER();
					p_mem_blk = mymalloc(0,TEMP_Message_Storage_text_size);
					OS_CRITICAL_EXIT();
						
						memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//清空数据
				
					if(p_mem_blk&&tmp_sms[0].alive==1&&tmp_sms[0].index==index){


						sprintf(p_mem_blk,"(%d/%d)",show_index+1,SMSBOX.SELETED_used);
						EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
						memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//清空数据
						sprintf(p_mem_blk,"FROM:%s\n\n%s\n\nReTime:%s",tmp_sms[0].from,tmp_sms[0].text,tmp_sms[0].retime);
						MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,p_mem_blk);
					

						
					}else{
						
					sprintf(p_mem_blk,"读取失败:(%d/0)",index+1);
					//EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
					MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,p_mem_blk);
					}
OS_CRITICAL_ENTER();
myfree(0,p_mem_blk);//退还内存块
OS_CRITICAL_EXIT();

sms_show.wait=0;//标记GUI等待消息数据返回 结束
}else{

	MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"NO SMS");


}
}
/*
*********************************************************************************************************
*	函 数 名: GUI_DELETESMS
*	功能说明: 
*	形    参：men:1 from SM  2:form ME
<delflag> 0 Delete the message specified in <index>
					1 Delete all read messages from <mem1> storage
					2 Delete all read messages from <mem1> storage and sent mobile originated messages
					3 Delete all read messages from <mem1> storage as well as all sent and unsent mobile originated messages
					4 Delete all messages from <mem1> storage
*	返 回 值: 无
*********************************************************************************************************
*/

void GUI_DELETESMS(u8 mem ,u8 delflag,int16_t *show_index){
	OS_ERR err;
	OS_ERR os_err;
		if(SMSBOX.SELETED_used>0){//先检查信箱消息数是否为0
			sms_show.wait=1;//标记GUI正在等待消息数据返回

				SENTCTR =sent_EC20_Delete_SMS;  //设定发送任务
				SENTCTR_PARA.Para1=mem;
				SENTCTR_PARA.Para2=delflag;
				SENTCTR_PARA.Para3=SMSBOX.INDEX[*show_index];
				OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
				uint8_t times =0;//最长等待1000ms 
				while((net_flags.EC20_Delete_SMS==0)&&(times<15)){	OSTimeDly(100,OS_OPT_TIME_DLY,&os_err);times++;}//debug_mark GUI_Delay(100);

				
					if(net_flags.EC20_Delete_SMS==1){
						if(delflag==0){//删除一条信息成功
									if(*show_index-1>0){
									--(*show_index);
								GUI_SHOW1SMS(mem,SMSBOX.INDEX[*show_index],*show_index);
								}else{
								GUI_SHOW1SMS(mem,SMSBOX.INDEX[0],0);
								}
						}else{//清空信息成功
						MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"无消息!");
						}

						
						net_flags.EC20_Delete_SMS=0;
					}else{
						
					
					//EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
					MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"Del fail!");
					GUI_SHOW1SMS(mem,SMSBOX.INDEX[*show_index],*show_index);
					}

WM_DeleteWindow(LISTSMSWIN_INBOX_SELETE_Handle);
WM_SetFocus(SMSWIN_INBOX);			

sms_show.wait=0;//标记GUI等待消息数据返回 结束
}
}
/*
*********************************************************************************************************
*	函 数 名: _cbSMSWIN_INBOX
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbSMSWIN_INBOX(WM_MESSAGE * pMsg) 
	{
//#ifdef DEBUG_MARK
//CPU_SR_ALLOC();
//OS_CRITICAL_ENTER();
//printf("_cbSMSWIN_INBOX:%d\n",pMsg->MsgId);
//OS_CRITICAL_EXIT();
//#endif

		
		
			switch (pMsg->MsgId) 
		{
				case WM_NOTIFY_OWNER_KEY:

					{
						
						switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
							
							case GUI_KEY_ENTER:
							{
										switch(LISTBOX_GetSel(LISTSMSWIN_INBOX_SELETE_Handle)){
							
											case 0://"Quick reply"
												break;
											case 1://"Del SMS"
												GUI_DELETESMS(sms_show.mem,0,&sms_show.index);

												break;
											case 2://"Del ALL READ SMS"
												GUI_DELETESMS(sms_show.mem,1,&sms_show.index);

												break;
											case 3://"Del ALL SMS"
												GUI_DELETESMS(sms_show.mem,4,&sms_show.index);

												break;
											case 4://"Block number"
												break;
											default:
												break;
										}	
							}
								break;
							case GUI_KEY_ESCAPE:
								if(WM_HasFocus(LISTSMSWIN_INBOX_SELETE_Handle)){
								WM_DeleteWindow(LISTSMSWIN_INBOX_SELETE_Handle);
								WM_SetFocus(SMSWIN_INBOX);}
								break;
					}//end switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key
					}//end case WM_NOTIFY_OWNER_KEY:
					break;
				
			case WM_KEY:
				{
					int TextOffset,TextSize;
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
						case GUI_KEY_ENTER:
									//创建选项列表
								LISTSMSWIN_INBOX_SELETE_Handle= LISTBOX_CreateEx(0,0,75,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN_INBOX,\
																									WM_CF_SHOW,0,MY_WIDGET_LISTSMSINBOXSELETE_ID_0,IBSELETEItem);	//
								LISTBOX_EnableWrapMode(LISTSMSWIN_INBOX_SELETE_Handle,1);//启用自动换行模式
								WM_SetFocus(LISTSMSWIN_INBOX_SELETE_Handle);	
							
							break;
						case 0 :
						case GUI_KEY_ESCAPE:
						{
						if(WM_HasFocus(LISTSMSWIN_INBOX_SELETE_Handle)){
							WM_DeleteWindow(LISTSMSWIN_INBOX_SELETE_Handle);
							WM_SetFocus(SMSWIN_INBOX);	
						}else{
							WM_DeleteWindow(SMSWIN_INBOX);
							//WM_HideWindow(SMSWIN);
							WM_ShowWindow(TEMP_WIN);
							//WM_BringToBottom(MIDWIN[1]) ;
							WM_BringToTop(TEMP_WIN);
							WM_SetFocus(TEMP_WIN);
							
						}

						}


									
							break;
						case GUI_KEY_LEFT:
							{
								GUI_Delay(50);

								
								if((sms_show.index-1>=0)&&(!sms_show.wait)){
										--sms_show.index;
										GUI_SHOW1SMS(sms_show.mem,SMSBOX.INDEX[sms_show.index],sms_show.index);
										//printf("GUI_KEY_LEFT\n");
									}else{
									sms_show.index=SMSBOX.SELETED_used-1;
									GUI_SHOW1SMS(sms_show.mem,SMSBOX.INDEX[sms_show.index],sms_show.index);
									}
								
								}

							
							break;
						case GUI_KEY_RIGHT:
							{
								GUI_Delay(50);

								if((sms_show.index+1<(SMSBOX.SELETED_used))&&(!sms_show.wait)){
										++sms_show.index;
										GUI_SHOW1SMS(sms_show.mem,SMSBOX.INDEX[sms_show.index],sms_show.index);
										//printf("GUI_KEY_RIGHT\n");
									}else{
									sms_show.index=0;
									GUI_SHOW1SMS(sms_show.mem,SMSBOX.INDEX[sms_show.index],sms_show.index);
									}
								}
							break;
						case GUI_KEY_UP:
								TextOffset=MULTIEDIT_GetCursorCharPos(SMSWIN_INBOX_MULTIEDIT_HANDLE)-96;
								TextSize=MULTIEDIT_GetTextSize(SMSWIN_INBOX_MULTIEDIT_HANDLE);
							if(TextOffset < 0){
								MULTIEDIT_SetCursorOffset(SMSWIN_INBOX_MULTIEDIT_HANDLE,0);
								}else{
								MULTIEDIT_SetCursorOffset(SMSWIN_INBOX_MULTIEDIT_HANDLE,TextOffset);}
							
							break;
						case GUI_KEY_DOWN:
								TextOffset=MULTIEDIT_GetCursorCharPos(SMSWIN_INBOX_MULTIEDIT_HANDLE)+96;
								TextSize=MULTIEDIT_GetTextSize(SMSWIN_INBOX_MULTIEDIT_HANDLE);
							if(TextOffset < TextSize){
								MULTIEDIT_SetCursorOffset(SMSWIN_INBOX_MULTIEDIT_HANDLE,TextOffset);
								}else{
								MULTIEDIT_SetCursorOffset(SMSWIN_INBOX_MULTIEDIT_HANDLE,TextSize);}
							
							break;
						default:
							break;
						
					
				}	


					//GUI_ClearKeyBuffer();//清除按键缓冲器
				
				}
			break;
				
		
		/* 重绘消息*/
			case WM_PAINT:
				{
					//printf("MidWin1重绘消息\n");
						//WM_SetFocus(SMSWIN_Handle);
						GUI_SetBkColor(GUI_BLACK);
						GUI_Clear();
				}			
			break;

		 default:
			WM_DefaultProc(pMsg);
			break;
		}

}

/*
*********************************************************************************************************
*	函 数 名: CreateSMSWIN_INBOX
*	功能说明: 
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateSMSWIN_INBOX(void)
{
	if(net_flags.EC20_INIT){
		SMSWIN_INBOX = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbSMSWIN_INBOX,0);
		//第一栏 SMSWIN_INBOX_EDIT_Handle
		SMSWIN_INBOX_EDIT_Handle =EDIT_CreateEx(80,0,128-80,14,SMSWIN_INBOX,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,0,MY_WIDGET_INBOX_EDIT_Handle_ID_0,20);
		EDIT_SetBkColor(SMSWIN_INBOX_EDIT_Handle,MULTIEDIT_CI_READONLY,GUI_GRAY_3F);// 只读模式颜色要使用的背景色。GUI_GRAY_E7 GUI_GRAY_3F
		EDIT_SetTextAlign(SMSWIN_INBOX_EDIT_Handle,GUI_TA_LEFT);//设定文本左对齐
		EDIT_SetFont(SMSWIN_INBOX_EDIT_Handle,&GUI_FontHZ12);
		EDIT_SetTextColor(SMSWIN_INBOX_EDIT_Handle,MULTIEDIT_CI_READONLY,GUI_BLACK);//只读模式颜色要使用的字体颜色 GUI_WHITE
		//消息详情 SMSWIN_INBOX_MULTIEDIT_HANDLE
		SMSWIN_INBOX_MULTIEDIT_HANDLE = MULTIEDIT_Create(0,14,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN_INBOX,\
																										MY_WIDGET_INBOX_MULTIEDIT_Handle_ID_0,WM_CF_SHOW|WM_CF_FGND,\
																											MULTIEDIT_CF_READONLY,"",300);//只读模式
		
		MULTIEDIT_SetFont(SMSWIN_INBOX_MULTIEDIT_HANDLE,&GUI_FontHZ12);
		MULTIEDIT_SetTextColor(SMSWIN_INBOX_MULTIEDIT_HANDLE,MULTIEDIT_CI_READONLY,GUI_WHITE);//
		MULTIEDIT_SetBkColor(SMSWIN_INBOX_MULTIEDIT_HANDLE,MULTIEDIT_CI_READONLY,GUI_BLACK);//MULTIEDIT_CI_EDIT 编辑模式。MULTIEDIT_CI_READONLY 只读模式
		MULTIEDIT_SetAutoScrollH(SMSWIN_INBOX_MULTIEDIT_HANDLE,0);//禁用水平滚动条
		MULTIEDIT_SetAutoScrollV(SMSWIN_INBOX_MULTIEDIT_HANDLE,0);//启用自动使用垂直滚动条
	  MULTIEDIT_SetTextAlign(SMSWIN_INBOX_MULTIEDIT_HANDLE,GUI_TA_LEFT);//设定文本左对齐
		MULTIEDIT_SetWrapWord(SMSWIN_INBOX_MULTIEDIT_HANDLE);//启用换行模式
		//MULTIEDIT_SetWrapNone(SMSWIN_INBOX_MULTIEDIT_HANDLE);//启用无换行模式
		
	WM_BringToTop(SMSWIN_INBOX);
	WM_SetFocus(SMSWIN_INBOX);
		//EC20_SET_CPMS(sms_show.mem);//取得信息现存的消息数量
		EC20_UPDATA_SMSBOX_INDEX(sms_show.mem);
		GUI_SHOW1SMS(sms_show.mem,SMSBOX.INDEX[0],0);
		
	
	}else{
	
			WM_ShowWindow(TEMP_WIN);
			WM_BringToTop(TEMP_WIN);
			WM_SetFocus(TEMP_WIN);
	
	}


}


/*
*********************************************************************************************************
*	函 数 名: _cbSMSWIN
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbSMSWIN(WM_MESSAGE * pMsg) 
	{
//#ifdef DEBUG_MARK
//CPU_SR_ALLOC();
//OS_CRITICAL_ENTER();
////printf("_cbSMSWIN_ID:%d\n",pMsg->MsgId);
//OS_CRITICAL_EXIT();
//#endif
		
			switch (pMsg->MsgId) 
		{
//			case WM_NOTIFY_OWNER_KEY:
//			{
//				switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
//					{
//					case GUI_KEY_ENTER:
//						
//						break;
//					case GUI_KEY_EXIT:
//						
//						break;
//					default:
//						break;
//				
//					}
//				}	
//			break;
				
			case WM_KEY:
				{
					
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
						case GUI_KEY_ENTER:
									{
									switch(LISTBOX_GetSel(SMSWIN_Handle)){
									
										case 0://INBOX
											TEMP_WIN=SMSWIN;
											WM_HideWindow(SMSWIN);
											CreateSMSWIN_INBOX();
											break;
										case 1:
											break;
										case 2:
											break;
										case 3:
											break;
										case 4:
											break;
										case 5:
											break;
										case 6:
											break;
										case 7:
											break;
										default:
											break;
									}
																		
								}
							break;
						case 0 :
						case GUI_KEY_ESCAPE:
										//printf("TEMP_WIN:%ld\n",TEMP_WIN);
										WM_DeleteWindow(SMSWIN);
										//WM_HideWindow(SMSWIN);
										WM_ShowWindow(MIDWIN[1]);
										//WM_BringToBottom(MIDWIN[1]) ;
										WM_BringToTop(MIDWIN[1]);
										//WM_SetFocus(MIDWIN[1]);}
										WM_SetFocus(LISTAPP_Handle);

									
							break;
						case GUI_KEY_LEFT:
							break;
						case GUI_KEY_RIGHT:
							break;
						case GUI_KEY_UP:
							break;
						case GUI_KEY_DOWN:
							break;
						default:
							break;
						
					
				}	
					//GUI_ClearKeyBuffer();//清除按键缓冲器
				
				}
			break;
				
		
		/* 重绘消息*/
			case WM_PAINT:
				{
					//printf("MidWin1重绘消息\n");
						WM_SetFocus(SMSWIN_Handle);
						GUI_SetBkColor(GUI_BLACK);
						GUI_Clear();
				}			
			break;

		 default:
			WM_DefaultProc(pMsg);
			break;
		}

}


/*
*********************************************************************************************************
*	函 数 名: CreateSMSWIN
*	功能说明: 二级菜单 SMS
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateSMSWIN(void)
{
		SMSWIN = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbSMSWIN,0);
		SMSWIN_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN,WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTSMSWINO_ID_0,SMSItem);	//

		//LISTBOX_SetItemSel(SMSWIN_Handle,3,0);//设置第一栏选定状态
		LISTBOX_EnableWrapMode(SMSWIN_Handle,1);//启用自动换行模式（wrap moade），即：从结尾立即滚动到开始或反之 1 启用自动换行模式，0 (默认)禁用。
		//WIDGET_SetFocusable(SMSWIN_Handle,1);//设置控件能接收输入焦点
		//WM_SetFocus(SMSWIN_Handle);
		
}

/*
*********************************************************************************************************
*	函 数 名: _cbCLOCKWIN
*	功能说明: 
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbCLOCKWIN(WM_MESSAGE * pMsg) 
	{
		OS_ERR err;
//#ifdef DEBUG_MARK
//CPU_SR_ALLOC();
//OS_CRITICAL_ENTER();
//printf("_cbCLOCKWIN:%d\n",pMsg->MsgId);
//OS_CRITICAL_EXIT();
//#endif
		
			switch (pMsg->MsgId) 
		{
//			case WM_NOTIFY_OWNER_KEY:
//			{
//				switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
//					{
//					case GUI_KEY_ENTER:
//						
//						break;
//					case GUI_KEY_EXIT:
//						
//						break;
//					default:
//						break;
//				
//					}
//				}	
//			break;
				
			case WM_KEY:
				{
					
					switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key){
						case GUI_KEY_ENTER:
									{
									switch(LISTBOX_GetSel(CLOCKWIN_Handle)){
									
										case 0://NTP SYNC CLOCK
											SENTCTR_PARA.Para1=0;
											SENTCTR =sent_EC20_SyncLocalTime;  //设定发送任务
											OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
											break;
										default:
											break;
									}
																		
								}
							break;
						case 0 :
						case GUI_KEY_ESCAPE:
										WM_DeleteWindow(CLOCKWIN);
										//WM_HideWindow(SMSWIN);
										WM_ShowWindow(MIDWIN[1]);
										//WM_BringToBottom(MIDWIN[1]) ;
										WM_BringToTop(MIDWIN[1]);
										//WM_SetFocus(MIDWIN[1]);}
										WM_SetFocus(LISTAPP_Handle);

									
							break;
						case GUI_KEY_LEFT:
							break;
						case GUI_KEY_RIGHT:
							break;
						case GUI_KEY_UP:
							break;
						case GUI_KEY_DOWN:
							break;
						default:
							break;
						
					
				}	
					//GUI_ClearKeyBuffer();//清除按键缓冲器
				
				}
			break;
				
		
		/* 重绘消息*/
			case WM_PAINT:
				{
						WM_SetFocus(CLOCKWIN_Handle);
						GUI_SetBkColor(GUI_BLACK);
						GUI_Clear();
				}			
			break;

		 default:
			WM_DefaultProc(pMsg);
			break;
		}

}


/*
*********************************************************************************************************
*	函 数 名: CreateCLOCKWIN
*	功能说明: 二级菜单 CLOCK
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateCLOCKWIN(void)
{
		CLOCKWIN = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbCLOCKWIN,0);
		CLOCKWIN_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,CLOCKWIN,WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTCLOCKWIN_ID_0,CLOCKItem);	//

		LISTBOX_EnableWrapMode(CLOCKWIN_Handle,1);//启用自动换行模式（wrap moade），即：从结尾立即滚动到开始或反之 1 启用自动换行模式，0 (默认)禁用。
		//WIDGET_SetFocusable(SMSWIN_Handle,1);//设置控件能接收输入焦点
		//WM_SetFocus(CLOCKWIN_Handle);
		
}

/*
*********************************************************************************************************
*	函 数 名: _CreateMessageBox
*	功能说明: 自定义消息框消息
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
 
//static WM_HWIN _CreateMessageBox(const char * sMessage, const char * sCaption, int Flags, const GUI_FONT * pFont)
//{
//WM_HWIN hWin;
//WM_HWIN hItem;
//GUI_RECT Rect;
//hWin = MESSAGEBOX_Create(sMessage, sCaption, Flags); //--------------（1）
////
//// Change font of message box window
////
//FRAMEWIN_SetFont(hWin, pFont); //--------------（2）
////
//// Adjust size
////
//WM_GetWindowRectEx(hWin, &Rect); //--------------（3）
//WM_SetWindowPos(hWin, Rect.x0,//窗口坐标x
//Rect.y0,//窗口坐标y
//Rect.x1 - Rect.x0,//窗口x size
//Rect.y1 - Rect.y0);//窗口y size
////
//// Change font of button widget
////
//hItem = WM_GetDialogItem(hWin, GUI_ID_OK); //--------------（4）
//BUTTON_SetFont(hItem, pFont);
////
//// Adjust size of button widget
////
//WM_GetWindowRectEx(hItem, &Rect); //--------------（5）
//WM_SetWindowPos(hItem, Rect.x0,
//Rect.y0,
//Rect.x1 - Rect.x0,
//Rect.y1 - Rect.y0);
////
//// Change font of text widget
////
//hItem = WM_GetDialogItem(hWin, GUI_ID_TEXT0); //--------------（6）
//TEXT_SetFont(hItem, pFont);
////
//// Adjust size text widget
////
//WM_GetWindowRectEx(hItem, &Rect); //--------------（7）
//WM_SetWindowPos(hItem, Rect.x0,
//Rect.y0,
//Rect.x1 - Rect.x0,
//Rect.y1 - Rect.y0);
////return hWin;
//return 0;
//}


/*
*********************************************************************************************************
*	函 数 名: GUI_Show_MYmsg1
*	功能说明: 显示消息
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
 
//void GUI_Show_MYmsg1(const char* text,const char* title) {
////			GUI_SetFont(&GUI_FontHZ12);
////			GUI_SetColor(GUI_RED);
////			GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
////			GUI_DispStringAt(text,5,0);//GSM
//////GUI_MessageBox(text, title, GUI_MESSAGEBOX_CF_MOVEABLE);
//////GUI_HWIN hwim =	MESSAGEBOX_Create(text, title, 0);
//////GUI_HWIN hwim = GUI_ExecCreatedDialog(_CreateMessageBox(text, title, 0, &GUI_FontHZ12));
//////WM_SetFocus( WM_GetClientWindow(hwim));
////	GUI_Delay(1000);
////	WM_InvalidateWindow(TEMP_WIN);//使指定窗口无效。
//// //GUI_EndDialog(hwim, 0);
//////GUI_HWIN ATwin =WM_GetActiveWindow() ;
//////WM_SetFocus(TEMP_WIN);
//}

/*
*********************************************************************************************************
*	函 数 名: _EnableSkinning
*	功能说明: 
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
 
//static void _EnableSkinning(void) {
//FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
//BUTTON_SetDefaultSkin (BUTTON_SKIN_FLEX);
//}

/*
*********************************************************************************************************
*	函 数 名: _cbBASEMIN
*	功能说明: 桌面窗口回调函数
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
//void _cbBASEMIN(WM_MESSAGE * pMsg) 
//	{
//switch(pMsg->MsgId){
//		 default:
//			WM_DefaultProc(pMsg);
//			break;

//}

//}
/*
*********************************************************************************************************
*	函 数 名: _cbBASEMIN
*	功能说明: 桌面窗口回调函数
*	形    参：WM_MESSAGE * pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
//void pingyingtest(void) 
//{
//	int key=0, index=0;
//u8 inputstr[7]={'2','4','0'};		//最大输入6个字符+结束符
//	inputstr[2]='\0';//添加结束符
//	key=t9.getpymb(inputstr);	//得到匹配的结果数
//	if(key){
//	printf("%s\r\n",t9.pymb[key-1]->pymb);
//	}
//	

//}

/*
*********************************************************************************************************
*	函 数 名: KEEP_LCD_ON_FOR_A_WHILES
*	功能说明: 
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
void KEEP_LCD_ON_FOR_A_WHILES(void){
	LCD_LED=1;
	stm32_flat.LCD_TURNOFF_TIME_COUNT=stm32_flat.LCD_TURNOFF_TIME;
}
/*
*********************************************************************************************************
*	函 数 名: ResetLCDBKtime
*	功能说明: 重置LCD背光时间 初始值60s
*	形    参：u32 times ,times 单位s，如果设定为0 则常亮
*	返 回 值: 无
*********************************************************************************************************
*/
void ResetLCDBKtime(u32 times){
	if(times==0){
	stm32_flat.LCD_KEEP_ON=1;
	stm32_flat.LCD_TURNOFF_TIME_COUNT=stm32_flat.LCD_TURNOFF_TIME;
	}else{
	stm32_flat.LCD_KEEP_ON=0;
	stm32_flat.LCD_TURNOFF_TIME=times;
	}
	
}

/*
*********************************************************************************************************
*	函 数 名: emWinMainApp
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void ucGUIMainApp(void)
{
	WM_InvalidateWindow(TOPWIN);//使指定窗口无效。//用于回调更新状态栏
	//WM_InvalidateWindow(MIDWIN[0]);//使指定窗口无效。
	//GUI_Delay(10);
	GUI_Exec();

	
	
}
/*
*********************************************************************************************************
*	函 数 名: ucGUIMainAppinit
*	功能说明: GUI主界面初始化 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ucGUIMainAppinit(void)
{

	//OS_ERR  err;
  
	//准备建立3个窗口，以下是使用到的用户定义参数，方便在回调函数中使用
	midWinPara.xSizeLCD = LCD_GetXSize();				            //LCD屏幕尺寸
	midWinPara.ySizeLCD = LCD_GetYSize();				            //LCD屏幕尺寸
	midWinPara.xPosWin	= MIDWIN_xPos;							          //窗口的起始位置
	midWinPara.yPosWin  = MIDWIN_yPos;							          //窗口的起始位置
	midWinPara.xSizeWin = midWinPara.xSizeLCD;						      //窗口尺寸
	midWinPara.ySizeWin = midWinPara.ySizeLCD-midWinPara.yPosWin;	//窗口尺寸
	
		/************************设置默认参数配置*****************************************/
  	LISTBOX_SetDefaultBkColor(LISTBOX_CI_UNSEL,GUI_BLACK);	   //设置列表框未被选中的背景颜色 LISTBOX_CI_UNSEL 未选元素
		LISTBOX_SetDefaultBkColor(LISTBOX_CI_SEL,GUI_BLACK);	   //设置列表框被选中的背景颜色 LISTBOX_CI_SEL 选定元素，不带焦点.
		LISTBOX_SetDefaultBkColor(LISTBOX_CI_SELFOCUS,GUI_LIGHTGRAY);	   //设置列表框被选中的背景颜色 LISTBOX_CI_SELFOCUS 选定元素，带焦点。	
		LISTBOX_SetDefaultFont(&GUI_FontHZ12);       					 //设置列表框中 默认文本字体
		LISTBOX_SetDefaultTextAlign(GUI_TA_LEFT|GUI_TA_VCENTER);		//设置新创建的 LISTBOX 控件所使用的默认文本对齐方式 
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_UNSEL,GUI_LIGHTGRAY);	   //设置新创建的 LISTBOX 控件所使用的默认文本颜色 LISTBOX_CI_UNSEL 未选元素。
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_SEL,GUI_RED);	   //设置列表框被选中的文本颜色 LISTBOX_CI_SEL 选定元素，不带焦点。
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_SELFOCUS,GUI_BLACK);	   //设置列表框被选中的文本颜色 LISTBOX_CI_SELFOCUS 选定元素，带焦点。
			
		/*************************************************************************/
	
	
	////设定初始化值
	sms_show.mem=1;//1:SM 2:ME
	sms_show.index=0;
	
//	WM_SetCallback(WM_HBKWIN, _cbBASEMIN);/* 设置桌面窗口的回调函数 */


	CreatTopWindow();
	CreateMidWin1();
	CreateMidWin0();



  GUI_Delay(20);



}

