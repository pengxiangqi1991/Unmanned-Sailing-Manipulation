#include "sys.h"
#include "timer.h"
#include "string.h"
#include "usart2.h"
#include "delay.h"
#include "gprs.h"
#include "lcd.h"
#include "led.h"

/***************************状态机的思想修改GPRS************************************/
u8 GprsState = 0;
#define WAIT_REG_S 0
#define CGDCONT_S 1
#define ETCPIP_S 2
#define ATE0_S 3
#define DNS_S 4

/***********************************************************************/
/***************************定义接收PC的命令数据的变量************************************/
#define DATALENGTH (2*15) //n个数据*2
/***********************************************************************/
u8 Public_ip[30]; //存放公网ip地址
u16 q=0,r=0;
u8 ErrorNum;//断线重连返回ERROR次数
#define ERR_NUM 30
/***********************************变量定义************************************/
u8 runxun_ser1 = 0,Flag_Buf1_Rec = 0;
u16 Start_weizi,End_weizi;
//u16 DataStartPos;//,DataEndPos;
u16 Second_Int = 0;
/*********************************发送换行回车函数******************************/
void Send_LR(void)
{
	Send_Hex(0x0D);	
	Send_Hex(0x0A);	
}
/**********************判断缓存中是否含有指定的字符串函数***********************/
u8 Hand(u8 *a)
{ 
    if(strstr(UART2Buffer,a)!=FALSE)
	    return 1;
	else
		return 0;
}
/*******************************定位字串"+32"符位置*****************************/
void Find_dw_TEXT(void)
{
	u16 k;
	for(k=0;k<500;k++)
	{
		if(UART2Buffer[k]=='+' && UART2Buffer[k+1]=='3' && UART2Buffer[k+2]=='2')
		{
			Start_weizi = k+6;	
			break;				
		}
	}
	for(k=0;k<500;k++)
	{
		if(UART2Buffer[k]=='O' && UART2Buffer[k+1]=='K')
		{
			End_weizi = k-5;	
			break;				
		}
	}
} 
/*********************************发送命令函数**********************************/
void Second_AT_Command(u8 *b,u16 wait_time) //不允许出错  
{
	u8 i;
	u8 *c;
	c = b; //保存字符串地址到c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if(!Hand("OK")) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //将字符串地址给b
			   	for (b; *b!='\0';b++)
			   	{
					Send_Hex(*b);
				}
				Send_LR();	
				Times = 0;
				shijian = wait_time;
				Timer0_Start = 1;
		   }
        }
 	    else
		{
			i = 1;
			Timer0_Start = 0;
		}
	}
	CLR_Buf0(); 
}
/*********************************发送命令函数***********************************/
void Second_AT_Command_ip(u8 *b,u16 wait_time) //不允许出错  
{
	u8 i;
	u8 *c;
	c = b; //保存字符串地址到c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if(!Hand("CONNECT")) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //将字符串地址给b
			   	for (b; *b!='\0';b++)
			   	{
					Send_Hex(*b);
				}
				Send_LR();//OK//发送换行，开始执行命令
				Times = 0;
				shijian = wait_time;
				Timer0_Start = 1;
		   }
        }
 	    else
		{
			i = 1;
			Timer0_Start = 0;
		}
	}
	CLR_Buf0(); 
}
/*********************************发送命令函数***********************************/
void Second_AT_ERROR(u8 *b,u16 wait_time) //允许出错    
{
	u8 i;
	u8 *c;
	c = b; //保存字符串地址到c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if((!Hand("OK"))&&(!Hand("ERROR"))) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //将字符串地址给b
			   	for (b; *b!='\0';b++)
			   	{
					Send_Hex(*b);
				}
				Send_LR();	
				Times = 0;
				shijian = wait_time;
				Timer0_Start = 1;
		   }
        }
 	    else
		{
			i = 1;
			Timer0_Start = 0;
		}
	}
	CLR_Buf0(); 
}
/*******************************等待模块注册成功函数*****************************/
void Wait_CREG(void)
{
//	u8 i;
	u16 k;
//	i = 0;
//    while(i == 0)        			
	if(WAIT_REG_S == GprsState)
	{  
//		CLR_Buf0();        
		Send_ASCII("AT+CREG?");
		Send_LR();
	    for(k=0;UART2Buffer[k] != '\0';k++)      			
    	{   
			if(UART2Buffer[k] == ':')
			{
				if((UART2Buffer[k+4] == '1') || (UART2Buffer[k+4] == '5'))
				{	
//					i = 1;
					GprsState++;
					CLR_Buf0();
				   	break;
				}
			}
		}
	}
}
/*************************************数据拼接函数p1->p2*******************************/
void array_copy(u8 *p1,u8 *p2)
{
  u16 m=0;
  for(m=0;m<strlen(p1);m++) //strlen要检测到'\0'才停止
  	{
		if(p1[m]=='.')
		{
		   p2[2*m+11]='2'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='E';
		}
		else if(p1[m]==',')
		{
		   p2[2*m+11]='2'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='C';
		}
		else if(p1[m]==':')
		{
		   p2[2*m+11]='3'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='A';
		}
		else if(p1[m]=='A')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='1';
		}
		else if(p1[m]=='B')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='2';
		}
		else if(p1[m]=='C')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='3';
		}
		else if(p1[m]=='D')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='4';
		}
		else if(p1[m]=='-')
		{
		   p2[2*m+11]='2'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='D';
		}
		else if(p1[m]=='E')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='5';
		}
		else if(p1[m]=='F')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='6';
		}
		else if(p1[m]=='G')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='7';
		}
		else if(p1[m]=='H')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='8';
		}
		else if(p1[m]=='I')
		{
		   p2[2*m+11]='4'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='9';
		}
		else if(p1[m]=='!')
		{
		   p2[2*m+11]='2'; //将P1中数据转换为ASC2码并存放在P2中
		   p2[2*m+11+1]='1';
		}
		else
		{
			p2[2*m+11]=((p1[m]>>4)+0x30); //将P1中数据转换为ASC2码并存放在P2中
			p2[2*m+11+1]=((p1[m]&0x0f)+0x30);
		}
	}
  p2[11+2*strlen(p1)]='"';
  p2[11+2*strlen(p1)+1]='\0';
}  	

