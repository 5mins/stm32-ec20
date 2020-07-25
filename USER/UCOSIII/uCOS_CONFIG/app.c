/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : YS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


//��ֵ�ź���
OS_SEM SemOfKey;          //��־KEY1�Ƿ񱻵����Ķ�ֵ�ź���
OS_SEM SemOfRec_EC20_OK;          //��ֵ�ź��� EC20:OK
OS_SEM SemOfATV; 			//ATV1 OK
OS_SEM SemOfCMEE;			 //AT+CMEE=2 OK
OS_SEM SemOfGSN; 			//AT+GSN OK
OS_SEM SemOfCPIN; 		//AT+CPIN? OK
OS_SEM SemOfCSQ;			// +CSQ: 18,99 OK
OS_SEM SemOfCREG; 		//+CREG: 0,1 OK
OS_SEM SemOfCGREG;		// +CGREG: 0,1 OK
OS_SEM SemOfCOPS;	 		//+COPS: 0,0,"CHN-UNICOM",7 OK
OS_SEM SemOfCMGF;	 		//1:MODE 0 ,2:MODE 1
OS_SEM SemOfCPMSS;//
OS_SEM SemOfCMGD;//delete sms
OS_SEM SemOfCMGDINDEX;//UPDATA sms INDEX
OS_SEM SemOfCMGS;//sent sms
OS_SEM SemOfCMGSOK;//sent sms
OS_SEM SemOfQNTP;//NTP TIME
OS_SEM SemOfCSCS;//AT+CSCS
OS_SEM SemOfCFUN;//����ģʽ ȫ����ģʽ
OS_SEM SemOfQPOWD;//
OS_SEM SemOfRDY;//

OS_FLAG_GRP FlagGropOf_CMGR;     //�����¼���־��CMGR rec sms
extern type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//EC20��ǰ���յ�������Ϣ
extern uint8_t LINKEC20_USART_RECEIVE_BUF[LINKEC20_USART_RECEIVE_BUF_SIZE];//LINKEC20����DMA���ջ�����
//extern uint8_t LINKEC20_USART_SEND_BUF[LINKEC20_USART_SEND_BUF_SIZE];//LINKEC20����DMA���ͻ�����

extern uint8_t LINKEC20_USART_Use_DMA_Tx_Flag;
enum EC20_SENT_CTR SENTCTR;//EC20���Ϳ���ָ����
EC20_SENT_CTR_Parameters SENTCTR_PARA;////���Ϳ���ָ������
extern NET_FLAGS net_flags;
KEY Key1;

//uint8_t ATMODECOH=0;//stm32 ֱ��ת��debug���ڵ�ATָ�EC20 �Ƿ����
STM32_CTR_FLAGS stm32_flat;

/* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   			AppTaskStartTCB;
static  CPU_STK  			AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

  OS_TCB       AppTaskDisposeKeySignTCB;
static  CPU_STK      AppTaskDisposeKeySignSTK[APP_TASK_DisposeKeySignSTK_SIZE];//������Ϣ��������

static  OS_TCB       AppTaskNormalKeyScanTCB;
static  CPU_STK      AppTaskNormalKeyScanSTK[APP_TASK_NormalKeyScan_STK_SIZE];

OS_TCB     AppTaskMailCenterTCB;
static  CPU_STK      AppTaskMailCenterSTK[APP_TASK_MailCenter_STK_SIZE];//EC20������Ϣ��������

static  OS_TCB       AppTaskGUITCB;
static  CPU_STK      AppTaskGUISTK[APP_TASK_GUI_STK_SIZE];//GUI������

static  OS_TCB       AppTaskEC20TmrTCB;//EC20��̨��ʱ����������Ϣ����
static  CPU_STK      AppTaskEC20TmrSTK[APP_TASK_EC20Tmr_STK_SIZE];

OS_TCB       AppTaskEC20CtrTCB;//EC20��Ϣ������������
static  CPU_STK      AppTaskEC20CtrSTK[APP_TASK_EC20Ctr_STK_SIZE];

static uint32_t EC20TMR1_STICK = 1000;//������Ϣ������ʱ��,Ĭ��1s

OS_MUTEX mutex_ec20sent;                         //���� EC20���ڷ���ʹ�õĻ����ź���

unsigned int insCtrFLAG;//EC20����ָ����

OS_TMR tmr1; //��ʱ�� 1
//extern uint32_t LCD_TURNOFF_TIME;//LCD��Ļ���� �رյ���ʱ ��λ��
//extern uint32_t LCD_TURNOFF_TIME_COUNT;
//extern u8 LCD_KEEP_ON;//LCD��Ļ���ⳣ��
//extern u8 STM32isAllowStopMode;//�Ƿ�����ֹͣģʽ��
//extern u8 StopModeState;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  		  (void  *p_arg);  //��ʼ����
static  void  AppTaskCreate         (void); // �����񴴽�����

static 	void 	AppTaskNormalKeyScan(void);//��ͨ����ɨ������
static  void  AppTaskDisposeKeySign ( void * p_arg );//��ֵ�ź�������ɨ������
static  void  AppTaskMailCenter ( void * p_arg );//��ֵ�ź�����ӦEC20��������
static  void  AppTaskGUI (void  *p_arg);  //������Ļ����
static  void  AppTaskEC20Tmr ( void * p_arg );// EC20��̨��ʱ����������Ϣ����
static  void  AppTaskEC20Ctr ( void * p_arg );//  EC20 ���ͣ�������Ϣ����
///����������
void Print_CPUusage (void);///��ʾCPU��ʹ����
void Creat_SEMs (void);///����������ֵ�ź���
void PostKeySign (uint8_t *msg ,uint16_t msg_size );//���ռ����ַ�//����ģ����̲���
uint8_t Compare_String1_with_23 (char* str1,char* str2,char* str3); //�Ƚ��ַ���1���Ƿ�ͬʱ�����ַ���2���ַ���3
void tmr1_callback(void *p_tmr, void *p_arg);//��ʱ��1 �ص�
void ResetLCDtime(void);//��������60s
void STM32CRTFLAT_Init(void);
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
	
    OS_ERR  err;

		//INTX_DISABLE();//�Զ�����жϷ���
    
    CPU_Init();                                                 /* Initialize the uC/CPU Services                       */
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;

   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */

		
