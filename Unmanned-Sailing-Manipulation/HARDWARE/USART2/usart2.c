#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
char  UART2Buffer[BUFSIZE]; //��������500���������
u32 UART2Count;
///////////////////�����Ǵ���2�������õ��ı���//////////////////////////////
#define USART2_b_uart_head  0x80  //�յ�A5 ͷ ��־λ
#define USART2_b_rx_over    0x40  //�յ�������֡��־
#define USART2_RX_BUFFER_SIZE 100 //���ջ������ֽ���
unsigned char USART2_rx_buffer[USART2_RX_BUFFER_SIZE]; //�������ݻ�����
volatile unsigned char USART2_rx_wr_index; //����дָ��
volatile unsigned char USART2_RC_Flag;  //����״̬��־�ֽ�
#define COMPASS_COMD 0xA6
////////////////////////////////////////////////////////////

/*///////////////////������յ���ָ���//////////////////////////////////////////////////////	 
 */

void USART2_GetCmd(void)
{	  
//static u8 Flag1;
	u8 res;	    
	res= USART2->DR; //��ȡ���յ����ֽ�
	if(res==0xa5) 
	  {
		USART2_RC_Flag|=USART2_b_uart_head; //������յ�A5 ��λ֡ͷ��רλ
	    USART2_rx_buffer[USART2_rx_wr_index++]=res; //��������ֽ�.
	  }
	  else if(res==0x5a)
	   { 
		   if(USART2_RC_Flag&USART2_b_uart_head) //�����һ���ֽ���A5 ��ô�϶� �����֡��ʼ�ֽ�
		     { USART2_rx_wr_index=0;  //���� ������ָ��
			   USART2_RC_Flag&=~USART2_b_rx_over; //���֡�Ÿոտ�ʼ��
		     }
		     else //��һ���ֽڲ���A5
			  USART2_rx_buffer[USART2_rx_wr_index++]=res;
		     USART2_RC_Flag&=~USART2_b_uart_head; //��֡ͷ��־
	   }
	   else
	   { 
	     USART2_rx_buffer[USART2_rx_wr_index++]=res;
		 USART2_RC_Flag&=~USART2_b_uart_head;
		 if(USART2_rx_wr_index==USART2_rx_buffer[0]) //�չ����ֽ���.
	     {  
			USART2_rx_wr_index = 0;
			USART2_RC_Flag|=USART2_b_rx_over; //��λ ����������һ֡����
	      }
	   }
	  if(USART2_rx_wr_index==USART2_RX_BUFFER_SIZE) //��ֹ���������
	  USART2_rx_wr_index--;
}
//--����ӳ�����Ҫ���������� ��ʱ����,�Լ�� �����Ƿ������һ֡����
void USART2_CommandRoute(void)
{
	if(USART2_RC_Flag&USART2_b_rx_over)
	{  //�Ѿ�������һ֡?
		USART2_RC_Flag&=~USART2_b_rx_over; //���־��
		if(USART2_Sum_check())
		{ 
			if(USART2_rx_buffer[1]==0xA6) // 0xA6
			{ 
				GetCmd_PC_Compass2();	 //ȡ����
			} 
		
		}//У���Ƿ�ͨ��?
	}
}
 //--У�鵱ǰ���յ���һ֡�����Ƿ� ��֡У���ֽ�һ��
unsigned char USART2_Sum_check(void)
{ 
  unsigned char i;
  u32 checksum=0; 
  for(i=0;i<USART2_rx_buffer[0]-2;i++)
   checksum+=USART2_rx_buffer[i];
  if((checksum%256)==USART2_rx_buffer[USART2_rx_buffer[0]-2])
   return(0x01); //Checksum successful
  else
   return(0x00); //Checksum error
}
u16 GoalCompsDri2; 

void GetCmd_PC_Compass2(void)
{
	u16 temp;
	
	temp = 0;
	temp = USART2_rx_buffer[2];
	temp <<= 8;
	temp |= USART2_rx_buffer[3];
	GoalCompsDri2=(u16)temp; //Ŀ�꺽��
//	LCD_ShowxNum(0,40,GoalCompsDri2,4,16,0);
	

}

////////////////////////////////////////////////////////////////////////////////////////end/
 	   

