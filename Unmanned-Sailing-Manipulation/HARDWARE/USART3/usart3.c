#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "led.h" 
#include "exti.h"
#define USART3_b_uart_head  0x80  //�յ�A5 ͷ ��־λ
#define USART3_b_rx_over    0x40  //�յ�������֡��־
#define USART3_RX_BUFFER_SIZE 100 //���ջ������ֽ���
unsigned char USART3_rx_buffer[USART3_RX_BUFFER_SIZE]; //�������ݻ�����
volatile unsigned char USART3_rx_wr_index; //����дָ��
volatile unsigned char USART3_RC_Flag;  //����״̬��־�ֽ�

#define FANERRORRANGE 30	  //����������Χ������ʮ���ĽǶȣ�

//���ڷ��ͻ����� 	
#ifdef USART3_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
void USART3_IRQHandler(void)
{
	u8 res;	    
	//	  res= USART3->DR; //��ȡ���յ����ֽ�
	if(USART3->SR&(1<<5))//���յ�����
	{
	  res= USART3->DR; //��ȡ���յ����ֽ�
// USART3->DR = res;
//	while((USART3->SR&0X40)==0);//�ȴ����ͽ���
	
	  if(res==0xa5) 
	  {
		USART3_RC_Flag|=USART3_b_uart_head; //������յ�A5 ��λ֡ͷ��רλ
	    USART3_rx_buffer[USART3_rx_wr_index++]=res; //��������ֽ�.
	  }
	  else if(res==0x5a)
	   { 
		   if(USART3_RC_Flag&USART3_b_uart_head) //�����һ���ֽ���A5 ��ô�϶� �����֡��ʼ�ֽ�
		     { USART3_rx_wr_index=0;  //���� ������ָ��
			   USART3_RC_Flag&=~USART3_b_rx_over; //���֡�Ÿոտ�ʼ��
		     }
		     else //��һ���ֽڲ���A5
			  USART3_rx_buffer[USART3_rx_wr_index++]=res;
		     USART3_RC_Flag&=~USART3_b_uart_head; //��֡ͷ��־
	   }
	   else
	   { 
	     USART3_rx_buffer[USART3_rx_wr_index++]=res;
		 USART3_RC_Flag&=~USART3_b_uart_head;
		 if(USART3_rx_wr_index==USART3_rx_buffer[0]) //�չ����ֽ���.
	     {  
			USART3_rx_wr_index = 0;
			USART3_RC_Flag|=USART3_b_rx_over; //��λ ����������һ֡����
	      }
	   }
	
	  if(USART3_rx_wr_index==USART3_RX_BUFFER_SIZE) //��ֹ���������
	  USART3_rx_wr_index--;
	  /* Clear the USART2 Recive interrupt 20130407��ֲδ�������1�жϱ�־λ���Ƿ���Ӱ�죿����֤  �����*/
	//  USART_ClearITPendingBit(USART2, USART_IT_RXNE); //���жϱ�־.
	}


//	u8 res;
//	if(USART3->SR&(1<<5))//���յ�����
//	{	 
//		LED1 = 0;	  
//		res = USART3->DR;
//		while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
//		USART3->DR = res;      
//	}
}   
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART3_Init(u32 pclk1,u32 bound)
{  	 		 
	RCC->APB2ENR|=1<<8;   	//ʹ��PORTG��ʱ��  
 	GPIOG->CRH&=0XFFFFFF0F;	//IO״̬����
	GPIOG->CRH|=0X00000030;	//IO״̬����

	RCC->APB2ENR|=1<<3;   	//ʹ��PORTB��ʱ��  	   ok
	GPIOB->CRH&=0XFFFF00FF;	//IO״̬����  PB10 PB11	  ok
	GPIOB->CRH|=0X00008B00;	//IO״̬����	 		ok
	RCC->APB1ENR|=1<<18;  	//ʹ�ܴ���ʱ�� 	 		ok
	RCC->APB1RSTR|=1<<18;   //��λ����3			   ok
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ3	   	   	   ok
	//����������
 	USART3->BRR=(pclk1*1000000)/(bound);// ����������	ok 
	USART3->CR1|=0X200C;  	//1λֹͣ,��У��λ.			ok
	//ʹ�ܽ����ж�
#ifdef USART3_RX_EN		  	//���ʹ���˽���			ok
	USART3->CR1|=1<<8;    	//PE�ж�ʹ��				 ok
	USART3->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	ok    	
	MY_NVIC_Init(1,3,USART3_IRQChannel,2);//��2��������ȼ� 
#endif										  	
}
//����2,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
}
#endif		 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 									 
//
////****************************���ڷ����ַ���******************************
//// ��ڲ�����	*str:�ַ����׵�ַ
////					ENTER:(�س�����) 0��������  1������ 
//// ���ڲ�����	��
//// �������ܣ�	�Ӵ��ڷ���һ�ַ���
////************************************************************************
//void Send_Hex(u8 achar)
//{
//	USART3->DR=achar;
//	while((USART3->SR&0X40)==0);//�ȴ����ͽ���
//
//}
/***************************�����ַ�(ASCII)����*********************/
void USART3_Send_ASCII(u8 *str)
{
	while(*str)
	{
		USART3->DR=*str;
		while((USART3->SR&0X40)==0);//�ȴ����ͽ���
		str++;
	}
}