////		/* �����ڴ������� mem */
//		OSMemCreate ((OS_MEM      *)&INTERNAL_MEM,         //ָ���ڴ�������
//								 (CPU_CHAR    *)"INTERNAL_MEM",         //�����ڴ�������
//								 (void        *)Internal_RamMemp,           //�ڴ�������׵�ַ
//								 (OS_MEM_QTY   )INTERNAL_MEM_NUM,                 //�ڴ�������ڴ����Ŀ
//								 (OS_MEM_SIZE  )INTERNAL_MEMBLOCK_SIZE,               //�ڴ����ֽ���Ŀ
//								 (OS_ERR      *)&err);            					//���ش�������

	STM32CRTFLAT_Init();
	
	
	
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

     if(err!=OS_ERR_NONE)  bsp_result|=BSP_OS;                                       

    APP_TRACE_DBG(("Creating Application Tasks\n\r"));
    AppTaskCreate();                                            /* Create Application tasks                             */


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			

        LED1_TOGGLE;
				OSTimeDly(1000,OS_OPT_TIME_DLY,&err);

    }
}


/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	
    OS_ERR  os_err;

	
		KeyCreate(&Key1,GetPinStateOfKey1); //������ʼ��KEY1
	
			/* ����ucGUI���� */
    OSTaskCreate((OS_TCB     *)&AppTaskGUITCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task UPDATE GUI",                             //��������
                 (OS_TASK_PTR ) AppTaskGUI,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_GUI_PRIO,                         						//��������ȼ�
                 (CPU_STK    *)&AppTaskGUISTK[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 0u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 100u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&os_err);                                       //���ش�������
						
		Creat_SEMs();///����������ֵ�ź���
								 
		/* ����ѭ��ɨ��EC20���ڽ������� */
    OSTaskCreate((OS_TCB     *)&AppTaskMailCenterTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"AppTaskMailCenter",                             //��������
                 (OS_TASK_PTR ) AppTaskMailCenter,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) AppTaskMailCenterSign_PRIO,                  	//��������ȼ�
                 (CPU_STK    *)&AppTaskMailCenterSTK[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_MailCenter_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_MailCenter_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 150u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&os_err);                                       //���ش�������
	
			/* ������ֵ�ź��� SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,    //ָ���ź���������ָ��
               (CPU_CHAR    *)"SemOfKey",    //�ź���������
               (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
               (OS_ERR      *)&os_err);         //��������
								 
			/* ������ֵ�ź�������ɨ������ */
    OSTaskCreate((OS_TCB     *)&AppTaskDisposeKeySignTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task DisposeKeySign",                             //��������
                 (OS_TASK_PTR ) AppTaskDisposeKeySign,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_TaskDisposeKeySign_PRIO,                         						//��������ȼ�
                 (CPU_STK    *)&AppTaskDisposeKeySignSTK[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_DisposeKeySignSTK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_DisposeKeySignSTK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 10u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 100u,                                         //�����ʱ��Ƭ������//��ʱ�趨200 ������
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&os_err);                                       //���ش�������
								 
		/* ���� EC20��Ϣ�������� */
    OSTaskCreate((OS_TCB     *)&AppTaskEC20CtrTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"AppTaskEC20Ctr",                             //��������
                 (OS_TASK_PTR ) AppTaskEC20Ctr,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_EC20Ctr_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskEC20CtrSTK[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_EC20Ctr_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_EC20Ctr_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 0u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 250u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&os_err);                                       //���ش�������
								 

								 
			/* ������ͨ����ɨ������ */
    OSTaskCreate((OS_TCB     *)&AppTaskNormalKeyScanTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task DisposeKeySign",                             //��������
                 (OS_TASK_PTR ) AppTaskNormalKeyScan,                                			//������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_NormalKeyScan_PRIO,                         						//��������ȼ�
                 (CPU_STK    *)&AppTaskNormalKeyScanSTK[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_NormalKeyScan_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_NormalKeyScan_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 0u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 60u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&os_err);                                       //���ش�������
								 
			//������ʱ�� 1  
							OSTmrCreate((OS_TMR* )&tmr1, // ��ʱ�� 1 (1)
							(CPU_CHAR* )"tmr1", // ��ʱ������
							(OS_TICK )10, //100*10=1000ms//��ʼ����ʱ�����ӳ�ֵ��
							(OS_TICK )10, //100*10=1000ms//�ظ�����
							(OS_OPT )OS_OPT_TMR_PERIODIC, // ����ģʽ ���ڶ�ʱ��
							(OS_TMR_CALLBACK_PTR)tmr1_callback, // ��ʱ�� 1 �ص�����
							(void* )0, // ����Ϊ 0
							(OS_ERR* )&os_err); // ���صĴ�����
							
			OSTmrStart(&tmr1,&os_err); //������ʱ�� 1		

		OSTaskDel ( & AppTaskStartTCB, & os_err );                     //ɾ����ʼ������������������
								 
//		    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
//						// LED1_TOGGLE;
//					OSTimeDly(1000,OS_OPT_TIME_DLY,&os_err);

//    }

}





/*
*********************************************************************************************************
*                  ��ֵ�ź���ʵ�ִ��ڼ���ɨ��
*********************************************************************************************************
*/

static  void  AppTaskDisposeKeySign ( void * p_arg )
{
    OS_ERR         err;
//		OS_MSG_SIZE    msg_size;
	extern NET_FLAGS net_flags;//����״̬
	extern uint8_t DEBUG_USART_RECEIVE_BUF[DEBUG_USART_RECEIVE_BUF_SIZE];//debug����DMA���ջ�����
	extern type_DEBUG_USART_Rx  DEBUG_USART_Rx;//��ǰ���յ�������Ϣ


	
		char * pMsg;
	pMsg = (char *)DEBUG_USART_Rx.DEBUG_USART_RECEIVE;
	

		CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���

    while (DEF_TRUE) {

						/* �������񣬵ȴ�������Ϣ */
						OSTaskSemPend ((OS_TICK      )0,                    //�����޵ȴ�
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											  (CPU_TS        *)0,                  //������Ϣ��������ʱ���
											  (OS_ERR        *)&err);                //���ش�������

			
//		/* �������񣬵ȴ�������Ϣ */
//		pMsg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
//											  (OS_MSG_SIZE   *)&msg_size,            //������Ϣ����
//											  (CPU_TS        *)0,                  //������Ϣ��������ʱ���
//											  (OS_ERR        *)&err);                //���ش�������
			
			OSTimeDly(30,OS_OPT_TIME_DLY,&err);//�ӳ�һ��ʱ��ȴ�������ɡ�
			
			OS_CRITICAL_ENTER();                          //�����ٽ�Σ�
				

        /* �رս���DMA  */
        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,DISABLE);  
        /* �����־λ Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags);  
			
				memset(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//����ǰ�����ԭ������...
        memcpy(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,DEBUG_USART_RECEIVE_BUF,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));  //��������
				memset(DEBUG_USART_RECEIVE_BUF,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//�����꣬���BUF��������
			
        /* �������ô������ݳ��� */
        DMA_SetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_RECEIVE_BUF_SIZE);  
        /* ��DMA */
        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);  
				
				OS_CRITICAL_EXIT(); 	
			
			

			//LED1_TOGGLE ;                            //�л�LED1������״̬
			KEEP_LCD_ON_FOR_A_WHILES();//��������60s
			
			if(pMsg[0]){
				int len = strlen(pMsg);
				if(len==1||len==3){//1λ����ģʽ
				
				switch(pMsg[0]){
					case 65 ://A left
						
						GUI_StoreKeyMsg(GUI_KEY_LEFT, 1);//GUI_StoreKeyMsg(GUI_KEY_DOWN, 1);GUI_SendKeyMsg(GUI_KEY_LEFT, 1);


						break;
					case 68://D right
						
					
						GUI_StoreKeyMsg(GUI_KEY_RIGHT, 1);//GUI_StoreKeyMsg(GUI_KEY_RIGHT, 1);GUI_SendKeyMsg(GUI_KEY_RIGHT, 1);
					

						break;
					case 87://W UP
						GUI_StoreKeyMsg(GUI_KEY_UP, 1);
						//printf("inter:%d\n",pmsg1);
						break;
					case 83://S DOWN
						GUI_StoreKeyMsg(GUI_KEY_DOWN, 1);
						//printf("exit:%d\n",pmsg1);
						break;
					case 81://Q enter
						GUI_StoreKeyMsg(GUI_KEY_ENTER, 1);
						//printf("inter:%d\n",pmsg1);
						break;
					case 69://E exit
						GUI_SendKeyMsg(GUI_KEY_ESCAPE, 1);
						//printf("exit:%d\n",pmsg1);
						break;
					case 90://Z 
					SENTCTR =sent_EC20_INIT;  //�趨��������
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
							
						break;
					case 88:// X
						
						if(	net_flags.EC20_AIR_MODE==0){
							stm32_flat.EC20AUTO_INIT=0;//������EC20���������Զ���ʼ��EC20
								SENTCTR =sent_EC20_AIR_Mode_On;  //�趨��������
								OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
							//EC20_AIR_Mode_On();//��������ģʽ	
						}else{
							
								SENTCTR =sent_EC20_AIR_Mode_Off;  //�趨��������
								OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
								stm32_flat.EC20AUTO_INIT=1;
							
							//EC20_AIR_Mode_Off();
						}
						
					 
							
						break;
					case 67:// C
					SENTCTR_PARA.Para1=0;
					SENTCTR =sent_EC20_SyncLocalTime;  //�趨��������
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
					
//					sscanf(strstr((const char*)DEBUG_USART_Rx.DEBUG_USART_RECEIVE,(const char*)"C "),"C %d,%d,%d,%d,%d,%d,%d,%d",&hour,&min,&sec,&ampm\
//						,&year,&month,&date,&week);//
//					RTC_Set_Time(hour,min,sec,ampm);
//					RTC_Set_Date(year,month,date, week);
					
						//EC20_UPDATA_SMSBOX_INDEX(1);
					
						break;
	
					default:
						break;

				}//end switch(pmsg1)
			}else if(len ==4){//2λָ��ģʽ
					if(pMsg[0]=='A'&&\
					pMsg[1]=='1'){
					stm32_flat.ATMODECOH=1;
					printf("A1\r\n");
					}else if(pMsg[0]=='A'&&\
					pMsg[1]=='0'){
					stm32_flat.ATMODECOH=0;
					printf("A0\r\n");
					}else if(pMsg[0]=='P'&&\
					pMsg[1]=='1'){//��EC20
					SENTCTR =sent_EC20_Power_on;  //�趨��������
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������
						
					}else if(pMsg[0]=='P'&&\
					pMsg[1]=='0'){//��EC20
					SENTCTR =sent_EC20_Power_off;  //�趨��������
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������
						
					}else if(pMsg[0]=='L'&&\
					pMsg[1]=='1'){//��������
					stm32_flat.STM32isAllowStopMode=1;//�������ֹͣģʽ
					ResetLCDBKtime(60);
						printf("L1\r\n");
					}else if(pMsg[0]=='L'&&\
					pMsg[1]=='0'){//���ⳣ��
					stm32_flat.STM32isAllowStopMode=0;//���������ֹͣģʽ
					ResetLCDBKtime(0);
						printf("L0\r\n");
					}
				
				
			}else{//len>1 ATģʽ
			
					if(pMsg[0]=='A'&&\
					pMsg[1]=='T'){
					char *   p_mem_blk;
					OS_CRITICAL_ENTER();
					p_mem_blk = mymalloc(0,300);
					memset(p_mem_blk,0,300*sizeof(uint8_t));//�������
					u8 sign =sscanf(strstr((const char*)pMsg,(const char*)"AT "),"AT <%[^>]",p_mem_blk);
					OS_CRITICAL_EXIT();
						if(sign==1){
						
						OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
						OS_CRITICAL_ENTER();
						sprintf(p_mem_blk,"%s\r\n",p_mem_blk);
						EC20_SendCMD(p_mem_blk);//ͬ��NTP�����±���ʱ��
						OS_CRITICAL_EXIT();
						OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
						
						}

				   myfree(0,p_mem_blk);//�˻��ڴ��
				}
				
			
			}
			
			}
			
				
		
			OSTimeDly(40,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������

    }
			
}

/*
*********************************************************************************************************
*                  EC20 ���ͣ�������Ϣ����
*********************************************************************************************************
*/

static  void  AppTaskEC20Ctr ( void * p_arg )
{	
	OS_ERR    err;
	static EC20TmrCTR_FLAGS ec20Tmrctr_flags;
	extern NET_FLAGS net_flags;//����״̬
	///�趨��ʼֵ
  net_flags.EC20_AIR_MODE=1;//��������ģʽ
	ec20Tmrctr_flags.EC20TMR1_ON=0;
	///�趨��ʼֵ
	(void)p_arg;
	//���������ź���mutex_ec20sent
	OSMutexCreate(&mutex_ec20sent,"mutex_ec20sent",&err);
	
    while (DEF_TRUE) {
			
						/* �������񣬵ȴ�������Ϣ */
						OSTaskSemPend ((OS_TICK      )0,                    //�����޵ȴ�
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											  (CPU_TS        *)0,                  //������Ϣ��������ʱ���
											  (OS_ERR        *)&err);                //���ش�������
			
			stm32_flat.STM32isAllowStopMode=0;//���������ֹͣģʽ
				
					if(err==OS_ERR_NONE){//
				
						switch((u8)SENTCTR){
								
									case sent_EC20_ATOK:
											{
												OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_ATOK();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
												if(res==1){printf("sent_EC20_ATOK\n");}	
												
											}
										break;
									case sent_EC20_Power_on:
											{
												stm32_flat.EC20AUTO_INIT=1;//�������Զ���ʼ��EC20
												stm32_flat.STM32isAllowStopMode=0;//���������ֹͣģʽ
												OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_Power_on();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
												if(res==1){printf("EC20_Power_ON\n");}	
												stm32_flat.STM32isAllowStopMode=1;//�������ֹͣģʽ
											}
										break;
									case sent_EC20_Power_off:
											{
												stm32_flat.EC20AUTO_INIT=0;//������EC20���������Զ���ʼ��EC20
												stm32_flat.STM32isAllowStopMode=0;//���������ֹͣģʽ
												OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_Power_Off();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
												if(res==1){printf("sent_EC20_Power_OFF\n");}	
												stm32_flat.STM32isAllowStopMode=1;//�������ֹͣģʽ
											}
										break;
									case sent_EC20_Get_COPS:
										{
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_Get_COPS();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
													if(res==1){printf("EC20_Get_COPS\n");}	
											}
										break;
									case sent_EC20_Read1SMS:
										{
													
													if(SENTCTR_PARA.Para1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
														u8 res = EC20_Read1SMS(SENTCTR_PARA.Para1,SENTCTR_PARA.Para3,0);//tmp_sms_idx Ĭ����0�������Ժ�Ҫ��Ҫ���Ԥ������
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
													
												}
										break;
										case sent_EC20_Delete_SMS:
										{
													
													if(SENTCTR_PARA.Para1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
														//u8 res = EC20_Delete_SMS(u8 mem ,u8 delflag,u16 index);
														u8 res = EC20_Delete_SMS(SENTCTR_PARA.Para1 ,SENTCTR_PARA.Para2,SENTCTR_PARA.Para3);
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
												}
										break;
									case sent_EC20_SENT_SMS:
										{

													if(*SENTCTR_PARA.Parachar1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
														u8 res = EC20_SENT_SMS((const char*)SENTCTR_PARA.Parachar1,(const char*)SENTCTR_PARA.Parachar2);//const char* des,const char* text
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
												}
										break;
									case sent_EC20_SyncLocalTime:
										{
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
											u8 res = EC20_SyncLocalTime(SENTCTR_PARA.Para1);//const char* des,const char* text
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
											if(res==1){
											//printf("EC20_Get_COPS\n");}	
												}
													
												}
										break;

									case sent_EC20_AIR_Mode_On:
										{
											OSMutexPend(&mutex_ec20sent,3200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�3200ms��
													u8 res = EC20_AIR_Mode_On();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
													if(res==1){printf("EC20_AIR_Mode_On\n");}	
											}
										break;
									case sent_EC20_AIR_Mode_Off:
											{
												OSMutexPend(&mutex_ec20sent,3200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_AIR_Mode_Off();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
													if(res==1){printf("EC20_AIR_Mode_Off\n");}	
											}
										break;
									case sent_EC20_INIT:
											{
												OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_INIT();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
													if(res==1){
														CPU_SR_ALLOC(); 
													OS_CRITICAL_ENTER(); 
													printf("EC20_INIT\n");
														OS_CRITICAL_EXIT(); 
													}	
											}
										break;
									case sent_EC20_Net_Reg_INIT:
										{
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�200ms��
													u8 res = EC20_Net_Reg_INIT();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
													if(res==1){printf("EC20_Net_Reg_INIT\n");}	
											}
										break;
									default:
										break;
								
								}//END switch(SENTCTR)	
						
					}else{
						printf("OSMutexPend err:%d\n",err);//debug_mark
					
					}//end if(err==OS_ERR_NONE){//����ɹ�
			

			
			
			
	//����ģʽ���� ��ʼ
		if((net_flags.EC20_AIR_MODE==1)||(net_flags.IS_EC20_OFF==1)){//�����������ģʽ ���� ec20�ر�
			if(ec20Tmrctr_flags.EC20TMR1_ON==1){//�����ʱ����1�ѿ���
				OS_ERR delerr;
						//ɾ����ʱ����1
							OSTaskDel ( &AppTaskEC20TmrTCB, & delerr );
							if(delerr==OS_ERR_NONE){
							ec20Tmrctr_flags.EC20TMR1_ON=0;
								 UpdataMidWin0();
								//printf("EC20TMR1_del");
							}
							

				}
		
		
		 }else{//����رշ���ģʽ
			 if(net_flags.EC20_INIT==1){//�Ƿ��ѳ�ʼ��
						if(ec20Tmrctr_flags.EC20TMR1_ON==0){//�����ʱ����1δ����
										//������ʱ����1
										/* ���� EC20��̨��ʱ����������Ϣ���� */
							OS_ERR crerr;
								OSTaskCreate((OS_TCB     *)&AppTaskEC20TmrTCB,                             //������ƿ��ַ
														 (CPU_CHAR   *)"AppTaskEC20Tmr",                             //��������
														 (OS_TASK_PTR ) AppTaskEC20Tmr,                                //������
														 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
														 (OS_PRIO     ) APP_TASK_EC20Tmr_PRIO,                         //��������ȼ�
														 (CPU_STK    *)&AppTaskEC20TmrSTK[0],                          //�����ջ�Ļ���ַ
														 (CPU_STK_SIZE) APP_TASK_EC20Tmr_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
														 (CPU_STK_SIZE) APP_TASK_EC20Tmr_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
														 (OS_MSG_QTY  ) 0u,                                         //����ɽ��յ������Ϣ��
														 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
														 (void       *) 0,                                          //������չ��0����չ��
														 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
														 (OS_ERR     *)&crerr);                                       //���ش�������
														 
											if(crerr==OS_ERR_NONE){
											ec20Tmrctr_flags.EC20TMR1_ON=1;
												//printf("EC20TMR1_cr");
											}
														 
						}
			 }

		
		}//����ģʽ����	����

		stm32_flat.STM32isAllowStopMode=1;//�������ֹͣģʽ
		
			OSTimeDly(50,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������

    }
			
}

/*
*********************************************************************************************************
*                             EC20��̨��ʱ����������Ϣ����
*********************************************************************************************************
*/
static  void  AppTaskEC20Tmr ( void * p_arg )
{
	extern SMS_SHOW sms_show;
	OS_ERR err;
	EC20TMR1_STICK = 3000;//������Ϣ������ʱ��,Ĭ��1s

		while(DEF_TRUE){
			OSTimeDly(EC20TMR1_STICK/2,OS_OPT_TIME_DLY,&err);//�ӳ��趨�õļ��ʱ��
						 //�����뷢�͵Ļ����ź���mutex_ec20sent
			OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�500ms��
			if(err==OS_ERR_NONE){//����ɹ�
				EC20_ATOK();

		OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
			}
			
		if(net_flags.EC20_ATOK==1){
			
							 //�����뷢�͵Ļ����ź���mutex_ec20sent
					OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�500ms��
					if(err==OS_ERR_NONE){//����ɹ�
						EC20_Get_Csq();

				OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
					}
	
					OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�500ms��
					if(err==OS_ERR_NONE){//����ɹ�

						EC20_Get_COPS();

				OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
					}
					
//				OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�500ms��
//					if(err==OS_ERR_NONE){//����ɹ�

//						EC20_UPDATA_SMSBOX_INDEX(sms_show.mem);
//					OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//�ͷŻ����ź�����mutex_ec20sent 
//					}
		
		}



					
				
				UpdataMidWin0();

		OSTimeDly(EC20TMR1_STICK/2,OS_OPT_TIME_DLY,&err);//�ӳ��趨�õļ��ʱ��
		}

	


	
}



/*
*********************************************************************************************************
*                  ɨ��DMA�������� EC20���ڽ���3 
*********************************************************************************************************
*/

static  void  AppTaskMailCenter ( void * p_arg )
{
    OS_ERR   err;
		memset(LINKEC20_USART_RECEIVE_BUF,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));
		CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���

    while (DEF_TRUE) {
			
//							/* �������񣬵ȴ�������Ϣ */
//						OSTaskSemPend ((OS_TICK      )0,                    //�����޵ȴ�
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
//											  (CPU_TS        *)0,                  //������Ϣ��������ʱ���
//											  (OS_ERR        *)&err);                //���ش�������
			
					//Ĭ����һ�ν��յ�һ�����ص����ݣ�������Ҫ��֤��ͬ����֮��ķ�����һ��ʱ��ļ��
					//�����Ҫһ�ν��ն������ݣ�������Ҫ��һ�����������ݷ���ĺ�����

			if(LINKEC20_USART_RECEIVE_BUF[0]!=0x00){//���յ�����
	
				//OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//���뻥���ź���mutex_ec20sent�������ȴ�500ms����������ʱ��������
				//������������ȼ��趨�ñȷ�������ߣ����Բ���Ҫ�ӻ����ź����ˡ�
				OSTimeDly(50,OS_OPT_TIME_DLY,&err);//
				uint8_t times =0;//��ȴ�990ms
				while(!(strstr((const char*)LINKEC20_USART_RECEIVE_BUF,(const char*)"OK"))&&(times<17)){
				OSTimeDly(55,OS_OPT_TIME_DLY,&err);//���û��OK��β �ӳ�һ��ʱ��ȴ�������ɡ�
				times++;
				}
				
				OS_CRITICAL_ENTER();                          //�����ٽ�Σ�
				
				 /* �رս���DMA  */
        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,DISABLE);  
			/* �����־λ Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags);  

        /* ��ý���֡֡�� */
       // LINKEC20_USART_Rx.LINKEC20_USART_RxLen = LINKEC20_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream);  
				
				memset(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//����ǰ�����ԭ�����յ�LINKEC20_USART_RECEIVE����
        memcpy(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,LINKEC20_USART_RECEIVE_BUF,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));  //��������
				memset(LINKEC20_USART_RECEIVE_BUF,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//�����꣬���BUF��������
			
        /* �������ô������ݳ��� */
        DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_RECEIVE_BUF_SIZE);  
        /* ��DMA */
        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE); 
					OS_CRITICAL_EXIT(); 	
				
				if(stm32_flat.ATMODECOH==1){//stm32 ֱ��ת��debug���ڵ�ATָ�EC20 �Ƿ����
					printf("EC20ECOH:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
				}
				
				if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"OK"))){
					
					//�ȴ��������ţ����绰�ȳ��õı�����Ϣ

					
					if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+COPS?")){//"AT+COPS?","OK"
							OSSemPost((OS_SEM  *)&SemOfCOPS,                          //���� SemOfCOPS
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CSQ")){//"AT+CSQ","OK"
							OSSemPost((OS_SEM  *)&SemOfCSQ,                          //���� SemOfCSQ
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGR=")){//EC20��ȡ����Ϣ(��ʱ???)����������ݷ��أ����Ե�һ������Ҫ�ж�OK��β
						
//								OSSemPost((OS_SEM  *)&SemOfCMGR1,                          //���� SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1|OS_OPT_POST_NO_SCHED,        //����Ϣ������
//									 (OS_ERR  *)&err);                                       //���ش�������
//								OSSemPost((OS_SEM  *)&SemOfCMGR2,                          //���� SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
//									 (OS_ERR  *)&err);                                       //���ش�������
					
						OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x03,OS_OPT_POST_FLAG_SET,&err); //����־��� BIT0\BIT1 ��1
						
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+CMGS:")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfCMGSOK,                          //���� 
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"ATV")){//"ATV","OK"
						
							OSSemPost((OS_SEM  *)&SemOfATV,                          //���� SemOfATV
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGF")){//"AT+CMGF=1","OK"
						
							OSSemPost((OS_SEM  *)&SemOfCMGF,                          //���� SemOfCMGF
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGD=?")){//AT+CPMS="SM","SM","ME" 
						
							OSSemPost((OS_SEM  *)&SemOfCMGDINDEX,                          //���� SemOfCMGDINDEX
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CSCS=")){//AT+CSCS="UCS2"
						
							OSSemPost((OS_SEM  *)&SemOfCSCS,                          //���� 
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPMS=")){//AT+CPMS="SM","SM","ME"  
						
							OSSemPost((OS_SEM  *)&SemOfCPMSS,                          //���� SemOfCPMS
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CFUN")){//:AT+CFUN?
						
							OSSemPost((OS_SEM  *)&SemOfCFUN,                          //���� SemOfCFUN
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGD=")){
							//printf("recCMEE_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCMGD,                          //���� SemOfCMGD
								 (OS_OPT   )OS_OPT_POST_1,                           //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                     //���ش�������
					
					}
//					else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPMS?")){//
//						
//							OSSemPost((OS_SEM  *)&SemOfCPMSG,                          //���� SemOfCPMS
//								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
//								 (OS_ERR  *)&err);                                       //���ش�������
//					
//					}
					else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMEE=2")){
							//printf("recCMEE_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCMEE,                          //���� SemOfCMEE
								 (OS_OPT   )OS_OPT_POST_1,                           //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                     //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+GSN")){
						//printf("recGSN_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfGSN,                          //���� SemOfGSN
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ����������highest priority task waiting��
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPIN")){
						//printf("recCPIN_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCPIN,                          //���� SemOfCPIN
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CREG")){
							OSSemPost((OS_SEM  *)&SemOfCREG,                          //���� SemOfCREG
								 (OS_OPT   )OS_OPT_POST_1,                           //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                     //���ش�������
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CGREG")){
							OSSemPost((OS_SEM  *)&SemOfCGREG,                          //���� SemOfCGREG
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"POWERED DOWN"))){//POWERED DOWN
										
								 OSSemPost((OS_SEM  *)&SemOfQPOWD,                          //���� Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"AT\r\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //���� Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //���� Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"ATE1\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //���� Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                             //���ش�������

					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+CMGR: ")){//EC20��ȡ����Ϣ(��ʱ???)����������ݷ��أ����Ե�һ������Ҫ�ж�OK��β
						
//								OSSemPost((OS_SEM  *)&SemOfCMGR2,                          //���� SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
//									 (OS_ERR  *)&err);                                       //���ش�������
							OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x02,OS_OPT_POST_FLAG_SET,&err); //����־���BIT1��1
					
						}else{//else ok
										

										if(stm32_flat.ATMODECOH!=1){
										printf("MAILCENTER_OK_ELSE:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
										}

									}
						
						
						

				
				}else{//else without OK
					
					if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGR")){//EC20��ȡ����Ϣ(��ʱ???)����������ݷ��أ����Ե�һ������Ҫ�ж�OK��β
						
//							OSSemPost((OS_SEM  *)&SemOfCMGR1,OS_OPT_POST_1, &err);  
							OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x01,OS_OPT_POST_FLAG_SET,&err); //����־���BIT0��1
					
						}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,">")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfCMGS,                          //���� 
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
						}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+QNTP:")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfQNTP,                          //���� 
								 (OS_OPT   )OS_OPT_POST_1,                            //����Ϣ������highest priority task waiting
								 (OS_ERR  *)&err);                                       //���ش�������
					
						}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"POWERED DOWN"))){//POWERED DOWN
										
											 OSSemPost((OS_SEM  *)&SemOfQPOWD,                          //���� Sem
											 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
											 (OS_ERR  *)&err);                                             //���ش�������

						}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"RDY"))){// 
										
											 OSSemPost((OS_SEM  *)&SemOfRDY,                          //���� Sem
											 (OS_OPT   )OS_OPT_POST_1,                                   //����Ϣ������highest priority task waiting
											 (OS_ERR  *)&err);                                             //���ش�������

							}else{
							if(stm32_flat.ATMODECOH!=1){printf("MAILCENTER_ELSE:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
							}
						}
				
				
				}
				
				if(err!=OS_ERR_NONE){
				if(stm32_flat.ATMODECOH!=1){printf("MAILCENTER_ERR:%d\n",err);}}//debug_mark
			
			}


			OSTimeDly(40,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
    }
			
}