/*连接到域名函数20130516添加*/
void ConnectDNS(u8 *dns) //连接到域名
{
  u8 i,j,k=0,b;
	u8 ip_star,ip_end;
	u8 temp_buf[20]={'A','T','%','D','N','S','R','=','"'};	
	u8 ip_cmd[64]={'A','T','%','I','P','O','P','E','N','=','"','T','C','P','"',',','"'}; //连接PC的AT指令



	Second_AT_Command("ATE0",30);/*特别注意，取消回显，否则发送数据返回会乱码！！！*/  //取消回显			 ok


	for(i=0;i<strlen(dns);i++) //拼接域名到temp_buf数组中
	{
		temp_buf[9+i] = dns[i];	
	}
	temp_buf[9+strlen(dns)]='"';
	temp_buf[9+strlen(dns)+1]='\0';
	CLR_Buf0();
 	LCD_ShowString(0,100,240,16,16,temp_buf);	 	   
	while(1)        			
	{  
		CLR_Buf0();        
		Send_ASCII(temp_buf);
	  	Send_LR();
		delay_ms(6000); 
		
		if(!Hand("DNSR")) 
		{
	        for(i=0;UART2Buffer[i]!='\0';i++) 
	        {
	           if((UART2Buffer[i]==':'&&UART2Buffer[i+3]=='.')||(UART2Buffer[i]==':'&&UART2Buffer[i+4]=='.'))              
		           ip_star= i;    
	           if(UART2Buffer[i]=='O'&&UART2Buffer[i+1]=='K')
	           {            
		           ip_end= i;
		           break;
	           }
	        }
	        for(j=ip_star+1;j<ip_end-4;j++)
	        {
				Public_ip[k]=UART2Buffer[j];     
				k++;
	        }
	        Public_ip[k]='\0';
		  	LCD_ShowString(0,40,200,16,16,Public_ip);	 	   
		  	if('0' != Public_ip[0])
				break;	//跳出while(1)循环
	 	} //end of if(Hand("DNSR")) 
	}//end of while(1)

	Public_ip[k]='"';
	Public_ip[k+1]=',';	
	Public_ip[k+2]='1'; 
	Public_ip[k+3]='2';
	Public_ip[k+4]='3';
	Public_ip[k+5]='4';
	Public_ip[k+6]='5';	//端口号12345							   //ok
	Public_ip[k+7]=',';
	Public_ip[k+8]=',';	//,,
	Public_ip[k+9]='4';
	Public_ip[k+10]='0';
	Public_ip[k+11]='9';
	Public_ip[k+12]='8';//4098(固定格式)
	Public_ip[k+13]='\0';
	for(b=0;b<(ip_end-ip_star-5+13);b++)    //ok	//AT%IPONEN="TCP","182.201.43.17",12345,,4098 (此处的IP地址只是举例)
	{
		ip_cmd[17+b]=Public_ip[b]; //AT指令+公网IP地址					  //ok
	}
 	LCD_ShowString(0,60,240,16,16,&ip_cmd[17]);	 	   
 	LCD_ShowString(0,80,240,16,16,ip_cmd);	 	   
	Second_AT_Command_ip(ip_cmd,3000); //给GPRS发送公网IP，与PC进行连接	 //OK
	q = 1; //连接成功标志位
}
//函数说明：具有断线自动连接的发送函数
//基本方法：返回ERROR则重新连接域名获得IP地址
//2013016彭向岐添加
//返回OK部分和Second_AT_Command(u8 *b,u16 wait_time)函数一样，若返回ERROR，则执行发送域名重新连接函数
void SecondDataOnline(u8 *b,u16 wait_time) //新添加，具有断线自动连接的发送函数--返回ERROR则重新连接IP地址
{
	u8 i;
	u8 *c;
	c = b; //保存字符串地址到c
	CLR_Buf0(); 
    i = 0;
	
	while(i == 0)                    
	{
	//USART2_CommandRoute();//使用新的方式查询并或得上位机发来的数据
//		if(Hand("IPDATA:"))  //收到数据
//		{
////			Timer0_Start = 0;
//			//提取发送的数据
//GetCommandFrPC(Find_Str_Pos("IPDATA:"));//获得数据
////			i = 1;	 //跳出while循环
//		}

		if(Hand("ERROR: 14")||(Hand("IPCLOSE:"))) //上位机断关闭会返回IPCLOSE:，再次发送，会返回错误"ERROR: 14"
		{
			POINT_COLOR=RED;
		 	LCD_ShowString(120,140,200,16,16,"ReConnect");	 	   
			POINT_COLOR=BLUE;
			ConnectDNS("pengxiangqi1991.eicp.net"); //连接到域名
			i = 1;	 //跳出while循环
			Timer0_Start = 0;
		}
		else if(Hand("OK")) 
		{
		 	LCD_ShowString(120,140,200,16,16,"         ");	 	   
			i = 1;	 //跳出while循环
			Timer0_Start = 0;
        }
 	    else
		{
				if(Timer0_Start == 0)
				{
					b = c; //将字符串地址给b
				   	for (b; *b!='\0';b++)
				   	{
						Send_Hex(*b);
					}
					Send_LR();	
					Times = 0;
					shijian = wait_time;
					Timer0_Start = 1;
			   }
		}
	}
	CLR_Buf0(); 


}

