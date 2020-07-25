/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
//#include "./Public_Func/delay.h"
#include  <includes.h>

/*********************************************************************
*
*       Global data
*/
static  OS_SEM  DispSem;
static  OS_SEM  EventSem;

//static  OS_SEM  KeySem;
//static  int     KeyPressed;
//static  char    KeyIsInited;


/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/

int GUI_X_GetTime(void) { 
	OS_ERR  err;
	return ((int)OSTimeGet(&err));   //获取系统时间,此处时间单元为1ms
 
}

void GUI_X_Delay(int ms) { 
//  int tEnd = OS_TimeMS + ms;
//  while ((tEnd - OS_TimeMS) > 0);
	//delay_ms(ms);
	
//		CPU_INT32U ticks;
//	ticks = (ms * 1000) / OSCfg_TickRate_Hz; 
	OS_ERR  err;
	
  OSTimeDly(ms, OS_OPT_TIME_DLY, &err);

	
}

/*
*********************************************************************************************************
*                                    MULTITASKING INTERFACE FUNCTIONS
*
* Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
*             which means you have more than one thread using the uC/GUI API.  In this case the #define 
*             GUI_OS 1   needs to be in GUIConf.h
*********************************************************************************************************
*/

void  GUI_X_InitOS (void)
{ 
	OS_ERR     err;

	/* 用于资源共享 cnt = 1*/
    OSSemCreate((OS_SEM    *)&DispSem,
                (CPU_CHAR  *)"DispSem",
                (OS_SEM_CTR )1,
                (OS_ERR    *)&err);
	/* 用于事件触发 cnt = 0*/
	OSSemCreate((OS_SEM    *)&EventSem,
                (CPU_CHAR  *)"EventSem",
                (OS_SEM_CTR )0,
                (OS_ERR    *)&err);
}


void  GUI_X_Lock (void)
{ 
  OS_ERR     err;
	   
	OSSemPend((OS_SEM *)&DispSem,
			  (OS_TICK )0,
			  (OS_OPT  )OS_OPT_PEND_BLOCKING,
			  0,
			  (OS_ERR *)&err);
}


void  GUI_X_Unlock (void)
{ 
  OS_ERR     err;

	OSSemPost((OS_SEM *)&DispSem,
	          (OS_OPT  )OS_OPT_POST_1,
	          (OS_ERR *)&err);
}


U32  GUI_X_GetTaskId (void) 
{ 
    return ((U32)(OSTCBCurPtr->Prio));
}


/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

void GUI_X_Init(void) {}


/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/


void GUI_X_ExecIdle (void) 
{
    GUI_X_Delay(1);
}
/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
