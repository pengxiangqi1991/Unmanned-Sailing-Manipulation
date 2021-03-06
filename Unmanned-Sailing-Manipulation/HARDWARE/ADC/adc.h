#ifndef __ADC_H
#define __ADC_H	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//ADC 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/7 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved					   
//////////////////////////////////////////////////////////////////////////////////	 


#define ADC_CH0  0 //通道0
#define ADC_CH1  1 //通道1
#define ADC_CH2  2 //通道2
#define ADC_CH3  3 //通道3	   
 
#define FAN_DIR_CH 0  //帆的角度ADC通道定义为ADC0
#define WIND_DIR_CH 1   //舵的角度ADC通道定义为ADC1
 
void Adc_Init(void);
u16  Get_Adc(u8 ch);  
u16 AdcGetChValue(u8 ch);   
/**************************实现函数********************************************
*函数原型:	   void ADC_FIFO_Init(void)
*功　　能:	   连续读取50次数据，以初始化FIFO数组
输入参数：  无
输出参数：  无
*******************************************************************************/
void ADC_FIFO_Init(void);
 /**************************实现函数********************************************
*函数原型:	   void  ADC_newValues(u16 x,u16 y)
*功　　能:	   更新一组数据到FIFO数组
输入参数：  ADC两通道采集的数据
输出参数：  无
*******************************************************************************/
void  ADC_newValues(u16 x,u16 y);
/**************************实现函数********************************************
*函数原型:	  void ADC_getValues(u16 *x,u16 *y) 

*功　　能:	   获得ADC两个通道的滤波之后的数值
输入参数：   	
输出参数：  无
*******************************************************************************/
void ADC_getValues(u16 *x,u16 *y); 

#endif 