/*
*********************************************************************************************************
*                  ����ģ����̲���
*********************************************************************************************************
*/

//void  PostKeySign (uint8_t *msg ,uint16_t msg_size )
//{
//			OS_ERR  err;

//					/* ������Ϣ������ AppTaskDisposeKeySign */
//					OSTaskQPost ((OS_TCB      *)&AppTaskDisposeKeySignTCB,                 //Ŀ������Ŀ��ƿ�
//											 (void        *)msg,            												 //��Ϣ����
//											 (OS_MSG_SIZE  )msg_size,  //��Ϣ����
//											 (OS_OPT       )OS_OPT_POST_FIFO,                //������������Ϣ���е���ڶ�
//											 (OS_ERR      *)&err);                           //���ش�������

//	
//}

/*
*********************************************************************************************************
*                 ������ʱɨ��Ķ�ʱ��
*********************************************************************************************************
*/

static void AppTaskNormalKeyScan(void)
{
	OS_ERR  err;
	//extern TEMP_Message_Storage tmp_sms[1];////��ʱ�洢��Ϣ������ʾʹ��
	//CPU_SR_ALLOC();  //ʹ�õ��ٽ�Σ��ڹ�/���ж�ʱ��ʱ����ú꣬�ú������Ͷ���һ���ֲ���
//	uint8_t key1times;//key1�����´���
		while(DEF_TRUE){
//			Key_RefreshState(&Key1);//ˢ�°���״̬
//			key1times = Key_AccessTimes(&Key1,KEY_ACCESS_READ);
//			if(key1times!=0)//���������¹�
//			{
//				LED1_TOGGLE;

//				
//				KEEP_LCD_ON_FOR_A_WHILES();//��������60s
//				//printf("sizeofu8:%d\n",sizeof(uint8_t));
//				//Print_CPUusage();
//				
//				Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
//			}
			
			///TTP226���̴���


				u8 keyget = ttp226_read();
				if(keyget!=0){
					KEEP_LCD_ON_FOR_A_WHILES();//��������60s
				printf("keyget:%d\r\n",keyget);//debug
					switch(keyget){
						case 1://1
							GUI_StoreKeyMsg(GUI_KEY_RIGHT, 1);//
							break;
						case 2://2
							GUI_SendKeyMsg(GUI_KEY_ESCAPE, 1);
							break;
						case 4://3
							GUI_StoreKeyMsg(GUI_KEY_DOWN, 1);
							break;
						case 8://4
							GUI_StoreKeyMsg(GUI_KEY_UP, 1);
							break;
						case 16://5
							GUI_StoreKeyMsg(GUI_KEY_LEFT, 1);
							break;
						case 32://6
							GUI_StoreKeyMsg(GUI_KEY_ENTER, 1);
							break;
						case 64://7  ��������
								stm32_flat.STM32isAllowStopMode=1;//�������ֹͣģʽ
								ResetLCDBKtime(60);
							break;
						case 128://8  ���ⳣ��
							stm32_flat.STM32isAllowStopMode=0;//���������ֹͣģʽ
							ResetLCDBKtime(0);
							break;
						case 83://1+2+7+5 
							SENTCTR =sent_EC20_INIT;  //�趨��������
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
							break;
						case 72://7+4 �ر�EC20
							SENTCTR =sent_EC20_Power_off;  //�趨��������
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������
							break;
						case 68://7+3 ����EC20
							SENTCTR =sent_EC20_Power_on;  //�趨��������
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������
							break;
						default:
							break;
					
					
					
					}
					
					
				}
			

			
			
			
			
			
			
			OSTimeDly(200,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
			
		}
		
}
/*
*********************************************************************************************************
*                 GetTaskMailCenterTCB
*********************************************************************************************************
*/
//OS_TCB * GetTaskMailCenterTCB (void){
//return &AppTaskMailCenterTCB;
//}

/*
*********************************************************************************************************
*                 ��ʾCPU��ʹ����
*********************************************************************************************************
*/


void Print_CPUusage (void)
{
	
//		OS_ERR         err;
	//CPU_TS_TMR     ts_int;
//	CPU_INT16U     version;
//	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();

	
	//(void)p_arg;

	//version = OSVersion(&err);                          //��ȡuC/OS�汾��
	
//  cpu_clk_freq = BSP_CPU_ClkFreq();                   //��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
	
	
	
		//ts_int = CPU_IntDisMeasMaxGet ();                 //��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              //�����ٽ�Σ����⴮�ڴ�ӡ�����

		//printf("��ǰ��Ծ�Ĵ���:%ld\n",WM_GetActiveWindow());//debug_mark
	
    //printf ( "\nuC/OS�汾��:V%d.%02d.%02d\n",
    //         version / 10000, version % 10000 / 100, version % 100 );
    
//		printf ( "CPU��Ƶ:%d MHz\n", cpu_clk_freq / 1000000 );  
		
		//printf ( "����ж�ʱ�䣺%d us\r\n", 
		//				ts_int / ( cpu_clk_freq / 1000000 ) ); 

//		printf ( "������ʱ��:%d us \n",OSSchedLockTimeMax / ( cpu_clk_freq / 1000000 ) );		

			printf("�����л�����:%d\n",OSTaskCtxSwCtr);	
		
//		printf ( "CPUʹ����:%d\r\n",
//             OSStatTaskCPUUsage  );  
//		
//		printf ( "CPU���ʹ����:%d\r\n",OSStatTaskCPUUsageMax );
						 



//	  printf ( "AppTaskGUI_CPU:%d,%d\r\n", 
//		         AppTaskGUITCB.CPUUsage, AppTaskGUITCB.CPUUsageMax ); 	
    printf ( "AppTaskGUITCB:%d,%d\r\n", 
		         AppTaskGUITCB.StkUsed, AppTaskGUITCB.StkFree ); 
						 ///////////////////////////
						 
		printf ( "AppTaskEC20CtrTCB:%d,%d\r\n", 
		         AppTaskEC20CtrTCB.StkUsed, AppTaskEC20CtrTCB.StkFree ); 		
						 
	  printf ( "AppTaskEC20TmrTCB:%d,%d\r\n", 
		         AppTaskEC20TmrTCB.StkUsed, AppTaskEC20TmrTCB.StkFree ); 		
						 
	  printf ( "AppTaskMailCenterTCB:%d,%d\r\n", 
		         AppTaskMailCenterTCB.StkUsed, AppTaskMailCenterTCB.StkFree ); 		 
		
    printf ( "AppTaskDisposeKeySignTCB:%d,%d\r\n", 
		         AppTaskDisposeKeySignTCB.StkUsed, AppTaskDisposeKeySignTCB.StkFree ); 
		
		OS_CRITICAL_EXIT();                               //�˳��ٽ��
	
}





/*
*********************************************************************************************************
*                 ����������ֵ�ź���
*********************************************************************************************************
*/

void Creat_SEMs (void){
	OS_ERR  os_err;
		/* ������ֵ�ź��� EC20 OK */
	OSSemCreate(&SemOfRec_EC20_OK,"SemOfRec_EC20_OK",0,&os_err); 
	OSSemCreate(&SemOfATV,"SemOfATV",0,&os_err); 
	OSSemCreate(&SemOfCMEE,"SemOfCMEE",0,&os_err); 
	OSSemCreate(&SemOfGSN,"SemOfGSN",0,&os_err); 
	OSSemCreate(&SemOfCPIN,"SemOfCPIN",0,&os_err); 
	OSSemCreate(&SemOfCSQ,"SemOfCSQ",0,&os_err); 
	OSSemCreate(&SemOfCREG,"SemOfCREG",0,&os_err); 
	OSSemCreate(&SemOfCGREG,"SemOfCGREG",0,&os_err); 
	OSSemCreate(&SemOfCOPS,"SemOfCOPS",0,&os_err); 
	OSSemCreate(&SemOfCMGF,"SemOfCMGF",0,&os_err); 
	OSSemCreate(&SemOfCPMSS,"SemOfCPMSS",0,&os_err); 
	OSSemCreate(&SemOfCMGD,"SemOfCMGD",0,&os_err);
	OSSemCreate(&SemOfCMGDINDEX,"SemOfCMGDINDEX",0,&os_err);
	OSSemCreate(&SemOfCMGS,"SemOfCMGS",0,&os_err);
	OSSemCreate(&SemOfCMGSOK,"SemOfCMGSOK",0,&os_err);	
	OSSemCreate(&SemOfQNTP,"SemOfQNTP",0,&os_err);	
	OSSemCreate(&SemOfCSCS,"SemOfCSCS",0,&os_err);	
	OSSemCreate(&SemOfCFUN,"SemOfCFUN",0,&os_err);	
	OSSemCreate(&SemOfQPOWD,"SemOfQPOWD",0,&os_err);	
	OSSemCreate(&SemOfRDY,"SemOfRDY",0,&os_err);	
	
//	OSSemCreate(&SemOfCPMSG,"SemOfCPMSG",0,&os_err); 
	//OSSemCreate(&SemOfCMGR1,"SemOfCMGR1",0,&os_err); //��ȡ����ͷ
	//OSSemCreate(&SemOfCMGR2,"SemOfCMGR2",0,&os_err); //��ȡ��������
	
  OSFlagCreate ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,        //ָ���¼���־���ָ��
                (CPU_CHAR     *)"FLAG FlagGropOf_CMGR",  //�¼���־�������
                (OS_FLAGS      )0,                //�¼���־��ĳ�ʼֵ
                (OS_ERR       *)&os_err);					  //���ش�������



	
}


