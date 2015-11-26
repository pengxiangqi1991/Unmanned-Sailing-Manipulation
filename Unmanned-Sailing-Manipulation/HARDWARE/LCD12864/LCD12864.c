#include <stm32f10x_lib.h>
#include <stm32f10x_gpio.h>
	   
#include "LCD12864.h"
#include "delay.h"

 
/*****汉字地址表*****/
u8 addr_tab[]={
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,//第一行汉字位置
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,//第二行汉字位置
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,//第三行汉字位置
0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,//第四行汉字位置
};

/*****初始化LCD*****/
void Lcd_Init(void)
{ 
	RCC->APB2ENR|=1<<3;    //使能PORTb时钟	   A是2   E是6///
	
	GPIOB->CRH&=0XFF00FFFF; 
	GPIOB->CRH|=0X00330000;//PB12 13 推挽输出   	 
											  


	delay_ms(50);
    Lcd_WriteCmd(0x30);        //选择基本指令集
	delay_ms(1);
    Lcd_WriteCmd(0x30);        //选择8bit数据流
	delay_ms(1);
    Lcd_WriteCmd(0x0c);        //开显示(无游标、不反白)
	delay_ms(1);
    Lcd_WriteCmd(0x01);        //清除显示，并且设定地址指针为00H
	delay_ms(30);
}

