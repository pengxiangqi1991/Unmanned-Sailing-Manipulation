#ifndef __USART3_H
#define __USART3_H
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//����2��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2013/2/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	     
 


#define USART3_RX_EN 			1					//0,������;1,����.
typedef signed short int int16_t;  //�ڴ��ļ��м���Ķ���  20130414


extern u16 WindDirDegree,FanPosDegree;

//////////////������Ʒ��ĵ��ת�����ٶ��Լ�"�е㲻ת��"λ�õĵ������ֵ////////////////////////////////	 
#define SAILSPEED 150
#define MIDDLEVALUE 1500
////////////////////////////////////////////////////////////////////////////////// 	   

void USART3_Init(u32 pclk2,u32 bound);				//����2��ʼ�� 
void u3_printf(char* fmt, ...);
//void Send_Hex(u8 achar);
//void CLR_Buf0(void);
void USART3_Send_ASCII(u8 *b);
void USART3_Put_String(unsigned char *Str);
void USART3_CommandRoute(void);
void UART3_ReportAllData(int16_t yaw,int16_t pitch,int16_t roll,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec,
						int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,int16_t hx,int16_t hy,int16_t hz,
						u32 longitude,u32 latitude,u8 svnum,u16 speed);  
void UART3_RepWindFanDir(float wind_dir_degree,float fan_dir_degree); 
void SteerSailAdjust(u16 temp_steer_val,u16 sail_goal_pos,u16 motor_speed);	
void InitMotor(void); //��ʼ�����
#endif
















