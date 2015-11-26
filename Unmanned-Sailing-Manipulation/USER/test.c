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

/////////SD////////////////	����
#include "mmc_sd.h"
#include "ff.h"
#include "integer.h"
#include "diskio.h"	 
#include "stdlib.h"

#define LCD_ON 1	//�򿪲�������رմ���3
#define REPORT_GPRS 1

FATFS fs;
UINT DataNum=1; //��ż�����
FIL file;
FRESULT res;
UINT br;
u16 i;
/////////SD////////////////����
/////////GPRS////////////////	����
u8 sj[10]={0,1,2,3,4,5,6,7,8,9},send_data[128]={'A','T','%','I','P','S','E','N','D','=','"'}; //AT�������������ʶ����
u8 all_date[128]={1,2,3,4,5};	//���һ�η��͵�ȫ����������
u8 a,b,c; //ȫ�ֱ���������ѭ��   ///////////GPS����///////////////
u8 date[20]; //����������	  ///////////GPS����///////////////
u8 time[20]; //ʱ������	  ///////////GPS����///////////////
u8 lon[20]; //��������	  ///////////GPS����///////////////
u8 lat[20]; //γ������	  ///////////GPS����///////////////
/////////GPRS////////////////����
u16 WindDirAdc,FanDirAdc;	//����	
u16 WindDir;	//����	
//float WindDirDegree,FanPosDegree;
u16 WindDirDegree,FanPosDegree;

__align(4) u8 dtbuf[128];   								//��ӡ������
/////////////////////////////////////SD
void WriteSd(void)
{
	LCD_ShowxNum(180,0,DataNum,10,16,0);
	f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //�Զ�д��ʽ�򿪲���������
	f_lseek(&file,file.fsize); //�ƶ�ָ��
	f_printf(&file,"%d",DataNum);	  //д���
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"%d",longitude*10000); //����	
	f_write(&file," ",1,&br);  //д�ո�
	f_printf(&file,"%d",latitude*100000); //γ��	
	f_write(&file," ",1,&br);  //д�ո�
	f_printf(&file,"%d",pitch*10); //γ��pitch);	// ƫ����	
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"%d",roll*10); //roll);	// ����	
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"%d",yaw*10); //yaw);	// ��ת	
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"%d",svnum); //svnum);	//	//�ɼ�������	
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"%d",speed*100); //speed);	//��������	
	f_write(&file," ",1,&br);  //д�ո�

	f_printf(&file,"\n");	 //����
	f_close(&file);			 //�ر��ļ�
	LED1=!LED1;				  	 //д��ָʾ��
	DataNum++;
}
	  
