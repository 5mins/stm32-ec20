#ifndef __PYINPUT_H
#define __PYINPUT_H
#include "./Public_Func/sys.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ƴ�����뷨 ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

//ƴ�������ƴ���Ķ�Ӧ��
typedef struct
{
  u8 *py_input;//������ַ���
  u8 *py;	   //��Ӧ��ƴ��
  u8 *pymb;	   //���
}py_index;

#define MAX_MATCH_PYMB 	10	//���ƥ����

//ƴ�����뷨
typedef struct
{
  u8(*getpymb)(u8 *instr);			//�ַ���������ȡ����
  py_index *pymb[MAX_MATCH_PYMB];	//�����λ��
}pyinput;

extern pyinput t9;
u8 str_match(u8*str1,u8*str2);
u8 get_matched_pymb(u8 *strin,py_index **matchlist);
u8 get_pymb(u8* str);
void test_py(u8 *inputstr);
#endif








