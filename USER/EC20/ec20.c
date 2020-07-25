//#include "stm32f4xx.h"
//#include "./BSP/usart/bsp_linkec20_usart.h"
#include  <includes.h>
#include "ff.h"   
#include <string.h>
#include "stdio.h"
#include "ec20.h"



/***********变量************/
extern GUI_HWIN TOPWIN;//顶部状态栏
extern GUI_HWIN MIDWIN[2];
extern type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//当前接收的完整消息
//extern uint8_t LINKEC20_USART_RECEIVE_BUF[LINKEC20_USART_RECEIVE_BUF_SIZE];//LINKEC20串口DMA接收缓冲区

//多值信号量
extern OS_SEM SemOfRec_EC20_OK; //多值信号量 EC20:OK
extern OS_SEM SemOfATV; 
extern OS_SEM SemOfCMEE; 
extern OS_SEM SemOfGSN; 
extern OS_SEM SemOfCPIN; 
extern OS_SEM SemOfCSQ;	 //+CSQ: 18,99 OK
extern OS_SEM SemOfCREG; //	+CREG: 0,1 OK
extern OS_SEM SemOfCGREG; //+CGREG: 0,1 OK
extern OS_SEM SemOfCOPS;	// +COPS: 0,0,"CHN-UNICOM",7 OK
extern OS_SEM SemOfCMGF;
extern OS_SEM SemOfCPMSS;//
extern OS_SEM SemOfCPMSG;//
extern OS_SEM SemOfCMGD;//delete sms
extern OS_SEM SemOfCMGDINDEX;//UPDATA sms INDEX
extern OS_SEM SemOfCMGS;//SENT SMS
extern OS_SEM SemOfCMGSOK;//sent sms
extern OS_SEM SemOfQNTP;//NTP TIME
extern OS_SEM SemOfCSCS;//AT+CSCS
extern OS_SEM SemOfCFUN;//飞行模式 全功能模式
extern OS_SEM SemOfQPOWD;//
extern OS_SEM SemOfRDY;//

extern OS_FLAG_GRP FlagGropOf_CMGR;     //声明事件标志组CMGR
//extern OS_SEM SemOfCMGR1;//AT+CMGR=0//读取短信头
//extern OS_SEM SemOfCMGR2;//+CMGR: //读取短信详情
//extern OS_PEND_DATA SEMOfCMGR_Arr;
extern OS_MUTEX mutex_ec20sent;      //声明 串口发送互斥信号量
NET_FLAGS net_flags;

volatile int rssi;//信号强度变量
volatile int ber;//误码率
/*
网络类型标识变量
0 GSM
2 UTRAN
3 GSM W/EGPRS
4 UTRAN W/HSDPA
5 UTRAN W/HSUPA
6 UTRAN W/HSDPA and HSUPA
7 E-UTRAN
100 CDMA
*/
Operation_Bus Operation;//储存运营商标识字符变量
Message_BOX_COUNT SMSBOX;//短消息信箱计数器2 供全局变量使用
TEMP_Message_Storage tmp_sms[1];////临时存储信息，供显示使用

