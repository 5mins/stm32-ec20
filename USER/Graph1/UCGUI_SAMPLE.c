#include "includes.h"
#include "Graph1/UCGUI_SAMPLE.h"
//#include "Graph1/GUI_BASE.h"
//#include "./Graph1/photo.h"
#include "ec20.h"
#include "pyinput.h"
/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/






/*
*********************************************************************************************************
״̬����ʱ�䣬�źţ����ӣ�����,
00-Ĭ��ҳ�棺
10-���ţ��ռ��䣬�����䣬�ݸ��䣬�ؼ������أ�����
20-�绰���绰����ͨ����¼���ڰ�����������
30-GPS:
40-С���ߣ����ӣ����±�������
50-���ã�ʱ�䣬������
*********************************************************************************************************
*/ 

/*
*********************************************************************************************************
*                                      ����
*********************************************************************************************************
*/ 
extern OS_MEM INTERNAL_MEM; 
//����ͼ��	
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou0;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou1;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou2;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou3;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou4;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou5;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou6;
extern GUI_CONST_STORAGE GUI_BITMAP bmzhou7;
extern GUI_CONST_STORAGE GUI_BITMAP bmAIR_MODE;//����ģʽͼ��
extern GUI_CONST_STORAGE GUI_BITMAP bmnosign;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign1;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign2;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign3;
extern GUI_CONST_STORAGE GUI_BITMAP bmsign4;

extern enum EC20_SENT_CTR SENTCTR;//EC20���Ϳ���ָ����
extern EC20_SENT_CTR_Parameters SENTCTR_PARA;////���Ϳ���ָ������
extern OS_TCB   AppTaskEC20CtrTCB;//EC20��Ϣ������������
//uint8_t UserApp_Flag=0;
extern RTC_TIME rtc_time;
///////���ڱ�־
GUI_HWIN TEMP_WIN;//���ڼ�¼����Ĵ��ڣ�����BACK���ؼ���
GUI_HWIN TOPWIN;//����״̬��
uint8_t TOP_IS_INIT=0;//���״̬��ͼ���Ƿ���ɳ�ʼ��
uint8_t  DATE_IS_CHANGE=1;//��������Ƿ���ɸı�
//uint8_t  WEEK_IS_CHANGE=1;//���WEEK�Ƿ�ı�
//PROGBAR_Handle PowerProgBar; /* ������ʾ��������� */
//TEXT_Handle voltage_TextHandle;/* ״̬��ʱ����ʾ�ı���� */
TEXT_Handle Time_TextHandle;/* ״̬��ʱ����ʾ�ı���� */

RTC_DateTypeDef RTC_OldDate;//  �����ж��Ƿ���Ҫˢ����Ļ�ϵ�����
char date_text_buffer[12]={0};//2018-11-11/0 ���ڻ������
char time_text_buffer[6]={0};//02:02:01/0 �����ʾ�룬�����С�趨Ϊ9��

char voltage_text_buffer[6]={0};//20%

/////////////
GUI_HWIN MIDWIN[2];
//MIDWIN0
TEXT_Handle MIDWIN0_Operation_Bus_handle;
TEXT_Handle MIDWIN0_Date_handle;
//struct Operation_Bus{
//int sign;//����������Ӫ�̱�ʶ�����Ƿ���Ч��1��Ч
//char COPS[16] ;//�洢���͹�������Ӫ�̱�ʶ
//}Operation1;
extern NET_FLAGS net_flags;//����״̬
extern Operation_Bus  Operation ;
extern volatile int rssi;//�ź�ǿ�ȱ���
extern volatile int ber;//������
extern EC20TmrCTR_FLAGS ec20Tmrctr_flags;

//MIDWIN1

LISTBOX_Handle LISTAPP_Handle ;
//SMSWIN
GUI_HWIN SMSWIN;
LISTBOX_Handle SMSWIN_Handle;
GUI_HWIN SMSWIN_INBOX;
EDIT_Handle SMSWIN_INBOX_EDIT_Handle;//��ʾ��Ϣinxex���
MULTIEDIT_HANDLE SMSWIN_INBOX_MULTIEDIT_HANDLE;//��ʾ��Ϣ����ҳ����
LISTBOX_Handle LISTSMSWIN_INBOX_SELETE_Handle ;//��ʾ��Ϣҳ��ѡ�ɾ�����ظ���
extern TEMP_Message_Storage tmp_sms[1];////��ʱ�洢��Ϣ������ʾʹ��
extern Message_BOX_COUNT SMSBOX;//����Ϣ���������2 ��ȫ�ֱ���ʹ��

