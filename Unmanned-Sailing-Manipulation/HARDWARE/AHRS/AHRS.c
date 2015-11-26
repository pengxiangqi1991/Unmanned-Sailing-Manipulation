
/* 
编写者：lisn3188
网址：www.chiplab7.com
作者E-mail：lisn3188@163.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2012-05-25
测试： 本程序已在第七实验室的mini IMU上完成测试
功能：
使用串行中断程序接收mini IMU 的数据。
移植者需在自己编写对应的串口初始化程序，并开启接收中断。
在中断子程序中 需要做相应的改动

为了使用这个程序，你需要：
1.初始化你的串口，将波特率与模块的波特率设置成一致的
2.开启串口的接收中断，并参考以下程序编写中断子程序
  参考 void USART2_IRQHandler(void) 
3.在主程序的循环体中，定时调用子程序：
  void UART1_CommandRoute(void)
  以检查是否收到新的数据帧
  所有的姿态数据和ADC值都会在 UART1_CommandRoute
  中进行更新。
4.使用 以下值会自动更新：

float 	yaw,  //偏航角
		pitch,//俯仰
		roll, //滚转
		alt,  //高度
		tempr,//温度
		press;//气压
int16_t ax, ay, az;//加速度计
int16_t gx, gy, gz;//陀螺仪
int16_t hx, hy, hz;//磁力计
------------------------------------
*/
 
#include "ahrs.h"
#include "led.h"
//#include "usart2.h"		



//uart reicer flag
#define b_uart_head  0x80  //收到A5 头 标志位
#define b_rx_over    0x40  //收到完整的帧标志
// USART Receiver buffer
#define RX_BUFFER_SIZE 100 //接收缓冲区字节数

unsigned char rx_buffer[RX_BUFFER_SIZE]; //接收数据缓冲区
volatile unsigned char rx_wr_index; //缓冲写指针
volatile unsigned char RC_Flag;  //接收状态标志字节


float 	yaw,  //偏航角
		pitch,//俯仰
		roll, //滚转
		alt,  //高度
		tempr,//温度
		press;//气压
//在接收完一帧IMU姿态报告后，调用这个子程序来取出姿态数据
void UART1_Get_IMU(void)
{
	int16_t temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	yaw=(float)temp / 10.0f; //偏航角
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	pitch=(float)temp / 10.0f;//俯仰
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	roll=(float)temp / 10.0f;//滚转

	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	alt=(float)temp / 10.0f;//高度
	
	temp = 0;
	temp = rx_buffer[10];
	temp <<= 8;
	temp |= rx_buffer[11];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	tempr=(float)temp / 10.0f;//温度
	
	temp = 0;
	temp = rx_buffer[12];
	temp <<= 8;
	temp |= rx_buffer[13];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	press=(float)temp * 10.0f;//气压

}

//ADC值
int16_t ax, ay, az;//加速度计
int16_t gx, gy, gz;//陀螺仪
int16_t hx, hy, hz;//磁力计
//在接收一帧ReportMotion 后调用这个子程序来取出ADC数据
void UART1_Get_Motion(void)
{
	int16_t temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ax=temp;//加速度计 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ay=temp;//加速度计 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	az=temp;//加速度计 Z轴的ADC值
	
	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gx=temp;//陀螺仪 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[10];
	temp <<= 8;
	temp |= rx_buffer[11];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gy=temp;//陀螺仪 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[12];
	temp <<= 8;
	temp |= rx_buffer[13];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gz=temp;//陀螺仪 Z轴的ADC值
	
	temp = 0;
	temp = rx_buffer[14];
	temp <<= 8;
	temp |= rx_buffer[15];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hx=temp;//磁力计 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[16];
	temp <<= 8;
	temp |= rx_buffer[17];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hy=temp;//磁力计 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[18];
	temp <<= 8;
	temp |= rx_buffer[19];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hz=temp;//磁力计 Z轴的ADC值
}

 //在接收完一帧IMU姿态报告后，调用这个子程序来取出/*GPS*/数据
float longitude,latitude; //接受的经纬度（单位：度）
void UART1_Get_GPS(void)
{
	u32 temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	temp <<= 8;
	temp |= rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	longitude=(float)temp / 100000.0f; //longitude经度
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	temp <<= 8;
	temp |= rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	latitude=(float)temp / 100000.0f; //latitude纬度
	
}
	u8 svnum;					//可见卫星数
	float speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