/*******************************定位字串"IPDATA:"符位置*****************************/
u16 Find_Str_Pos(u8 *str)   //返回接收到的数据“”内的数据  例如 %IPDATA:3,"120ABC" 返回11
{
	u16 k,j;
	for(k=0;(UART2Buffer[k] != '\0')||(k<500);k++)
	{
		if((UART2Buffer[k]==str[0])&&(UART2Buffer[k+1]==str[1])&&(UART2Buffer[k+2]==str[2])&&(UART2Buffer[k+3]==str[3])&&(UART2Buffer[k+4]==str[4])&&(UART2Buffer[k+5]==str[5])&&(UART2Buffer[k+6]==str[6]))
		{	
//		LCD_ShowString(0,140,200,16,16,"IPDATA:");	 	   

			for(j=k;(UART2Buffer[j] != '\0')||(j<500);j++)
			{	
				if((UART2Buffer[j] ==',')&&(UART2Buffer[j+1] =='"'))
				{
//					LCD_ShowString(0,120,200,16,16,",\"");	 	   
					return (u16)(j+1+1);
				}				
			}
		}
	}	
}

///* 接收十六进制数字   */
//u8 GetCommandFrPC(u16 data_pos)// 
//{
//	u16 temp_i,temp_j = 0;
//	u8 j = 0;		 //特别注意，j如果不赋初值，随机分配变量会出现错误！！！！！
//	u8 temp_buf[50];//存储的数据临时数组
//	u8 data_buf[30];//处理后的十六进制数据，A55A....AA 校验和处理在此数组内完成
//	u8 temp_num1,temp_num2;
///*从数组UART2Buffer[]中提取第data_pos个数据,处理后开始存储到data_buf[]中*/
//	for(temp_i=data_pos;temp_i<data_pos+DATALENGTH;temp_i++)//;UART2Buffer[temp_i]!='"' 
//	{
//		temp_buf[temp_j++]=UART2Buffer[temp_i];
//	}	
//	temp_buf[temp_j] = '\0';
//
//
//	LCD_ShowString(0,120,200,16,16,temp_buf);	 	   
//
//
//	for(temp_i=0;temp_i<DATALENGTH;temp_i++)  //将收到的数据合并，“两个字节合成一个数字”
//	{
//		if(temp_buf[temp_i]<':')   //数据的高四位
//			temp_num1 = temp_buf[temp_i]-'0';
//		else
//			temp_num1 = temp_buf[temp_i]-'A' + 10;
//
//		temp_i++;
//		
//		if(temp_buf[temp_i]<':')	//数据的低四位
//			temp_num2 = temp_buf[temp_i]-'0';
//		else
//			temp_num2 = temp_buf[temp_i]-'A' + 10;
//
//	data_buf[j++] = 16*temp_num1 + temp_num2; //合并数据
//	}//end of for
//	data_buf[j] = '\0';
//	/*将收到的数据进行校验并赋值给相应的全局变量，如GoalCompsDri*/
//	if(data_buf[3]==0xa6) // 0xA6接收
//	{
//		LCD_ShowString(120,200,200,16,16,"Check ");	 	   
//		if(CommandFrPC_Sum_check(data_buf))	//A55A06A6015F02AA
//		{
//			LCD_ShowString(120,220,200,16,16,"sus");	 	   
//			GetCmd_PC_Compass(data_buf);
//		}
//		return 1; 	//成功接收数据
//	}
//
//
//	return 0;	 //接收数据失败
//}
//
/* 接收字符 */
u8 GetCommandFrPC(u16 data_pos)// 
{
	u16 temp_i,temp_j = 0;
	u8 j = 0;		 //特别注意，j如果不赋初值，随机分配变量会出现错误！！！！！
	u8 temp_buf[50];//存储的数据临时数组
	u8 data_buf[30];//处理后的十六进制数据，A55A....AA 校验和处理在此数组内完成
	u8 temp_num1,temp_num2;
/*从数组UART2Buffer[]中提取第data_pos个数据,处理后开始存储到data_buf[]中*/
	for(temp_i=data_pos;temp_i<data_pos+DATALENGTH;temp_i++)//;UART2Buffer[temp_i]!='"' 
	{
		temp_buf[temp_j++]=UART2Buffer[temp_i];
	}	
	temp_buf[temp_j] = '\0';


	LCD_ShowString(0,120,200,16,16,temp_buf);	 	   


	for(temp_i=0;temp_i<DATALENGTH;temp_i++)  //将收到的数据合并，“两个字节合成一个数字”
	{
		if(temp_buf[temp_i]<':')   //数据的高四位
			temp_num1 = temp_buf[temp_i]-'0';
		else
			temp_num1 = temp_buf[temp_i]-'A' + 10;

		temp_i++;
		
		if(temp_buf[temp_i]<':')	//数据的低四位
			temp_num2 = temp_buf[temp_i]-'0';
		else
			temp_num2 = temp_buf[temp_i]-'A' + 10;

	data_buf[j++] = 16*temp_num1 + temp_num2; //合并数据
	}//end of for
	data_buf[j] = '\0';
	/*将收到的数据进行校验并赋值给相应的全局变量，如GoalCompsDri*/
	if(data_buf[0]=='A')
	{
		GetCmd_PC_Compass(data_buf);
		return 1;
	}
	return 0;	 //接收数据失败
}

