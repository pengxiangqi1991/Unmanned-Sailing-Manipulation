#include <stm32f10x_lib.h>
#include <stm32f10x_gpio.h>
	   
#include "LCD12864.h"
#include "delay.h"

 
/*****���ֵ�ַ��*****/
u8 addr_tab[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,//��һ�к���λ��
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,//�ڶ��к���λ��
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,//�����к���λ��
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,//�����к���λ��
};

/*****��ʼ��LCD*****/
void Lcd_Init(void)
{ 
	RCC->APB2ENR|=1<<3;    //ʹ��PORTbʱ��	   A��2   E��6///
	
	GPIOB->CRH&=0XFF00FFFF; 
	GPIOB->CRH|=0X00330000;//PB12 13 �������   	 
											  


	delay_ms(50);
    Lcd_WriteCmd(0x30);        //ѡ�����ָ�
	delay_ms(1);
    Lcd_WriteCmd(0x30);        //ѡ��8bit������
	delay_ms(1);
    Lcd_WriteCmd(0x0c);        //����ʾ(���αꡢ������)
	delay_ms(1);
    Lcd_WriteCmd(0x01);        //�����ʾ�������趨��ַָ��Ϊ00H
	delay_ms(30);
}

/*****���з���һ���ֽ�*****/
void SendByte(u8 Dbyte)
{
     u8 i;
     for(i=0;i<8;i++)
     {
       SCLK = 0;

       //Dbyte=Dbyte<<1;   //����һλ
       //SID = CY;         //�Ƴ���λ��SID   CY��ʲô�滻	��	 �ѽ��
							/*DS=temp&0x80; 	�������
							temp<<=1;*/ 

	   if(Dbyte&0x80)
	   	SID=1;
	   else
	   	SID=0; 
		Dbyte<<=1;
       SCLK = 1;
       SCLK = 0;
     }
}

/*****дָ��*****/
void Lcd_WriteCmd(u8 Cbyte )
{
     delay_ms(1);//���æ���ӳ������ǾͲ�д�ˣ���Ϊ���еĲ�֧�ֶ����������Ǿ��ø���ʱ��
     SendByte(0xf8);            //11111,RW(0),RS(0),0		 0xf8Ҫ����
     SendByte(0xf0&Cbyte);      //����λ
     SendByte(0xf0&Cbyte<<4);   //����λ(��ִ��<<)
}
/*****д����*****/
void Lcd_WriteData(u8 Dbyte )
{
     delay_ms(1);
     SendByte(0xfa);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);      //����λ
     SendByte(0xf0&Dbyte<<4);   //����λ(��ִ��<<)
}
/******************************************************************************
** �������� ��
** �������� ����ȡ1���ֽ� 												  
** ��ڲ��� ��
** ���ڲ��� ��
** ˵��     ��
*****************************************************************************/
u8 Lcd_ReadData(void)													 ///////////////////
{
	u8 i;
	u8 state; 

	for(i = 0; i < 8; i++)
	{
	 	state = state << 1;
		SCLK=1;
		if(SID == 1)
		{
			state = state | 0x01;	//SIDΪ1ʱ��state��ӦλΪ1
		}	
		SCLK=0;	    		
	}	
	return state; 
}
/*****��ʾһ���ֽ�*****/
void Disp_achar(u8 x,u8 y,u8 achar)//x:0~3   y:
{ 
	Lcd_WriteCmd(addr_tab[8*x+y]); //д��ַ
	Lcd_WriteData(achar);    //д����
}