//
void UART3_Put_Char(unsigned char DataToSend)
{
	//��Ҫ���͵��ֽ�д��UART1�ķ��ͻ�����
	//USART_SendData(USART3, (unsigned char) DataToSend);
	//�ȴ��������
  	//while (!(USART3->SR & USART_FLAG_TXE));
	 USART3->DR = DataToSend;	   //�����ݷ���ȥ��pengxq�޸�ʹ�üĴ���
		while((USART3->SR&0X40)==0);//�ȴ����ͽ���

//	TxBuffer[count++] = DataToSend;  
//    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  
}
void USART3_Put_String(unsigned char *Str)
{
	//�ж�Strָ��������Ƿ���Ч.
	while(*Str)
	{
	//�Ƿ��ǻس��ַ� �����,������Ӧ�Ļس� 0x0d 0x0a
	if(*Str=='\r')
		UART3_Put_Char(0x0d);
		else if(*Str=='\n')UART3_Put_Char(0x0a);
			else UART3_Put_Char(*Str);
	//�ȴ��������.
  	//while (!(USART3->SR & USART_FLAG_TXE));
	//ָ��++ ָ����һ���ֽ�.
	Str++;
	}
}
 //�ڽ�����һ֡IMU��̬����󣬵�������ӳ�����ȡ��/*GPS*/����
u16 SailPos ,SteerPos = 1400; //��λ�úͶ����λ��
void USART3_Get_Motor(void)
{
	u16 temp;
	
	temp = 0;
	temp = USART3_rx_buffer[2];
	temp <<= 8;
	temp |= USART3_rx_buffer[3];
	SteerPos=(u16)temp; //���λ��
	
	temp = 0;
	temp = USART3_rx_buffer[4];
	temp <<= 8;
	temp |= USART3_rx_buffer[5];
	SailPos=(u16)temp; //��λ��
	
}
u8 InitFlag;
void USART3_Get_Init(void)
{
	u16 temp;
	
	temp = 0;
	temp = USART3_rx_buffer[2];
	temp <<= 8;
	temp |= USART3_rx_buffer[3];
	SteerPos=(u16)temp; //����ٶ�
	
	temp = 0;
	temp = USART3_rx_buffer[4];
	temp <<= 8;
	temp |= USART3_rx_buffer[5];
	SailPos=(u16)temp; //����ٶ�

	InitFlag = 1;			
}

 //--У�鵱ǰ���յ���һ֡�����Ƿ� ��֡У���ֽ�һ��
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



//--����ӳ�����Ҫ���������� ��ʱ����,�Լ�� �����Ƿ������һ֡����
void USART3_CommandRoute(void)
{
	if(USART3_RC_Flag&USART3_b_rx_over)
	{  //�Ѿ�������һ֡?
		USART3_RC_Flag&=~USART3_b_rx_over; //���־��
		if(Usart3_Sum_check())
		{ 
			if(USART3_rx_buffer[1]==0xA6) // 0xA6���շ��ĽǶ� 0~360
			{ 
				USART3_Get_Motor();	 //ȡALL����
			} 
			if(USART3_rx_buffer[1]==0xA9) // 0xA9������ʼ��ָ��
			{ 
				USART3_Get_Init();	 //ȡALL����
			} 
		
		}//У���Ƿ�ͨ��?
	}
}