u16 GoalCompsDri; //A4位
u16 GoalSpeed; 	  //B4位
u16 GoalSteer; 	  //C4位

void GetCmd_PC_Compass(u8 *buf)
{
	GoalCompsDri = 1000*(buf[1]-'0')+100*(buf[2]-'0')+10*(buf[3]-'0')+(buf[4]-'0'); //目标航向：帆
	if(buf[5]=='B')
		GoalSpeed = 1000*(buf[6]-'0')+100*(buf[7]-'0')+10*(buf[8]-'0')+(buf[9]-'0'); //速度
	if(buf[10]=='C')
		GoalSteer = 1000*(buf[11]-'0')+100*(buf[12]-'0')+10*(buf[13]-'0')+(buf[14]-'0'); //舵机
}

 //--校验当前接收到的一帧数据是否 与帧校验字节一致
unsigned char CommandFrPC_Sum_check(u8 *temp_buf) 	//A55A06A6015F02AA
{ 
  unsigned char i;
  u32 checksum=0; 
  for(i=2;i<temp_buf[2];i++)
   checksum+=temp_buf[i];
  if((checksum%256)==temp_buf[temp_buf[2]])
   return(0x01); //Checksum successful
  else
   return(0x00); //Checksum error
}






/**************************20130618添加GPRS"状态机"方式处理函数********************************/
//函数说明：状态的方式发送GPRS指令，通过查询GprsState发送相应的指令
//基本方法：
//20130618彭向岐添加
//
void GprsCommand(void)
{
	switch (GprsState)
	{
		case WAIT_REG_S:
			Send_ASCII("AT+CREG?");//查询GPRS是否注册
			Send_LR();
			break;
		case CGDCONT_S:	////接入APN无线接入点	  //OK
			Send_ASCII("AT+CGDCONT=1,\"IP\",\"CMNET\"");
			Send_LR();
			break;
		case ETCPIP_S: //进行PPP拨号					 //OK
			Send_ASCII("AT%ETCPIP=\"\",\"\"");
			Send_LR();
			break;
		case ATE0_S:
			Send_ASCII("ATE0");///*特别注意，取消回显，否则发送数据返回会乱码！！！*/  //取消回显
			Send_LR();
			break;
//		case ATE0_S:
//			break;
		default:
			break;
	}

}
//函数说明：检查GPRS应答状态
//基本方法：
//#define CGDCONT 1
//#define ETCPIP 2
//
//20130618彭向岐添加
void CheckGprsAck(void)
{
	switch (GprsState)
	{
		case WAIT_REG_S:
			CheckRegS(); //检查是否收到注册的应答
			break;
		case CGDCONT_S:
			CheckOk(); //
			break;
		case ETCPIP_S:
			CheckOk();
			break;
		case ATE0_S:  //取消回显检查
			CheckOk();
			break;
		default:
			break;
	}
	
}