SMS_SHOW sms_show;
//CLOCKWIN
GUI_HWIN CLOCKWIN;
LISTBOX_Handle CLOCKWIN_Handle;
/////
typedef struct WIN_PARA{			//����ʹ�õ����û���������������ڻص�������ʹ��
	int xSizeLCD, ySizeLCD;			//LCD��Ļ�ߴ�
	int xPosWin,  yPosWin;			//���ڵ���ʼλ��
	int xSizeWin, ySizeWin;			//���ڳߴ�	
	int xSizeBM,  ySizeBM;		//ͼ���ȣ��߶�	
}WIN_PARA;

WIN_PARA midWinPara;			//�û�ʹ�õĴ��ڶ�������


//int32_t LCD_TURNOFF_TIME =30;//LCD��Ļ���⵹��ʱ ��λ��
//uint32_t LCD_TURNOFF_TIME_TEMP=30;

extern STM32_CTR_FLAGS stm32_flat;
//u8 LCD_KEEP_ON =0;//LCD��Ļ���ⳣ��

/* ���������б�Ĵ��� */
 GUI_ConstString applistItem[] = 
{
	"SMS","Notepad","Weather","Clock","Safari", "Phone","Picture","Calculator",NULL
};
/* �����˵� */
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
*	�� �� ��: ucGUI_UpDataTopWindow_Time
*	����˵��: ��ʱ�� ����״̬��ʱ�������
*	��    �Σ�
*	�� �� ֵ: ��
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
				RTC_OldDate.RTC_WeekDay!=rtc_time.RTC_Date.RTC_WeekDay)//�������
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
			
			
			
			TEXT_SetText(Time_TextHandle,time_text_buffer)  ;		//����ָ�� TEXT �ؼ����ı���
					
						GUI_SetColor(GUI_BLACK);//����
						GUI_SetBkColor(GUI_WHITE);
						sprintf(time_text_buffer,"%02d",(u8)rtc_time.RTC_Time.RTC_Seconds);
						GUI_DispStringAt(time_text_buffer,TOPWINTIME_xPos+32,TOPWINTIME_yPos);//
				
		}
}

