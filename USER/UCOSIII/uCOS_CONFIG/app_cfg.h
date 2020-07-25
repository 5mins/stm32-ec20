/*
*********************************************************************************************************
*                                              uC/OS-II
*                                        The Real-Time Kernel
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/OS-II is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/OS-II in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to
*               help you experience uC/OS-II.  The fact that the source code is provided
*               does NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                       APPLICATION CONFIGURATION
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/



//////�������� 
void  PostKeySign (unsigned  char *msg ,unsigned short int msg_size );//���ռ����ַ�
typedef struct {
unsigned int EC20TMR1_ON : 	 		1;//��ʱ����1������
//unsigned int EC20TMR1_INIT : 	  1;
unsigned int EC20TMR2_ON : 	 		1;
//unsigned int EC20TMR2_INIT : 	  1;
unsigned int EC20TMR3_ON : 	 		1;
//unsigned int EC20TMR3_INIT : 	  1;
unsigned int EC20TMR4_ON : 	 	  1;
//unsigned int EC20TMR4_INIT : 	  1;
}EC20TmrCTR_FLAGS;

enum EC20_SENT_CTR{
sent_EC20_ATOK =	 1,
sent_EC20_GSN_Check =	 		2,//imei
sent_EC20_Get_Csq =	 		  3,
sent_EC20_Get_CREG =	 		4,//The Read Command returns the network registration status
sent_EC20_Get_COPS =	 		5,
sent_EC20_Set_Power_Off =	 6,
sent_EC20_Power_on =	 		7,
sent_EC20_AIR_Mode_On =	 	8,
sent_EC20_AIR_Mode_Off =	9,
sent_EC20_INIT =	 			  10,
sent_EC20_Net_Reg_INIT =	11,
sent_EC20_Read1SMS =   12,//Para1:mem  Para3:index 
sent_EC20_Delete_SMS =  13,//Para1:mem  Para2:delflag  Para3:index 
sent_EC20_SENT_SMS =  14,//Para1:*des  Para2:*text  
sent_EC20_SyncLocalTime =  15,//Para1:0 ֻ��ȡNTPʱ�� 1,��ȡNTPʱ�䲢�Ҹ��µ�����ʱ��
sent_EC20_Power_off =	 		16,
};//EC20���Ϳ���ָ����

typedef struct {
	unsigned int ATMODECOH : 1;//stm32 ֱ��ת��debug���ڵ�ATָ�EC20 �Ƿ����
	unsigned int EC20AUTO_INIT : 1;//�����Զ���ʼ��EC20
	unsigned int EC20AUTO_INIT_COUNT : 8;//�����Զ���ʼ��EC20 ����ʱ
	unsigned int LCD_KEEP_ON : 1;	//��Ļ���ⳣ������
	unsigned int LCD_TURNOFF_TIME : 16;	//����ʱ��
	unsigned int LCD_TURNOFF_TIME_COUNT : 16;	//����ʱ�䵹��ʱ
	unsigned int STM32isAllowStopMode : 1;	//�Ƿ�����ֹͣģʽ��(����)
	unsigned int STM32isAllowStopMode_MASTER : 1; //ֹͣģʽ�ܿ��ء�(����)
	unsigned int StopModeState : 1;	//ֹͣģʽ��ǰ״̬
	
	
}STM32_CTR_FLAGS;//stm32 ����״̬�ͱ��


typedef struct {
 int Para1 :8;
 int Para2 :8;
 int Para3 :16;
char* Parachar1;
char* Parachar2;
//unsigned int Para4 : 	1;

}EC20_SENT_CTR_Parameters;//����ָ������

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
///����ucOSiiiϵͳ�ж����ȼ� 1,2,3Ԥ����ϵͳ
#define  APP_CFG_TASK_START_PRIO                          2u
#define  AppTaskMailCenterSign_PRIO												4u
#define  APP_TASK_EC20Ctr_PRIO														5u

#define  APP_TASK_TaskDisposeKeySign_PRIO  								7u
#define  APP_TASK_NormalKeyScan_PRIO  										8u  
#define  APP_TASK_EC20Tmr_PRIO														9u	
#define  APP_TASK_GUI_PRIO  															10u


//�ж����ȼ� 13u ���趨Ϊͳ������  Timer Task ���ȼ�Ϊ 11u 
//ʱ������OS_CFG_TICK_TASK_PRIO 6u

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE                 128u
#define  APP_TASK_EC20Tmr_STK_SIZE                  512u
#define  APP_TASK_EC20Ctr_STK_SIZE                  1024u
#define  APP_TASK_MailCenter_STK_SIZE                512u
#define  APP_TASK_DisposeKeySignSTK_SIZE						 256u
#define  APP_TASK_GUI_STK_SIZE						 					 2048u //
#define  APP_TASK_NormalKeyScan_STK_SIZE						 128u



/*
*********************************************************************************************************
*                                            TASK STACK SIZES LIMIT
*********************************************************************************************************
*/

//#define  APP_CFG_TASK_START_STK_SIZE_PCT_FULL             90u
//#define  APP_CFG_TASK_START_STK_SIZE_LIMIT       (APP_CFG_TASK_START_STK_SIZE     * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))    / 100u
//#define  APP_CFG_TASK_BLINKY_STK_SIZE_LIMIT      (APP_CFG_TASK_BLINKY_STK_SIZE    * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))    / 100u


/*
*********************************************************************************************************
*                                       TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                0
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO               1
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                2
#endif

#define  APP_CFG_TRACE_LEVEL             TRACE_LEVEL_OFF
#define  APP_CFG_TRACE                   printf

#define  BSP_CFG_TRACE_LEVEL             TRACE_LEVEL_OFF
#define  BSP_CFG_TRACE                   printf

#define  APP_TRACE_INFO(x)               ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

#define  BSP_TRACE_INFO(x)               ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO) ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)                ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)  ? (void)(BSP_CFG_TRACE x) : (void)0)

#endif