/*
*********************************************************************************************************
*                 tmr1_callback //��ʱ�� 1 �Ļص�����
*********************************************************************************************************

*/


void tmr1_callback(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	if(stm32_flat.LCD_KEEP_ON){
	
	}else if(stm32_flat.LCD_TURNOFF_TIME_COUNT>1){
	stm32_flat.LCD_TURNOFF_TIME_COUNT--;
	}
	
	 if(stm32_flat.EC20AUTO_INIT_COUNT>1){
		 stm32_flat.STM32isAllowStopMode_MASTER=0;
		stm32_flat.EC20AUTO_INIT_COUNT--;
	}//�����Զ���ʼ��EC20����ʱ
	
	if(stm32_flat.EC20AUTO_INIT_COUNT==1){
		stm32_flat.STM32isAllowStopMode_MASTER=1;
			if(net_flags.EC20_INIT==0){//���EC20δ��ʼ�������ʼ��
				if(stm32_flat.EC20AUTO_INIT==1){
					if(net_flags.IS_EC20_OFF){//���EC20�ѹر�,����EC20
						if(!net_flags.EC20_IS_TURNNING_ON){
							SENTCTR =sent_EC20_Power_on;  //�趨��������
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������
						}

					
					}else{//���򣬳�ʼ��EC20
						SENTCTR =sent_EC20_INIT;  //�趨��������
						OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//��������	
					
					}
			}
		}
	
	}


	
	
	if(stm32_flat.LCD_TURNOFF_TIME_COUNT==1){
		
		stm32_flat.LCD_TURNOFF_TIME_COUNT=0;
		LCD_LED=0;//�ر�LCD����
		mySTART_STM32STOPMODE();//test ����ֹͣģʽ
	}	


}





