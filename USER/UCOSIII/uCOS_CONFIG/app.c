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


//多值信号量
OS_SEM SemOfKey;          //标志KEY1是否被单击的多值信号量
OS_SEM SemOfRec_EC20_OK;          //多值信号量 EC20:OK
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
OS_SEM SemOfCFUN;//飞行模式 全功能模式
OS_SEM SemOfQPOWD;//
OS_SEM SemOfRDY;//

OS_FLAG_GRP FlagGropOf_CMGR;     //声明事件标志组CMGR rec sms
extern type_LINKEC20_USART_Rx  LINKEC20_USART_Rx;//EC20当前接收的完整消息
extern uint8_t LINKEC20_USART_RECEIVE_BUF[LINKEC20_USART_RECEIVE_BUF_SIZE];//LINKEC20串口DMA接收缓冲区
//extern uint8_t LINKEC20_USART_SEND_BUF[LINKEC20_USART_SEND_BUF_SIZE];//LINKEC20串口DMA发送缓冲区

extern uint8_t LINKEC20_USART_Use_DMA_Tx_Flag;
enum EC20_SENT_CTR SENTCTR;//EC20发送控制指令标记
EC20_SENT_CTR_Parameters SENTCTR_PARA;////发送控制指令传入参数
extern NET_FLAGS net_flags;
KEY Key1;

//uint8_t ATMODECOH=0;//stm32 直接转发debug串口的AT指令到EC20 是否回显
STM32_CTR_FLAGS stm32_flat;

/* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   			AppTaskStartTCB;
static  CPU_STK  			AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

  OS_TCB       AppTaskDisposeKeySignTCB;
static  CPU_STK      AppTaskDisposeKeySignSTK[APP_TASK_DisposeKeySignSTK_SIZE];//按键消息处理任务

static  OS_TCB       AppTaskNormalKeyScanTCB;
static  CPU_STK      AppTaskNormalKeyScanSTK[APP_TASK_NormalKeyScan_STK_SIZE];

OS_TCB     AppTaskMailCenterTCB;
static  CPU_STK      AppTaskMailCenterSTK[APP_TASK_MailCenter_STK_SIZE];//EC20串口消息处理任务

static  OS_TCB       AppTaskGUITCB;
static  CPU_STK      AppTaskGUISTK[APP_TASK_GUI_STK_SIZE];//GUI主任务

static  OS_TCB       AppTaskEC20TmrTCB;//EC20后台定时发送心跳消息任务
static  CPU_STK      AppTaskEC20TmrSTK[APP_TASK_EC20Tmr_STK_SIZE];

OS_TCB       AppTaskEC20CtrTCB;//EC20消息处理任务任务
static  CPU_STK      AppTaskEC20CtrSTK[APP_TASK_EC20Ctr_STK_SIZE];

static uint32_t EC20TMR1_STICK = 1000;//心跳消息任务间隔时间,默认1s

OS_MUTEX mutex_ec20sent;                         //声明 EC20串口发送使用的互斥信号量

unsigned int insCtrFLAG;//EC20控制指令标记

OS_TMR tmr1; //定时器 1
//extern uint32_t LCD_TURNOFF_TIME;//LCD屏幕背光 关闭倒计时 单位秒
//extern uint32_t LCD_TURNOFF_TIME_COUNT;
//extern u8 LCD_KEEP_ON;//LCD屏幕背光常亮
//extern u8 STM32isAllowStopMode;//是否允许停止模式。
//extern u8 StopModeState;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  		  (void  *p_arg);  //开始任务
static  void  AppTaskCreate         (void); // 子任务创建函数

static 	void 	AppTaskNormalKeyScan(void);//普通键盘扫描任务
static  void  AppTaskDisposeKeySign ( void * p_arg );//多值信号量键盘扫描任务
static  void  AppTaskMailCenter ( void * p_arg );//多值信号量响应EC20串口任务
static  void  AppTaskGUI (void  *p_arg);  //更新屏幕任务
static  void  AppTaskEC20Tmr ( void * p_arg );// EC20后台定时发送心跳消息任务
static  void  AppTaskEC20Ctr ( void * p_arg );//  EC20 发送，处理消息任务
///其他函数：
void Print_CPUusage (void);///显示CPU等使用量
void Creat_SEMs (void);///批量创建多值信号量
void PostKeySign (uint8_t *msg ,uint16_t msg_size );//接收键盘字符//串口模拟键盘测试
uint8_t Compare_String1_with_23 (char* str1,char* str2,char* str3); //比较字符串1，是否同时含有字符串2和字符串3
void tmr1_callback(void *p_tmr, void *p_arg);//定时器1 回调
void ResetLCDtime(void);//开启背光60s
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

		//INTX_DISABLE();//自定义关中断方法
    
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

		
////		/* 创建内存管理对象 mem */
//		OSMemCreate ((OS_MEM      *)&INTERNAL_MEM,         //指向内存管理对象
//								 (CPU_CHAR    *)"INTERNAL_MEM",         //命名内存管理对象
//								 (void        *)Internal_RamMemp,           //内存分区的首地址
//								 (OS_MEM_QTY   )INTERNAL_MEM_NUM,                 //内存分区中内存块数目
//								 (OS_MEM_SIZE  )INTERNAL_MEMBLOCK_SIZE,               //内存块的字节数目
//								 (OS_ERR      *)&err);            					//返回错误类型

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

	
		KeyCreate(&Key1,GetPinStateOfKey1); //创建初始化KEY1
	
			/* 创建ucGUI任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskGUITCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task UPDATE GUI",                             //任务名称
                 (OS_TASK_PTR ) AppTaskGUI,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_GUI_PRIO,                         						//任务的优先级
                 (CPU_STK    *)&AppTaskGUISTK[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_GUI_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 0u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 100u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&os_err);                                       //返回错误类型
						
		Creat_SEMs();///批量创建多值信号量
								 
		/* 创建循环扫描EC20串口接收任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskMailCenterTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"AppTaskMailCenter",                             //任务名称
                 (OS_TASK_PTR ) AppTaskMailCenter,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) AppTaskMailCenterSign_PRIO,                  	//任务的优先级
                 (CPU_STK    *)&AppTaskMailCenterSTK[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_MailCenter_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_MailCenter_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 150u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&os_err);                                       //返回错误类型
	
			/* 创建多值信号量 SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,    //指向信号量变量的指针
               (CPU_CHAR    *)"SemOfKey",    //信号量的名字
               (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
               (OS_ERR      *)&os_err);         //错误类型
								 
			/* 创建多值信号量按键扫描任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskDisposeKeySignTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task DisposeKeySign",                             //任务名称
                 (OS_TASK_PTR ) AppTaskDisposeKeySign,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_TaskDisposeKeySign_PRIO,                         						//任务的优先级
                 (CPU_STK    *)&AppTaskDisposeKeySignSTK[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_DisposeKeySignSTK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_DisposeKeySignSTK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 10u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 100u,                                         //任务的时间片节拍数//暂时设定200 做调试
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&os_err);                                       //返回错误类型
								 
		/* 创建 EC20消息处理任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskEC20CtrTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"AppTaskEC20Ctr",                             //任务名称
                 (OS_TASK_PTR ) AppTaskEC20Ctr,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_EC20Ctr_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskEC20CtrSTK[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_EC20Ctr_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_EC20Ctr_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 0u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 250u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&os_err);                                       //返回错误类型
								 

								 
			/* 创建普通按键扫描任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskNormalKeyScanTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App Task DisposeKeySign",                             //任务名称
                 (OS_TASK_PTR ) AppTaskNormalKeyScan,                                			//任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_NormalKeyScan_PRIO,                         						//任务的优先级
                 (CPU_STK    *)&AppTaskNormalKeyScanSTK[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_NormalKeyScan_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_NormalKeyScan_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 0u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 60u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&os_err);                                       //返回错误类型
								 
			//创建定时器 1  
							OSTmrCreate((OS_TMR* )&tmr1, // 定时器 1 (1)
							(CPU_CHAR* )"tmr1", // 定时器名字
							(OS_TICK )10, //100*10=1000ms//初始化定时器的延迟值。
							(OS_TICK )10, //100*10=1000ms//重复周期
							(OS_OPT )OS_OPT_TMR_PERIODIC, // 周期模式 周期定时器
							(OS_TMR_CALLBACK_PTR)tmr1_callback, // 定时器 1 回调函数
							(void* )0, // 参数为 0
							(OS_ERR* )&os_err); // 返回的错误码
							
			OSTmrStart(&tmr1,&os_err); //开启定时器 1		

		OSTaskDel ( & AppTaskStartTCB, & os_err );                     //删除起始任务本身，该任务不再运行
								 
//		    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
//						// LED1_TOGGLE;
//					OSTimeDly(1000,OS_OPT_TIME_DLY,&os_err);

//    }

}





/*
*********************************************************************************************************
*                  多值信号量实现串口键盘扫描
*********************************************************************************************************
*/