/////////////////////�����20130410���///////
void InitSd(void)
{
	///////////////	��ʼ���ļ�ϵͳ/////////									  
	f_mount(0, &fs);			
	res=f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //�Զ�д��ʽ�򿪲���������	  //����0��ʾ�ɹ�
	f_close(&file);			 //�ر��ļ�
	f_open(&file, "GPS_test.TXT", FA_OPEN_ALWAYS|FA_WRITE ); 	   //�Զ�д��ʽ�򿪲���������
	f_lseek(&file,file.fsize); //�ƶ�ָ��
	f_write(&file,"\\\\\\\\\\\\\\\\\\\\",10,&br);				   //ÿ��rest��ָ��ļ�
	f_printf(&file,"\n");
	f_close(&file);			 //�ر��ļ�
////////����ʼ��ok,����̵�������������///////
	if(res==0)//��ʼ��SD���ɹ�				 
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
	 else  //ʧ��
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

		if(q==1) //����յ���Ϣ������PC�ɹ�
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

all_date[34] = Frequence/1000+'0';	//���� Frequence  9999
all_date[35] = Frequence%1000/100+'0';	//���� Frequence  9999
all_date[36] = Frequence%100/10+'0';	//���� Frequence  9999
all_date[37] = Frequence%10+'0';	//���� Frequence  9999
all_date[38] = 'E';

temp2	= (u16)(WindDirDegree*10);	//����  WindDirDegree   1.5~356.0
all_date[39] = temp2/1000+'0';
all_date[40] = temp2%1000/100+'0';	
all_date[41] = temp2%100/10+'0';
all_date[42] = '.';	
all_date[43] = temp2%10+'0';
all_date[44] = 'F';

all_date[45] = svnum/10+'0';  //�ɼ���������
all_date[46] = svnum%10+'0';
all_date[47] = 'G';

temp = (u32)(speed*100); //GPS��õ��ٶ�
all_date[48] = temp%100000/10000+'0';
all_date[49] = temp%10000/1000+'0';
all_date[50] = temp%1000/100+'0';
all_date[51] = '.';
all_date[52] = temp%100/10+'0';
all_date[53] = temp%10+'0';
all_date[54] = 'H';

//temp_uint16	= (u16)(FanPosDegree*10);	//����  FanPosDegree   1.5~356.0
temp2	= FanPosDegree;	//����  FanPosDegree   10~3600
all_date[55] = temp2/1000+'0';
all_date[56] = temp2%1000/100+'0';	
all_date[57] = temp2%100/10+'0';
all_date[58] = temp2%10+'0';
all_date[59] = 'I';

temp2	= GoalCompsDri;	//����  FanPosDegree   10~3600
all_date[60] = temp2/1000+'0';
all_date[61] = temp2%1000/100+'0';	
all_date[62] = temp2%100/10+'0';
all_date[63] = temp2%10+'0';

all_date[64] = '!';

array_copy(all_date,send_data);	//������ת��Ϊʮ�����������뷢�����ݵ�AT�����ŵ�һ��������
SecondDataOnline(send_data,30); //���ж����Զ����ӵķ��ͺ���--����ERROR����������IP��ַ
LCD_ShowString(0,140,200,16,16,"Sending...");	 	   
POINT_COLOR=RED;
	
	 }//if() �յ���Ϣ

}

