#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "led.h" 
#include "exti.h"
#define USART3_b_uart_head  0x80  //收到A5 头 标志位
#define USART3_b_rx_over    0x40  //收到完整的帧标志
#define USART3_RX_BUFFER_SIZE 100 //接收缓冲区字节数
unsigned char USART3_rx_buffer[USART3_RX_BUFFER_SIZE]; //接收数据缓冲区
volatile unsigned char USART3_rx_wr_index; //缓冲写指针
volatile unsigned char USART3_RC_Flag;  //接收状态标志字节

#define FANERRORRANGE 30	  //帆的允许误差范围（扩大十倍的角度）

//串口发送缓存区 	
#ifdef USART3_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
void USART3_IRQHandler(void)
{
	u8 res;	    
	//	  res= USART3->DR; //读取接收到的字节
	if(USART3->SR&(1<<5))//接收到数据
	{
	  res= USART3->DR; //读取接收到的字节
// USART3->DR = res;
//	while((USART3->SR&0X40)==0);//等待发送结束
	
	  if(res==0xa5) 
	  {
		USART3_RC_Flag|=USART3_b_uart_head; //如果接收到A5 置位帧头标专位
	    USART3_rx_buffer[USART3_rx_wr_index++]=res; //保存这个字节.
	  }
	  else if(res==0x5a)
	   { 
		   if(USART3_RC_Flag&USART3_b_uart_head) //如果上一个字节是A5 那么认定 这个是帧起始字节
		     { USART3_rx_wr_index=0;  //重置 缓冲区指针
			   USART3_RC_Flag&=~USART3_b_rx_over; //这个帧才刚刚开始收
		     }
		     else //上一个字节不是A5
			  USART3_rx_buffer[USART3_rx_wr_index++]=res;
		     USART3_RC_Flag&=~USART3_b_uart_head; //清帧头标志
	   }
	   else
	   { 
	     USART3_rx_buffer[USART3_rx_wr_index++]=res;
		 USART3_RC_Flag&=~USART3_b_uart_head;
		 if(USART3_rx_wr_index==USART3_rx_buffer[0]) //收够了字节数.
	     {  
			USART3_rx_wr_index = 0;
			USART3_RC_Flag|=USART3_b_rx_over; //置位 接收完整的一帧数据
	      }
	   }
	
	  if(USART3_rx_wr_index==USART3_RX_BUFFER_SIZE) //防止缓冲区溢出
	  USART3_rx_wr_index--;
	  /* Clear the USART2 Recive interrupt 20130407移植未清除串口1中断标志位，是否有影响？待验证  彭向岐*/
	//  USART_ClearITPendingBit(USART2, USART_IT_RXNE); //清中断标志.
	}


//	u8 res;
//	if(USART3->SR&(1<<5))//接收到数据
//	{	 
//		LED1 = 0;	  
//		res = USART3->DR;
//		while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
//		USART3->DR = res;      
//	}
}   
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART3_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<8;   	//使能PORTG口时钟  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO状态设置
	GPIOG->CRH|=0X00000030;	//IO状态设置

	RCC->APB2ENR|=1<<3;   	//使能PORTB口时钟  	   ok
	GPIOB->CRH&=0XFFFF00FF;	//IO状态设置  PB10 PB11	  ok
	GPIOB->CRH|=0X00008B00;	//IO状态设置	 		ok
	RCC->APB1ENR|=1<<18;  	//使能串口时钟 	 		ok
	RCC->APB1RSTR|=1<<18;   //复位串口3			   ok
	RCC->APB1RSTR&=~(1<<18);//停止复位3	   	   	   ok
	//波特率设置
 	USART3->BRR=(pclk1*1000000)/(bound);// 波特率设置	ok 
	USART3->CR1|=0X200C;  	//1位停止,无校验位.			ok
	//使能接收中断
