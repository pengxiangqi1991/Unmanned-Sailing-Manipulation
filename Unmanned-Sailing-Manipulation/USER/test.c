#include "sys.h"
#include "usart.h"		
#include "usart2.h"		
#include "usart3.h" 
#include "delay.h"	
#include "led.h"    	 	 
#include "key.h"	 	 
#include "exti.h"	 	 
#include "wdg.h" 	 
#include "timer.h"	 	 	 
#include "lcd.h"
#include "ahrs.h"
#include "usmart.h"		 	 
#include "string.h"	
#include "gps.h" 			    
#include "hc05.h"
#include "adc.h"
#include "LCD12864.h"

#include "gprs.h" 	 
#include "24cxx.h"

/////////SD////////////////	以下
#include "mmc_sd.h"
#include "ff.h"
#include "integer.h"
#include "diskio.h"	 
#include "stdlib.h"

#define LCD_ON 1	//打开彩屏，则关闭串口3
#define REPORT_GPRS 1

FATFS fs;
UINT DataNum=1; //序号计数器
FIL file;
FRESULT res;
UINT br;
u16 i;
/////////SD////////////////以上
/////////GPRS////////////////	以下
u8 sj[10]={0,1,2,3,4,5,6,7,8,9},send_data[128]={'A','T','%','I','P','S','E','N','D','=','"'}; //AT发送数据命令标识数组
u8 all_date[128]={1,2,3,4,5};	//存放一次发送的全部数据数组
u8 a,b,c; //全局变量，用做循环   ///////////GPS数据///////////////
u8 date[20]; //年月日数组	  ///////////GPS数据///////////////
u8 time[20]; //时间数组	  ///////////GPS数据///////////////
u8 lon[20]; //经度数组	  ///////////GPS数据///////////////
u8 lat[20]; //纬度数组	  ///////////GPS数据///////////////
/////////GPRS////////////////以上
u16 WindDirAdc,FanDirAdc;	//风向	
u16 WindDir;	//风向	
//float WindDirDegree,FanPosDegree;
u16 WindDirDegree,FanPosDegree;

__align(4) u8 dtbuf[128];   								//打印缓存器
/////////////////////////////////////SD
void WriteSd(void)
{
	LCD_ShowxNum(180,0,DataNum,10,16,0);
	f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //以读写方式打开不存在则建立
	f_lseek(&file,file.fsize); //移动指针
	f_printf(&file,"%d",DataNum);	  //写序号
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"%d",longitude*10000); //经度	
	f_write(&file," ",1,&br);  //写空格
	f_printf(&file,"%d",latitude*100000); //纬度	
	f_write(&file," ",1,&br);  //写空格
	f_printf(&file,"%d",pitch*10); //纬度pitch);	// 偏航角	
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"%d",roll*10); //roll);	// 俯仰	
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"%d",yaw*10); //yaw);	// 滚转	
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"%d",svnum); //svnum);	//	//可见卫星数	
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"%d",speed*100); //speed);	//地面速率	
	f_write(&file," ",1,&br);  //写空格

	f_printf(&file,"\n");	 //换行
	f_close(&file);			 //关闭文件
	LED1=!LED1;				  	 //写卡指示灯
	DataNum++;
}
	  
/////////////////////彭向岐20130410添加///////
void InitSd(void)
{
	///////////////	初始化文件系统/////////									  
	f_mount(0, &fs);			
	res=f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //以读写方式打开不存在则建立	  //返回0表示成功
	f_close(&file);			 //关闭文件
	f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //以读写方式打开不存在则建立
	f_lseek(&file,file.fsize); //移动指针
	f_write(&file,"\\\\\\\\\\\\\\\\\\\\",10,&br);				   //每次rest后分隔文件
	f_printf(&file,"\n");
	f_close(&file);			 //关闭文件
////////卡初始化ok,完成绿灯闪。错误红灯闪///////
	if(res==0)//初始化SD卡成功				 
	{ 
		for(i=0;i<10;i++)
		  {
		 	 LED1=!LED1;
		  	delay_ms(100);
		  }
		  LED0=1;
	  	LCD_ShowString(30,60,200,16,16, "Init_SD OK");
	  	delay_ms(1000);

	 }
	 else  //失败
	  {
		  while(1)
		   {
		   LED0=!LED0;	
		    delay_ms(100);
		  	LCD_ShowString(30,60,200,16,16, "Init_SD ERROR");
		   }
	}

}

