/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     BOARD SUPPORT PACKAGE (BSP)
*
*                                       IAR Development Kits
*                                              on the
*
*                                    STM32F429II-SK KICKSTART KIT
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <stdarg.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_def.h>
#include  <lib_ascii.h>


#include "stm32f4xx.h"

#include "./BSP/LED/led.h" 
#include "./BSP/usart/bsp_debug_usart.h"
#include "./BSP/usart/bsp_linkec20_usart.h"
#include "./BSP/key/bsp_key.h" 
#include "./BSP/SPI/spi_flash.h"
#include "./BSP/1.44TFT/LCD_SPI_hard.h"
#include "./BSP/1.44TFT/LCD_drive_hard.h"
#include "GUI.H"
#include "./BSP/RTC/rtc.h"
#include "./BSP/TIMER/timer.h"
#include "TTP226.h"
#include "bsp_adc.h"
#include "ec20.h"


/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              DEFINE
*********************************************************************************************************
*/
#define   BSP_OK              (uint16_t)     0				/*  (00) Succeeded                              */
#define 	BSP_OS              (uint16_t)0x0001				/*  (01) The emWin drive cannot work            */ 
#define 	BSP_SDRAM           (uint16_t)0x0002				/*  (02) The sdram drive cannot work            */  
#define 	BSP_GTP             (uint16_t)0x0004				/*  (03) The gt9xx drive cannot work            */ 
#define 	BSP_TPAD            (uint16_t)0x0008				/*  (04) The touch pad drive cannot work        */ 
#define 	BSP_SD              (uint16_t)0x0010				/*  (05) The SD Card drive cannot work          */ 
#define 	BSP_SPIFLASH        (uint16_t)0x0020				/*  (06) The SPI FLASH drive cannot work        */ 
#define 	BSP_GUI             (uint16_t)0x0040				/*  (07) The emWin drive cannot work            */ 
#define 	BSP_WM8978          (uint16_t)0x0080				/*  (08) The WM8978 drive cannot work           */
#define 	BSP_ALLOC           (uint16_t)0x0100				/*  (09) The memory alloc fail                  */
#define 	BSP_XBF             (uint16_t)0x0200				/*  (10) The XBF Font cannot work               */
#define   BSP_RTC             (uint16_t)0x0400        /*  (11) The RTC cannot work                    */
#define   BSP_NETWORK         (uint16_t)0x0800        /*  (12) The Network cannot work                */
#define   BSP_CAMERA          (uint16_t)0x1000        /*  (13) The Camera cannot work                 */
/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

extern unsigned short  int  bsp_result;
/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                          (void);

CPU_INT32U  BSP_CPU_ClkFreq                   (void);

void        BSP_Tick_Init                     (void);



/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */

