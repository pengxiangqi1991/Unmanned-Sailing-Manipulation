#include "timer.h"
#include "exti.h"
////////////////////////////////////////////////////////////////////////////////// 	  
u16  Count_time = 0;
u8   Timer0_Start = 0;
u16  Times = 0;
u16  shijian = 0;
u16 FreTimeFlag; //����Ƶ����
u16 Frequence; //����Ƶ����

//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
		if(Timer0_Start == 1)
			Times++;
		if(Times > (1*shijian)) //20*shijian  
		{
			Timer0_Start = 0;
			Times = 0;
		} 			    	 
		FreTimeFlag	++;	   //��ʱ1S����Ƶ��������  ���ڲ�Ƶ��
		if(1000 == FreTimeFlag)
		{
			FreTimeFlag = 0;
			Frequence = FreCount;	//����Ƶ��
			FreCount = 0;
		}
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void Timerx_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  //�趨�������Զ���װֵ//�պ�1ms    
	TIM3->PSC=psc;  //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	//����������Ҫͬʱ���òſ���ʹ���ж�
	TIM3->DIER|=1<<0;   //��������ж�				
	TIM3->DIER|=1<<6;   //�������ж�
		  							    
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQChannel,2);//��ռ1�������ȼ�3����2									 
}

//TIM3 PWM����
//����ԭ��@ALIENTEK
//2010/6/2	 

//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<1;       //TIM3ʱ��ʹ��    

	GPIOA->CRH&=0XFFFFFFF0;//PA8���
	GPIOA->CRH|=0X00000004;//��������
	  	
	GPIOA->CRL&=0X0FFFFFFF;//PA7���
	GPIOA->CRL|=0XB0000000;//���ù������ 	  
	GPIOA->ODR|=1<<7;//PA7����	

	TIM3->ARR=arr;//�趨�������Զ���װֵ 
	TIM3->PSC=psc;//Ԥ��Ƶ������Ƶ
	
	TIM3->CCMR1|=7<<12;  //CH2 PWM2ģʽ		 
	TIM3->CCMR1|=1<<11; //CH2Ԥװ��ʹ��	   

	TIM3->CCER|=1<<4;   //OC2 ���ʹ��	   

	TIM3->CR1=0x8000;   //ARPEʹ�� 
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3 										  
}  	 
////////////////////////////////��ʱ��2������֧��USMART�жϵ���////////////////////
//�Ѿ�����usmart�齨.
////��ʱ��2�жϷ������	 
//void TIM2_IRQHandler(void)
//{ 		    		  			    
//	if(TIM2->SR&0X0001)//����ж�
//	{
//		usmart_dev.scan();//ִ��usmartɨ��			    				   				     	    	
//	}				   
//	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
//}
////ʹ�ܶ�ʱ��2,ʹ���ж�.
//void Timer2_Init(u16 arr,u16 psc)
//{
//	RCC->APB1ENR|=1<<0;//TIM2ʱ��ʹ��    
// 	TIM2->ARR=arr;  //�趨�������Զ���װֵ  
//	TIM2->PSC=psc;  //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
//	//����������Ҫͬʱ���òſ���ʹ���ж�
//	TIM2->DIER|=1<<0;   //��������ж�				
//	TIM2->DIER|=1<<6;   //�������ж�
//		  							    
//	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
//  	MY_NVIC_Init(3,3,TIM2_IRQChannel,2);//��ռ3�������ȼ�3����2(��2�����ȼ���͵�)									 
//}