extern OS_MEM INTERNAL_MEM; 
/*********************************************************************************************************
*	函 数 名: EC20_Power_Off
*	功能说明: 
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
u8 EC20_Power_Off(void)
{

	OS_ERR err;
	
	
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+QPOWD=1\r\n");
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfQPOWD,15000,OS_OPT_PEND_BLOCKING,0,&err);  //最长等待15s
		if(err==OS_ERR_NONE){
			EC20_PW_OFF;
			if(EC20_PW_GOIP_CHECK == 1 )  {//高电平代表已关机
					
			net_flags.EC20_INIT=0;
			net_flags.IS_EC20_OFF=1;
			}else{
			net_flags.IS_EC20_OFF=0;
			}
			
			
		}else{
		net_flags.IS_EC20_OFF=0;
		}
	
	GUI_SetBkColor(GUI_BLACK);
	GUI_ClearRect(TopWindow_Signa_xPos,TopWindow_Signa_yPos,TopWindow_SignaWord_xPos+16,TOPWIN_Hight);//清除原来的图标
	UpdataMidWin0();
	return net_flags.IS_EC20_OFF;
    
}
/*********************************************************************************************************
*	函 数 名: EC20_Power_on
*	功能说明: 
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
u8 EC20_Power_on(void)
{
	OS_ERR err;
	
if(!net_flags.EC20_IS_TURNNING_ON){
	net_flags.EC20_IS_TURNNING_ON=1;
	
				EC20_PW_ON;
		
			if(EC20_PW_GOIP_CHECK == 0 )  {//低电平代表已开机
			
				//等待开机完成
				OSSemPend (&SemOfRDY,15000,OS_OPT_PEND_BLOCKING,0,&err);  //最长等待15s
				if(err==OS_ERR_NONE){
					net_flags.IS_EC20_OFF=0;
					net_flags.EC20_INIT=0;
				}else{
					if(EC20_ATOK()){
					net_flags.IS_EC20_OFF=0;
					}else{
						net_flags.IS_EC20_OFF=1;
						net_flags.EC20_INIT=0;
					}
				
				
				}

			}else{
			net_flags.IS_EC20_OFF=1;
			net_flags.EC20_INIT=0;
			}
			
			GUI_SetBkColor(GUI_BLACK);
			GUI_ClearRect(TopWindow_Signa_xPos,TopWindow_Signa_yPos,TopWindow_SignaWord_xPos+16,TOPWIN_Hight);//清除原来的图标
			UpdataMidWin0();	
		
}
		

return !net_flags.IS_EC20_OFF;	


}

/*********************************************************************************************************
*	函 数 名: EC20_ARIMODE_CHECK
*	功能说明: AT+CFUN=0 ：最少功能模式（关闭 RF 和(U)SIM 卡）。
						AT+CFUN=1 ：全功能模式（默认）。
						AT+CFUN=4 ：关闭 RF 功能（飞行模式）。
*	形    参：
*	返 回 值: 0 AIRMODE OFF,1 AIRMODE ON
*********************************************************************************************************
*/
u8 EC20_ARIMODE_CHECK(void)
{
if(net_flags.EC20_INIT){//要先初始化EC20 才能进行下一步操作
		int mode =-1;
		OS_ERR err;
		CPU_SR_ALLOC(); 
		OS_CRITICAL_ENTER();  
		EC20_SendCMD("AT+CFUN?\r\n");
		OS_CRITICAL_EXIT(); 
			OSSemPend (&SemOfCFUN,1500,OS_OPT_PEND_BLOCKING,0,&err);  
		
		if(err==OS_ERR_NONE){
				u8 sign=0;
				 sign=	sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+CFUN: "),"+CFUN: %d\r\n",&mode);
				if(sign==1){
						if(mode==1){//如果 飞行模式关闭状态
						net_flags.EC20_AIR_MODE=0;
						}else if(mode==4){
						net_flags.EC20_AIR_MODE=1;
						
						}else{
						net_flags.EC20_AIR_MODE=1;
						}

					}
		
		}

}

return net_flags.EC20_AIR_MODE;
}
/*********************************************************************************************************
*	函 数 名: EC20_AIR_Mode_Off
*	功能说明: AT+CFUN=0 ：最少功能模式（关闭 RF 和(U)SIM 卡）。
						AT+CFUN=1 ：全功能模式（默认）。
						AT+CFUN=4 ：关闭 RF 功能（飞行模式）。
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
u8 EC20_AIR_Mode_Off(void)
{
	if(net_flags.EC20_INIT){//要先初始化EC20 才能进行下一步操作
			u8 AIR_Mode =EC20_ARIMODE_CHECK();
			OS_ERR err;
			
				if(AIR_Mode==0){//如果 EC20未开启飞行模式，则跳过
				net_flags.EC20_AIR_MODE=0;
				
					}else{//如果 非EC20飞行模式关闭状态,则设定 全功能模式
						CPU_SR_ALLOC(); 
						OS_CRITICAL_ENTER();  
						EC20_SendCMD("AT+CFUN=1\r\n");
						OS_CRITICAL_EXIT(); 	
						OSSemPend (&SemOfCFUN,3500,OS_OPT_PEND_BLOCKING,0,&err);  
						if(err==OS_ERR_NONE){
							GUI_SetBkColor(GUI_BLACK);
							GUI_ClearRect(TopWindow_Signa_xPos,TopWindow_Signa_yPos,TopWindow_SignaWord_xPos+16,TOPWIN_Hight);//清除原来的图标
							
							net_flags.EC20_AIR_MODE=0;

						
						}else{
						net_flags.EC20_AIR_MODE=1;
						
						}
				}
	}

	

return !net_flags.EC20_AIR_MODE;
	
}

/*********************************************************************************************************
*	函 数 名: EC20_AIR_Mode_On
*	功能说明: AT+CFUN=0 ：最少功能模式（关闭 RF 和(U)SIM 卡）。
						AT+CFUN=1 ：全功能模式（默认）。
						AT+CFUN=4 ：关闭 RF 功能（飞行模式）。
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
u8 EC20_AIR_Mode_On(void)
{
	OS_ERR err;
	if(net_flags.EC20_INIT){//要先初始化EC20 才能进行下一步操作
			u8 AIR_Mode =EC20_ARIMODE_CHECK();
			
				if(AIR_Mode==1){//EC20已开启飞行模式
					GUI_SetBkColor(GUI_BLACK);
					GUI_ClearRect(TopWindow_Signa_xPos,TopWindow_Signa_yPos,TopWindow_SignaWord_xPos+16,TOPWIN_Hight);//清除原来的图标
					if(net_flags.EC20_AIR_MODE==0){//如果EC20已开启飞行模式但STM32显示未开启飞行模式
						net_flags.Net_Reg_INIT=0;
						net_flags.EC20_AIR_MODE=1;
					}
				}else{//EC20未开启飞行模式
						CPU_SR_ALLOC(); 
						OS_CRITICAL_ENTER();  
						EC20_SendCMD("AT+CFUN=4\r\n");
						OS_CRITICAL_EXIT(); 
							OSSemPend (&SemOfCFUN,3500,OS_OPT_PEND_BLOCKING,0,&err);  
							if(err==OS_ERR_NONE){
								GUI_SetBkColor(GUI_BLACK);
								GUI_ClearRect(TopWindow_Signa_xPos,TopWindow_Signa_yPos,TopWindow_SignaWord_xPos+16,TOPWIN_Hight);//清除原来的图标
								net_flags.Net_Reg_INIT=0;
								net_flags.EC20_AIR_MODE=1;
							
							}else{
							net_flags.EC20_AIR_MODE=0;
							}
					
				}
			}
return net_flags.EC20_AIR_MODE;
}



/*********************************************************************************************************
*	函 数 名: EC20_ATOK
*	功能说明: 检查EC20模块是否开启
*	形    参：
*	返 回 值: 0：未开启 1：正常
*********************************************************************************************************
*/
u8 EC20_ATOK(void)
{
	net_flags.EC20_ATOK=0;
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT\r\n");
	OS_CRITICAL_EXIT(); 
	//OSTimeDly(800,OS_OPT_TIME_DLY,&err);
  		OSSemPend ((OS_SEM   *)&SemOfRec_EC20_OK,             //等待该信号量被发布
								 (OS_TICK   )800,                     //等待300时钟节拍
								 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果没有信号量可用 等待 OS_OPT_PEND_NON_BLOCKING
								 (CPU_TS   *)0,          //获取信号量最后一次被发布的时间戳
								 (OS_ERR   *)&err);                 //返回错误类型
	

	if(OS_ERR_NONE==err){
		//printf("ec20ATOK:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
		net_flags.EC20_ATOK=1;
		return 1;
		}else{
			net_flags.EC20_ATOK=0;
			OS_CRITICAL_ENTER();      //进入临界段，不希望下面串口打印遭到中断
			printf("AT ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		return 0;
		}

}
/*********************************************************************************************************
*	函 数 名: EC20_ATV1_Check
*	功能说明: Use ATV1 to set the response format
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_ATV1_Check(void)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("ATV1\r\n");///* Use ATV1 to set the response format */ATV1回复OK,ATV0 回复0
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfATV,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	
	if(OS_ERR_NONE==err){
		net_flags.EC20_ATV1_SET=1;
		return 1;
		}else{
			net_flags.EC20_ATV1_SET=0;
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("ATV1 ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		return 0;
		}

}

/*********************************************************************************************************
*	函 数 名: EC20_ECHO_ON
*	功能说明: 
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_ECHO_ON(void)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("ATE1\r\n");//
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfRec_EC20_OK,500,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	
	if(OS_ERR_NONE==err){
		net_flags.EC20_ATE1=1;
		return 1;
		}else{
			net_flags.EC20_ATE1=0;
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("ATE1 ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		return 0;
		}

}

/*********************************************************************************************************
*	函 数 名: EC20_CMEE_Check
*	功能说明: Error Message Format
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_CMEE_Check(void)
{
	net_flags.EC20_CMEE=0;
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+CMEE=2\r\n");///* Error Message Format*/0：输出ERROR,1：输出ERROR+CODE  2:输出详细错误信息
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCMEE,800,OS_OPT_PEND_BLOCKING,0,&err); //等待300时钟节拍   
	//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
	if(OS_ERR_NONE==err){
		net_flags.EC20_CMEE=1;
		
		}else{
			OS_CRITICAL_ENTER();    //进入临界段，不希望下面串口打印遭到中断
			printf("AT+CMEE ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		net_flags.EC20_CMEE=0;
		
		}
return net_flags.EC20_CMEE;
}
/*********************************************************************************************************
*	函 数 名: EC20_GSN_Check
*	功能说明: Use AT+GSN to query the IMEI of module
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_GSN_Check(void)
{
	net_flags.EC20_IMEI=0;
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+GSN\r\n");/* Use AT+GSN to query the IMEI of module */
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfGSN,800,OS_OPT_PEND_BLOCKING,0,&err); //等待500时钟节拍 
	
	//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
	if(OS_ERR_NONE==err){
		net_flags.EC20_IMEI=1;
		
		}else{
			net_flags.EC20_IMEI=0;
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("AT+GSN ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		 
		}
return net_flags.EC20_IMEI;
}
/*********************************************************************************************************
*	函 数 名: EC20_GSN_Check
*	功能说明: Use AT+CPIN? to query the SIM card status : SIM card inserted or not, locked or unlocked
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_CPIN_Check(void)
{
	net_flags.EC20_SIM_READY=0;
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+CPIN?\r\n");/* Use AT+CPIN? to query the SIM card status : SIM card inserted or not, locked or unlocked */
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCPIN,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍  
	

	if(OS_ERR_NONE==err){
			char *temp;
		
			temp = strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"READY");// SIM PIN
					if(temp){
						net_flags.EC20_SIM_READY=1;
						return net_flags.EC20_SIM_READY;
					}else if(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"SIM PIN")){
									OS_CRITICAL_ENTER();  
									EC20_SendCMD("AT+CPIN=4797444\r\n");/* Use AT+CPIN? to query the SIM card status : SIM card inserted or not, locked or unlocked */
									OS_CRITICAL_EXIT(); 
									OSSemPend (&SemOfCPIN,3000,OS_OPT_PEND_BLOCKING,0,&err); //等待1500时钟节拍  
									if(OS_ERR_NONE==err){
											OSTimeDly(500,OS_OPT_TIME_DLY,&err);//等待一段时间
											net_flags.EC20_SIM_READY=1;
											
									}else{
											net_flags.EC20_SIM_READY=0;
											OS_CRITICAL_ENTER();    //进入临界段，不希望下面串口打印遭到中断
											printf("EC20RE:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//输出错误信息
											printf("AT+CPIN= ERR:%d\n",err);//输出错误信息
											OS_CRITICAL_EXIT(); 
										}

					
						}else{
						net_flags.EC20_SIM_READY=0;

						}
		
		
		}else{
			OS_CRITICAL_ENTER();    //进入临界段，不希望下面串口打印遭到中断
			printf("AT+CPIN? ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
			net_flags.EC20_SIM_READY=0;
		}

return net_flags.EC20_SIM_READY;
}



