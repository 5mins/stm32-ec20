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

#define LCD_BITSPERPIXEL (16) /*lcd 颜色深度*///可能影响颜色转换函数
#define LCD_CONTROLLER (0) /*lcd 控制器的具体型号*/
#define LCD_FIXEDPALETTE (565) /*RGB 颜色位数*/  //将数值设置为 0 表示使用一个颜色查询表而不是一个固定设色板模式。这样LCD_PHYSCOLORS 宏必须定义。
#define LCD_SWAP_RB (1) /*红蓝反色交换*/
//#define LCD_INIT_CONTROLLER() my_LCD_Init(); /*底层初始化函数，自己写的，而非源码自带，这一步非常重要*/

//#define LCD_PHYSCOLORS 0xffffff, 0xaaaaaa, 0x555555, 0x000000 


 
#endif /* LCDCONF_H */

	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