/*****串行发送一个字节*****/
void SendByte(u8 Dbyte)
{
     u8 i;
     for(i=0;i<8;i++)
     {
       SCLK = 0;

       //Dbyte=Dbyte<<1;   //左移一位
       //SID = CY;         //移出的位给SID   CY用什么替换	？	 已解决
							/*DS=temp&0x80; 	正点给的
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

/*****写指令*****/
void Lcd_WriteCmd(u8 Cbyte )
{
     delay_ms(1);//检测忙的子程序我们就不写了，因为串行的不支持读操作，我们就用个延时吧
     SendByte(0xf8);            //11111,RW(0),RS(0),0		 0xf8要改吗？
     SendByte(0xf0&Cbyte);      //高四位
     SendByte(0xf0&Cbyte<<4);   //低四位(先执行<<)
}
/*****写数据*****/
void Lcd_WriteData(u8 Dbyte )
{
     delay_ms(1);
     SendByte(0xfa);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dbyte);      //高四位
     SendByte(0xf0&Dbyte<<4);   //低四位(先执行<<)
}
/******************************************************************************
** 函数名称 ：
** 函数功能 ：读取1个字节 												  
** 入口参数 ：
** 出口参数 ：
** 说明     ：
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
			state = state | 0x01;	//SID为1时，state相应位为1
		}	
		SCLK=0;	    		
	}	
	return state; 
}
/*****显示一个字节*****/
void Disp_achar(u8 x,u8 y,u8 achar)//x:0~3   y:
{ 
	Lcd_WriteCmd(addr_tab[8*x+y]); //写地址
	Lcd_WriteData(achar);    //写数据
}

/*****显示汉字*****/
void hanzi_Disp(u8 x,u8 y,u8 *s)//x:0~3   y:
{ 
Lcd_WriteCmd(addr_tab[8*x+y]); //写地址
while(*s>0)
    { 
      Lcd_WriteData(*s);    //写数据
      s++;     
    }
}
void Table_Disp(u8 x,u8 y,u8  *s)//x:0~3   y:
{
	Lcd_WriteCmd(addr_tab[8*x+y]); //写地址
	while(*s)
    { 
      Lcd_WriteData(*s);    //写数据
      s++;     
    }
}
/*****画点函数*****/
void Lcd_DrawPoint(u8 X,u8 Y,u8 Color )	  //0000 1111     0000 0000    
{
    u8 Row , Lie , Lie_bit    ;
    u8 ReadOldH, ReadOldL ;
    Lcd_WriteCmd(0x34);
    Lcd_WriteCmd(0x36);

    Lie=X>> 4 ;    							   //0000 0000		  右移4等同于除以16，得到大地址
    Lie_bit=X&0x0f ;						   //0000 1111		  15	得到小地址
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

    Lcd_ReadData();		                 //虚读一次
    ReadOldH =Lcd_ReadData(); 
    ReadOldL =Lcd_ReadData(); 

    Lcd_WriteCmd(Row+0x80);
    Lcd_WriteCmd(Lie+0x80);

    if(Lie_bit<8)
    {
        switch( Color)
        {
            case 0 : ReadOldH&=(~(0x01<<(7-Lie_bit))) ; break;       //将该点变为0
            case 1 : ReadOldH|=(0x01<<(7-Lie_bit)); break;		      //将该点变为1
            case 2 : ReadOldH^=(0x01<<(7-Lie_bit )); break;		  //将该点翻转
            default : break ;    
        }
        Lcd_WriteData(ReadOldH) ;
        Lcd_WriteData(ReadOldL) ;
    }
    else
    {
        switch(Color)
        {
            case 0 : ReadOldL&=(~(0x01<<(15-Lie_bit)));break ;		   //将该点变为0
            case 1 : ReadOldL|=(0x01<<(15-Lie_bit));break ;		   //将该点变为1
            case 2 : ReadOldL^=(0x01<<(15-Lie_bit));break ;		//将该点翻转
            default : break ;
        }
        Lcd_WriteData(ReadOldH);
        Lcd_WriteData(ReadOldL);
    }
    Lcd_WriteCmd(0x30);
}

/*********水平线函数*********有问题：不能画水平线			 */
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

/*********垂直线函数***********/
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

/*******任意斜率线段***********/
void Lcd_AnyLine(u8 StartX,u8 StartY,u8 EndX,u8 EndY,u8 Color )
{
    s16 t, distance;      /*根据屏幕大小改变变量类型(如改为int型)*/		 //////////////////////////////////////////////////////
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

/*****显示图片（显示Gdram内容）*****/

/*绘图显示的步骤
1 关闭绘图显示功能
2 先将垂直的坐标(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM中
5 将D7－D0写入到RAM中
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
void pic_Disp(u8 *pic)    //显示Gdram内容（显示图片）
{
u8 x,y;
//for(i=0;i<9;i=i+8)
for(y=0;y<16;y++)
{
  for(x=0;x<1;x++)
  {
   Lcd_WriteCmd(0x36);   //扩充指令，开绘图显示
   Lcd_WriteCmd(0x80+y);  //行地址
   Lcd_WriteCmd(0x80+x);  //列地址
   Lcd_WriteData(*pic++);  //写数据 D15－D8
   Lcd_WriteData(*pic++);  //写数据 D7－D0
   Lcd_WriteCmd(0x30);
  }
}
}


/*****整屏显示竖条*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void LcdFill_Vertical(void)
{  
u8 x,y,i;
for(i=0;i<9;i=i+8)   
for(y=0;y<0x20;y++)
{     
  for(x=0;x<8;x++)
       { 
          Lcd_WriteCmd(0x36);   //扩充指令 绘图显示
          Lcd_WriteCmd(y+0x80);       //行地址
          Lcd_WriteCmd(x+0x80+i);     //列地址     
          Lcd_WriteData(0x55);   //写数据 D15－D8 
          Lcd_WriteData(0x55);   //写数据 D7－D0  
          Lcd_WriteCmd(0x30);
       }
} 
}

/*****整屏显示横条*****/
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
       			Lcd_WriteCmd(0x36);   //扩充指令 绘图显示
                Lcd_WriteCmd(y+0x80); //行地址
                Lcd_WriteCmd(x+0x80+i); //列地址            
                Lcd_WriteData(k);
                Lcd_WriteData(k);
         		Lcd_WriteCmd(0x30);
            }
        }     
}

