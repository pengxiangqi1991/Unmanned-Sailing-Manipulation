#ifndef __gprs_H__ 
#define __gprs_H__

#include <stm32f10x_lib.h>

//#include "type.h"
#include "usart.h"
#include "delay.h"
//#include "timer.h"

extern u8 runxun_ser1; //��������
extern u8 Flag_Buf1_Rec;
extern u16 Start_weizi,End_weizi;
extern u16 Second_Int ;
extern u16 q,r;
 
u8 Hand(u8 *a); //��������
void Find_dw_TEXT(void);
void Second_AT_Command(u8 *b,u16 wait_time);
void SecondDataOnline(u8 *b,u16 wait_time);	 //����ӣ����ж����Զ����ӵķ��ͺ���--����ERROR����������IP��ַ

void Second_AT_ERROR(u8 *b,u16 wait_time);
void Set_Text_Mode(void);
void Check_Message_rec(void);
void Wait_CREG(void);
void Send_LR(void);
void array_copy(u8 *p1,u8 *p2);
void ConnectDNS(u8 *dns); //���ӵ�����

u16 Find_Str_Pos(u8 *str);  //���ؽ��յ������ݡ����ڵ�����  ���� %IPDATA:3,"120ABC" ����11

u8 GetCommandFrPC(u16 data_pos);// 
extern u16 GoalCompsDri; 
extern u16 GoalSpeed; 	  //B4λ
extern u16 GoalSteer; 	  //C4λ
void GetCmd_PC_Compass(u8 *buf);
unsigned char CommandFrPC_Sum_check(u8 *temp_buf);//У��PCָ���


extern u8 GprsState;
void GprsCommand(void);
void WaitRegS(void);
void CheckGprsAck(void);
void CheckRegS(void);
void CheckOk(void);
#endif
