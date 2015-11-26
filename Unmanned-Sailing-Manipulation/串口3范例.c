#ifdef EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���    
u8 USART_RX_BUF[64];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART_RX_STA=0;       //����״̬���   
  
void USART3_IRQHandler(void)
{
 u8 res;     
 if(USART3->SR&(1<<5))//���յ�����
 {  
  res=USART3->DR; 
  if((USART_RX_STA&0x80)==0)//����δ���
  {
   if(USART_RX_STA&0x40)//���յ���0x0d
   {
    if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
    else USART_RX_STA|=0x80; //��������� 
   }else //��û�յ�0X0D
   { 
    if(res==0x0d)USART_RX_STA|=0x40;
    else
    {
     USART_RX_BUF[USART_RX_STA&0X3F]=res;
     USART_RX_STA++;
     if(USART_RX_STA>63)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����   
    }   
   }
  }                   
 }              
} 
#endif           
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
//CHECK OK
//091209
void uart_init(u32 pclk2,u32 bound)
{    
 float temp;
 u16 mantissa;
 u16 fraction;    
 temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
 mantissa=temp;     //�õ���������
 fraction=(temp-mantissa)*16; //�õ�С������  
    mantissa<<=4;
 mantissa+=fraction; 
 RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
 RCC->APB2ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
 GPIOB->CRH&=0XFFFF00FF; 
 GPIOB->CRH|=0X00008B00;//IO״̬����
    
 RCC->APB1RSTR|=1<<18;   //��λ����1
 RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ        
 //����������
  USART3->BRR=mantissa; // ����������  
 USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
#ifdef EN_USART3_RX    //���ʹ���˽���
 //ʹ�ܽ����ж�
 USART3->CR1|=1<<8;    //PE�ж�ʹ��
 USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��      
 MY_NVIC_Init(3,3,USART3_IRQChannel,2);//��2��������ȼ� 
#endif


}