//函数说明：CheckRegS(); //检查是否收到注册的应答
//基本方法：
//20130618彭向岐添加
//
void CheckRegS(void)
{
	u16 k;
    for(k=0;UART2Buffer[k] != '\0';k++)      			
	{   
		if(UART2Buffer[k] == ':')
		{
			if((UART2Buffer[k+4] == '1') || (UART2Buffer[k+4] == '5'))
			{	
				GprsState++;//GPRS注册成功,则切换到下一状态
				CLR_Buf0();
			   	break;
			}
		}
	}
}
//函数说明：CheckOk(); //检查是否收到OK
//基本方法：
//20130618彭向岐添加
//
void CheckOk(void)
{
	u16 k;
    for(k=0;UART2Buffer[k] != '\0';k++)      			
	{   
		if(UART2Buffer[k] == 'O')
		{
			if(UART2Buffer[k+1] == 'K')	
			{
				GprsState++;//GPRS注册成功,则切换到下一状态
				CLR_Buf0();
			   	break;
			}
		}
	}
	
}
//函数说明：/*连接到域名函数20130516添加*/
//基本方法：
//20130618彭向岐添加
//
void ConnectDNS_S(u8 *dns) //连接到域名
{
	u8 i,j,k=0,b;
	u8 ip_star,ip_end;
	u8 temp_buf[20]={'A','T','%','D','N','S','R','=','"'};	
	u8 ip_cmd[64]={'A','T','%','I','P','O','P','E','N','=','"','T','C','P','"',',','"'}; //连接PC的AT指令
	for(i=0;i<strlen(dns);i++) //拼接域名到temp_buf数组中
	{
		temp_buf[9+i] = dns[i];	
	}
	temp_buf[9+strlen(dns)]='"';
	temp_buf[9+strlen(dns)+1]='\0';
LCD_ShowString(0,100,240,16,16,temp_buf);	 	   
		Send_ASCII(temp_buf);
	  	Send_LR();
		delay_ms(6000); 
		
		if(!Hand("DNSR")) 
		{
	        for(i=0;UART2Buffer[i]!='\0';i++) 
	        {
	           if((UART2Buffer[i]==':'&&UART2Buffer[i+3]=='.')||(UART2Buffer[i]==':'&&UART2Buffer[i+4]=='.'))              
		           ip_star= i;    
	           if(UART2Buffer[i]=='O'&&UART2Buffer[i+1]=='K')
	           {            
		           ip_end= i;
		           break;
	           }
	        }
	        for(j=ip_star+1;j<ip_end-4;j++)
	        {
				Public_ip[k]=UART2Buffer[j];     
				k++;
	        }
	        Public_ip[k]='\0';
		  	LCD_ShowString(0,40,200,16,16,Public_ip);	 	   
		  	if('0' != Public_ip[0])
				break;	//跳出while(1)循环

		Public_ip[k]='"';
		Public_ip[k+1]=',';	
		Public_ip[k+2]='1'; 
		Public_ip[k+3]='2';
		Public_ip[k+4]='3';
		Public_ip[k+5]='4';
		Public_ip[k+6]='5';	//端口号12345							   //ok
		Public_ip[k+7]=',';
		Public_ip[k+8]=',';	//,,
		Public_ip[k+9]='4';
		Public_ip[k+10]='0';
		Public_ip[k+11]='9';
		Public_ip[k+12]='8';//4098(固定格式)
		Public_ip[k+13]='\0';
		for(b=0;b<(ip_end-ip_star-5+13);b++)    //ok	//AT%IPONEN="TCP","182.201.43.17",12345,,4098 (此处的IP地址只是举例)
		{
			ip_cmd[17+b]=Public_ip[b]; //AT指令+公网IP地址					  //ok
		}
	 	LCD_ShowString(0,60,240,16,16,&ip_cmd[17]);	 	   
	 	LCD_ShowString(0,80,240,16,16,ip_cmd);	 	   
		Second_AT_Command_ip(ip_cmd,3000); //给GPRS发送公网IP，与PC进行连接	 //OK
		q = 1; //连接成功标志位
 	} //end of if(Hand("DNSR")) 

}


/*********************************************************************************/