static  void  AppTaskDisposeKeySign ( void * p_arg )
{
    OS_ERR         err;
//		OS_MSG_SIZE    msg_size;
	extern NET_FLAGS net_flags;//网络状态
	extern uint8_t DEBUG_USART_RECEIVE_BUF[DEBUG_USART_RECEIVE_BUF_SIZE];//debug串口DMA接收缓冲区
	extern type_DEBUG_USART_Rx  DEBUG_USART_Rx;//当前接收的完整消息


	
		char * pMsg;
	pMsg = (char *)DEBUG_USART_Rx.DEBUG_USART_RECEIVE;
	

		CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变

    while (DEF_TRUE) {

						/* 阻塞任务，等待任务消息 */
						OSTaskSemPend ((OS_TICK      )0,                    //无期限等待
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //没有消息就阻塞任务
											  (CPU_TS        *)0,                  //返回消息被发布的时间戳
											  (OS_ERR        *)&err);                //返回错误类型

			
//		/* 阻塞任务，等待任务消息 */
//		pMsg = OSTaskQPend ((OS_TICK        )0,                    //无期限等待
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //没有消息就阻塞任务
//											  (OS_MSG_SIZE   *)&msg_size,            //返回消息长度
//											  (CPU_TS        *)0,                  //返回消息被发布的时间戳
//											  (OS_ERR        *)&err);                //返回错误类型
			
			OSTimeDly(30,OS_OPT_TIME_DLY,&err);//延迟一段时间等待接收完成。
			
			OS_CRITICAL_ENTER();                          //进入临界段，
				

        /* 关闭接收DMA  */
        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,DISABLE);  
        /* 清除标志位 Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_DMA_Rx_Flags);  
			
				memset(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//拷贝前，清空原来接收...
        memcpy(DEBUG_USART_Rx.DEBUG_USART_RECEIVE,DEBUG_USART_RECEIVE_BUF,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));  //拷贝数据
				memset(DEBUG_USART_RECEIVE_BUF,0,DEBUG_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//拷贝完，清空BUF缓存数据
			
        /* 重新设置传输数据长度 */
        DMA_SetCurrDataCounter(DEBUG_USART_DMA_Rx_Tream,DEBUG_USART_RECEIVE_BUF_SIZE);  
        /* 打开DMA */
        DMA_Cmd(DEBUG_USART_DMA_Rx_Tream,ENABLE);  
				
				OS_CRITICAL_EXIT(); 	
			
			

			//LED1_TOGGLE ;                            //切换LED1的亮灭状态
			KEEP_LCD_ON_FOR_A_WHILES();//开启背光60s
			
			if(pMsg[0]){
				int len = strlen(pMsg);
				if(len==1||len==3){//1位键盘模式
				
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
					SENTCTR =sent_EC20_INIT;  //设定发送任务
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
							
						break;
					case 88:// X
						
						if(	net_flags.EC20_AIR_MODE==0){
							stm32_flat.EC20AUTO_INIT=0;//主动关EC20后不允许开机自动初始化EC20
								SENTCTR =sent_EC20_AIR_Mode_On;  //设定发送任务
								OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
							//EC20_AIR_Mode_On();//开启飞行模式	
						}else{
							
								SENTCTR =sent_EC20_AIR_Mode_Off;  //设定发送任务
								OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
								stm32_flat.EC20AUTO_INIT=1;
							
							//EC20_AIR_Mode_Off();
						}
						
					 
							
						break;
					case 67:// C
					SENTCTR_PARA.Para1=0;
					SENTCTR =sent_EC20_SyncLocalTime;  //设定发送任务
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
					
//					sscanf(strstr((const char*)DEBUG_USART_Rx.DEBUG_USART_RECEIVE,(const char*)"C "),"C %d,%d,%d,%d,%d,%d,%d,%d",&hour,&min,&sec,&ampm\
//						,&year,&month,&date,&week);//
//					RTC_Set_Time(hour,min,sec,ampm);
//					RTC_Set_Date(year,month,date, week);
					
						//EC20_UPDATA_SMSBOX_INDEX(1);
					
						break;
	
					default:
						break;

				}//end switch(pmsg1)
			}else if(len ==4){//2位指令模式
					if(pMsg[0]=='A'&&\
					pMsg[1]=='1'){
					stm32_flat.ATMODECOH=1;
					printf("A1\r\n");
					}else if(pMsg[0]=='A'&&\
					pMsg[1]=='0'){
					stm32_flat.ATMODECOH=0;
					printf("A0\r\n");
					}else if(pMsg[0]=='P'&&\
					pMsg[1]=='1'){//开EC20
					SENTCTR =sent_EC20_Power_on;  //设定发送任务
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送
						
					}else if(pMsg[0]=='P'&&\
					pMsg[1]=='0'){//关EC20
					SENTCTR =sent_EC20_Power_off;  //设定发送任务
					OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送
						
					}else if(pMsg[0]=='L'&&\
					pMsg[1]=='1'){//背光正常
					stm32_flat.STM32isAllowStopMode=1;//允许进入停止模式
					ResetLCDBKtime(60);
						printf("L1\r\n");
					}else if(pMsg[0]=='L'&&\
					pMsg[1]=='0'){//背光常亮
					stm32_flat.STM32isAllowStopMode=0;//不允许进入停止模式
					ResetLCDBKtime(0);
						printf("L0\r\n");
					}
				
				
			}else{//len>1 AT模式
			
					if(pMsg[0]=='A'&&\
					pMsg[1]=='T'){
					char *   p_mem_blk;
					OS_CRITICAL_ENTER();
					p_mem_blk = mymalloc(0,300);
					memset(p_mem_blk,0,300*sizeof(uint8_t));//清空数据
					u8 sign =sscanf(strstr((const char*)pMsg,(const char*)"AT "),"AT <%[^>]",p_mem_blk);
					OS_CRITICAL_EXIT();
						if(sign==1){
						
						OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
						OS_CRITICAL_ENTER();
						sprintf(p_mem_blk,"%s\r\n",p_mem_blk);
						EC20_SendCMD(p_mem_blk);//同步NTP并更新本地时间
						OS_CRITICAL_EXIT();
						OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
						
						}

				   myfree(0,p_mem_blk);//退还内存块
				}
				
			
			}
			
			}
			
				
		
			OSTimeDly(40,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务

    }
			
}

/*
*********************************************************************************************************
*                  EC20 发送，处理消息任务
*********************************************************************************************************
*/