void ReportGprs(void)
{
//u8 i;
//
u32 temp;
u16 temp2;

		if(q==1) //如果收到信息并连接PC成功
	    {
//		LED0 = 0;	
 	POINT_COLOR=BLUE;
	delay_ms(300);
all_date[0] = ':';
temp = (u32)(latitude*100000);
all_date[1] = temp/1000000+'0';
all_date[2] = temp%1000000/100000+'0';
all_date[3] = '.';
all_date[4] = temp%100000/10000+'0';
all_date[5] = temp%10000/1000+'0';
all_date[6] = temp%1000/100+'0';
all_date[7] = temp%100/10+'0';
all_date[8] = temp%10+'0';
all_date[9] = 'A';


temp = (u32)(longitude*100000);
all_date[10] = temp/10000000+'0';
all_date[11] = temp%10000000/1000000+'0';
all_date[12] = temp%1000000/100000+'0';
all_date[13] = '.';
all_date[14] = temp%100000/10000+'0';
all_date[15] = temp%10000/1000+'0';
all_date[16] = temp%1000/100+'0';
all_date[17] = temp%100/10+'0';
all_date[18] = temp%10+'0';
all_date[19] = 'B';
roll = 0;
if(roll<0.0)
{
	all_date[20] = '-';
	temp = (u32)(0-roll*10);
}
else
	{
	all_date[20] = '0';
	temp = (u32)(roll*10);
	}
//all_date[20] = '0';
all_date[21] = temp%10000/1000+'0';
all_date[22] = temp%1000/100+'0';
all_date[23] = temp%100/10+'0';
all_date[24] = '.';
all_date[25] = temp%10+'0';
all_date[26] = 'C';

if(yaw<0.0)
{
	all_date[27] = '-';
	temp = (u32)(0-yaw*10);
}
else
	
	{
	all_date[27] = '0';
	temp = (u32)(yaw*10);
	}
all_date[28] = temp%10000/1000+'0';
all_date[29] = temp%1000/100+'0';
all_date[30] = temp%100/10+'0';
all_date[31] = '.';
all_date[32] = temp%10+'0';
all_date[33] = 'D';

all_date[34] = Frequence/1000+'0';	//风速 Frequence  9999
all_date[35] = Frequence%1000/100+'0';	//风速 Frequence  9999
all_date[36] = Frequence%100/10+'0';	//风速 Frequence  9999
all_date[37] = Frequence%10+'0';	//风速 Frequence  9999
all_date[38] = 'E';

temp2	= (u16)(WindDirDegree*10);	//风向  WindDirDegree   1.5~356.0
all_date[39] = temp2/1000+'0';
all_date[40] = temp2%1000/100+'0';	
all_date[41] = temp2%100/10+'0';
all_date[42] = '.';	
all_date[43] = temp2%10+'0';
all_date[44] = 'F';

all_date[45] = svnum/10+'0';  //可见卫星数量
all_date[46] = svnum%10+'0';
all_date[47] = 'G';

temp = (u32)(speed*100); //GPS获得的速度
all_date[48] = temp%100000/10000+'0';
all_date[49] = temp%10000/1000+'0';
all_date[50] = temp%1000/100+'0';
all_date[51] = '.';
all_date[52] = temp%100/10+'0';
all_date[53] = temp%10+'0';
all_date[54] = 'H';

//temp_uint16	= (u16)(FanPosDegree*10);	//帆向  FanPosDegree   1.5~356.0
temp2	= FanPosDegree;	//帆向  FanPosDegree   10~3600
all_date[55] = temp2/1000+'0';
all_date[56] = temp2%1000/100+'0';	
all_date[57] = temp2%100/10+'0';
all_date[58] = temp2%10+'0';
all_date[59] = 'I';

temp2	= GoalCompsDri;	//帆向  FanPosDegree   10~3600
all_date[60] = temp2/1000+'0';
all_date[61] = temp2%1000/100+'0';	
all_date[62] = temp2%100/10+'0';
all_date[63] = temp2%10+'0';

all_date[64] = '!';

array_copy(all_date,send_data);	//将数据转化为十六进制数并与发送数据的AT命令存放到一个数组中
SecondDataOnline(send_data,30); //具有断线自动连接的发送函数--返回ERROR则重新连接IP地址
LCD_ShowString(0,140,200,16,16,"Sending...");	 	   
POINT_COLOR=RED;
	
	 }//if() 收到信息

}