/*****整屏充满亮点或暗点*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void Lcd_Fill(u8 test)
{  
u8 x,y,i;
    for(i=0;i<9;i=i+8)   
		for(y=0;y<0x20;y++)
		{     
		  for(x=0;x<8;x++)
		       { 
		          Lcd_WriteCmd(0x36);   //扩充指令 绘图显示
		          Lcd_WriteCmd(y+0x80);       //行地址
		          Lcd_WriteCmd(x+0x80+i);     //列地址     
		          Lcd_WriteData(test);   //写数据 D15－D8 
		          Lcd_WriteData(test);   //写数据 D7－D0  
		      	  Lcd_WriteCmd(0x30);
		       }
		} 
}
/******************************************************** 
 名称：GUI_Point(打点) 
 说明：X:横坐标(代表列的位置,0~127)    Y:纵坐标(代表行的位置,0~63) 
       根据Datesheet需先把数据写到GDRAM，读取当前位置的数据，经过或和移位后再把数据重新写入 
           GDRAM每行有16个位元，共64行，每个位元有两个8位(高低8位) 
           计算出在那个位元，在该位元的哪一位 
易出现的问题：1、打点位置的定位 
              2、读取当前显示的数据 
              3、或和移位，即重新写人的新数据 
********************************************************/ 
void GUI_Point(u8 x,u8 y,u8 flag) 
{       
   u8 x_Dyte,x_byte;                       //定义列地址的字节位，及在字节中的哪1位  
   u8 y_Dyte,y_byte;                       //定义为上下两个屏(取值为0，1)，行地址(取值为0~31)
   u8 GDRAM_hbit,GDRAM_lbit; 
   Lcd_WriteCmd(0x36);                       //绘图模式命令 
   /***X,Y坐标互换，即普通的X,Y坐标***/ 
   x_Dyte=x/16;                               //计算在16个字节中的哪一个 
   x_byte=x&0x0f;                             //计算在该字节中的哪一位 
   y_Dyte=y/32;                               //0为上半屏，1为下半屏 
   y_byte=y&0x1f;                             //计算在0~31当中的哪一行 
    
   Lcd_WriteCmd(0x80+y_byte);                //设定行地址(y坐标) 
   Lcd_WriteCmd(0x80+x_Dyte+8*y_Dyte);       //设定列地址(x坐标)，并通过8*y_Dyte选定上下屏 
                       
   Lcd_ReadData(); 
   GDRAM_hbit=Lcd_ReadData();                    //读取当前显示高8位数据 
   GDRAM_lbit=Lcd_ReadData();                    //读取当前显示低8位数据 
   delay_us(5); 
    
   if(flag==1) 
   {  
     Lcd_WriteCmd(0x80+y_byte);                //设定行地址(y坐标) 
     Lcd_WriteCmd(0x80+x_Dyte+8*y_Dyte);       //设定列地址(x坐标)，并通过8*y_Dyte选定上下屏 
     delay_us(5); 
         if(x_byte<8)                                   //判断其在高8位，还是在低8位 
     { 
          Lcd_WriteData(GDRAM_hbit|(0X01<<(7-x_byte)));   //显示GDRAM区高8位数据 
      Lcd_WriteData(GDRAM_lbit);                      //显示GDRAM区低8位数据  
         } 
         else 
         { 
          Lcd_WriteData(GDRAM_hbit); 
          Lcd_WriteData(GDRAM_lbit|(0x01<<(15-x_byte))); 
         }  
   } 
   else 
   { 
     Lcd_WriteData(0x00);                           //清除GDRAM区高8位数据 
     Lcd_WriteData(0x00);                           //清除GDRAM区低8位数据 
   }         
}

/***************************************** 
名称：Draw_circle (在任意位置画圆) 
说明：使用Bresenham法画1/8个圆，在用对称性画出其他的7/8个圆  
   	 在32板上还没调好

    按下图把圆分为8份 
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
di=3-2*r;             //判断下个点位置的标志 
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
  /***使用Bresenham算法画圆**/    
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