//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
#ifdef USART2_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		UART2Buffer[UART2Count] = USART2->DR; //�����ܵ������ݸ�������
		UART2Count++;
		if ( UART2Count == BUFSIZE )
		{
		  UART2Count = 0; //����BUFSIZE����	
		}

//	USART2_GetCmd();//��ô�PC����������A55A06A6......AA
	}  											 
//	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART2->DR = res;      
}   
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART2_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<8;   	//ʹ��PORTG��ʱ��  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO״̬����
	GPIOG->CRH|=0X00000030;	//IO״̬����
	RCC->APB2ENR|=1<<2;   	//ʹ��PORTA��ʱ��  
	GPIOA->CRL&=0XFFFF00FF;	//IO״̬����
	GPIOA->CRL|=0X00008B00;	//IO״̬����	 
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���ʱ�� 	 
	RCC->APB1RSTR|=1<<17;   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=(pclk1*1000000)/(bound);// ����������	 
	USART2->CR1|=0X200C;  	//1λֹͣ,��У��λ.
	USART2->CR3=1<<7;   	//ʹ�ܴ���2��DMA����
	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 
#ifdef USART2_RX_EN		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
	USART2->CR1|=1<<8;    	//PE�ж�ʹ��
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(2,3,USART2_IRQChannel,2);//��2��������ȼ� 
#endif										  	
}
//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//ͨ��dma���ͳ�ȥ
}
#endif		 
///////////////////////////////////////USART2 DMA�������ò���//////////////////////////////////	   		    
//DMA1�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ    
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar)
{
 	RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	delay_us(5);
	DMA_CHx->CPAR=cpar; 		//DMA1 �����ַ 
	DMA_CHx->CMAR=cmar; 		//DMA1,�洢����ַ	 
	DMA_CHx->CCR=0X00000000;	//��λ
	DMA_CHx->CCR|=1<<4;  		//�Ӵ洢����
	DMA_CHx->CCR|=0<<5;  		//��ͨģʽ
	DMA_CHx->CCR|=0<<6;  		//�����ַ������ģʽ
	DMA_CHx->CCR|=1<<7;  		//�洢������ģʽ
	DMA_CHx->CCR|=0<<8;  		//�������ݿ��Ϊ8λ
	DMA_CHx->CCR|=0<<10; 		//�洢�����ݿ��8λ
	DMA_CHx->CCR|=1<<12; 		//�е����ȼ�
	DMA_CHx->CCR|=0<<14; 		//�Ǵ洢�����洢��ģʽ		  	
} 
//����һ��DMA����
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len)
{
	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
	DMA_CHx->CNDTR=len;          //DMA1,���������� 
	DMA_CHx->CCR|=1<<0;          //����DMA����
}	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 

//****************************���ڷ����ַ���******************************
// ��ڲ�����	*str:�ַ����׵�ַ
//					ENTER:(�س�����) 0��������  1������ 
// ���ڲ�����	��
// �������ܣ�	�Ӵ��ڷ���һ�ַ���
//************************************************************************
void Send_Hex(u8 achar)
{
	USART2->DR=achar;
	while((USART2->SR&0X40)==0);//�ȴ����ͽ���

}
/***************************�����ַ�(ASCII)����*********************/
void Send_ASCII(u8 *str)
{
	while(*str)
	{
		USART2->DR=*str;
		str++;
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
}

void UART_TXDSTRING( u8 *str,u8 ENTER)
{
	while(*str)
	{
		USART2->DR=*str;
		str++;
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
//---------------------------------------------------------
	if( ENTER!=0 )						// �Ƿ���Ҫ�������
	{
		USART2->DR = 0x0d;					// �س�ASCII��
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
		USART2->DR=0x0a;						// ����ASCII��
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���
	}
}
/*****************************�������0�������ݺ���*****************/
void CLR_Buf0(void)
{
	u16 k;
    for(k=0;k<BUFSIZE;k++)          //��������������
    {
		UART2Buffer[k] = 0x30;  //ununderstend		'0'
	}
    UART2Count = 0;              //�����ַ�������ʼ�洢λ��	First_Int
}

/******************************************************************************
**                            End Of File
******************************************************************************/




















