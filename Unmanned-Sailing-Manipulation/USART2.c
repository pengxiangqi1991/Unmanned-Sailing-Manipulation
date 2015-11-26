#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
char  UART2Buffer[BUFSIZE]; //缓存数组500，存放数据
u32 UART2Count;
//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 	   

//串口发送缓存区 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
#ifdef USART2_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		UART2Buffer[UART2Count] = USART2->DR; //将接受到的数据给缓冲区
		UART2Count++;
		if ( UART2Count == BUFSIZE )
		{
		  UART2Count = 0; //超过BUFSIZE清零	
		}

	}  											 
//	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART2->DR = res;      
}   
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<8;   	//使能PORTG口时钟  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO状态设置
	GPIOG->CRH|=0X00000030;	//IO状态设置
	RCC->APB2ENR|=1<<2;   	//使能PORTA口时钟  
	GPIOA->CRL&=0XFFFF00FF;	//IO状态设置
	GPIOA->CRL|=0X00008B00;	//IO状态设置	 
	RCC->APB1ENR|=1<<17;  	//使能串口时钟 	 
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置	 
	USART2->CR1|=0X200C;  	//1位停止,无校验位.
	USART2->CR3=1<<7;   	//使能串口2的DMA发送
	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 
#ifdef USART2_RX_EN		  	//如果使能了接收
	//使能接收中断
	USART2->CR1|=1<<8;    	//PE中断使能
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(2,3,USART2_IRQChannel,2);//组2，最低优先级 
#endif										  	
}
//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//通过dma发送出去
}
#endif		 
///////////////////////////////////////USART2 DMA发送配置部分//////////////////////////////////	   		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
 	RCC->AHBENR|=1<<0;			//开启DMA1时钟
	delay_us(5);
	DMA_CHx->CPAR=cpar; 		//DMA1 外设地址 
	DMA_CHx->CMAR=cmar; 		//DMA1,存储器地址	 
	DMA_CHx->CCR=0X00000000;	//复位
	DMA_CHx->CCR|=1<<4;  		//从存储器读
	DMA_CHx->CCR|=0<<5;  		//普通模式
	DMA_CHx->CCR|=0<<6;  		//外设地址非增量模式
	DMA_CHx->CCR|=1<<7;  		//存储器增量模式
	DMA_CHx->CCR|=0<<8;  		//外设数据宽度为8位
	DMA_CHx->CCR|=0<<10; 		//存储器数据宽度8位
	DMA_CHx->CCR|=1<<12; 		//中等优先级
	DMA_CHx->CCR|=0<<14; 		//非存储器到存储器模式		  	
} 
//开启一次DMA传输
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
	DMA_CHx->CNDTR=len;          //DMA1,传输数据量 
	DMA_CHx->CCR|=1<<0;          //开启DMA传输
}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 

//****************************串口发送字符串******************************
// 入口参数：	*str:字符串首地址
//					ENTER:(回车换行) 0：不换行  1：换行 
// 出口参数：	无
// 函数功能：	从串口发送一字符串
//************************************************************************
void Send_Hex(u8 achar)
{
	USART2->DR=achar;
	while((USART2->SR&0X40)==0);//等待发送结束

}
/***************************发送字符(ASCII)函数*********************/
void Send_ASCII(u8 *str)
{
	while(*str)
	{
		USART2->DR=*str;
		str++;
		while((USART2->SR&0X40)==0);//等待发送结束
	}
}

void UART_TXDSTRING( u8 *str,u8 ENTER)
{
	while(*str)
	{
		USART2->DR=*str;
		str++;
		while((USART2->SR&0X40)==0);//等待发送结束
	}
//---------------------------------------------------------
	if( ENTER!=0 )						// 是否需要输出换行
	{
		USART2->DR = 0x0d;					// 回车ASCII码
		while((USART2->SR&0X40)==0);//等待发送结束
		USART2->DR=0x0a;						// 换行ASCII码
		while((USART2->SR&0X40)==0);//等待发送结束
	}
}
/*****************************清除串口0缓存数据函数*****************/
void CLR_Buf0(void)
{
	u16 k;
    for(k=0;k<BUFSIZE;k++)          //将缓存内容清零
    {
		UART2Buffer[k] = 0x30;  //ununderstend		'0'
	}
    UART2Count = 0;              //接收字符串的起始存储位置	First_Int
}

/******************************************************************************
**                            End Of File
******************************************************************************/




