#ifdef USART3_RX_EN		  	//如果使能了接收			ok
	USART3->CR1|=1<<8;    	//PE中断使能				 ok
	USART3->CR1|=1<<5;    	//接收缓冲区非空中断使能	ok    	
	MY_NVIC_Init(1,3,USART3_IRQChannel,2);//组2，最低优先级 
#endif										  	
}
//串口2,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
}
#endif		 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 
//
////****************************串口发送字符串******************************
//// 入口参数：	*str:字符串首地址
////					ENTER:(回车换行) 0：不换行  1：换行 
//// 出口参数：	无
//// 函数功能：	从串口发送一字符串
////************************************************************************
//void Send_Hex(u8 achar)
//{
//	USART3->DR=achar;
//	while((USART3->SR&0X40)==0);//等待发送结束
//
//}
/***************************发送字符(ASCII)函数*********************/
void USART3_Send_ASCII(u8 *str)
{
	while(*str)
	{
		USART3->DR=*str;
		while((USART3->SR&0X40)==0);//等待发送结束
		str++;
	}
}

//
void UART3_Put_Char(unsigned char DataToSend)
{
	//将要发送的字节写到UART1的发送缓冲区
	//USART_SendData(USART3, (unsigned char) DataToSend);
	//等待发送完成
  	//while (!(USART3->SR & USART_FLAG_TXE));
	 USART3->DR = DataToSend;	   //将数据发出去，pengxq修改使用寄存器
		while((USART3->SR&0X40)==0);//等待发送结束

//	TxBuffer[count++] = DataToSend;  
//    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  
}
void USART3_Put_String(unsigned char *Str)
{
	//判断Str指向的数据是否有效.
	while(*Str)
	{
	//是否是回车字符 如果是,则发送相应的回车 0x0d 0x0a
	if(*Str=='\r')
		UART3_Put_Char(0x0d);
		else if(*Str=='\n')UART3_Put_Char(0x0a);
			else UART3_Put_Char(*Str);
	//等待发送完成.
  	//while (!(USART3->SR & USART_FLAG_TXE));
	//指针++ 指向下一个字节.
	Str++;
	}
}
 //在接收完一帧IMU姿态报告后，调用这个子程序来取出/*GPS*/数据
u16 SailPos ,SteerPos = 1400; //帆位置和舵机的位置
void USART3_Get_Motor(void)
{
	u16 temp;
	
	temp = 0;
	temp = USART3_rx_buffer[2];
	temp <<= 8;
	temp |= USART3_rx_buffer[3];
	SteerPos=(u16)temp; //舵机位置
	
	temp = 0;
	temp = USART3_rx_buffer[4];
	temp <<= 8;
	temp |= USART3_rx_buffer[5];
	SailPos=(u16)temp; //帆位置
	
}
u8 InitFlag;
void USART3_Get_Init(void)
{
	u16 temp;
	
	temp = 0;
	temp = USART3_rx_buffer[2];
	temp <<= 8;
	temp |= USART3_rx_buffer[3];
	SteerPos=(u16)temp; //电机速度
	
	temp = 0;
	temp = USART3_rx_buffer[4];
	temp <<= 8;
	temp |= USART3_rx_buffer[5];
	SailPos=(u16)temp; //舵机速度

	InitFlag = 1;			
}

 //--校验当前接收到的一帧数据是否 与帧校验字节一致
unsigned char Usart3_Sum_check(void)
{ 
  unsigned char i;
  u32 checksum=0; 
  for(i=0;i<USART3_rx_buffer[0]-2;i++)
   checksum+=USART3_rx_buffer[i];
  if((checksum%256)==USART3_rx_buffer[USART3_rx_buffer[0]-2])
   return(0x01); //Checksum successful
  else
   return(0x00); //Checksum error
}



//--这个子程序需要在主程序中 定时调用,以检查 串口是否接收完一帧数据
void USART3_CommandRoute(void)
{
	if(USART3_RC_Flag&USART3_b_rx_over)
	{  //已经接收完一帧?
		USART3_RC_Flag&=~USART3_b_rx_over; //清标志先
		if(Usart3_Sum_check())
		{ 
			if(USART3_rx_buffer[1]==0xA6) // 0xA6接收帆的角度 0~360
			{ 
				USART3_Get_Motor();	 //取ALL数据
			} 
			if(USART3_rx_buffer[1]==0xA9) // 0xA9解析初始化指令
			{ 
				USART3_Get_Init();	 //取ALL数据
			} 
		
		}//校验是否通过?
	}
}