void LcdDispAll(void)
{
	u8 temp_posx=80;
	u8 temp_posy=0;

  	POINT_COLOR=BLUE;
	sprintf((char *)dtbuf,"WindDirD:%d    ",WindDirDegree);		//风向
 	LCD_ShowString(temp_posx,temp_posy,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"FanPosD:%d    ",FanPosDegree);		//风向
	LCD_ShowString(temp_posx,temp_posy+20,200,16,16,dtbuf);//帆向	 	   

	sprintf((char *)dtbuf,"roll:%5.1f    ",roll);	// 俯仰
 	LCD_ShowString(temp_posx,temp_posy+40,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"yaw:%5.1f   ",yaw);	// 滚转
 	LCD_ShowString(temp_posx,temp_posy+60,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"svnum:%2d    ",svnum);	//	//可见卫星数
 	LCD_ShowString(temp_posx,temp_posy+80,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"speed:%5.2f    ",speed);	//地面速率
 	LCD_ShowString(temp_posx,temp_posy+100,200,16,16,dtbuf);	 	   

	sprintf((char *)dtbuf,"longitude:%.5f     ",longitude);	// //纬度
 	LCD_ShowString(temp_posx,temp_posy+120,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"latitude:%.5f     ",latitude);	// //经度
 	LCD_ShowString(temp_posx,temp_posy+140,200,16,16,dtbuf);	 	   

	sprintf((char *)dtbuf,"GoalCompsDri_PC:%d      ",GoalCompsDri);	// 
	LCD_ShowString(temp_posx,temp_posy+160,200,16,16,dtbuf);
	sprintf((char *)dtbuf,"GoalSteer_PC:%d      ",GoalSteer);	// 
	LCD_ShowString(temp_posx,temp_posy+180,200,16,16,dtbuf);
	sprintf((char *)dtbuf,"GoalSpeed_PC:%d      ",GoalSpeed);	// 
	LCD_ShowString(temp_posx,temp_posy+200,200,16,16,dtbuf);

	sprintf((char *)dtbuf,"Frequence:%d      ",Frequence);	// 
	LCD_ShowString(temp_posx,temp_posy+220,200,16,16,dtbuf);

 	POINT_COLOR=RED;
}
/////////////////////////////////////SDu16 SailPos,SteerPos; //接收的电机和舵机控制参数