/*********************************************************************************************************
*	函 数 名: EC20_Get_Csq
*	功能说明: Use AT+CSQ to query current signal quality 测信号强度 +CSQ: <rssi>,<ber>
						<rssi> 0 -113dBm or less
						1 -111dBm
						2...30 -109dBm...-53dBm
						31 -51dBm or greater
						99 Not known or not detectable
						100 -116dBm or less
						101 -115dBm
						102...190 -114dBm…-26dBm
						191 -25dBm or greater
						199 Not known or not detectable
						100~199 Extended to be used in TD-SCDMA indicating received signal code
						power (RSCP)
						<ber> Channel bit error rate (in percent)
						0...7 As RXQUAL values in the table in 3GPP TS 45.008 subclause 8.2.4
						99 Not known or not detectable
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Get_Csq(void)
{
	OS_ERR    err;
	net_flags.EC20_CSQ=0;

	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+CSQ\r\n");//
	OS_CRITICAL_EXIT(); 

	
	OSSemPend (&SemOfCSQ,1000,OS_OPT_PEND_BLOCKING,0,&err); //等待500时钟节拍                 //返回错误类型
	

	if(err==OS_ERR_NONE){
			uint8_t sign = sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+CSQ: "),"+CSQ: %d,%d",&rssi,&ber);
				if(sign ==2){
					net_flags.EC20_CSQ=1;
						}

		}else{
			net_flags.EC20_CSQ=0;
			rssi=ber=-1;		
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CSQ ERR:%d\nCSQec20:%s\n",err,LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 

		}

return net_flags.EC20_CSQ;
}

/*********************************************************************************************************
*	函 数 名: EC20_Get_CREG
*	功能说明: The Read Command returns the network registration status. 
						The Write Command sets whether or not to present URC.
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Get_CREG(void)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+CREG?\r\n");//
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCREG,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	

	if(OS_ERR_NONE==err){
		net_flags.EC20_CREG=1;
		return 1;
		}else{
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CREG ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		net_flags.EC20_CREG=0;
		return 0;
		}
//return 0;
}
/*********************************************************************************************************
*	函 数 名: EC20_Get_CGREG
*	功能说明: The command queries the network registration status and controls the presentation of an unsolicited
						result code +CGREG: <stat> when <n>=1 and there is a change in the MT?s GPRS network registration
						status in GERAN/UTRAN, or unsolicited result code +CGREG: <stat>[,[<lac>],[<ci>],[<Act>],[<rac>]]
						when <n>=2 and there is a change of the network cell in GERAN/UTRAN.
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Get_CGREG(void)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+CGREG?\r\n");///
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCGREG,800,OS_OPT_PEND_BLOCKING,0,&err); //等待500时钟节拍                 //返回错误类型
	
	if(OS_ERR_NONE==err){
		net_flags.EC20_CREG=1;
		return 1;
		}else{
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CGREG ERR:%d\n",err);//输出错误信息
			printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		net_flags.EC20_CREG=0;
		return 0;
		}
//return 0;
}
/*********************************************************************************************************
*	函 数 名: EC20_Get_COPS
*	功能说明: The command returns the current operators and their status, and allows setting automatic or manual network selection.
///////////////////////////////
					TA returns the current mode and the currently selected
						operator. If no operator is selected, <format>, <oper> and
						<Act> are omitted.
						+COPS: <mode>[,<format>[,<oper>][,<Act>]]
						OK
						If there is any error related to ME functionality:
						+CME ERROR: <err>
//////////////////////////////
						<stat> 0 Unknown
						1 Operator available
						2 Current operator
						3 Operator forbidden
						<oper> Operator in format as per <mode>
						<mode> 0 Automatic mode. <oper> field is ignored
						1 Manual operator selection. <oper> field shall be present and <Act> optionally
						2 Manually deregister from network
						3 Set only <format> (for AT+COPS? Read Command), and do not attempt
						registration/deregistration (<oper> and <Act> fields are ignored). This value is
						invalid in the response of Read Command.
						4 Manual/automatic selection. <oper> field shall be presented. If manual selection
						fails, automatic mode (<mode>=0) is entered
						<format> 0 Long format alphanumeric <oper> which can be up to 16 characters long
						1 Short format alphanumeric <oper>
						2 Numeric <oper>. GSM location area identification number
						<Act> Access technology selected. Values 3, 4, 5, 6 occur only in the response of Read
						Command while MS is in data service state and is not intended for the AT+COPS Write
						Command.
						0 GSM
						2 UTRAN
						3 GSM W/EGPRS
						4 UTRAN W/HSDPA
						5 UTRAN W/HSUPA
						6 UTRAN W/HSDPA and HSUPA
						7 E-UTRAN
						100 CDMA
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Get_COPS(void)
{
	OS_ERR    err;
	Operation.sign=0;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER();  
	EC20_SendCMD("AT+COPS?\r\n");///*
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCOPS,1000,OS_OPT_PEND_BLOCKING,0,&err); //等待500时钟节拍                 //返回错误类型
	

	if(OS_ERR_NONE==err){
		//获取运营商信息
		
		uint8_t sign = sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+COPS: "),"+COPS: %*d,%*d,\"%[^\"]\",%d",Operation.COPS,&Operation.Act);//"+COPS: %*d,%*d,\"%[^\"],%d\r\n\r\nOK"
		
		if(sign==2){
			Operation.sign=1;
			//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			//printf("2COPS:%s\n2ACT:%d\n",Operation.COPS,Operation.Act);//debug_mark

		}else{
			Operation.sign=0;
			Operation.Act=11;//设定网络制式为未知。
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("1cpos ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			//printf("1COPS:%s\n1ACT:%d\n",Operation.COPS,Operation.Act);//debug_mark
			OS_CRITICAL_EXIT(); 
		}
		
		
		return 1;
		}else{
			Operation.sign=0;
			Operation.Act=11;//设定网络制式为未知。
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			//printf("COPS ERR:%d\n",err);//输出错误信息
			printf("COPS ERR:%d\n1cpos ec20:%s\n",err,LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//输出错误信息
			OS_CRITICAL_EXIT(); 
		return 0;
		}
//return 0;
}
/*********************************************************************************************************
*	函 数 名: EC20_Net_Reg_INIT
*	功能说明: 初始化注册网络，并获取信息
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/

u8 EC20_Net_Reg_INIT(void){
//	OS_ERR osTimeErr;
	//OSTimeDly(300,OS_OPT_TIME_DLY,&osTimeErr);
	
if(!net_flags.EC20_AIR_MODE){
	if(net_flags.EC20_INIT==1){
				if(net_flags.Net_Reg_INIT==0){
					//OSTimeDly(300,OS_OPT_TIME_DLY,&osTimeErr);
					Operation.Act=11;//设定网络制式为未知。
					if(EC20_CPIN_Check()){			
						
						if(EC20_Get_Csq()){
							
							if(EC20_Get_CREG()){	
								
								if(EC20_Get_COPS()){
									net_flags.Net_Reg_INIT=1;
									return 1;						
									}
							}
						}
					}else{net_flags.Net_Reg_INIT=0;return 0;}	
		}	
	}else if(net_flags.Net_Reg_INIT==1){return 1;
		}else{
		net_flags.Net_Reg_INIT=0;	
		return 0;
		}
}

net_flags.Net_Reg_INIT=0;	
return 0;

}

/*********************************************************************************************************
*	函 数 名: EC20_INIT
*	功能说明: 初始化EC20各项设定
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/

u8 EC20_INIT(void){
	
net_flags.EC20_INIT=0;
	
	if(EC20_PW_GOIP_CHECK == 0 )  {//低电平代表已开机
		net_flags.IS_EC20_OFF=0;
		
							if(net_flags.EC20_INIT==0){
						//OSTimeDly(100,OS_OPT_TIME_DLY,&osTimeErr);
							if(EC20_ATOK()){
								if(EC20_ECHO_ON()){
									if(EC20_ATV1_Check()){
										if(EC20_CMEE_Check()){
											if(EC20_GSN_Check()){
												if(EC20_CPIN_Check()){
													if(EC20_SET_CSCS(3)){
														net_flags.EC20_INIT=1;
														UpdataMidWin0();
														}
															

														
													}
											}
										}
									}			
								}
							}
					}else if(net_flags.EC20_INIT==1){
					}else{	net_flags.EC20_INIT=0;
					}
			//初始化时检查EC20是否开启飞行模式		
				
			EC20_ARIMODE_CHECK();	
					

	
	
	}else{//ec20 未开启
	net_flags.EC20_AIR_MODE=1;
	net_flags.EC20_INIT=0;
	net_flags.IS_EC20_OFF=1;
	}


return net_flags.EC20_INIT;	

}

  

/*********************************************************************************************************
*	函 数 名: EC20_chr2hex
*	功能说明: 将1个字符转换为16进制数字  chr:字符,0~9/A~F/a~F
*	形    参：
*	返 回 值: chr对应的16进制数值
*********************************************************************************************************
*/