static  void  AppTaskEC20Ctr ( void * p_arg )
{	
	OS_ERR    err;
	static EC20TmrCTR_FLAGS ec20Tmrctr_flags;
	extern NET_FLAGS net_flags;//网络状态
	///设定初始值
  net_flags.EC20_AIR_MODE=1;//开启飞行模式
	ec20Tmrctr_flags.EC20TMR1_ON=0;
	///设定初始值
	(void)p_arg;
	//创建互斥信号量mutex_ec20sent
	OSMutexCreate(&mutex_ec20sent,"mutex_ec20sent",&err);
	
    while (DEF_TRUE) {
			
						/* 阻塞任务，等待任务消息 */
						OSTaskSemPend ((OS_TICK      )0,                    //无期限等待
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //没有消息就阻塞任务
											  (CPU_TS        *)0,                  //返回消息被发布的时间戳
											  (OS_ERR        *)&err);                //返回错误类型
			
			stm32_flat.STM32isAllowStopMode=0;//不允许进入停止模式
				
					if(err==OS_ERR_NONE){//
				
						switch((u8)SENTCTR){
								
									case sent_EC20_ATOK:
											{
												OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_ATOK();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
												if(res==1){printf("sent_EC20_ATOK\n");}	
												
											}
										break;
									case sent_EC20_Power_on:
											{
												stm32_flat.EC20AUTO_INIT=1;//允许开机自动初始化EC20
												stm32_flat.STM32isAllowStopMode=0;//不允许进入停止模式
												OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_Power_on();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
												if(res==1){printf("EC20_Power_ON\n");}	
												stm32_flat.STM32isAllowStopMode=1;//允许进入停止模式
											}
										break;
									case sent_EC20_Power_off:
											{
												stm32_flat.EC20AUTO_INIT=0;//主动关EC20后不允许开机自动初始化EC20
												stm32_flat.STM32isAllowStopMode=0;//不允许进入停止模式
												OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_Power_Off();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
												if(res==1){printf("sent_EC20_Power_OFF\n");}	
												stm32_flat.STM32isAllowStopMode=1;//允许进入停止模式
											}
										break;
									case sent_EC20_Get_COPS:
										{
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_Get_COPS();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
													if(res==1){printf("EC20_Get_COPS\n");}	
											}
										break;
									case sent_EC20_Read1SMS:
										{
													
													if(SENTCTR_PARA.Para1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
														u8 res = EC20_Read1SMS(SENTCTR_PARA.Para1,SENTCTR_PARA.Para3,0);//tmp_sms_idx 默认是0，看看以后要不要添加预读功能
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
													
												}
										break;
										case sent_EC20_Delete_SMS:
										{
													
													if(SENTCTR_PARA.Para1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
														//u8 res = EC20_Delete_SMS(u8 mem ,u8 delflag,u16 index);
														u8 res = EC20_Delete_SMS(SENTCTR_PARA.Para1 ,SENTCTR_PARA.Para2,SENTCTR_PARA.Para3);
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
												}
										break;
									case sent_EC20_SENT_SMS:
										{

													if(*SENTCTR_PARA.Parachar1!=0){
														OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
														u8 res = EC20_SENT_SMS((const char*)SENTCTR_PARA.Parachar1,(const char*)SENTCTR_PARA.Parachar2);//const char* des,const char* text
														OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
														if(res==1){
															
														//printf("EC20_Get_COPS\n");}	
															}
													}
												}
										break;
									case sent_EC20_SyncLocalTime:
										{
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
											u8 res = EC20_SyncLocalTime(SENTCTR_PARA.Para1);//const char* des,const char* text
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
											if(res==1){
											//printf("EC20_Get_COPS\n");}	
												}
													
												}
										break;

									case sent_EC20_AIR_Mode_On:
										{
											OSMutexPend(&mutex_ec20sent,3200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待3200ms。
													u8 res = EC20_AIR_Mode_On();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
													if(res==1){printf("EC20_AIR_Mode_On\n");}	
											}
										break;
									case sent_EC20_AIR_Mode_Off:
											{
												OSMutexPend(&mutex_ec20sent,3200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_AIR_Mode_Off();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
													if(res==1){printf("EC20_AIR_Mode_Off\n");}	
											}
										break;
									case sent_EC20_INIT:
											{
												OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_INIT();
												OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
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
											OSMutexPend(&mutex_ec20sent,200,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待200ms。
													u8 res = EC20_Net_Reg_INIT();
											OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
													if(res==1){printf("EC20_Net_Reg_INIT\n");}	
											}
										break;
									default:
										break;
								
								}//END switch(SENTCTR)	
						
					}else{
						printf("OSMutexPend err:%d\n",err);//debug_mark
					
					}//end if(err==OS_ERR_NONE){//申请成功
			

			
			
			
	//飞行模式处理 开始
		if((net_flags.EC20_AIR_MODE==1)||(net_flags.IS_EC20_OFF==1)){//如果开启飞行模式 或者 ec20关闭
			if(ec20Tmrctr_flags.EC20TMR1_ON==1){//如果定时任务1已开启
				OS_ERR delerr;
						//删除定时任务1
							OSTaskDel ( &AppTaskEC20TmrTCB, & delerr );
							if(delerr==OS_ERR_NONE){
							ec20Tmrctr_flags.EC20TMR1_ON=0;
								 UpdataMidWin0();
								//printf("EC20TMR1_del");
							}
							

				}
		
		
		 }else{//如果关闭飞行模式
			 if(net_flags.EC20_INIT==1){//是否已初始化
						if(ec20Tmrctr_flags.EC20TMR1_ON==0){//如果定时任务1未开启
										//开启定时任务1
										/* 创建 EC20后台定时发送心跳消息任务 */
							OS_ERR crerr;
								OSTaskCreate((OS_TCB     *)&AppTaskEC20TmrTCB,                             //任务控制块地址
														 (CPU_CHAR   *)"AppTaskEC20Tmr",                             //任务名称
														 (OS_TASK_PTR ) AppTaskEC20Tmr,                                //任务函数
														 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
														 (OS_PRIO     ) APP_TASK_EC20Tmr_PRIO,                         //任务的优先级
														 (CPU_STK    *)&AppTaskEC20TmrSTK[0],                          //任务堆栈的基地址
														 (CPU_STK_SIZE) APP_TASK_EC20Tmr_STK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
														 (CPU_STK_SIZE) APP_TASK_EC20Tmr_STK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
														 (OS_MSG_QTY  ) 0u,                                         //任务可接收的最大消息数
														 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
														 (void       *) 0,                                          //任务扩展（0表不扩展）
														 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
														 (OS_ERR     *)&crerr);                                       //返回错误类型
														 
											if(crerr==OS_ERR_NONE){
											ec20Tmrctr_flags.EC20TMR1_ON=1;
												//printf("EC20TMR1_cr");
											}
														 
						}
			 }

		
		}//飞行模式处理	结束

		stm32_flat.STM32isAllowStopMode=1;//允许进入停止模式
		
			OSTimeDly(50,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务

    }
			
}

/*
*********************************************************************************************************
*                             EC20后台定时发送心跳消息任务
*********************************************************************************************************
*/
static  void  AppTaskEC20Tmr ( void * p_arg )
{
	extern SMS_SHOW sms_show;
	OS_ERR err;
	EC20TMR1_STICK = 3000;//心跳消息任务间隔时间,默认1s

		while(DEF_TRUE){
			OSTimeDly(EC20TMR1_STICK/2,OS_OPT_TIME_DLY,&err);//延迟设定好的间隔时间
						 //先申请发送的互斥信号量mutex_ec20sent
			OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待500ms。
			if(err==OS_ERR_NONE){//申请成功
				EC20_ATOK();

		OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
			}
			
		if(net_flags.EC20_ATOK==1){
			
							 //先申请发送的互斥信号量mutex_ec20sent
					OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待500ms。
					if(err==OS_ERR_NONE){//申请成功
						EC20_Get_Csq();

				OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
					}
	
					OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待500ms。
					if(err==OS_ERR_NONE){//申请成功

						EC20_Get_COPS();

				OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
					}
					
//				OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待500ms。
//					if(err==OS_ERR_NONE){//申请成功

//						EC20_UPDATA_SMSBOX_INDEX(sms_show.mem);
//					OSMutexPost(&mutex_ec20sent,OS_OPT_POST_NO_SCHED,&err);//释放互斥信号量，mutex_ec20sent 
//					}
		
		}



					
				
				UpdataMidWin0();

		OSTimeDly(EC20TMR1_STICK/2,OS_OPT_TIME_DLY,&err);//延迟设定好的间隔时间
		}

	


	
}



/*
*********************************************************************************************************
*                  扫描DMA缓存数组 EC20串口接收3 
*********************************************************************************************************
*/

static  void  AppTaskMailCenter ( void * p_arg )
{
    OS_ERR   err;
		memset(LINKEC20_USART_RECEIVE_BUF,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));
		CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变

    while (DEF_TRUE) {
			
//							/* 阻塞任务，等待任务消息 */
//						OSTaskSemPend ((OS_TICK      )0,                    //无期限等待
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //没有消息就阻塞任务
//											  (CPU_TS        *)0,                  //返回消息被发布的时间戳
//											  (OS_ERR        *)&err);                //返回错误类型
			
					//默认是一次接收到一条返回的数据，所以需要保证不同任务之间的发送有一定时间的间隔
					//如果需要一次接收多条数据，这里需要加一个将多条数据分离的函数。

			if(LINKEC20_USART_RECEIVE_BUF[0]!=0x00){//接收到数据
	
				//OSMutexPend(&mutex_ec20sent,500,OS_OPT_PEND_BLOCKING,0,&err);//申请互斥信号量mutex_ec20sent，阻塞等待500ms。接收数据时不允许发送
				//接收任务的优先级设定得比发送任务高，所以不需要加互斥信号量了。
				OSTimeDly(50,OS_OPT_TIME_DLY,&err);//
				uint8_t times =0;//最长等待990ms
				while(!(strstr((const char*)LINKEC20_USART_RECEIVE_BUF,(const char*)"OK"))&&(times<17)){
				OSTimeDly(55,OS_OPT_TIME_DLY,&err);//如果没有OK结尾 延迟一段时间等待接收完成。
				times++;
				}
				
				OS_CRITICAL_ENTER();                          //进入临界段，
				
				 /* 关闭接收DMA  */
        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,DISABLE);  
			/* 清除标志位 Clear DMA Transfer Complete Flags */
        DMA_ClearFlag(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_DMA_Rx_Flags);  

        /* 获得接收帧帧长 */
       // LINKEC20_USART_Rx.LINKEC20_USART_RxLen = LINKEC20_USART_RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream);  
				
				memset(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//拷贝前，清空原来接收的LINKEC20_USART_RECEIVE数据
        memcpy(LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,LINKEC20_USART_RECEIVE_BUF,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));  //拷贝数据
				memset(LINKEC20_USART_RECEIVE_BUF,0,LINKEC20_USART_RECEIVE_BUF_SIZE*sizeof(uint8_t));//拷贝完，清空BUF缓存数据
			
        /* 重新设置传输数据长度 */
        DMA_SetCurrDataCounter(LINKEC20_USART_DMA_Rx_Tream,LINKEC20_USART_RECEIVE_BUF_SIZE);  
        /* 打开DMA */
        DMA_Cmd(LINKEC20_USART_DMA_Rx_Tream,ENABLE); 
					OS_CRITICAL_EXIT(); 	
				
				if(stm32_flat.ATMODECOH==1){//stm32 直接转发debug串口的AT指令到EC20 是否回显
					printf("EC20ECOH:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
				}
				
				if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"OK"))){
					
					//先处理来短信，来电话等常用的被动消息

					
					if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+COPS?")){//"AT+COPS?","OK"
							OSSemPost((OS_SEM  *)&SemOfCOPS,                          //发布 SemOfCOPS
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CSQ")){//"AT+CSQ","OK"
							OSSemPost((OS_SEM  *)&SemOfCSQ,                          //发布 SemOfCSQ
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGR=")){//EC20读取短消息(有时???)会分两条数据返回，所以第一条不需要判断OK结尾
						
//								OSSemPost((OS_SEM  *)&SemOfCMGR1,                          //发布 SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1|OS_OPT_POST_NO_SCHED,        //把消息发布到
//									 (OS_ERR  *)&err);                                       //返回错误类型
//								OSSemPost((OS_SEM  *)&SemOfCMGR2,                          //发布 SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
//									 (OS_ERR  *)&err);                                       //返回错误类型
					
						OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x03,OS_OPT_POST_FLAG_SET,&err); //将标志组的 BIT0\BIT1 置1
						
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+CMGS:")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfCMGSOK,                          //发布 
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"ATV")){//"ATV","OK"
						
							OSSemPost((OS_SEM  *)&SemOfATV,                          //发布 SemOfATV
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGF")){//"AT+CMGF=1","OK"
						
							OSSemPost((OS_SEM  *)&SemOfCMGF,                          //发布 SemOfCMGF
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGD=?")){//AT+CPMS="SM","SM","ME" 
						
							OSSemPost((OS_SEM  *)&SemOfCMGDINDEX,                          //发布 SemOfCMGDINDEX
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CSCS=")){//AT+CSCS="UCS2"
						
							OSSemPost((OS_SEM  *)&SemOfCSCS,                          //发布 
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPMS=")){//AT+CPMS="SM","SM","ME"  
						
							OSSemPost((OS_SEM  *)&SemOfCPMSS,                          //发布 SemOfCPMS
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CFUN")){//:AT+CFUN?
						
							OSSemPost((OS_SEM  *)&SemOfCFUN,                          //发布 SemOfCFUN
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGD=")){
							//printf("recCMEE_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCMGD,                          //发布 SemOfCMGD
								 (OS_OPT   )OS_OPT_POST_1,                           //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                     //返回错误类型
					
					}
//					else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPMS?")){//
//						
//							OSSemPost((OS_SEM  *)&SemOfCPMSG,                          //发布 SemOfCPMS
//								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
//								 (OS_ERR  *)&err);                                       //返回错误类型
//					
//					}
					else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMEE=2")){
							//printf("recCMEE_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCMEE,                          //发布 SemOfCMEE
								 (OS_OPT   )OS_OPT_POST_1,                           //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                     //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+GSN")){
						//printf("recGSN_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfGSN,                          //发布 SemOfGSN
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到队列highest priority task waiting。
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CPIN")){
						//printf("recCPIN_ok\n");//debug_mark
							OSSemPost((OS_SEM  *)&SemOfCPIN,                          //发布 SemOfCPIN
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CREG")){
							OSSemPost((OS_SEM  *)&SemOfCREG,                          //发布 SemOfCREG
								 (OS_OPT   )OS_OPT_POST_1,                           //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                     //返回错误类型
					
					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CGREG")){
							OSSemPost((OS_SEM  *)&SemOfCGREG,                          //发布 SemOfCGREG
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"POWERED DOWN"))){//POWERED DOWN
										
								 OSSemPost((OS_SEM  *)&SemOfQPOWD,                          //发布 Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"AT\r\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //发布 Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //发布 Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型

					}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"ATE1\r\nOK\r\n"))){
							//printf("recAT_ok\n");//debug_mark
								 OSSemPost((OS_SEM  *)&SemOfRec_EC20_OK,                          //发布 Sem
								 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                             //返回错误类型

					}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+CMGR: ")){//EC20读取短消息(有时???)会分两条数据返回，所以第一条不需要判断OK结尾
						
//								OSSemPost((OS_SEM  *)&SemOfCMGR2,                          //发布 SemOfCMGR
//									 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
//									 (OS_ERR  *)&err);                                       //返回错误类型
							OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x02,OS_OPT_POST_FLAG_SET,&err); //将标志组的BIT1置1
					
						}else{//else ok
										

										if(stm32_flat.ATMODECOH!=1){
										printf("MAILCENTER_OK_ELSE:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
										}

									}
						
						
						

				
				}else{//else without OK
					
					if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"AT+CMGR")){//EC20读取短消息(有时???)会分两条数据返回，所以第一条不需要判断OK结尾
						
//							OSSemPost((OS_SEM  *)&SemOfCMGR1,OS_OPT_POST_1, &err);  
							OSFlagPost ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,0x01,OS_OPT_POST_FLAG_SET,&err); //将标志组的BIT0置1
					
						}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,">")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfCMGS,                          //发布 
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
						}else if(strstr((char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,"+QNTP:")){//+CMGS: 69
						
							OSSemPost((OS_SEM  *)&SemOfQNTP,                          //发布 
								 (OS_OPT   )OS_OPT_POST_1,                            //把消息发布到highest priority task waiting
								 (OS_ERR  *)&err);                                       //返回错误类型
					
						}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"POWERED DOWN"))){//POWERED DOWN
										
											 OSSemPost((OS_SEM  *)&SemOfQPOWD,                          //发布 Sem
											 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
											 (OS_ERR  *)&err);                                             //返回错误类型

						}else if((strstr((const char*)LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE,(const char*)"RDY"))){// 
										
											 OSSemPost((OS_SEM  *)&SemOfRDY,                          //发布 Sem
											 (OS_OPT   )OS_OPT_POST_1,                                   //把消息发布到highest priority task waiting
											 (OS_ERR  *)&err);                                             //返回错误类型

							}else{
							if(stm32_flat.ATMODECOH!=1){printf("MAILCENTER_ELSE:%s\n",LINKEC20_USART_Rx.LINKEC20_USART_RECEIVE);//debug_mark
							}
						}
				
				
				}
				
				if(err!=OS_ERR_NONE){
				if(stm32_flat.ATMODECOH!=1){printf("MAILCENTER_ERR:%d\n",err);}}//debug_mark
			
			}


			OSTimeDly(40,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
    }
			
}