void LcdDispAll(void)
{
	u8 temp_posx=80;
	u8 temp_posy=0;

  	POINT_COLOR=BLUE;
	sprintf((char *)dtbuf,"WindDirD:%d    ",WindDirDegree);		//����
 	LCD_ShowString(temp_posx,temp_posy,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"FanPosD:%d    ",FanPosDegree);		//����
	LCD_ShowString(temp_posx,temp_posy+20,200,16,16,dtbuf);//����	 	   

	sprintf((char *)dtbuf,"roll:%5.1f    ",roll);	// ����
 	LCD_ShowString(temp_posx,temp_posy+40,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"yaw:%5.1f   ",yaw);	// ��ת
 	LCD_ShowString(temp_posx,temp_posy+60,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"svnum:%2d    ",svnum);	//	//�ɼ�������
 	LCD_ShowString(temp_posx,temp_posy+80,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"speed:%5.2f    ",speed);	//��������
 	LCD_ShowString(temp_posx,temp_posy+100,200,16,16,dtbuf);	 	   

	sprintf((char *)dtbuf,"longitude:%.5f     ",longitude);	// //γ��
 	LCD_ShowString(temp_posx,temp_posy+120,200,16,16,dtbuf);	 	   
	sprintf((char *)dtbuf,"latitude:%.5f     ",latitude);	// //����
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
/////////////////////////////////////SDu16 SailPos,SteerPos; //���յĵ���Ͷ�����Ʋ���

int main(void)
{	
//	u8 temp_buf[30];	
	u16 times=0;  
//	u16 lenx;
//	u16 temp_dat = 0;
  	Stm32_Clock_Init(9);	//ϵͳʱ������
	delay_init(72);			//��ʱ��ʼ��
	uart_init(72,9600); 	//����1��ʼ��Ϊ38400
	USART2_Init(36,9600);	//����GPRS�����ں��G����λ��ͨ��20130512
if(!LCD_ON)
	USART3_Init(36,115200);
//	AT24CXX_Init();		//IIC��ʼ�� 
	EXTIX_Init();	
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
 	POINT_COLOR=RED;
	Timerx_Init(720,99);//1Khz    1ms 
	Adc_Init();

	if(LCD_ON)
	    LCD_Init();				//��ʼ��LCD
//    Lcd_Init();//12864��ʼ��
	if(!LCD_ON)
		USART3_Put_String("USART3 OK\r\n");
	delay_ms(2000);
while(1)
{
	times++;
	delay_ms(1);   
	if(times%500==0)
	{
		LED1=!LED1;//��˸LED,��ʾϵͳ��������.
//		LCD_ShowString(0,0,200,16,16, "Wait_CREG...");	 	   
		sprintf((char *)dtbuf,"GprsState=:%d     ",GprsState);	// //����
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
		Wait_CREG(); //�ȴ�ע��		//OK
		LCD_ShowString(0,20,200,16,16, "APNconect...");	 	   
		Second_AT_Command("AT+CGDCONT=1,\"IP\",\"CMNET\"",30); //����APN���߽����	  //OK
		LCD_ShowString(120,20,200,16,16,"PPPdialed...");	 	   
		Second_AT_Command("AT%ETCPIP=\"\",\"\"",30); //����PPP����					 //OK
		
		ConnectDNS("pengxiangqi1991.eicp.net"); //���ӵ�����
	}
	InitMotor();
while(1)
{
	UART1_CommandRoute();//--����ӳ�����Ҫ���������� ����IMU����
	if(Hand("IPDATA:"))  //�յ�����
	{
		GetCommandFrPC(Find_Str_Pos("IPDATA:"));//�������
	}
	if(REPORT_GPRS)
	{
		if(q == 1)
			ReportGprs();
	}

//	if(!LCD_ON)
//		USART3_CommandRoute(); //���FriendlyARM���͵Ŀ�����Ϣ
//	times++;
//	FanDirAdc=Get_Adc(FAN_DIR_CH);//CH0:PA0	   ��PA0Ϊģ���ź������   �����ѹ�����3.3V
//	WindDirAdc=Get_Adc(WIND_DIR_CH);//CH1:PA1	   ��PA1Ϊģ���ź������   �����ѹ�����3.3V
//
//	ADC_getValues(&FanDirAdc,&WindDirAdc);//ѭ�����вɼ�ƽ������
//	if(temp_dat < FanDirAdc)
//		temp_dat = FanDirAdc;
//	FanPosDegree = 3600 - (u16)(FanDirAdc/4060.0*3600.0);	   ///3.27V
//	WindDirDegree = (u16)(WindDirAdc/4060.0*3600.0);
//	if(Hand("IPDATA:"))  //�յ�����
//	{
//		GetCommandFrPC(Find_Str_Pos("IPDATA:"));//�������
//	}
//	SteerSailAdjust(GoalSteer,GoalCompsDri,GoalSpeed);//��������Ƕȡ�����ٶ�
//	if(times%7==0)
//	{
//		if(LCD_ON) //�����Һ�����ԣ�����ʾ
//		{
//			LcdDispAll();
//		}
//		else   //����򿪴���3
//		{
////			UART3_ReportAllData(yaw*10,pitch*10,roll*10,0,0,0,0,
////								ax,ay,az,gx,gy,gz,hx,hy,hz,
////								longitude*100000,latitude*100000,svnum,speed*1000);  //nmea_msg *gpsx�Ƕ���Ľṹ��
////			UART3_RepWindFanDir(WindDirDegree,FanPosDegree);
//		}
//		LED1=!LED1;//��˸LED,��ʾϵͳ��������.
//		if(REPORT_GPRS)
//		{
//			if(q == 1)
//				ReportGprs();
//		}
//	}
//	delay_ms(1);   
}
}	
