u8 EC20_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
/*********************************************************************************************************
*	函 数 名: EC20_hex2chr
*	功能说明: //将1个16进制数字转换为字符 hex:16进制数字,0~15;
*	形    参：
*	返 回 值: 字符
*********************************************************************************************************
*/
u8 EC20_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
/*********************************************************************************************************
*	函 数 名: EC20_unigbk_exchange
*	功能说明: unicode gbk 转换函数
*	形    参：src:输入字符串
						dst:输出(uni2gbk时为gbk内码,gbk2uni时,为unicode字符串)
						mode:0,unicode到gbk转换;
								1,gbk到unicode转换;
*	返 回 值: 无
*********************************************************************************************************
*/

void EC20_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
	u16 temp; 
	u8 buf[2];
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//非汉字
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//汉字,占2个字节
			{
				buf[1]=*src++;
				buf[0]=*src++; 
				temp=(u16)ff_convert((WCHAR)*(u16*)buf,1); 
			}
			*dst++=EC20_hex2chr((temp>>12)&0X0F);
			*dst++=EC20_hex2chr((temp>>8)&0X0F);
			*dst++=EC20_hex2chr((temp>>4)&0X0F);
			*dst++=EC20_hex2chr(temp&0X0F);
		}
	}else	//unicode 2 gbk
	{ 
		while(*src!=0)
		{
			buf[1]=EC20_chr2hex(*src++)*16;
			buf[1]+=EC20_chr2hex(*src++);
			buf[0]=EC20_chr2hex(*src++)*16;
			buf[0]+=EC20_chr2hex(*src++);
 			temp=(u16)ff_convert((WCHAR)*(u16*)buf,0);
			if(temp<0X80){*dst=temp;dst++;}
			else {*(u16*)dst=swap16(temp);dst+=2;}
		} 
	}
	*dst=0;//添加结束符
}


