/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : LCDConf_1375_C8_C320x240.h
Purpose     : Sample configuration file
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*                   General configuration of LCD
*
**********************************************************************
*/

#define LCD_XSIZE      (128)   /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE      (128)   /* Y-resolution of LCD, Logical coor. */

#define LCD_BITSPERPIXEL (16) /*lcd ��ɫ���*///����Ӱ����ɫת������
#define LCD_CONTROLLER (0) /*lcd �������ľ����ͺ�*/
#define LCD_FIXEDPALETTE (565) /*RGB ��ɫλ��*/  //����ֵ����Ϊ 0 ��ʾʹ��һ����ɫ��ѯ�������һ���̶���ɫ��ģʽ������LCD_PHYSCOLORS ����붨�塣
#define LCD_SWAP_RB (1) /*������ɫ����*/
//#define LCD_INIT_CONTROLLER() my_LCD_Init(); /*�ײ��ʼ���������Լ�д�ģ�����Դ���Դ�����һ���ǳ���Ҫ*/

//#define LCD_PHYSCOLORS 0xffffff, 0xaaaaaa, 0x555555, 0x000000 


 
#endif /* LCDCONF_H */

	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
