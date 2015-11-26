#ifndef __USART3_H
#define __USART3_H
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//串口2驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2013/2/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	     
 


#define USART3_RX_EN 			1					//0,不接收;1,接收.
typedef signed short int int16_t;  //在此文件中加入的定义  20130414


extern u16 WindDirDegree,FanPosDegree;

//////////////定义控制帆的电机转动的速度以及"中点不转动"位置的电调控制值////////////////////////////////	 
#define SAILSPEED 150
#define MIDDLEVALUE 1500
////////////////////////////////////////////////////////////////////////////////// 	   

void USART3_Init(u32 pclk2,u32 bound);				//串口2初始化 
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
void InitMotor(void); //初始化电调
#endif
