/*
*********************************************************************************************************
*                 �Ƚ��ַ���1���Ƿ�ͬʱ�����ַ���2���ַ���3
*********************************************************************************************************


uint8_t Compare_String1_with_23 (char* str1,char* str2,char* str3)
{
	
if((strstr((const char*)str1,(const char*)str2))&&(strstr((const char*)str1,(const char*)str3))){
		return 1;
		}else{return 0;}
		
}

*/

/*
*********************************************************************************************************
*                 GUI������
*********************************************************************************************************
*/

static  void  AppTaskGUI (void  *p_arg)
{
	OS_ERR err;
		WM_SetCreateFlags(WM_CF_MEMDEV);//ʹ���ڴ滭���ڣ�����GUI_INIT֮ǰ
		GUI_Init();				//UCGUI ��ʼ��
	
	ucGUIMainAppinit();
  while(1)
  {
		//LED1_TOGGLE;
    ucGUIMainApp();
		OSTimeDly(40,OS_OPT_TIME_DLY,&err);//��һЩ���Ĵ���������
  }
}

/*
*********************************************************************************************************
*               stm32 ���Ʊ�����ʼ����ֵ
*********************************************************************************************************
*/
void STM32CRTFLAT_Init(void){

	stm32_flat.ATMODECOH=0;//stm32 ֱ��ת��debug���ڵ�ATָ�EC20 �Ƿ����
	stm32_flat.EC20AUTO_INIT=1;//�����Զ���ʼ��EC20
		stm32_flat.EC20AUTO_INIT_COUNT=40;//�����Զ���ʼ��EC20
	stm32_flat.LCD_KEEP_ON=0;//�������ⳣ��
	stm32_flat.LCD_TURNOFF_TIME_COUNT=30;//����ʱ�䵹��ʱ
	stm32_flat.LCD_TURNOFF_TIME=30;//����ʱ��
	stm32_flat.STM32isAllowStopMode=1;//����������ģʽ
	stm32_flat.STM32isAllowStopMode_MASTER=1;
	stm32_flat.StopModeState=0;
	


}
