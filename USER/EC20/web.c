#include "web.h"
#include <stdio.h>
#include <string.h>
//#include "queue.h"
//#include "chipid.h"
//#include "ATT7022.h"
//#include "md5.h"
#include "ec20.h"
#include "stm32f4xx.h"

//const char httphead[]="POST /receive HTTP/1.0\r\n\
//Host: 101.132.115.71\r\n\
//Accept: */*\r\n\
//User-Agent: QUECTEL_MODULE\r\n\
//Connection: Keep-Alive\r\n\
//Content-Type: application/x-www-form-urlencoded\r\n\
//Content-Length: %d\r\n\r\n";

//#define POST "POST /receive HTTP/1.0\r\n"
//#define HOST "%sHost: %s\r\n%s%s%sContent-Length: %d\r\n\r\n"
//#define ACCEPT "Accept: */*\r\n"
//#define USER_AGENT "User-Agent: QUECTEL_MODULE\r\n"
//#define CONNECTION "Connection: Keep-Alive\r\n"
//#define CONTENT_TYPE "Content-Type: application/x-www-form-urlencoded\r\n"
//#define CONTENT_LENGTH "Content-Length: %d\r\n\r\n"

//#define SUB_1 "{\"phase\":\"%c\",\"phaseSequence\":\"%02d\",\"U\":\"%d\",\"I\":\"%d\",\"equipmentSubId\":\"%s\",\"status\":\"%02d\"}"
//#define MAIN "\"U\":\"%d\",\"V\":\"%d\",\"W\":\"%d\",\"IA\":\"%d\",\"sign\":\"%s\",\"IB\":\"%d\",\"IC\":\"%d\",\"equipmentId\":\"%s\",\"IN\":\"%d\"}\r\n"

//char GPRS_Send_Buf[4096];
//char HTTP_Head_Buf[512];
//char CRC_Clr_Buf[1024];
//const char Key[8]="shandong";


//u8 http_Content_Generation(void)
//{
//	u8 chipid[12];
//	u8 head[300];
//	u8 chipid_buf[24];
//	u8 md5_value[32];
//	u32 vol[3];
//	u32 cur[3];
//	
//	Q_Tpyedef elem;
//	u16 cnt = 0,Acnt= 0,Bcnt= 0,Ccnt= 0;
//	u16 len = 0,i = 0;
//	u8 ret = 0;
//	u16 md5_cnt = 0;
//	u16 gprs_cnt = 0;
//	u16 comma_flag = 0;
//	
//	
//	
//	Acnt =Q_size(U_Phase);
//	Bcnt =Q_size(V_Phase);
//	Ccnt =Q_size(W_Phase);
//	
//	
//	Get_CPU_ID(chipid,12);
//	for(i = 0; i <12; i ++){//生成chipid
//		sprintf(&CRC_Clr_Buf[md5_cnt],"%02x",chipid[i]);
//		md5_cnt += 2;
//	}
//	memcpy(chipid_buf,CRC_Clr_Buf,24);
//	//master u v w ia ib ic in
//	ret = Get_Avg_Current(cur);
//	ret |= Get_Avg_Voltage(vol);
//	if(ret == 0)//是否已采样
//		return 0;
//	
//	len = sprintf(&CRC_Clr_Buf[md5_cnt],"%d%d%d%d%d%d%d",vol[0],vol[1],vol[2],cur[0],cur[1],cur[2],cur[0]+cur[1]+cur[2]);
//	md5_cnt+=len;
//	len=sprintf(GPRS_Send_Buf,"{\r\n\"equipmentSubs:[\r\n\"");
//	gprs_cnt+= len;
//	for(cnt = 0; cnt < (Acnt+Bcnt+Ccnt); cnt ++)
//	{
//		
//		memset((void *)&elem,0,sizeof(Q_Tpyedef));
//		if(cnt < Acnt)
//		{
//			if(Q_find_index(U_Phase,cnt,DEL_Disable,&elem))
//			{
//			}
//			else
//				continue;
//		}
//		else if(cnt < (Acnt+Bcnt))
//		{
//			if(Q_find_index(V_Phase,(cnt-Acnt),DEL_Disable,&elem))
//			{
//			}
//			else
//				continue;
//		}
//		else
//		{
//			if(Q_find_index(W_Phase,(cnt-Acnt-Bcnt),DEL_Disable,&elem))
//			{
//			}
//			else
//				continue;
//		}
//		if(comma_flag >= 1){
//			len = sprintf(&GPRS_Send_Buf[gprs_cnt],",\r\n");
//			gprs_cnt += len;
//		}
//		len = sprintf(&GPRS_Send_Buf[gprs_cnt],SUB_1,elem.pha,elem.seq,elem.vol,elem.cur,elem.Eqid,elem.drop);
//		gprs_cnt += len;
//		len = sprintf(&CRC_Clr_Buf[md5_cnt],"%s%02d%c%02d%d%d",elem.Eqid,elem.stamp,elem.pha,elem.seq,elem.vol,elem.cur);
//		md5_cnt+=len;
//		comma_flag += 1;
//	}
//	len = sprintf(&CRC_Clr_Buf[md5_cnt],"%s",Key);
//	md5_cnt += len;
//	getMd5_32((u8 *)CRC_Clr_Buf, md5_cnt, md5_value);
//	
//	len = sprintf(&GPRS_Send_Buf[gprs_cnt],"\r\n],\r\n");
//	gprs_cnt += len;
//	//#define MAIN "\"U\":\"%d\",\"V\":\"%d\",\"W\":\"%d\",\"IA\":\"%d\",\"sign\":\"%s\",\"IB\":\"%d\",\"IC\":\"%d\",\"equipmentId\":\"%s\",\"IN\":\"%d\""
//	len = sprintf(&GPRS_Send_Buf[gprs_cnt],MAIN,vol[0],vol[1],vol[2],cur[0],md5_value,cur[1],cur[2],chipid_buf,(cur[0]+cur[1]+cur[2]));
//	gprs_cnt += len;
//	sprintf(HTTP_Head_Buf,httphead,gprs_cnt);
//	
//	
//	EC20_Http_Set_Url("http://101.132.115.71/receive",29);
//	memcpy(G_Post_Buf,GPRS_Send_Buf,gprs_cnt);
//	Post_Len = gprs_cnt;
//	//Http_Post_Data();
//	
//	return 0;	
//}
	
	