/*********************************************************************************************************
*	函 数 名: EC20_SET_CMGF1
*	功能说明: Message Format
						AT+CMGF[=<mode>] <mode> 0 PDU   mode 1 Text mode
*	形    参：1:MODE 0 ,2:MODE 1
*	返 回 值: 0:fail 1:success
*********************************************************************************************************
*/
u8 EC20_SET_CMGF(u8 mode)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER(); 
	switch(mode){
		case 1:
			EC20_SendCMD("AT+CMGF=0\r\n");//
			break;
		case 2:
			EC20_SendCMD("AT+CMGF=1\r\n");//
			break;
		default:
			break;
	
	}	
	
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCMGF,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	

	if(OS_ERR_NONE==err){
		net_flags.EC20_CMGF=mode;
		return 1;
		}else{
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CMGF ERR:%d\n",err);//输出错误信息
			//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		net_flags.EC20_CMGF=0;//mode,只能是1或2模式，设定为0则为出错。
		return 0;
		}
//return 0;
}

/*********************************************************************************************************
*	函 数 名: EC20_SET_CPMS
*	功能说明: Preferred Message Storage
						The command selects the memory storages <mem1>, <mem2> and <mem3> to be used for reading, writing, etc.
*	形    参：1:<mem1:SM>, <mem2:SM> and <mem3:ME>
						2:<mem1:ME>, <mem2:SM> and <mem3:ME>
						3:<mem1:SM>, <mem2:SM> and <mem3:SM>
*	返 回 值: 	0:fail 1:success
*********************************************************************************************************
*/
u8 EC20_SET_CPMS(u8 mode)
{
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER(); 
	switch(mode){
		case 1:
			EC20_SendCMD("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");//
			break;
		case 2:
			EC20_SendCMD("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n");//
			break;
		case 3:
			EC20_SendCMD("AT+CPMS=\"SM\",\"SM\",\"ME\"\r\n");//
			break;
		default:
			break;
	
	}	
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCPMSS,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	

	if(OS_ERR_NONE==err){
			//提取消息信箱信息
			Preferred_Message_Storage NEW_PMES;//短消息信箱信息
			uint8_t sign = sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+CPMS: "),"+CPMS: %d,%d,%d,%d,%d,%d\r\n\r\nOK\r\n",
				&NEW_PMES.used1,&NEW_PMES.total1,&NEW_PMES.used2,&NEW_PMES.total2,&NEW_PMES.used3,&NEW_PMES.total3);//
			
					if(sign==6){
						switch(mode){
							case 1:
								//SMSBOX.new_SMsms_count = NEW_PMES.used1-SMSBOX.SM_used;
//								SMSBOX.new_MEsms_count = NEW_PMES.used3-SMSBOX.new_MEsms_count;
								SMSBOX.SELETED_used= SMSBOX.SM_used=NEW_PMES.used1;
								SMSBOX.SELETED_total= SMSBOX.SM_total=NEW_PMES.total1;
//								SMSBOX.ME_used=NEW_PMES.used3;
//								SMSBOX.ME_total=NEW_PMES.total3;
								break;
							case 2:
//								SMSBOX.new_SMsms_count = NEW_PMES.used2-SMSBOX.SM_used;
								//SMSBOX.new_MEsms_count = NEW_PMES.used1-SMSBOX.new_MEsms_count;
//								SMSBOX.SM_used=NEW_PMES.used2;
//								SMSBOX.SM_total=NEW_PMES.total2;
								SMSBOX.SELETED_used= SMSBOX.ME_used=NEW_PMES.used1;
								SMSBOX.SELETED_total= SMSBOX.ME_total=NEW_PMES.total1;
								break;
							case 3:
								//SMSBOX.new_SMsms_count = NEW_PMES.used1-SMSBOX.SM_used;
								//SMSBOX.new_MEsms_count = NEW_PMES.used3-SMSBOX.new_MEsms_count;
								SMSBOX.SELETED_used= SMSBOX.SM_used=NEW_PMES.used1;
								SMSBOX.SELETED_total= SMSBOX.SM_total=NEW_PMES.total1;
								SMSBOX.ME_used=NEW_PMES.used3;
								SMSBOX.ME_total=NEW_PMES.total3;
								break;
							default:
								break;
						
						}
					net_flags.EC20_CPMS=mode;
					return 1;
					}else{
					net_flags.EC20_CPMS=0;
					return 0;}
		

		
		
		}else{
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CPMS_SET ERR:%d\n",err);//输出错误信息
			//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			OS_CRITICAL_EXIT(); 
		net_flags.EC20_CPMS=0;//mode,只能是1`2`3模式，设定为0则为出错。
		return 0;
		}
//return 0;
}

