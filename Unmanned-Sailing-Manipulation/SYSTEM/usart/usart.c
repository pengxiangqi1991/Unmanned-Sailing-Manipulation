#include "sys.h"
#include "usart.h" 
#include "ahrs.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2011/6/14
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
////////////////////////////////////////////////////////////////  
//////////////////////串口1中断：用于接收IMU的数据/////////////  
void USART1_IRQHandler(void)
{
	u8 res;	    
	//	  res= USART1->DR; //读取接收到的字节
	if(USART1->SR&(1<<5))//接收到数据
	{
	  res= USART1->DR; //读取接收到的字节
// USART1->DR = res;
//	while((USART1->SR&0X40)==0);//等待发送结束
	
	  if(res==0xa5) 
	  {
		RC_Flag|=b_uart_head; //如果接收到A5 置位帧头标专位
	    rx_buffer[rx_wr_index++]=res; //保存这个字节.
	  }
	  else if(res==0x5a)
	   { 
		   if(RC_Flag&b_uart_head) //如果上一个字节是A5 那么认定 这个是帧起始字节
		     { rx_wr_index=0;  //重置 缓冲区指针
			   RC_Flag&=~b_rx_over; //这个帧才刚刚开始收
		     }
		     else //上一个字节不是A5
			  rx_buffer[rx_wr_index++]=res;
		     RC_Flag&=~b_uart_head; //清帧头标志
	   }
	   else
	   { 
	     rx_buffer[rx_wr_index++]=res;
		 RC_Flag&=~b_uart_head;
		 if(rx_wr_index==rx_buffer[0]) //收够了字节数.
	     {  
			rx_wr_index = 0;
			RC_Flag|=b_rx_over; //置位 接收完整的一帧数据
	      }
	   }
	
	  if(rx_wr_index==RX_BUFFER_SIZE) //防止缓冲区溢出
	  rx_wr_index--;
	  /* Clear the USART2 Recive interrupt 20130407移植未清除串口1中断标志位，是否有影响？待验证  彭向岐*/
	//  USART_ClearITPendingBit(USART2, USART_IT_RXNE); //清中断标志.
	
	
	}
} 
#endif										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
#if EN_USART1_RX		  //如果使能了接收
	//使能接收中断
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，最低优先级 
#endif
}
