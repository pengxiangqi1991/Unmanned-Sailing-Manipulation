#ifndef __AHRS_H
#define __AHRS_H	 
#include "sys.h"  

//uart reicer flag
#define b_uart_head  0x80  //收到A5 头 标志位
#define b_rx_over    0x40  //收到完整的帧标志
// USART Receiver buffer
#define RX_BUFFER_SIZE 100 //接收缓冲区字节数

typedef signed short int int16_t;  //在此文件中加入的定义  20130414

extern unsigned char rx_buffer[RX_BUFFER_SIZE]; //接收数据缓冲区
extern volatile unsigned char rx_wr_index; //缓冲写指针
extern volatile unsigned char RC_Flag;  //接收状态标志字节

//在接收完一帧IMU姿态报告后，调用这个子程序来取出姿态数据
extern void UART1_Get_IMU(void);

//--这个子程序需要在主程序中 定时调用,以检查 串口是否接收完一帧数据
void UART1_CommandRoute(void);
extern float 	yaw,  //偏航角
		pitch,//俯仰
		roll, //滚转
		alt,  //高度
		tempr,//温度
		press;//气压
extern int16_t ax, ay, az;//加速度计
extern int16_t gx, gy, gz;//陀螺仪
extern int16_t hx, hy, hz;//磁力计

extern float longitude,latitude;
extern u8 svnum;					//可见卫星数
extern float speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 










#endif









