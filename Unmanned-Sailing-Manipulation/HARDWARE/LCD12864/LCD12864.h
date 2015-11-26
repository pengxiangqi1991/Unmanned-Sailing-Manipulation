#ifndef __LCD12864_H
#define __LCD12864_H	 
						   
#include <stm32f10x_type.h>
#include <stm32f10x_gpio.h>
#include "sys.h"

/*****LCD接口定义****			*/	   

#define	SID	    PBout(12)  //数据/命令        PC8	   
#define	SCLK	PBout(13)  //写数据			 P

/*****初始化LCD  GPIO口的初始化  设置其时钟、功能等*****/
void Lcd_Init(void)	 ;

/*****串行发送一个字节*****/
void SendByte(u8 Dbyte)	;

/*****写指令*****/
void Lcd_WriteCmd(u8 Cbyte );

/*****写数据*****/
void Lcd_WriteData(u8 Dbyte );

/*****读数据*****/
u8 Lcd_ReadData(void);
void Disp_achar(u8 x,u8 y,u8 achar);////x:0~3   y:
/*****显示汉字*****/
void hanzi_Disp(u8 x,u8 y,u8 *s) ;
void Table_Disp(u8 x,u8 y,u8  *s);//x:0~3   y:

/*****画点函数*****/
void Lcd_DrawPoint(u8 X,u8 Y,u8 Color );	  //0000 1111     0000 0000    

/*********水平线函数**********/
void Lcd_Level(u8 X0,u8 X1,u8 Y,u8 Color );

/*********垂直线函数***********/
void Lcd_Vertical(u8 X,u8 Y0,u8 Y1,u8 Color ) ;


/*******任意斜率线段***********/
void Lcd_AnyLine(u8 StartX,u8 StartY,u8 EndX,u8 EndY,u8 Color );


/*****显示图片（显示Gdram内容）*****/

/*绘图显示的步骤
1 关闭绘图显示功能
2 先将垂直的坐标(Y)写入绘图RAM地址
3 再将水平的位元组坐标(X)写入绘图RAM地址
4 将D15－D8写入RAM中
5 将D7－D0写入到RAM中
6 打开绘图显示功能*/

//图片取模方式：横向取模，字节正序
void pic_Disp(u8 *pic);    //显示Gdram内容（显示图片）



/*****整屏显示竖条*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void LcdFill_Vertical(void);


/*****整屏显示横条*****/
void LcdFill_Level(void);


/*****整屏充满亮点或暗点*****/
//当i=0时显示上面128×32,当i=8时显示下面128×32
//当i=0时显示上面128×32,当i=8时显示下面128×32
void Lcd_Fill(u8 test) ;

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
void GUI_Point(u8 x,u8 y,u8 flag); 


/***************************************** 
名称：Draw_circle (在任意位置画圆) 
说明：使用Bresenham法画1/8个圆，在用对称性画出其他的7/8个圆  
   

    按下图把圆分为8份 
        0 
      7   1 
    6       2 
      5   3 
        4  
*****************************************/ 
void Draw_circle(u8 x0,u8 y0,u8 r); 

#endif