int main(void)
{	
//	u8 temp_buf[30];	
	u16 times=0;  
//	u16 lenx;
//	u16 temp_dat = 0;
  	Stm32_Clock_Init(9);	//系统时钟设置
	delay_init(72);			//延时初始化
	uart_init(72,9600); 	//串口1初始化为38400
	USART2_Init(36,9600);	//挂载GPRS，用于和翯男上位机通信20130512
if(!LCD_ON)
	USART3_Init(36,115200);
//	AT24CXX_Init();		//IIC初始化 
	EXTIX_Init();	
	LED_Init();				//初始化与LED连接的硬件接口
 	POINT_COLOR=RED;
	Timerx_Init(720,99);//1Khz    1ms 
	Adc_Init();

	if(LCD_ON)
	    LCD_Init();				//初始化LCD
//    Lcd_Init();//12864初始化
	if(!LCD_ON)
		USART3_Put_String("USART3 OK\r\n");
	delay_ms(2000);
while(1)
{
	times++;
	delay_ms(1);   
	if(times%500==0)
	{
		LED1=!LED1;//闪烁LED,提示系统正在运行.
//		LCD_ShowString(0,0,200,16,16, "Wait_CREG...");	 	   
		sprintf((char *)dtbuf,"GprsState=:%d     ",GprsState);	// //经度
	 	LCD_ShowString(0,0,200,16,16,dtbuf);	 	   
	}
	if(times%500==0)
	{
		GprsCommand();
		CheckGprsAck();
	}

}
	if(REPORT_GPRS)
	{
		LCD_ShowString(0,0,200,16,16, "Wait_CREG...");	 	   
		Wait_CREG(); //等待注册		//OK
		LCD_ShowString(0,20,200,16,16, "APNconect...");	 	   
		Second_AT_Command("AT+CGDCONT=1,\"IP\",\"CMNET\"",30); //接入APN无线接入点	  //OK
		LCD_ShowString(120,20,200,16,16,"PPPdialed...");	 	   
		Second_AT_Command("AT%ETCPIP=\"\",\"\"",30); //进行PPP拨号					 //OK
		
		ConnectDNS("pengxiangqi1991.eicp.net"); //连接到域名
	}
	InitMotor();
while(1)
{
	UART1_CommandRoute();//--这个子程序需要在主程序中 接收IMU数据
	if(Hand("IPDATA:"))  //收到数据
	{
		GetCommandFrPC(Find_Str_Pos("IPDATA:"));//获得数据
	}
	if(REPORT_GPRS)
	{
		if(q == 1)
			ReportGprs();
	}

//	if(!LCD_ON)
//		USART3_CommandRoute(); //获得FriendlyARM发送的控制信息
//	times++;
//	FanDirAdc=Get_Adc(FAN_DIR_CH);//CH0:PA0	   以PA0为模拟信号输入口   输入电压勿大于3.3V
//	WindDirAdc=Get_Adc(WIND_DIR_CH);//CH1:PA1	   以PA1为模拟信号输入口   输入电压勿大于3.3V
//
//	ADC_getValues(&FanDirAdc,&WindDirAdc);//循环队列采集平均数据
//	if(temp_dat < FanDirAdc)
//		temp_dat = FanDirAdc;
//	FanPosDegree = 3600 - (u16)(FanDirAdc/4060.0*3600.0);	   ///3.27V
//	WindDirDegree = (u16)(WindDirAdc/4060.0*3600.0);
//	if(Hand("IPDATA:"))  //收到数据
//	{
//		GetCommandFrPC(Find_Str_Pos("IPDATA:"));//获得数据
//	}
//	SteerSailAdjust(GoalSteer,GoalCompsDri,GoalSpeed);//舵机、帆角度、电机速度
//	if(times%7==0)
//	{
//		if(LCD_ON) //如果打开液晶调试，则显示
//		{
//			LcdDispAll();
//		}
//		else   //否则打开串口3
//		{
////			UART3_ReportAllData(yaw*10,pitch*10,roll*10,0,0,0,0,
////								ax,ay,az,gx,gy,gz,hx,hy,hz,
////								longitude*100000,latitude*100000,svnum,speed*1000);  //nmea_msg *gpsx是定义的结构体
////			UART3_RepWindFanDir(WindDirDegree,FanPosDegree);
//		}
//		LED1=!LED1;//闪烁LED,提示系统正在运行.
//		if(REPORT_GPRS)
//		{
//			if(q == 1)
//				ReportGprs();
//		}
//	}
//	delay_ms(1);   
}
}	
