/*********************************************************************************************************
*	�� �� ��: ucGUI_UpDataTopWindow_Signal
*	����˵��: ��ʱ�� ����״̬����ѹֵ
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void ucGUI_UpDataTopWindow_voltage(void)
{
	
	float stm32inputvoltage = GetSTM32INPUTVOLTAGE();//stm32vcc����˵�ѹ
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
	
	
	//TEXT_SetText(voltage_TextHandle,voltage_text_buffer)  ;		//����ָ�� TEXT �ؼ����ı���
	

}
/*********************************************************************************************************
*	�� �� ��: ucGUI_UpDataTopWindow_Signal
*	����˵��: ��ʱ�� ����״̬���ź�ֵ
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void ucGUI_UpDataTopWindow_Signal(void )
{

	
	if(net_flags.EC20_AIR_MODE==1||net_flags.IS_EC20_OFF==1||net_flags.EC20_INIT==0){
		
		GUI_DrawBitmap(&bmAIR_MODE,TopWindow_Signa_xPos,0);//����ģʽͼ��

		
	}else{
		
	if(Operation.sign||net_flags.EC20_CSQ){
			
			if((rssi>=2&&rssi<=30)||(rssi>=102&&rssi<=190)){ //-109dBm...-53dBm �ź����� 3��
			//Gui_DrawbmpSign(58,0,gImage_sign3);
			GUI_DrawBitmap(&bmsign3,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
				
			}else if(rssi==31||rssi==191){ //-51dBm or greater �ź����� 4��
			//Gui_DrawbmpSign(58,0,gImage_sign4);
				GUI_DrawBitmap(&bmsign4,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==1||rssi==101){ //-111dBm  �ź�΢�� 2��
			//Gui_DrawbmpSign(58,0,gImage_sign2);
				GUI_DrawBitmap(&bmsign2,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==0||rssi==100){//-113dBm or less �źż�΢�� 1��
				//Gui_DrawbmpSign(58,0,gImage_sign1);
				GUI_DrawBitmap(&bmsign1,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else if(rssi==99||rssi==199){ //Not known or not detectable ���ź�
			//Gui_DrawbmpSign(58,0,gImage_sign0);
				GUI_DrawBitmap(&bmnosign,TopWindow_Signa_xPos,TopWindow_Signa_yPos);
			}else{
			//Gui_DrawbmpSign(58,0,gImage_sign0);//δ֪���
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
							GUI_DispStringAt("??",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//δ֪
							break;

			
			}
			

			

		}else{
			GUI_DrawBitmap(&bmnosign,TopWindow_Signa_xPos,TopWindow_Signa_yPos);//δ֪���
		GUI_DispStringAt("??",TopWindow_SignaWord_xPos,TopWindow_SignaWord_yPos);//δ֪
		}
	
	
	
	
	
	}


		
}



/*********************************************************************************************************
*	�� �� ��: UpDataTopWindow
*	����˵��: ���¶���״̬��
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void UpDataTopWindow() 
//{
//	if(!topwin_hide){
//	//printf("UpDataTopWindow\n");//debug_mark1

//	//LCD_Fill(0,0,lcddev.width,14,m_TimeLine_BC_color);//����
//	ucGUI_UpDataTopWindow_Time(GUI_WHITE);
//	ucGUI_UpDataTopWindow_Signal(GUI_WHITE);
//		}

//}
/*
*********************************************************************************************************
*	�� �� ��: _cbTOPWin
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbTOPWin(WM_MESSAGE * pMsg) 
{
	//printf("_cbTOPWin_ID:%x\n",pMsg->MsgId);
	switch (pMsg->MsgId) 
	{
		
	
		case WM_TIMER:
					ucGUI_UpDataTopWindow_Time();//����״̬��ʱ��
					ucGUI_UpDataTopWindow_Signal();//����״̬���ź�ֵ
					ucGUI_UpDataTopWindow_voltage();//���µ�ѹ
					//WM_InvalidateWindow(TOPWIN);//ʹָ��������Ч��
					WM_BringToBottom(TOPWIN) ;
					WM_RestartTimer(pMsg->Data.v, 1000);  
		break;
		
		/* �ػ���Ϣ*/
		case WM_PAINT:
			{
			
				//printf("_cbTOPWin�ػ���Ϣ\n");
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
*	�� �� ��: CreatTopWindow
*	����˵��: ��������״̬��
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void CreatTopWindow() 
{
	TOPWIN = WM_CreateWindow(TOPWINTIME_xPos,TOPWINTIME_yPos,T0PWIN_Time_Width,TOPWIN_Hight,WM_CF_SHOW|WM_CF_MEMDEV,_cbTOPWin,0);//0,0,34,TOPWIN_Hight,WM_CF_SHOW|WM_CF_MEMDEV,_cbTOPWin,0
	///������ʾ
//	timeProgBar = PROGBAR_CreateAsChild(0,2,40,14,TOPWIN,1,WM_CF_SHOW|WM_CF_MEMDEV);
//	PROGBAR_SetFont(timeProgBar,&GUI_FontHZ12);
//	//PROGBAR_SetText(timeProgBar,"11");
//	PROGBAR_SetMinMax(timeProgBar,0,60);
//	voltage_TextHandle = TEXT_CreateEx(TopWindow_voltage_xPos,TopWindow_voltage_yPos,T0PWIN_voltage_Width,TOPWIN_Hight,TOPWIN,WM_CF_SHOW|WM_CF_MEMDEV,TEXT_CF_LEFT|TEXT_CF_VCENTER,0,"3.7v");
//	TEXT_SetFont(voltage_TextHandle,&GUI_FontHZ12) ;			//��������ָ�� TEXT �ؼ������塣
//	TEXT_SetTextColor(voltage_TextHandle,GUI_WHITE)  ;				//����ָ�� TEXT �ؼ����ı���ɫ
	
	//////ʱ����ʾ
	Time_TextHandle = TEXT_CreateEx(TOPWINTIME_xPos,TOPWINTIME_yPos,T0PWIN_Time_Width,TOPWIN_Hight,TOPWIN,WM_CF_SHOW|WM_CF_MEMDEV,TEXT_CF_LEFT|TEXT_CF_TOP,0,"20:20");
	TEXT_SetFont(Time_TextHandle,&GUI_FontHZ12) ;			//��������ָ�� TEXT �ؼ������塣
	//TEXT_SetText(Time_TextHandle,)  ;									//����ָ�� TEXT �ؼ����ı���
	//TEXT_SetTextAlign(Time_TextHandle,TEXT_CF_LEFT|TEXT_CF_VCENTER)  ;				//����ָ�� TEXT �ؼ����ı����뷽ʽ��
	TEXT_SetTextColor(Time_TextHandle,GUI_WHITE)  ;				//����ָ�� TEXT �ؼ����ı���ɫ
	
	WM_CreateTimer(TOPWIN,0, 1000, 0);//���ö�ʱ������״̬��
}



/*
*********************************************************************************************************
*	�� �� ��: Update_Operation_Bus
*	����˵��: 
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: UpData_Date
*	����˵��: 
*	��    �Σ�forceUpdata 1:ǿ�Ƹ���ʱ�䣬
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UpData_Date(u8 forceUpdata) 
{
	if(MIDWIN0_Date_handle){

	      if(!(bsp_result&BSP_RTC))
      {
				if(forceUpdata){
						//����RTCʱ��
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

						//�����м�����
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
				return;//ǿ���˳�����
			
			}


	}	
}


/*
*********************************************************************************************************
*	�� �� ��: _cbMidWin0
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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

					//int kkey = GUI_GetKey();//���ذ����������е����ݡ�
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
//					//GUI_ClearKeyBuffer();//�������������
//				
				}
			break;
		
			
		/* �ػ���Ϣ*/
		case WM_PAINT:
			{
			
					GUI_SetColor(GUI_BLACK);
					GUI_Clear();
					Update_Operation_Bus();//��Ӫ�̱�ʶ
					UpData_Date(0) ;//����
				  //WM_SetFocus(MIDWIN[0]);//�����Ƿ���Ҫ�趨����
					}	
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}



