#ifdef EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误    
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记   
  
void USART3_IRQHandler(void)
{
 u8 res;     
 if(USART3->SR&(1<<5))//接收到数据
 {  
  res=USART3->DR; 
  if((USART_RX_STA&0x80)==0)//接收未完成
  {
   if(USART_RX_STA&0x40)//接收到了0x0d
   {
    if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
    else USART_RX_STA|=0x80; //接收完成了 
   }else //还没收到0X0D
   { 
    if(res==0x0d)USART_RX_STA|=0x40;
    else
    {
     USART_RX_BUF[USART_RX_STA&0X3F]=res;
     USART_RX_STA++;
     if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收   
    }   
   }
  }                   
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
 mantissa=temp;     //得到整数部分
 fraction=(temp-mantissa)*16; //得到小数部分  
    mantissa<<=4;
 mantissa+=fraction; 
 RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
 RCC->APB2ENR|=1<<18;  //使能串口时钟 
 GPIOB->CRH&=0XFFFF00FF; 
 GPIOB->CRH|=0X00008B00;//IO状态设置
    
 RCC->APB1RSTR|=1<<18;   //复位串口1
 RCC->APB1RSTR&=~(1<<18);//停止复位        
 //波特率设置
  USART3->BRR=mantissa; // 波特率设置  
 USART3->CR1|=0X200C;  //1位停止,无校验位.
#ifdef EN_USART3_RX    //如果使能了接收
 //使能接收中断
 USART3->CR1|=1<<8;    //PE中断使能
 USART3->CR1|=1<<5;    //接收缓冲区非空中断使能      
 MY_NVIC_Init(3,3,USART3_IRQChannel,2);//组2，最低优先级 
#endif


}