/*********************************************************************************************************
*	函 数 名: EC20_GET_CPMS
*	功能说明: Preferred Message Storage
						The command selects the memory storages <mem1>, <mem2> and <mem3> to be used for reading, writing, etc.
*	形    参：
*	返 回 值: 	0:fail 1:success
*********************************************************************************************************

u8 EC20_GET_CPMS(void)
{
		if(net_flags.EC20_CPMS!=0){
			OS_ERR    err;
			CPU_SR_ALLOC(); 
			OS_CRITICAL_ENTER(); 
			EC20_SendCMD("AT+CPMS?\r\n");//
			OS_CRITICAL_EXIT(); 
			OSSemPend (&SemOfCPMSG,500,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
			

			if(OS_ERR_NONE==err){
			//提取消息信箱信息
					Preferred_Message_Storage NEW_PMES;//短消息信箱信息
					uint8_t sign = sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+CPMS: "),"+CPMS: \"%*[^\"]\",%d,%d,\"%*[^\"]\",%d,%d,\"%*[^\"]\",%d,%d\r\n\r\nOK\r\n",
						&NEW_PMES.used1,&NEW_PMES.total1,&NEW_PMES.used2,&NEW_PMES.total2,&NEW_PMES.used3,&NEW_PMES.total3);//
					
					if(sign==6){
						switch(net_flags.EC20_CPMS){
							case 1:
								SMSBOX.new_SMsms_count = NEW_PMES.used1-SMSBOX.SM_used;
//								SMSBOX.new_MEsms_count = NEW_PMES.used3-SMSBOX.new_MEsms_count;
								SMSBOX.SM_used=NEW_PMES.used1;
								SMSBOX.SM_total=NEW_PMES.total1;
//								SMSBOX.ME_used=NEW_PMES.used3;
//								SMSBOX.ME_total=NEW_PMES.total3;
								break;
							case 2:
//								SMSBOX.new_SMsms_count = NEW_PMES.used2-SMSBOX.SM_used;
								SMSBOX.new_MEsms_count = NEW_PMES.used1-SMSBOX.new_MEsms_count;
//								SMSBOX.SM_used=NEW_PMES.used2;
//								SMSBOX.SM_total=NEW_PMES.total2;
								SMSBOX.ME_used=NEW_PMES.used1;
								SMSBOX.ME_total=NEW_PMES.total1;
								break;
							case 3:
								SMSBOX.new_SMsms_count = NEW_PMES.used1-SMSBOX.SM_used;
								SMSBOX.new_MEsms_count = NEW_PMES.used3-SMSBOX.new_MEsms_count;
								SMSBOX.SM_used=NEW_PMES.used1;
								SMSBOX.SM_total=NEW_PMES.total1;
								SMSBOX.ME_used=NEW_PMES.used3;
								SMSBOX.ME_total=NEW_PMES.total3;
								break;
							default:
								break;
						
						}
					return 1;
					}else{return 0;}
				
				
				
				}else{
					OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
					printf("CPMS_GET ERR:%d\n",err);//输出错误信息
					//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
					OS_CRITICAL_EXIT(); 
				
				return 0;
				}
		
		}

return 0;
}
*/




/*********************************************************************************************************
*	函 数 名: EC20_SET_CSCS
*	功能说明: Select TE Character Set
						
*	形    参：1:GSM ,2:IRA 3:UCS2
*	返 回 值: 0:fail 1:success
*********************************************************************************************************
*/
u8 EC20_SET_CSCS(u8 mode)
{
	
	
	OS_ERR    err;
	CPU_SR_ALLOC(); 
	OS_CRITICAL_ENTER(); 
	switch(mode){
		case 1:
			EC20_SendCMD("AT+CSCS=\"GSM\"\r\n");//
			break;
		case 2:
			EC20_SendCMD("AT+CSCS=\"IRA\"\r\n");//
			break;
		case 3:
			EC20_SendCMD("AT+CSCS=\"UCS2\"\r\n");//
			break;
		default:
			break;
	
	}	
	
	OS_CRITICAL_EXIT(); 
	OSSemPend (&SemOfCSCS,800,OS_OPT_PEND_BLOCKING,0,&err); //等待 300 时钟节拍                 //返回错误类型
	

	if(OS_ERR_NONE==err){
		
		return 1;
		}else{
			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("CSCS ERR:%d\n",err);//输出错误信息
			OS_CRITICAL_EXIT(); 
		return 0;
		}

}
/*********************************************************************************************************
*	函 数 名: EC20_Read1SMS
*	功能说明: 读取一条短消息
*	形    参：index:消息序列号
tmp_sms_idx:接收到的消息存放的位置 0：tmp_sms[0]  1:tmp_sms[1]
men:1 from SM  2:form ME

*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Read1SMS(u8 mem ,u16 index,uint8_t tmp_sms_idx){
	
	tmp_sms[tmp_sms_idx].alive=0;
	OS_ERR    err;
	//设定text mode
	if(net_flags.EC20_CMGF!=2){//如果非text mode
		if(!EC20_SET_CMGF(2))
		{
		return 0;//设定模式失败，返回0
		}
	}
	
	//设定首选信箱
if(net_flags.EC20_CPMS!=mem){

	if(!EC20_SET_CPMS(mem)){
		return 0;
	}
}
//开始读取信息
	char   * p_mem_blk;
	char *from;//来自
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	p_mem_blk = mymalloc(0,TEMP_Message_Storage_text_size);
	
	memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//清空数据

	sprintf(p_mem_blk,"AT+CMGR=%d\r\n",index);
	
	EC20_SendCMD(p_mem_blk);///*
	OS_CRITICAL_EXIT(); 


				 OSFlagPend ((OS_FLAG_GRP *)&FlagGropOf_CMGR,                  //等待标志组的的BIT0和BIT1均被置1 
                (OS_FLAGS     )( 0X01 | 0X02 ),
                (OS_TICK      )0,
                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL |
															OS_OPT_PEND_FLAG_CONSUME|
		                           OS_OPT_PEND_BLOCKING,
                (CPU_TS      *)800,
                (OS_ERR      *)&err);
		

			if(OS_ERR_NONE==err){
				
					OS_CRITICAL_ENTER();
					from = mymalloc(0,TEMP_Message_Storage_from_size);
				OS_CRITICAL_EXIT(); 
				memset(from,0,TEMP_Message_Storage_from_size*sizeof(uint8_t));//清空数据
					tmp_sms[tmp_sms_idx].mem=mem;
					tmp_sms[tmp_sms_idx].index=(u8)index;
					
					uint8_t sign=	sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+CMGR: "),"+CMGR: \"%[^\"]\",\"%[^\"]\",,\"%[^\"]\"\r\n%[^//r]",\
					tmp_sms[tmp_sms_idx].stat,from,tmp_sms[tmp_sms_idx].retime,p_mem_blk);
		
					if(sign==4){
						EC20_unigbk_exchange((u8 *)from,(u8*)tmp_sms[tmp_sms_idx].from,0);//mode:0,unicode到gbk转换;1,gbk到unicode转换;
						EC20_unigbk_exchange((u8 *)p_mem_blk,(u8*)tmp_sms[tmp_sms_idx].text,0);//mode:0,unicode到gbk转换;1,gbk到unicode转换;
						tmp_sms[tmp_sms_idx].alive=1;
						
				//			CPU_SR_ALLOC(); 
				//			OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
				//			printf("EC20_Read1SMS:%s\n",tmp_sms[tmp_sms_idx].text);//debug
				//			//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
				//			OS_CRITICAL_EXIT(); 
						
					}else{
					tmp_sms[tmp_sms_idx].alive=0;
					}
			
			

		

	
	}else{
			tmp_sms[tmp_sms_idx].alive=0;
			//CPU_SR_ALLOC(); 
			//OS_CRITICAL_ENTER();     //进入临界段，不希望下面串口打印遭到中断
			printf("EC20_Read1SMS1 ERR:%d\n",err);//输出错误信息
			//printf("ec20:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
			//OS_CRITICAL_EXIT(); 
			
	
	}

	OS_CRITICAL_ENTER();
	myfree(0,from);//退还内存块
	myfree(0,p_mem_blk);//退还内存块
	OS_CRITICAL_EXIT();

return tmp_sms[tmp_sms_idx].alive;
}
	
/*********************************************************************************************************
*	函 数 名: EC20_Delete_SMS
*	功能说明: 删除短消息 The command deletes short messages from the preferred message storage <mem1> location <index>. If
<delflag> is presented and not set to 0, then the ME shall ignore <index> and follow the rules of
<delflag> shown as below.
AT+CMGD=<index>[,<delflag>]
<index> Integer type value in the range of location numbers supported by the associated memory.
<delflag> 0 Delete the message specified in <index>
					1 Delete all read messages from <mem1> storage
					2 Delete all read messages from <mem1> storage and sent mobile originated messages
					3 Delete all read messages from <mem1> storage as well as all sent and unsent mobile originated messages
					4 Delete all messages from <mem1> storage

*	形    参：index:消息序列号
preferred message storage  men:1 from SM  2:form ME
delflag:

*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_Delete_SMS(u8 mem ,u8 delflag,u16 index){
	OS_ERR    err;

	//设定首选信箱
if(net_flags.EC20_CPMS!=mem){

	if(!EC20_SET_CPMS(mem)){
		return 0;
	}
}
//开始读取信息
	char *   p_mem_blk;

	
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	p_mem_blk = mymalloc(0,30);
	OS_CRITICAL_EXIT();
	memset(p_mem_blk,0,30*sizeof(uint8_t));//清空数据
	//char buffer[11+3]={0};
	switch(delflag){
		case 0:
			sprintf(p_mem_blk,"AT+CMGD=%d\r\n",index);//0 Delete the message specified in <index>
			break;
		case 1:
			sprintf(p_mem_blk,"AT+CMGD=1,1\r\n");//1 Delete all read messages from <mem1> storage
			break;
		case 2:
			sprintf(p_mem_blk,"AT+CMGD=1,2\r\n");//2 Delete all read messages from <mem1> storage and sent mobile originated messages
			break;
		case 3:
			sprintf(p_mem_blk,"AT+CMGD=1,3\r\n");//3 Delete all read messages from <mem1> storage as well as all sent and unsent mobile originated messages
			break;
		case 4:
			sprintf(p_mem_blk,"AT+CMGD=1,4\r\n");//4 Delete all messages from <mem1> storage
			break;
		
	}

	OS_CRITICAL_ENTER();  
	EC20_SendCMD(p_mem_blk);///*
	myfree(0,p_mem_blk);//退还内存块
	OS_CRITICAL_EXIT(); 
	
   
	OSSemPend (&SemOfCMGD,800,OS_OPT_PEND_BLOCKING,0,&err); //等待短消息详情，500时钟节拍 

			if(OS_ERR_NONE==err){
				
				//更新信箱消息数量
				EC20_UPDATA_SMSBOX_INDEX(mem);

					net_flags.EC20_Delete_SMS=1;
					return 1;
						
					}else{
					net_flags.EC20_Delete_SMS=0;
					return 0;
					}					
 
}
/*********************************************************************************************************
*	函 数 名: pick_up_index
*	功能说明: 提取消息字符串内的索引  AT+CMGD=? +CMGD: (0,1,2,4,5,6,7,8,9),(0-4)
*	形    参：*str 
						*num 存放结果的数组
*	返 回 值: 
*********************************************************************************************************
*/
u16 pick_up_index(const char *str, u8 *num){


	u8 i=0,k=0,zerosign=0;//k，括号内逗号的个数+1 等于数字的数目
	char strtemp[5];//最多解析3位数的字符
	//char num1[20];
	while((*str!='(')&&(*str!=0)){str++;}//跳过（前面的内容
	str++;
	
	while((*str!=')')&&(*str!=0)){
		zerosign++;
		
	if((*str!=',')){
	
		strtemp[i]=*str;
		i++;
		str++;
	}else{
		strtemp[++i]=0;
		*num=atoi(strtemp);
			k++;
			num++;
			str++;
			i=0;
	}
	
	
	}
	
	if(zerosign){//如果非空，处理最后一个字符
		strtemp[++i]=0;
		*num=atoi(strtemp);
		k++;
		++num;
	
	}

	
*num=0;//添加结束符
return k;
}