/**
*********************************************************************************************************
  * @brief  CreateMidWin�������м�Ĵ���0
  * @param  none
  * @retval none
*********************************************************************************************************
  */
static void CreateMidWin0(void)
{
	MIDWIN[0] = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW,_cbMidWin0,0);//�м䴰��Ĭ��ҳ��	hFrame[0] _cbMidWin0
	MIDWIN0_Operation_Bus_handle = TEXT_CreateEx(0,24,midWinPara.xSizeWin,16,MIDWIN[0],WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,0,"cmcc");
	MIDWIN0_Date_handle = TEXT_CreateEx(0,44,midWinPara.xSizeWin,12,MIDWIN[0],WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,0,"2018-10-09");
	WM_SetFocus(MIDWIN[0]);
	TEMP_WIN=MIDWIN[0];
	//WM_SetCallback(MIDWIN[0],_cbMidWin0);
}

void UpdataMidWin0(void){
WM_InvalidateWindow(MIDWIN[0]);//ʹָ��������Ч��

}


/*
*********************************************************************************************************
*	�� �� ��: _cbMidWin1
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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
				//	int kkey = GUI_GetKey();//���ذ����������е����ݡ�
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
					GUI_ClearKeyBuffer();//�������������
				
				}
			break;
				
		
		/* �ػ���Ϣ*/
			case WM_PAINT:
				{
					//printf("MidWin1�ػ���Ϣ\n");
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
*	�� �� ��: CreateMidWin1
*	����˵��: �������ܲ˵�
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateMidWin1(void)
{
//		uint8_t i=0;
		MIDWIN[1] = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_HIDE|WM_CF_FGND|WM_CF_MEMDEV,_cbMidWin1,0);//�м䴰��Ĭ��ҳ��	hFrame[0] _cbMidWin0

		LISTAPP_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,MIDWIN[1],WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTAPP_ID_0,applistItem);	//
//		LISTBOX_SetBackColor(LISTAPP_Handle,0,GUI_BLACK);	   //�����б��δ��ѡ�еı�����ɫ
//		LISTBOX_SetBackColor(LISTAPP_Handle,1,GUI_LIGHTGRAY);	   //�����б��ѡ�еı�����ɫ
//		LISTBOX_SetFont(LISTAPP_Handle,&GUI_FontHZ12);        //�����б�����ı�����
//		LISTBOX_SetTextColor(LISTAPP_Handle,0,GUI_LIGHTGRAY);	   //�����б��δ��ѡ�е��ı���ɫ
//		LISTBOX_SetTextColor(LISTAPP_Handle,1,GUI_BLACK);	   //�����б��ѡ�е��ı���ɫ
		//LISTBOX_SetItemSel(LISTAPP_Handle,3,1);//���õ�һ��ѡ��״̬
		LISTBOX_EnableWrapMode(LISTAPP_Handle,1);//�����Զ�����ģʽ��wrap moade���������ӽ�β������������ʼ��֮ 1 �����Զ�����ģʽ��0 (Ĭ��)���á�
		//WIDGET_SetFocusable(LISTAPP_Handle,1);//���ÿؼ��ܽ������뽹��
		WM_SetFocus(LISTAPP_Handle);
		
		//printf("WM_GetParent(MIDWIN[0]):%ld \nMIDWIN[0]:%ld  \n WM_GetParent(MIDWIN[0][1]):%ld\n MIDWIN[0][1]:%ld\n",WM_GetParent(MIDWIN[0][0]),MIDWIN[0][0],WM_GetParent(MIDWIN[0][1]),MIDWIN[0][1]);
		
}



