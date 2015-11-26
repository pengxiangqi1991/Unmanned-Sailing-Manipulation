#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
 
u16 FreCount;	// //勿改
//外部中断0服务程序	  测频率
//void EXTI0_IRQHandler(void)
//{
//	FreCount ++;	 //PA0
//	EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
//}

//外部中断15~10服务程序
void EXTI15_10_IRQHandler(void)
{			
	FreCount ++;	 //PA0
	EXTI->PR=1<<15;     //清除LINE15上的中断标志位  
}
////外部中断初始化程序
////初始化PA0,PA13,PA15为中断输入.
//void EXTIX_Init(void)
//{
//	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
//	JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   
//
//	GPIOA->CRL&=0XFFFFFFF0;//PA0设置成输入	  
//	GPIOA->CRL|=0X00000008;   
//	GPIOA->CRH&=0X0F0FFFFF;//PA13,15设置成输入	  
//	GPIOA->CRH|=0X80800000; 				   
//	GPIOA->ODR|=1<<13;	   //PA13上拉,PA0默认下拉
//	GPIOA->ODR|=1<<15;	   //PA15上拉
//
//	Ex_NVIC_Config(GPIO_A,0,RTIR); //上升沿触发
//	Ex_NVIC_Config(GPIO_A,13,FTIR);//下降沿触发
//	Ex_NVIC_Config(GPIO_A,15,FTIR);//下降沿触发
//
//	MY_NVIC_Init(2,2,EXTI0_IRQChannel,2);    //抢占2，子优先级2，组2
//	MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//抢占2，子优先级1，组2	   
//}

//外部中断初始化程序
//初始化PA0,PA13,PA15为中断输入.
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

//	GPIOA->CRL&=0XFFFFFFF0;//PA0设置成输入	  
//	GPIOA->CRL|=0X00000008;   
	GPIOA->CRH&=0X0FFFFFFF;//PA15设置成输入	  
	GPIOA->CRH|=0X80000000; 				   
	GPIOA->ODR|=1<<15;	   //PA15上拉
//	Ex_NVIC_Config(GPIO_A,0,RTIR); //上升沿触发
//	Ex_NVIC_Config(GPIO_A,13,FTIR);//下降沿触发
	Ex_NVIC_Config(GPIO_A,15,RTIR);//下降沿触发
//	MY_NVIC_Init(2,2,EXTI0_IRQChannel,2);    //抢占2，子优先级2，组2
	MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//抢占2，子优先级1，组2	   
}