/*****��ʾ����*****/
void hanzi_Disp(u8 x,u8 y,u8 *s)//x:0~3   y:
{ 
Lcd_WriteCmd(addr_tab[8*x+y]); //д��ַ
while(*s>0)
    { 
      Lcd_WriteData(*s);    //д����
      s++;     
    }
}
void Table_Disp(u8 x,u8 y,u8  *s)//x:0~3   y:
{
	Lcd_WriteCmd(addr_tab[8*x+y]); //д��ַ
	while(*s)
    { 
      Lcd_WriteData(*s);    //д����
      s++;     
    }
}
/*****���㺯��*****/
void Lcd_DrawPoint(u8 X,u8 Y,u8 Color )	  //0000 1111     0000 0000    
{
    u8 Row , Lie , Lie_bit    ;
    u8 ReadOldH, ReadOldL ;
    Lcd_WriteCmd(0x34);
    Lcd_WriteCmd(0x36);

    Lie=X>> 4 ;    							   //0000 0000		  ����4��ͬ�ڳ���16���õ����ַ
    Lie_bit=X&0x0f ;						   //0000 1111		  15	�õ�С��ַ
    if(Y<32)
    {
        Row=Y;								  //0000 0000		   0
    }
    else
    {
        Row=Y-32;
        Lie+=8;
    }

    Lcd_WriteCmd(Row+0x80);						 //0
    Lcd_WriteCmd(Lie+0x80);					 //0

    Lcd_ReadData();		                 //���һ��
    ReadOldH =Lcd_ReadData(); 
    ReadOldL =Lcd_ReadData(); 

    Lcd_WriteCmd(Row+0x80);
    Lcd_WriteCmd(Lie+0x80);

    if(Lie_bit<8)
    {
        switch( Color)
        {
            case 0 : ReadOldH&=(~(0x01<<(7-Lie_bit))) ; break;       //���õ��Ϊ0
            case 1 : ReadOldH|=(0x01<<(7-Lie_bit)); break;		      //���õ��Ϊ1
            case 2 : ReadOldH^=(0x01<<(7-Lie_bit )); break;		  //���õ㷭ת
            default : break ;    
        }
        Lcd_WriteData(ReadOldH) ;
        Lcd_WriteData(ReadOldL) ;
    }
    else
    {
        switch(Color)
        {
            case 0 : ReadOldL&=(~(0x01<<(15-Lie_bit)));break ;		   //���õ��Ϊ0
            case 1 : ReadOldL|=(0x01<<(15-Lie_bit));break ;		   //���õ��Ϊ1
            case 2 : ReadOldL^=(0x01<<(15-Lie_bit));break ;		//���õ㷭ת
            default : break ;
        }
        Lcd_WriteData(ReadOldH);
        Lcd_WriteData(ReadOldL);
    }
    Lcd_WriteCmd(0x30);
}

/*********ˮƽ�ߺ���*********�����⣺���ܻ�ˮƽ��			 */
void Lcd_Level(u8 X0,u8 X1,u8 Y,u8 Color )
{   
	u8 Temp ;
    if( X0 > X1 )
    {
        Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    Lcd_DrawPoint( X0, Y, Color ) ;    
}

/*********��ֱ�ߺ���***********/
void Lcd_Vertical(u8 X,u8 Y0,u8 Y1,u8 Color )
{
    u8 Temp ;
    if( Y0 > Y1 )
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)
    Lcd_DrawPoint( X, Y0, Color)    ;
}

/*******����б���߶�***********/
void Lcd_AnyLine(u8 StartX,u8 StartY,u8 EndX,u8 EndY,u8 Color )
{
    s16 t, distance;      /*������Ļ��С�ı��������(���Ϊint��)*/		 //////////////////////////////////////////////////////
    s16 x=0,y=0,delta_x,delta_y ;
    s8 incx, incy ;													///////////////////////////////////////////////////////
    delta_x=EndX-StartX;
    delta_y=EndY-StartY;
    if(delta_x>0)
    {
        incx = 1;
    }
    else if( delta_x == 0 )
    {
        Lcd_Vertical( StartX, StartY, EndY, Color ) ;
        return ;
    }
    else
    {
        incx = -1 ;				   ///////////////////////
    }
    if( delta_y > 0 )
    {
        incy = 1 ;
    }
    else if(delta_y == 0 )
    {
        Lcd_Level( StartX, EndX, StartY, Color ) ;    
        return ;
    }
    else
    {
        incy = -1 ;					   
    }
	if(delta_x<0)
	delta_x=-delta_x;
	if(delta_y<0)
	delta_y=-delta_y;

    if( delta_x > delta_y )
    {
        distance = delta_x ;
    }
    else
    {
        distance = delta_y ; 
    }
    Lcd_DrawPoint( StartX, StartY, Color ) ;    
    /* Draw Line*/
    for( t = 0 ; t <= distance+1 ; t++ )
    {
        Lcd_DrawPoint( StartX, StartY, Color ) ;
        x += delta_x ;
        y += delta_y ;
        if( x > distance )
        {
            x -= distance ;
            StartX += incx ;
        }
        if( y > distance )
        {
            y -= distance ;
            StartY += incy ;
        }
    }
}