/*
*********************************************************************************************************
*	�� �� ��: GUI_SHOW1SMS
*	����˵��: ���һ��SMS��Ϣ�� SMSWIN_INBOX_MULTIEDIT_HANDLE 
*	��    �Σ�men:1 from SM  2:form ME  
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void GUI_SHOW1SMS(uint8_t mem,uint16_t index,int16_t show_index){
	OS_ERR err;
	OS_ERR os_err;
	
	if(SMSBOX.SELETED_used>0){//�ȼ��������Ϣ���Ƿ�Ϊ0
			sms_show.wait=1;//���GUI���ڵȴ���Ϣ���ݷ���

				SENTCTR =sent_EC20_Read1SMS;  //�趨��������
				SENTCTR_PARA.Para1=mem;
				SENTCTR_PARA.Para3=index;
				OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
				uint8_t times =0;//��ȴ�1500ms 
				while(((tmp_sms[0].alive==0)||(tmp_sms[0].index!=index))&&(times<15)){	
				OSTimeDly(100,OS_OPT_TIME_DLY,&os_err);times++;}//debug_mark GUI_Delay(100);
					
						char  * p_mem_blk;
					CPU_SR_ALLOC();
					OS_CRITICAL_ENTER();
					p_mem_blk = mymalloc(0,TEMP_Message_Storage_text_size);
					OS_CRITICAL_EXIT();
						
						memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//�������
				
					if(p_mem_blk&&tmp_sms[0].alive==1&&tmp_sms[0].index==index){


						sprintf(p_mem_blk,"(%d/%d)",show_index+1,SMSBOX.SELETED_used);
						EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
						memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//�������
						sprintf(p_mem_blk,"FROM:%s\n\n%s\n\nReTime:%s",tmp_sms[0].from,tmp_sms[0].text,tmp_sms[0].retime);
						MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,p_mem_blk);
					

						
					}else{
						
					sprintf(p_mem_blk,"��ȡʧ��:(%d/0)",index+1);
					//EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
					MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,p_mem_blk);
					}
OS_CRITICAL_ENTER();
myfree(0,p_mem_blk);//�˻��ڴ��
OS_CRITICAL_EXIT();

sms_show.wait=0;//���GUI�ȴ���Ϣ���ݷ��� ����
}else{

	MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"NO SMS");


}
}
/*
*********************************************************************************************************
*	�� �� ��: GUI_DELETESMS
*	����˵��: 
*	��    �Σ�men:1 from SM  2:form ME
<delflag> 0 Delete the message specified in <index>
					1 Delete all read messages from <mem1> storage
					2 Delete all read messages from <mem1> storage and sent mobile originated messages
					3 Delete all read messages from <mem1> storage as well as all sent and unsent mobile originated messages
					4 Delete all messages from <mem1> storage
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void GUI_DELETESMS(u8 mem ,u8 delflag,int16_t *show_index){
	OS_ERR err;
	OS_ERR os_err;
		if(SMSBOX.SELETED_used>0){//�ȼ��������Ϣ���Ƿ�Ϊ0
			sms_show.wait=1;//���GUI���ڵȴ���Ϣ���ݷ���

				SENTCTR =sent_EC20_Delete_SMS;  //�趨��������
				SENTCTR_PARA.Para1=mem;
				SENTCTR_PARA.Para2=delflag;
				SENTCTR_PARA.Para3=SMSBOX.INDEX[*show_index];
				OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
				uint8_t times =0;//��ȴ�1000ms 
				while((net_flags.EC20_Delete_SMS==0)&&(times<15)){	OSTimeDly(100,OS_OPT_TIME_DLY,&os_err);times++;}//debug_mark GUI_Delay(100);

				
					if(net_flags.EC20_Delete_SMS==1){
						if(delflag==0){//ɾ��һ����Ϣ�ɹ�
									if(*show_index-1>0){
									--(*show_index);
								GUI_SHOW1SMS(mem,SMSBOX.INDEX[*show_index],*show_index);
								}else{
								GUI_SHOW1SMS(mem,SMSBOX.INDEX[0],0);
								}
						}else{//�����Ϣ�ɹ�
						MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"����Ϣ!");
						}

						
						net_flags.EC20_Delete_SMS=0;
					}else{
						
					
					//EDIT_SetText(SMSWIN_INBOX_EDIT_Handle,p_mem_blk);
					MULTIEDIT_SetText(SMSWIN_INBOX_MULTIEDIT_HANDLE,"Del fail!");
					GUI_SHOW1SMS(mem,SMSBOX.INDEX[*show_index],*show_index);
					}

WM_DeleteWindow(LISTSMSWIN_INBOX_SELETE_Handle);
WM_SetFocus(SMSWIN_INBOX);			

sms_show.wait=0;//���GUI�ȴ���Ϣ���ݷ��� ����
}
}
/*
*********************************************************************************************************
*	�� �� ��: _cbSMSWIN_INBOX
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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
									//����ѡ���б�
								LISTSMSWIN_INBOX_SELETE_Handle= LISTBOX_CreateEx(0,0,75,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN_INBOX,\
																									WM_CF_SHOW,0,MY_WIDGET_LISTSMSINBOXSELETE_ID_0,IBSELETEItem);	//
								LISTBOX_EnableWrapMode(LISTSMSWIN_INBOX_SELETE_Handle,1);//�����Զ�����ģʽ
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


					//GUI_ClearKeyBuffer();//�������������
				
				}
			break;
				
		
		/* �ػ���Ϣ*/
			case WM_PAINT:
				{
					//printf("MidWin1�ػ���Ϣ\n");
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
*	�� �� ��: CreateSMSWIN_INBOX
*	����˵��: 
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateSMSWIN_INBOX(void)
{
	if(net_flags.EC20_INIT){
		SMSWIN_INBOX = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbSMSWIN_INBOX,0);
		//��һ�� SMSWIN_INBOX_EDIT_Handle
		SMSWIN_INBOX_EDIT_Handle =EDIT_CreateEx(80,0,128-80,14,SMSWIN_INBOX,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,0,MY_WIDGET_INBOX_EDIT_Handle_ID_0,20);
		EDIT_SetBkColor(SMSWIN_INBOX_EDIT_Handle,MULTIEDIT_CI_READONLY,GUI_GRAY_3F);// ֻ��ģʽ��ɫҪʹ�õı���ɫ��GUI_GRAY_E7 GUI_GRAY_3F
		EDIT_SetTextAlign(SMSWIN_INBOX_EDIT_Handle,GUI_TA_LEFT);//�趨�ı������
		EDIT_SetFont(SMSWIN_INBOX_EDIT_Handle,&GUI_FontHZ12);
		EDIT_SetTextColor(SMSWIN_INBOX_EDIT_Handle,MULTIEDIT_CI_READONLY,GUI_BLACK);//ֻ��ģʽ��ɫҪʹ�õ�������ɫ GUI_WHITE
		//��Ϣ���� SMSWIN_INBOX_MULTIEDIT_HANDLE
		SMSWIN_INBOX_MULTIEDIT_HANDLE = MULTIEDIT_Create(0,14,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN_INBOX,\
																										MY_WIDGET_INBOX_MULTIEDIT_Handle_ID_0,WM_CF_SHOW|WM_CF_FGND,\
																											MULTIEDIT_CF_READONLY,"",300);//ֻ��ģʽ
		
		MULTIEDIT_SetFont(SMSWIN_INBOX_MULTIEDIT_HANDLE,&GUI_FontHZ12);
		MULTIEDIT_SetTextColor(SMSWIN_INBOX_MULTIEDIT_HANDLE,MULTIEDIT_CI_READONLY,GUI_WHITE);//
		MULTIEDIT_SetBkColor(SMSWIN_INBOX_MULTIEDIT_HANDLE,MULTIEDIT_CI_READONLY,GUI_BLACK);//MULTIEDIT_CI_EDIT �༭ģʽ��MULTIEDIT_CI_READONLY ֻ��ģʽ
		MULTIEDIT_SetAutoScrollH(SMSWIN_INBOX_MULTIEDIT_HANDLE,0);//����ˮƽ������
		MULTIEDIT_SetAutoScrollV(SMSWIN_INBOX_MULTIEDIT_HANDLE,0);//�����Զ�ʹ�ô�ֱ������
	  MULTIEDIT_SetTextAlign(SMSWIN_INBOX_MULTIEDIT_HANDLE,GUI_TA_LEFT);//�趨�ı������
		MULTIEDIT_SetWrapWord(SMSWIN_INBOX_MULTIEDIT_HANDLE);//���û���ģʽ
		//MULTIEDIT_SetWrapNone(SMSWIN_INBOX_MULTIEDIT_HANDLE);//�����޻���ģʽ
		
	WM_BringToTop(SMSWIN_INBOX);
	WM_SetFocus(SMSWIN_INBOX);
		//EC20_SET_CPMS(sms_show.mem);//ȡ����Ϣ�ִ����Ϣ����
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
*	�� �� ��: _cbSMSWIN
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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
					//GUI_ClearKeyBuffer();//�������������
				
				}
			break;
				
		
		/* �ػ���Ϣ*/
			case WM_PAINT:
				{
					//printf("MidWin1�ػ���Ϣ\n");
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
*	�� �� ��: CreateSMSWIN
*	����˵��: �����˵� SMS
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateSMSWIN(void)
{
		SMSWIN = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbSMSWIN,0);
		SMSWIN_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,SMSWIN,WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTSMSWINO_ID_0,SMSItem);	//

		//LISTBOX_SetItemSel(SMSWIN_Handle,3,0);//���õ�һ��ѡ��״̬
		LISTBOX_EnableWrapMode(SMSWIN_Handle,1);//�����Զ�����ģʽ��wrap moade���������ӽ�β������������ʼ��֮ 1 �����Զ�����ģʽ��0 (Ĭ��)���á�
		//WIDGET_SetFocusable(SMSWIN_Handle,1);//���ÿؼ��ܽ������뽹��
		//WM_SetFocus(SMSWIN_Handle);
		
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCLOCKWIN
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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
											SENTCTR =sent_EC20_SyncLocalTime;  //�趨��������
											OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
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
					//GUI_ClearKeyBuffer();//�������������
				
				}
			break;
				
		
		/* �ػ���Ϣ*/
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
*	�� �� ��: CreateCLOCKWIN
*	����˵��: �����˵� CLOCK
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateCLOCKWIN(void)
{
		CLOCKWIN = WM_CreateWindow(0,MIDWIN_yPos,midWinPara.xSizeWin,midWinPara.ySizeWin,WM_CF_SHOW|WM_CF_FGND|WM_CF_MEMDEV,_cbCLOCKWIN,0);
		CLOCKWIN_Handle= LISTBOX_CreateEx(0,0,midWinPara.xSizeWin,midWinPara.ySizeWin-MIDWIN_yPos,CLOCKWIN,WM_CF_SHOW|WM_CF_FGND,0,MY_WIDGET_LISTCLOCKWIN_ID_0,CLOCKItem);	//

		LISTBOX_EnableWrapMode(CLOCKWIN_Handle,1);//�����Զ�����ģʽ��wrap moade���������ӽ�β������������ʼ��֮ 1 �����Զ�����ģʽ��0 (Ĭ��)���á�
		//WIDGET_SetFocusable(SMSWIN_Handle,1);//���ÿؼ��ܽ������뽹��
		//WM_SetFocus(CLOCKWIN_Handle);
		
}