/*********************************************************************************************************
*	函 数 名: EC20_UPDATA_SMSBOX_INDEX
*	功能说明: 更新信箱消息索引
AT+CMGD=?

*	形    参：preferred message storage  men:1 from SM  2:form ME
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_UPDATA_SMSBOX_INDEX(u8 mem ){
	OS_ERR    err;

	//设定首选信箱
if(net_flags.EC20_CPMS!=mem){

	if(!EC20_SET_CPMS(mem)){
		return 0;
	}
}
//开始读取信息

	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();
	EC20_SendCMD("AT+CMGD=?\r\n");///*
	OS_CRITICAL_EXIT();
	

	OSSemPend (&SemOfCMGDINDEX,800,OS_OPT_PEND_BLOCKING,0,&err); //等待短消息详情，500时钟节拍 

			if(OS_ERR_NONE==err){
				
				
				
				u8 size =pick_up_index((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(u8 *)SMSBOX.INDEX);
				SMSBOX.SELETED_used=size;
				
//				for(int i=0;i<size;i++){
//				printf("num:%d\n",SMSBOX.SMSBOX_INDEX[i]);
//				
//				}
				
				

					net_flags.EC20_SMSBOX_INDEX=1;
					return 1;
						
					}else{
					net_flags.EC20_SMSBOX_INDEX=0;
					return 0;
					}					
 
   //myfree(0,num);//退还内存块
}
/*********************************************************************************************************
*	函 数 名: EC20_SENT_SMS
*	功能说明: 发送信息 AT+CSMP=17,167,0,8
AT+CSCS="UCS2"
AT+CMGS="10086"

*	形    参：* des 目标号码，const char* text 发送文本
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_SENT_SMS(const char* des,const char* text){
	OS_ERR    err;
	CPU_SR_ALLOC();
char *   p_mem_blk;
char *   des1;
	if(des[0]){
		
	OS_CRITICAL_ENTER();
	p_mem_blk = mymalloc(0,TEMP_Message_Storage_text_size);
	des1 = mymalloc(0,TEMP_Message_Storage_from_size);
	memset(p_mem_blk,0,TEMP_Message_Storage_text_size*sizeof(uint8_t));//清空数据
	memset(des1,0,TEMP_Message_Storage_from_size*sizeof(uint8_t));//清空数据
	EC20_unigbk_exchange((u8 *)des,(u8*)des1,1);//mode:0,unicode到gbk转换;1,gbk到unicode转换;
	sprintf(p_mem_blk,"AT+CMGS=\"%s\"\r\n",des1);
	EC20_SendCMD(p_mem_blk);///*
	OS_CRITICAL_EXIT();
	}

	

	OSSemPend (&SemOfCMGS,800,OS_OPT_PEND_BLOCKING,0,&err); //等待500时钟节拍 

			if(OS_ERR_NONE==err){
					OS_CRITICAL_ENTER();
					memset(p_mem_blk,0,400*sizeof(uint8_t));//清空数据
					EC20_unigbk_exchange((u8 *)text,(u8*)p_mem_blk,1);//mode:0,unicode到gbk转换;1,gbk到unicode转换;
					EC20_SendCMD(p_mem_blk);///*
					EC20_SendCMD("\x1A\r\n");///*
					OS_CRITICAL_EXIT();
				
				OSSemPend (&SemOfCMGSOK,2500,OS_OPT_PEND_BLOCKING,0,&err); //等待1500时钟节拍 
						if(OS_ERR_NONE==err){
							net_flags.EC20_CMGS=1;
			
						}else{
							net_flags.EC20_CMGS=0;

							}
								
					}else{
					net_flags.EC20_CMGS=0;
	
					}					
 
   myfree(0,p_mem_blk);//退还内存块
	 myfree(0,des1);//退还内存块
return net_flags.EC20_CMGS;
}
//根据日期计算周
u8 CaculateWeekDay(int y,int m, int d)
{
    if(m==1||m==2) {
        m+=12;
        y--;
    }
    u8 iWeek=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
  	return iWeek+1;
}


/*********************************************************************************************************
*	函 数 名: EC20_SyncLocalTime
*	功能说明:同步NTP网络时间
AT+QNTP=1,"114.118.7.161",123,1

	+QNTP: 0,"2018/10/19,12:55:34+00"
*	形    参：u8 updatalocaltime 0 只获取NTP时间 1,获取NTP时间并且更新到本地时间
*	返 回 值: 
*********************************************************************************************************
*/
u8 EC20_SyncLocalTime(u8 updatalocaltime){
	OS_ERR    err;
	int  hour,min,sec,ampm;
	int  year,month,date, week;
	char *   p_mem_blk;
	
	CPU_SR_ALLOC();	
	OS_CRITICAL_ENTER();
		switch(updatalocaltime){
			case 0:
				EC20_SendCMD("AT+QNTP=1,\"114.118.7.161\",123,0\r\n");//同步NTP并更新本地时间
				break;
			case 1:
				EC20_SendCMD("AT+QNTP=1,\"114.118.7.161\",123,1\r\n");//同步NTP并更新本地时间
				break;
		}
	
	OS_CRITICAL_EXIT();



	OSSemPend (&SemOfQNTP,1500,OS_OPT_PEND_BLOCKING,0,&err); //等待1500时钟节拍 

			if(OS_ERR_NONE==err){
					OS_CRITICAL_ENTER();
					p_mem_blk = mymalloc(0,300);
					memset(p_mem_blk,0,300*sizeof(uint8_t));//清空数据
					OS_CRITICAL_EXIT();
				//+QNTP: 0,"2018/10/19,12:55:34+00"
					uint8_t sign=	sscanf(strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"+QNTP: "),"+QNTP: %*d,\"%d/%d/%d,%d:%d:%d+%*d\"",\
						&year,&month,&date,&hour,&min,&sec);
				if(sign==6){
					if(hour>16||hour==16){//切换成东八区的时间
						hour = hour-16;
						date=date+1;
					
					}else{
					hour = hour+8;
					}
					
					
					week=CaculateWeekDay(year,month,date);
					year=(year%10)+(((year/10)%10)*10);//year===2018 取后两位
					
					RTC_Set_Time(hour,min,sec,ampm);
					RTC_Set_Date(year,month,date, week);
					net_flags.EC20_NTP=1;
					UpData_Date(1) ;//日期
					UpdataMidWin0();
					
				}else{
				net_flags.EC20_NTP=0;
				}
					

					}else{
					net_flags.EC20_NTP=0;
	
					}					
 
   myfree(0,p_mem_blk);//退还内存块