/*
*********************************************************************************************************
*                  串口模拟键盘测试
*********************************************************************************************************
*/

//void  PostKeySign (uint8_t *msg ,uint16_t msg_size )
//{
//			OS_ERR  err;

//					/* 发布消息到任务 AppTaskDisposeKeySign */
//					OSTaskQPost ((OS_TCB      *)&AppTaskDisposeKeySignTCB,                 //目标任务的控制块
//											 (void        *)msg,            												 //消息内容
//											 (OS_MSG_SIZE  )msg_size,  //消息长度
//											 (OS_OPT       )OS_OPT_POST_FIFO,                //发布到任务消息队列的入口端
//											 (OS_ERR      *)&err);                           //返回错误类型

//	
//}

/*
*********************************************************************************************************
*                 按键定时扫描的定时器
*********************************************************************************************************
*/

static void AppTaskNormalKeyScan(void)
{
	OS_ERR  err;
	//extern TEMP_Message_Storage tmp_sms[1];////临时存储信息，供显示使用
	//CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
//	uint8_t key1times;//key1被按下次数
		while(DEF_TRUE){
//			Key_RefreshState(&Key1);//刷新按键状态
//			key1times = Key_AccessTimes(&Key1,KEY_ACCESS_READ);
//			if(key1times!=0)//按键被按下过
//			{
//				LED1_TOGGLE;

//				
//				KEEP_LCD_ON_FOR_A_WHILES();//开启背光60s
//				//printf("sizeofu8:%d\n",sizeof(uint8_t));
//				//Print_CPUusage();
//				
//				Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
//			}
			
			///TTP226键盘处理


				u8 keyget = ttp226_read();
				if(keyget!=0){
					KEEP_LCD_ON_FOR_A_WHILES();//开启背光60s
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
						case 64://7  背光正常
								stm32_flat.STM32isAllowStopMode=1;//允许进入停止模式
								ResetLCDBKtime(60);
							break;
						case 128://8  背光常亮
							stm32_flat.STM32isAllowStopMode=0;//不允许进入停止模式
							ResetLCDBKtime(0);
							break;
						case 83://1+2+7+5 
							SENTCTR =sent_EC20_INIT;  //设定发送任务
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
							break;
						case 72://7+4 关闭EC20
							SENTCTR =sent_EC20_Power_off;  //设定发送任务
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送
							break;
						case 68://7+3 开启EC20
							SENTCTR =sent_EC20_Power_on;  //设定发送任务
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送
							break;
						default:
							break;
					
					
					
					}
					
					
				}
			

			
			
			
			
			
			
			OSTimeDly(200,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
			
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
*                 显示CPU等使用量
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

	//version = OSVersion(&err);                          //获取uC/OS版本号
	
//  cpu_clk_freq = BSP_CPU_ClkFreq();                   //获取CPU时钟，时间戳是以该时钟计数
	
	
	
		//ts_int = CPU_IntDisMeasMaxGet ();                 //获取最大关中断时间

		OS_CRITICAL_ENTER();                              //进入临界段，避免串口打印被打断

		//printf("当前活跃的窗口:%ld\n",WM_GetActiveWindow());//debug_mark
	
    //printf ( "\nuC/OS版本号:V%d.%02d.%02d\n",
    //         version / 10000, version % 10000 / 100, version % 100 );
    
//		printf ( "CPU主频:%d MHz\n", cpu_clk_freq / 1000000 );  
		
		//printf ( "最大中断时间：%d us\r\n", 
		//				ts_int / ( cpu_clk_freq / 1000000 ) ); 

//		printf ( "最大调度时间:%d us \n",OSSchedLockTimeMax / ( cpu_clk_freq / 1000000 ) );		

			printf("任务切换次数:%d\n",OSTaskCtxSwCtr);	
		
//		printf ( "CPU使用率:%d\r\n",
//             OSStatTaskCPUUsage  );  
//		
//		printf ( "CPU最大使用率:%d\r\n",OSStatTaskCPUUsageMax );
						 



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
		
		OS_CRITICAL_EXIT();                               //退出临界段
	
}





/*
*********************************************************************************************************
*                 批量创建多值信号量
*********************************************************************************************************
*/

void Creat_SEMs (void){
	OS_ERR  os_err;
		/* 创建多值信号量 EC20 OK */
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
	//OSSemCreate(&SemOfCMGR1,"SemOfCMGR1",0,&os_err); //读取短信头
	//OSSemCreate(&SemOfCMGR2,"SemOfCMGR2",0,&os_err); //读取短信详情
	
  OSFlagCreate ((OS_FLAG_GRP  *)&FlagGropOf_CMGR,        //指向事件标志组的指针
                (CPU_CHAR     *)"FLAG FlagGropOf_CMGR",  //事件标志组的名字
                (OS_FLAGS      )0,                //事件标志组的初始值
                (OS_ERR       *)&os_err);					  //返回错误类型



	
}


/*
*********************************************************************************************************
*                 tmr1_callback //定时器 1 的回调函数
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
	}//开机自动初始化EC20倒计时
	
	if(stm32_flat.EC20AUTO_INIT_COUNT==1){
		stm32_flat.STM32isAllowStopMode_MASTER=1;
			if(net_flags.EC20_INIT==0){//如果EC20未初始化，则初始化
				if(stm32_flat.EC20AUTO_INIT==1){
					if(net_flags.IS_EC20_OFF){//如果EC20已关闭,则开启EC20
						if(!net_flags.EC20_IS_TURNNING_ON){
							SENTCTR =sent_EC20_Power_on;  //设定发送任务
							OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送
						}

					
					}else{//否则，初始化EC20
						SENTCTR =sent_EC20_INIT;  //设定发送任务
						OSTaskSemPost(&AppTaskEC20CtrTCB,OS_OPT_POST_NONE,&err);//启动发送	
					
					}
			}
		}
	
	}


	
	
	if(stm32_flat.LCD_TURNOFF_TIME_COUNT==1){
		
		stm32_flat.LCD_TURNOFF_TIME_COUNT=0;
		LCD_LED=0;//关闭LCD背光
		mySTART_STM32STOPMODE();//test 进入停止模式
	}	


}





/*
*********************************************************************************************************
*                 比较字符串1，是否同时含有字符串2和字符串3
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
*                 GUI主任务
*********************************************************************************************************
*/

static  void  AppTaskGUI (void  *p_arg)
{
	OS_ERR err;
		WM_SetCreateFlags(WM_CF_MEMDEV);//使用内存画窗口，放在GUI_INIT之前
		GUI_Init();				//UCGUI 初始化
	
	ucGUIMainAppinit();
  while(1)
  {
		//LED1_TOGGLE;
    ucGUIMainApp();
		OSTimeDly(40,OS_OPT_TIME_DLY,&err);//留一些节拍处理别的任务
  }
}

/*
*********************************************************************************************************
*               stm32 控制变量初始化赋值
*********************************************************************************************************
*/
void STM32CRTFLAT_Init(void){

	stm32_flat.ATMODECOH=0;//stm32 直接转发debug串口的AT指令到EC20 是否回显
	stm32_flat.EC20AUTO_INIT=1;//开机自动初始化EC20
		stm32_flat.EC20AUTO_INIT_COUNT=40;//开机自动初始化EC20
	stm32_flat.LCD_KEEP_ON=0;//不允许背光常亮
	stm32_flat.LCD_TURNOFF_TIME_COUNT=30;//背光时间倒计时
	stm32_flat.LCD_TURNOFF_TIME=30;//背光时间
	stm32_flat.STM32isAllowStopMode=1;//允许开启节能模式
	stm32_flat.STM32isAllowStopMode_MASTER=1;
	stm32_flat.StopModeState=0;
	


}