/*
*********************************************************************************************************
*	�� �� ��: _CreateMessageBox
*	����˵��: �Զ�����Ϣ����Ϣ
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 
//static WM_HWIN _CreateMessageBox(const char * sMessage, const char * sCaption, int Flags, const GUI_FONT * pFont)
//{
//WM_HWIN hWin;
//WM_HWIN hItem;
//GUI_RECT Rect;
//hWin = MESSAGEBOX_Create(sMessage, sCaption, Flags); //--------------��1��
////
//// Change font of message box window
////
//FRAMEWIN_SetFont(hWin, pFont); //--------------��2��
////
//// Adjust size
////
//WM_GetWindowRectEx(hWin, &Rect); //--------------��3��
//WM_SetWindowPos(hWin, Rect.x0,//��������x
//Rect.y0,//��������y
//Rect.x1 - Rect.x0,//����x size
//Rect.y1 - Rect.y0);//����y size
////
//// Change font of button widget
////
//hItem = WM_GetDialogItem(hWin, GUI_ID_OK); //--------------��4��
//BUTTON_SetFont(hItem, pFont);
////
//// Adjust size of button widget
////
//WM_GetWindowRectEx(hItem, &Rect); //--------------��5��
//WM_SetWindowPos(hItem, Rect.x0,
//Rect.y0,
//Rect.x1 - Rect.x0,
//Rect.y1 - Rect.y0);
////
//// Change font of text widget
////
//hItem = WM_GetDialogItem(hWin, GUI_ID_TEXT0); //--------------��6��
//TEXT_SetFont(hItem, pFont);
////
//// Adjust size text widget
////
//WM_GetWindowRectEx(hItem, &Rect); //--------------��7��
//WM_SetWindowPos(hItem, Rect.x0,
//Rect.y0,
//Rect.x1 - Rect.x0,
//Rect.y1 - Rect.y0);
////return hWin;
//return 0;
//}


/*
*********************************************************************************************************
*	�� �� ��: GUI_Show_MYmsg1
*	����˵��: ��ʾ��Ϣ
*	��    �Σ�
*	�� �� ֵ: ��
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
////	WM_InvalidateWindow(TEMP_WIN);//ʹָ��������Ч��
//// //GUI_EndDialog(hwim, 0);
//////GUI_HWIN ATwin =WM_GetActiveWindow() ;
//////WM_SetFocus(TEMP_WIN);
//}

/*
*********************************************************************************************************
*	�� �� ��: _EnableSkinning
*	����˵��: 
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 
//static void _EnableSkinning(void) {
//FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
//BUTTON_SetDefaultSkin (BUTTON_SKIN_FLEX);
//}

/*
*********************************************************************************************************
*	�� �� ��: _cbBASEMIN
*	����˵��: ���洰�ڻص�����
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
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
*	�� �� ��: _cbBASEMIN
*	����˵��: ���洰�ڻص�����
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void pingyingtest(void) 
//{
//	int key=0, index=0;
//u8 inputstr[7]={'2','4','0'};		//�������6���ַ�+������
//	inputstr[2]='\0';//��ӽ�����
//	key=t9.getpymb(inputstr);	//�õ�ƥ��Ľ����
//	if(key){
//	printf("%s\r\n",t9.pymb[key-1]->pymb);
//	}
//	

//}

/*
*********************************************************************************************************
*	�� �� ��: KEEP_LCD_ON_FOR_A_WHILES
*	����˵��: 
*	��    �Σ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEEP_LCD_ON_FOR_A_WHILES(void){
	LCD_LED=1;
	stm32_flat.LCD_TURNOFF_TIME_COUNT=stm32_flat.LCD_TURNOFF_TIME;
}
/*
*********************************************************************************************************
*	�� �� ��: ResetLCDBKtime
*	����˵��: ����LCD����ʱ�� ��ʼֵ60s
*	��    �Σ�u32 times ,times ��λs������趨Ϊ0 ����
*	�� �� ֵ: ��
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
*	�� �� ��: emWinMainApp
*	����˵��: GUI������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void ucGUIMainApp(void)
{
	WM_InvalidateWindow(TOPWIN);//ʹָ��������Ч��//���ڻص�����״̬��
	//WM_InvalidateWindow(MIDWIN[0]);//ʹָ��������Ч��
	//GUI_Delay(10);
	GUI_Exec();

	
	
}
/*
*********************************************************************************************************
*	�� �� ��: ucGUIMainAppinit
*	����˵��: GUI�������ʼ�� 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ucGUIMainAppinit(void)
{

	//OS_ERR  err;
  
	//׼������3�����ڣ�������ʹ�õ����û���������������ڻص�������ʹ��
	midWinPara.xSizeLCD = LCD_GetXSize();				            //LCD��Ļ�ߴ�
	midWinPara.ySizeLCD = LCD_GetYSize();				            //LCD��Ļ�ߴ�
	midWinPara.xPosWin	= MIDWIN_xPos;							          //���ڵ���ʼλ��
	midWinPara.yPosWin  = MIDWIN_yPos;							          //���ڵ���ʼλ��
	midWinPara.xSizeWin = midWinPara.xSizeLCD;						      //���ڳߴ�
	midWinPara.ySizeWin = midWinPara.ySizeLCD-midWinPara.yPosWin;	//���ڳߴ�
	
		/************************����Ĭ�ϲ�������*****************************************/
  	LISTBOX_SetDefaultBkColor(LISTBOX_CI_UNSEL,GUI_BLACK);	   //�����б��δ��ѡ�еı�����ɫ LISTBOX_CI_UNSEL δѡԪ��
		LISTBOX_SetDefaultBkColor(LISTBOX_CI_SEL,GUI_BLACK);	   //�����б��ѡ�еı�����ɫ LISTBOX_CI_SEL ѡ��Ԫ�أ���������.
		LISTBOX_SetDefaultBkColor(LISTBOX_CI_SELFOCUS,GUI_LIGHTGRAY);	   //�����б��ѡ�еı�����ɫ LISTBOX_CI_SELFOCUS ѡ��Ԫ�أ������㡣	
		LISTBOX_SetDefaultFont(&GUI_FontHZ12);       					 //�����б���� Ĭ���ı�����
		LISTBOX_SetDefaultTextAlign(GUI_TA_LEFT|GUI_TA_VCENTER);		//�����´����� LISTBOX �ؼ���ʹ�õ�Ĭ���ı����뷽ʽ 
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_UNSEL,GUI_LIGHTGRAY);	   //�����´����� LISTBOX �ؼ���ʹ�õ�Ĭ���ı���ɫ LISTBOX_CI_UNSEL δѡԪ�ء�
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_SEL,GUI_RED);	   //�����б��ѡ�е��ı���ɫ LISTBOX_CI_SEL ѡ��Ԫ�أ��������㡣
		LISTBOX_SetDefaultTextColor(LISTBOX_CI_SELFOCUS,GUI_BLACK);	   //�����б��ѡ�е��ı���ɫ LISTBOX_CI_SELFOCUS ѡ��Ԫ�أ������㡣
			
		/*************************************************************************/
	
	
	////�趨��ʼ��ֵ
	sms_show.mem=1;//1:SM 2:ME
	sms_show.index=0;
	
//	WM_SetCallback(WM_HBKWIN, _cbBASEMIN);/* �������洰�ڵĻص����� */


	CreatTopWindow();
	CreateMidWin1();
	CreateMidWin0();



  GUI_Delay(20);



}