return net_flags.EC20_NTP;
}


 /**
  * @brief  配置 PC13 为PWfoot //非中断模式 ec20开关控制
  * @param  无
  * @retval 无
  */
void EC20PWFoot_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 


	RCC_AHB1PeriphClockCmd(EC20PWFoot_INT_GPIO_CLK,ENABLE);//
  
  GPIO_InitStructure.GPIO_Pin = EC20PWFoot_INT_GPIO_PIN;  //选择按键的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	 ////普通输出模式   		
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;//设置引脚速度
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//设置引脚下拉 ec20 默认开机
  GPIO_Init(EC20PWFoot_INT_GPIO_PORT, &GPIO_InitStructure);      


}

 /**
  * @brief  配置 PB2 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
//void EXTI_RI_FOOT_PIN_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure; 
//	EXTI_InitTypeDef EXTI_InitStructure;
//  
//	/*开启按键GPIO口的时钟*/
//	RCC_AHB1PeriphClockCmd(EC20RIFOOT_INT_GPIO_CLK,ENABLE);
//  
//  /* 使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟*/
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//  

//	
//	/* 选择按键的引脚 */ 
//  GPIO_InitStructure.GPIO_Pin = EC20RIFOOT_INT_GPIO_PIN;
//  /* 设置引脚为输入模式 */ 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	
//  /* 设置引脚不上拉也不下拉 */
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  /* 使用上面的结构体初始化按键 */
//  GPIO_Init(EC20RIFOOT_INT_GPIO_PORT, &GPIO_InitStructure); 

//	/* 连接 EXTI 中断源 到key引脚 */
//  SYSCFG_EXTILineConfig(EC20RIFOOT_INT_EXTI_PORTSOURCE,EC20RIFOOT_INT_EXTI_PINSOURCE);

//  /* 选择 EXTI 中断源 */
//  EXTI_InitStructure.EXTI_Line = EC20RIFOOT_INT_EXTI_LINE;
//  /* 中断模式 */
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  /* 触发 */
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
//  /* 使能中断/事件线 */
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	
//  EXTI_Init(&EXTI_InitStructure);
//	
//	
//	
//	  /* 配置 NVIC */
//  //NVIC_Configuration();
//    NVIC_InitTypeDef NVIC_InitStructure;
// 
//  /* 配置中断源： */
//  NVIC_InitStructure.NVIC_IRQChannel = EC20RIFOOT_INT_EXTI_IRQ;
//  /* 配置抢占优先级：1 */
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//  /* 配置子优先级：2 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  /* 使能中断通道 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//  

//}