/*****��ʾͼƬ����ʾGdram���ݣ�*****/

/*��ͼ��ʾ�Ĳ���
1 �رջ�ͼ��ʾ����
2 �Ƚ���ֱ������(Y)д���ͼRAM��ַ
3 �ٽ�ˮƽ��λԪ������(X)д���ͼRAM��ַ
4 ��D15��D8д��RAM��
5 ��D7��D0д�뵽RAM��
6 �򿪻�ͼ��ʾ����*/

//ͼƬȡģ��ʽ������ȡģ���ֽ�����
void pic_Disp(u8 *pic)    //��ʾGdram���ݣ���ʾͼƬ��
{
u8 x,y;
//for(i=0;i<9;i=i+8)
for(y=0;y<16;y++)
{
  for(x=0;x<1;x++)
  {
   Lcd_WriteCmd(0x36);   //����ָ�����ͼ��ʾ
   Lcd_WriteCmd(0x80+y);  //�е�ַ
   Lcd_WriteCmd(0x80+x);  //�е�ַ
   Lcd_WriteData(*pic++);  //д���� D15��D8
   Lcd_WriteData(*pic++);  //д���� D7��D0
   Lcd_WriteCmd(0x30);
  }
}
}


/*****������ʾ����*****/
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
void LcdFill_Vertical(void)
{  
u8 x,y,i;
for(i=0;i<9;i=i+8)   
for(y=0;y<0x20;y++)
{     
  for(x=0;x<8;x++)
       { 
          Lcd_WriteCmd(0x36);   //����ָ�� ��ͼ��ʾ
          Lcd_WriteCmd(y+0x80);       //�е�ַ
          Lcd_WriteCmd(x+0x80+i);     //�е�ַ     
          Lcd_WriteData(0x55);   //д���� D15��D8 
          Lcd_WriteData(0x55);   //д���� D7��D0  
          Lcd_WriteCmd(0x30);
       }
} 
}

/*****������ʾ����*****/
void LcdFill_Level(void)
{ 
  u8 x,y,i;
    u8 k;   
    for(i=0;i<9;i+=8)    
       for(y=0;y<0x20;y++)
       {     
   if((y%2)==1) k=0xff;
            else k=0;
            for(x=0;x<8;x++)
            {  
       			Lcd_WriteCmd(0x36);   //����ָ�� ��ͼ��ʾ
                Lcd_WriteCmd(y+0x80); //�е�ַ
                Lcd_WriteCmd(x+0x80+i); //�е�ַ            
                Lcd_WriteData(k);
                Lcd_WriteData(k);
         		Lcd_WriteCmd(0x30);
            }
        }     
}