void UART1_Get_ALL(void)
{
//void UART1_Get_IMU(void)
	int16_t temp;
	u32 temp_gps;
	u16 temp_uint;	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	yaw=(float)temp / 10.0f; //偏航角
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	pitch=(float)temp / 10.0f;//俯仰
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	roll=(float)temp / 10.0f;//滚转

//	temp = 0;
//	temp = rx_buffer[8];
//	temp <<= 8;
//	temp |= rx_buffer[9];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	alt=(float)temp / 10.0f;//高度
//	
//	temp = 0;
//	temp = rx_buffer[10];
//	temp <<= 8;
//	temp |= rx_buffer[11];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	tempr=(float)temp / 10.0f;//温度
//	
//	temp = 0;
//	temp = rx_buffer[12];
//	temp <<= 8;
//	temp |= rx_buffer[13];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	press=(float)temp * 10.0f;//气压

////在接收一帧ReportMotion 后调用这个子程序来取出ADC数据
//void UART1_Get_Motion(void)
/*IMUpersec  占用两个字节14  15，没用解析使用，以后可添加（估计是更新速率）*/	
	temp = 0;
	temp = rx_buffer[16];
	temp <<= 8;
	temp |= rx_buffer[17];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ax=temp;//加速度计 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[18];
	temp <<= 8;
	temp |= rx_buffer[19];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ay=temp;//加速度计 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[20];
	temp <<= 8;
	temp |= rx_buffer[21];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	az=temp;//加速度计 Z轴的ADC值
	
	temp = 0;
	temp = rx_buffer[22];
	temp <<= 8;
	temp |= rx_buffer[23];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gx=temp;//陀螺仪 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[24];
	temp <<= 8;
	temp |= rx_buffer[25];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gy=temp;//陀螺仪 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[26];
	temp <<= 8;
	temp |= rx_buffer[27];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gz=temp;//陀螺仪 Z轴的ADC值
	
	temp = 0;
	temp = rx_buffer[28];
	temp <<= 8;
	temp |= rx_buffer[29];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hx=temp;//磁力计 X轴的ADC值
	
	temp = 0;
	temp = rx_buffer[30];
	temp <<= 8;
	temp |= rx_buffer[31];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hy=temp;//磁力计 Y轴的ADC值
	
	temp = 0;
	temp = rx_buffer[32];
	temp <<= 8;
	temp |= rx_buffer[33];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hz=temp;//磁力计 Z轴的ADC值
// //在接收完一帧IMU姿态报告后，调用这个子程序来取出/*GPS*/数据
//void UART1_Get_GPS(void)
	
	temp_gps = 0;
	temp_gps = rx_buffer[34];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[35];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[36];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[37];
	longitude=(float)temp_gps / 100000.0f; //longitude经度
	
	temp_gps = 0;
	temp_gps = rx_buffer[38];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[39];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[40];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[41];
	latitude=(float)temp_gps / 100000.0f; //latitude纬度


//	u8 svnum;					//可见卫星数
//	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
	temp_uint = 0;
	temp_uint = rx_buffer[42];
	svnum = temp_uint;					//可见卫星数

	temp_uint = 0;
	temp_uint = rx_buffer[43];
	temp_uint <<= 8;
	temp_uint |= rx_buffer[44];
	speed = temp_uint/1000.0;					//地面速率,放大了1000倍,实际除以1000.单位:0.001公里/小时

}


 //--校验当前接收到的一帧数据是否 与帧校验字节一致
unsigned char Sum_check(void)
{ 
  unsigned char i;
  u32 checksum=0; 
  for(i=0;i<rx_buffer[0]-2;i++)
   checksum+=rx_buffer[i];
  if((checksum%256)==rx_buffer[rx_buffer[0]-2])
   return(0x01); //Checksum successful
  else
   return(0x00); //Checksum error
}



//--这个子程序需要在主程序中 定时调用,以检查 串口是否接收完一帧数据
void UART1_CommandRoute(void)
{
	if(RC_Flag&b_rx_over)
	{  //已经接收完一帧?
		RC_Flag&=~b_rx_over; //清标志先
		if(Sum_check())
		{ 
//		u2_printf(rx_buffer);
			//校验通过
//			if(rx_buffer[1]==0xA1)
//			{ //UART1_ReportIMU 的数据
//				UART1_Get_IMU();	//取数据
//			}
//			if(rx_buffer[1]==0xA2)
//			{ //UART1_ReportMotion 的数据
//				UART1_Get_Motion();	 //取数据
//			} 
//			if(rx_buffer[1]==0xA4) //REGPS  解析GPS的数据
//			{ 
//				UART1_Get_GPS();	 //取GPS数据
//			} 
			if(rx_buffer[1]==0xA3) //REALL  解析ALL的数据
			{ 
				UART1_Get_ALL();	 //取ALL数据
			} 
		
		}//校验是否通过?
	}
}