//	u8 svnum;					//可见卫星数
//	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 
void UART3_ReportAllData(int16_t yaw,int16_t pitch,int16_t roll,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec,
						int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,int16_t hx,int16_t hy,int16_t hz,
						u32 longitude,u32 latitude,u8 svnum,u16 speed)  //nmea_msg *gpsx是定义的结构体
{
 	u32 temp=0xA3+14+18+11+4;	 //固定的数据（同样需要校验）不用再累加
	char ctemp;
	
	UART3_Put_Char(0xa5);  //一帧数据的数据头
	UART3_Put_Char(0x5a);  //一帧数据的数据头
	UART3_Put_Char(14+18+11+4);  //14+18+8个字节的数据（经纬度）	+4个字节的开始（长度和0xa4）和结束部分 (校验和结束)
	UART3_Put_Char(0xA3); //AllData：A3

/*1*/
	if(yaw<0)yaw=32768-yaw;
	ctemp=yaw>>8;
	UART3_Put_Char(ctemp); //1
	temp+=ctemp;
	ctemp=yaw;
	UART3_Put_Char(ctemp);
	temp+=ctemp;

	if(pitch<0)pitch=32768-pitch;
	ctemp=pitch>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=pitch;
	UART3_Put_Char(ctemp);
	temp+=ctemp;

	if(roll<0)roll=32768-roll;
	ctemp=roll>>8;
	UART3_Put_Char(ctemp); //5
	temp+=ctemp;
	ctemp=roll;
	UART3_Put_Char(ctemp);
	temp+=ctemp;

	if(alt<0)alt=32768-alt;
	ctemp=alt>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=alt;
	UART3_Put_Char(ctemp);//8
	temp+=ctemp;

	if(tempr<0)tempr=32768-tempr;
	ctemp=tempr>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=tempr;
	UART3_Put_Char(ctemp);	 //10
	temp+=ctemp;

	if(press<0)press=32768-press;
	ctemp=press>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=press;
	UART3_Put_Char(ctemp);	 //12
	temp+=ctemp;

	ctemp=IMUpersec>>8;			 //没有用到这个数据
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=IMUpersec;
	UART3_Put_Char(ctemp);	  //14
	temp+=ctemp;
/*1end*/

//temp = temp%256;	   //解决u32数据长度不够问题

/*2*/
	if(ax<0)ax=32768-ax;
	ctemp=ax>>8;
	UART3_Put_Char(ctemp);	   
	temp+=ctemp;
	ctemp=ax;
	UART3_Put_Char(ctemp);	   //16
	temp+=ctemp;

	if(ay<0)ay=32768-ay;
	ctemp=ay>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=ay;
	UART3_Put_Char(ctemp);		//18
	temp+=ctemp;

	if(az<0)az=32768-az;
	ctemp=az>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=az;
	UART3_Put_Char(ctemp);		   //20
	temp+=ctemp;

	if(gx<0)gx=32768-gx;
	ctemp=gx>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gx;
	UART3_Put_Char(ctemp);			   //22
	temp+=ctemp;

	if(gy<0)gy=32768-gy;
	ctemp=gy>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gy;
	UART3_Put_Char(ctemp);				   //24
	temp+=ctemp;
//-------------------------
	if(gz<0)gz=32768-gz;
	ctemp=gz>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gz;
	UART3_Put_Char(ctemp);	//26
	temp+=ctemp;

	if(hx<0)hx=32768-hx;
	ctemp=hx>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hx;
	UART3_Put_Char(ctemp);		//28
	temp+=ctemp;

	if(hy<0)hy=32768-hy;
	ctemp=hy>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hy;
	UART3_Put_Char(ctemp);			//30
	temp+=ctemp;

	if(hz<0)hz=32768-hz;
	ctemp=hz>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hz;
	UART3_Put_Char(ctemp);			   //32
	temp+=ctemp;


/*2end*/
//temp = temp%256;	   //解决u32数据长度不够问题
/*3*/

	ctemp=longitude>>24;  //32位的数据，从高8位开始发送
	UART3_Put_Char(ctemp);		 //33
	temp+=ctemp;
	ctemp=longitude>>16;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=longitude>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=longitude;
	UART3_Put_Char(ctemp);		  //36
	temp+=ctemp;

	ctemp=latitude>>24;	//32位的数据，从高8位开始发送
	UART3_Put_Char(ctemp);		   //37
	temp+=ctemp;
	ctemp=latitude>>16;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=latitude>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=latitude;
	UART3_Put_Char(ctemp);			//40
	temp+=ctemp;
//	u8 svnum;					//可见卫星数
//	u16 speed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	 

	ctemp=svnum;	//			   u8 svnum;					//可见卫星数
	UART3_Put_Char(ctemp);		   //41
	temp+=ctemp;

	ctemp=speed>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=speed;
	UART3_Put_Char(ctemp);		//43	u16 speed;	地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时
	temp+=ctemp;

/*3end*/

	
	UART3_Put_Char(temp%256);
	UART3_Put_Char(0xaa);
}
void UART3_RepWindFanDir(float wind_dir_degree,float fan_dir_degree)
{
 	u32 temp=0xA6+4+4;	 //固定的数据（同样需要校验）不用再累加
	u16 temp2;
	char ctemp;
	
	UART3_Put_Char(0xa5);  //一帧数据的数据头
	UART3_Put_Char(0x5a);  //一帧数据的数据头
	UART3_Put_Char(4+4);  //4个字节的数据	+4个字节的开始（长度和0xa4）和结束部分 (校验和结束)
	UART3_Put_Char(0xA6); //AllData：A3

   	temp2 = (u16)(wind_dir_degree*10); //风向（0~3600），实际扩大了10倍
	ctemp=temp2>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=temp2;
	UART3_Put_Char(ctemp);		
	temp+=ctemp;

   	temp2 = (u16)(fan_dir_degree*10); //帆向（0~3600），实际扩大了10倍
	ctemp=temp2>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=temp2;
	UART3_Put_Char(ctemp);		
	temp+=ctemp;

	UART3_Put_Char(temp%256);
	UART3_Put_Char(0xaa);
}
__align(4) u8 tempbuf[128];   								//打印缓存器
u16	SailValue;
void SteerSailAdjust(u16 temp_steer_val,u16 sail_goal_pos,u16 motor_speed)	
{
	if(sail_goal_pos > FanPosDegree)
	{
		if((sail_goal_pos - FanPosDegree) > FANERRORRANGE)
			{
				if((sail_goal_pos - FanPosDegree) > 1800)
					SailValue = MIDDLEVALUE + SAILSPEED;// 大于180度反向转
				else
					SailValue = MIDDLEVALUE - SAILSPEED;//正常
			}
		else
			SailValue = MIDDLEVALUE;//正常
	}
	else
	{
		if((FanPosDegree - sail_goal_pos) > FANERRORRANGE)
		{
			if((FanPosDegree - sail_goal_pos) > 1800)
				SailValue = MIDDLEVALUE - SAILSPEED;// 大于180度反向转
			else
				SailValue = MIDDLEVALUE + SAILSPEED;//正常
		}
		else
			SailValue = MIDDLEVALUE;//正常
	}
	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",temp_steer_val,SailValue,motor_speed);	// 俯仰
	printf(tempbuf);
//	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",temp_steer_val,sail_goal_pos,motor_speed);	// 俯仰
//	printf(tempbuf);
}


void InitMotor(void) //初始化电调
{
	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",MIDDLEVALUE,1400,1400);	// 俯仰
	printf(tempbuf);

}