/*****������������򰵵�*****/
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
//��i=0ʱ��ʾ����128��32,��i=8ʱ��ʾ����128��32
void Lcd_Fill(u8 test)
{  
u8 x,y,i;
    for(i=0;i<9;i=i+8)   
		for(y=0;y<0x20;y++)
		{     
		  for(x=0;x<8;x++)
		       { 
		          Lcd_WriteCmd(0x36);   //����ָ�� ��ͼ��ʾ
		          Lcd_WriteCmd(y+0x80);       //�е�ַ
		          Lcd_WriteCmd(x+0x80+i);     //�е�ַ     
		          Lcd_WriteData(test);   //д���� D15��D8 
		          Lcd_WriteData(test);   //д���� D7��D0  
		      	  Lcd_WriteCmd(0x30);
		       }
		} 
}
/******************************************************** 
 ���ƣ�GUI_Point(���) 
 ˵����X:������(�����е�λ��,0~127)    Y:������(�����е�λ��,0~63) 
       ����Datesheet���Ȱ�����д��GDRAM����ȡ��ǰλ�õ����ݣ����������λ���ٰ���������д�� 
           GDRAMÿ����16��λԪ����64�У�ÿ��λԪ������8λ(�ߵ�8λ) 
           ��������Ǹ�λԪ���ڸ�λԪ����һλ 
�׳��ֵ����⣺1�����λ�õĶ�λ 
              2����ȡ��ǰ��ʾ������ 
              3�������λ��������д�˵������� 
********************************************************/ 
void GUI_Point(u8 x,u8 y,u8 flag) 
{       
   u8 x_Dyte,x_byte;                       //�����е�ַ���ֽ�λ�������ֽ��е���1λ  
   u8 y_Dyte,y_byte;                       //����Ϊ����������(ȡֵΪ0��1)���е�ַ(ȡֵΪ0~31)
   u8 GDRAM_hbit,GDRAM_lbit; 
   Lcd_WriteCmd(0x36);                       //��ͼģʽ���� 
   /***X,Y���껥��������ͨ��X,Y����***/ 
   x_Dyte=x/16;                               //������16���ֽ��е���һ�� 
   x_byte=x&0x0f;                             //�����ڸ��ֽ��е���һλ 
   y_Dyte=y/32;                               //0Ϊ�ϰ�����1Ϊ�°��� 
   y_byte=y&0x1f;                             //������0~31���е���һ�� 
    
   Lcd_WriteCmd(0x80+y_byte);                //�趨�е�ַ(y����) 
   Lcd_WriteCmd(0x80+x_Dyte+8*y_Dyte);       //�趨�е�ַ(x����)����ͨ��8*y_Dyteѡ�������� 
                       
   Lcd_ReadData(); 
   GDRAM_hbit=Lcd_ReadData();                    //��ȡ��ǰ��ʾ��8λ���� 
   GDRAM_lbit=Lcd_ReadData();                    //��ȡ��ǰ��ʾ��8λ���� 
   delay_us(5); 
    
   if(flag==1) 
   {  
     Lcd_WriteCmd(0x80+y_byte);                //�趨�е�ַ(y����) 
     Lcd_WriteCmd(0x80+x_Dyte+8*y_Dyte);       //�趨�е�ַ(x����)����ͨ��8*y_Dyteѡ�������� 
     delay_us(5); 
         if(x_byte<8)                                   //�ж����ڸ�8λ�������ڵ�8λ 
     { 
          Lcd_WriteData(GDRAM_hbit|(0X01<<(7-x_byte)));   //��ʾGDRAM����8λ���� 
      Lcd_WriteData(GDRAM_lbit);                      //��ʾGDRAM����8λ����  
         } 
         else 
         { 
          Lcd_WriteData(GDRAM_hbit); 
          Lcd_WriteData(GDRAM_lbit|(0x01<<(15-x_byte))); 
         }  
   } 
   else 
   { 
     Lcd_WriteData(0x00);                           //���GDRAM����8λ���� 
     Lcd_WriteData(0x00);                           //���GDRAM����8λ���� 
   }         
}

/***************************************** 
���ƣ�Draw_circle (������λ�û�Բ) 
˵����ʹ��Bresenham����1/8��Բ�����öԳ��Ի���������7/8��Բ  
   	 ��32���ϻ�û����

    ����ͼ��Բ��Ϊ8�� 
        0 
      7   1 
    6       2 
      5   3 
        4  
*****************************************/ 
void Draw_circle(u8 x0,u8 y0,u8 r) 
{ 
s16 a,b; 													   ///////////////////////////////////
s16 di; 
a=0; 
b=r; 
di=3-2*r;             //�ж��¸���λ�õı�־ 
while(a<=b) 
{ 
  GUI_Point(x0-b,y0-a,1);           //3            
  GUI_Point(x0+b,y0-a,1);           //0            
  GUI_Point(x0-a,y0+b,1);           //1      
  GUI_Point(x0-b,y0-a,1);           //7            
  GUI_Point(x0-a,y0-b,1);           //2            
  GUI_Point(x0+b,y0+a,1);           //4                
  GUI_Point(x0+a,y0-b,1);           //5 
  GUI_Point(x0+a,y0+b,1);           //6  
  GUI_Point(x0-b,y0+a,1);            
  a++; 
  /***ʹ��Bresenham�㷨��Բ**/    
  if(di<0) 
  di +=4*a+6; 
  else 
  { 
  di +=10+4*(a-b);    
  b--; 
  }  
  GUI_Point(x0+a,y0+b,1); 
  } 
}