//	u8 svnum;					//�ɼ�������
//	u16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
void UART3_ReportAllData(int16_t yaw,int16_t pitch,int16_t roll,int16_t alt,int16_t tempr,int16_t press,int16_t IMUpersec,
						int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,int16_t hx,int16_t hy,int16_t hz,
						u32 longitude,u32 latitude,u8 svnum,u16 speed)  //nmea_msg *gpsx�Ƕ���Ľṹ��
{
 	u32 temp=0xA3+14+18+11+4;	 //�̶������ݣ�ͬ����ҪУ�飩�������ۼ�
	char ctemp;
	
	UART3_Put_Char(0xa5);  //һ֡���ݵ�����ͷ
	UART3_Put_Char(0x5a);  //һ֡���ݵ�����ͷ
	UART3_Put_Char(14+18+11+4);  //14+18+8���ֽڵ����ݣ���γ�ȣ�	+4���ֽڵĿ�ʼ�����Ⱥ�0xa4���ͽ������� (У��ͽ���)
	UART3_Put_Char(0xA3); //AllData��A3

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

	ctemp=IMUpersec>>8;			 //û���õ��������
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=IMUpersec;
	UART3_Put_Char(ctemp);	  //14
	temp+=ctemp;
/*1end*/

//temp = temp%256;	   //���u32���ݳ��Ȳ�������

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
//temp = temp%256;	   //���u32���ݳ��Ȳ�������
/*3*/

	ctemp=longitude>>24;  //32λ�����ݣ��Ӹ�8λ��ʼ����
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

	ctemp=latitude>>24;	//32λ�����ݣ��Ӹ�8λ��ʼ����
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
//	u8 svnum;					//�ɼ�������
//	u16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 

	ctemp=svnum;	//			   u8 svnum;					//�ɼ�������
	UART3_Put_Char(ctemp);		   //41
	temp+=ctemp;

	ctemp=speed>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=speed;
	UART3_Put_Char(ctemp);		//43	u16 speed;	��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ
	temp+=ctemp;

/*3end*/

	
	UART3_Put_Char(temp%256);
	UART3_Put_Char(0xaa);
}
void UART3_RepWindFanDir(float wind_dir_degree,float fan_dir_degree)
{
 	u32 temp=0xA6+4+4;	 //�̶������ݣ�ͬ����ҪУ�飩�������ۼ�
	u16 temp2;
	char ctemp;
	
	UART3_Put_Char(0xa5);  //һ֡���ݵ�����ͷ
	UART3_Put_Char(0x5a);  //һ֡���ݵ�����ͷ
	UART3_Put_Char(4+4);  //4���ֽڵ�����	+4���ֽڵĿ�ʼ�����Ⱥ�0xa4���ͽ������� (У��ͽ���)
	UART3_Put_Char(0xA6); //AllData��A3

   	temp2 = (u16)(wind_dir_degree*10); //����0~3600����ʵ��������10��
	ctemp=temp2>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=temp2;
	UART3_Put_Char(ctemp);		
	temp+=ctemp;

   	temp2 = (u16)(fan_dir_degree*10); //����0~3600����ʵ��������10��
	ctemp=temp2>>8;
	UART3_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=temp2;
	UART3_Put_Char(ctemp);		
	temp+=ctemp;

	UART3_Put_Char(temp%256);
	UART3_Put_Char(0xaa);
}
__align(4) u8 tempbuf[128];   								//��ӡ������
u16	SailValue;
void SteerSailAdjust(u16 temp_steer_val,u16 sail_goal_pos,u16 motor_speed)	
{
	if(sail_goal_pos > FanPosDegree)
	{
		if((sail_goal_pos - FanPosDegree) > FANERRORRANGE)
			{
				if((sail_goal_pos - FanPosDegree) > 1800)
					SailValue = MIDDLEVALUE + SAILSPEED;// ����180�ȷ���ת
				else
					SailValue = MIDDLEVALUE - SAILSPEED;//����
			}
		else
			SailValue = MIDDLEVALUE;//����
	}
	else
	{
		if((FanPosDegree - sail_goal_pos) > FANERRORRANGE)
		{
			if((FanPosDegree - sail_goal_pos) > 1800)
				SailValue = MIDDLEVALUE - SAILSPEED;// ����180�ȷ���ת
			else
				SailValue = MIDDLEVALUE + SAILSPEED;//����
		}
		else
			SailValue = MIDDLEVALUE;//����
	}
	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",temp_steer_val,SailValue,motor_speed);	// ����
	printf(tempbuf);
//	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",temp_steer_val,sail_goal_pos,motor_speed);	// ����
//	printf(tempbuf);
}


void InitMotor(void) //��ʼ�����
{
	sprintf((char *)tempbuf,"#1P%d#2P%d#3P%dT500\r\n",MIDDLEVALUE,1400,1400);	// ����
	printf(tempbuf);

}









