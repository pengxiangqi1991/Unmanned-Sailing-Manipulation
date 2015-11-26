#include "sys.h"
#include "timer.h"
#include "string.h"
#include "usart2.h"
#include "delay.h"
#include "gprs.h"
#include "lcd.h"
#include "led.h"

/***************************״̬����˼���޸�GPRS************************************/
u8 GprsState = 0;
#define WAIT_REG_S 0
#define CGDCONT_S 1
#define ETCPIP_S 2
#define ATE0_S 3
#define DNS_S 4

/***********************************************************************/
/***************************�������PC���������ݵı���************************************/
#define DATALENGTH (2*15) //n������*2
/***********************************************************************/
u8 Public_ip[30]; //��Ź���ip��ַ
u16 q=0,r=0;
u8 ErrorNum;//������������ERROR����
#define ERR_NUM 30
/***********************************��������************************************/
u8 runxun_ser1 = 0,Flag_Buf1_Rec = 0;
u16 Start_weizi,End_weizi;
//u16 DataStartPos;//,DataEndPos;
u16 Second_Int = 0;
/*********************************���ͻ��лس�����******************************/
void Send_LR(void)
{
	Send_Hex(0x0D);	
	Send_Hex(0x0A);	
}
/**********************�жϻ������Ƿ���ָ�����ַ�������***********************/
u8 Hand(u8 *a)
{ 
    if(strstr(UART2Buffer,a)!=FALSE)
	    return 1;
	else
		return 0;
}
/*******************************��λ�ִ�"+32"��λ��*****************************/
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
/*********************************���������**********************************/
void Second_AT_Command(u8 *b,u16 wait_time) //���������  
{
	u8 i;
	u8 *c;
	c = b; //�����ַ�����ַ��c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if(!Hand("OK")) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //���ַ�����ַ��b
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
/*********************************���������***********************************/
void Second_AT_Command_ip(u8 *b,u16 wait_time) //���������  
{
	u8 i;
	u8 *c;
	c = b; //�����ַ�����ַ��c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if(!Hand("CONNECT")) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //���ַ�����ַ��b
			   	for (b; *b!='\0';b++)
			   	{
					Send_Hex(*b);
				}
				Send_LR();//OK//���ͻ��У���ʼִ������
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
/*********************************���������***********************************/
void Second_AT_ERROR(u8 *b,u16 wait_time) //�������    
{
	u8 i;
	u8 *c;
	c = b; //�����ַ�����ַ��c
	CLR_Buf0(); 
    i = 0;
	while(i == 0)                    
	{
		if((!Hand("OK"))&&(!Hand("ERROR"))) 
		{
			if(Timer0_Start == 0)
			{
				b = c; //���ַ�����ַ��b
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
/*******************************�ȴ�ģ��ע��ɹ�����*****************************/
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
/*************************************����ƴ�Ӻ���p1->p2*******************************/
void array_copy(u8 *p1,u8 *p2)
{
  u16 m=0;
  for(m=0;m<strlen(p1);m++) //strlenҪ��⵽'\0'��ֹͣ
  	{
		if(p1[m]=='.')
		{
		   p2[2*m+11]='2'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='E';
		}
		else if(p1[m]==',')
		{
		   p2[2*m+11]='2'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='C';
		}
		else if(p1[m]==':')
		{
		   p2[2*m+11]='3'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='A';
		}
		else if(p1[m]=='A')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='1';
		}
		else if(p1[m]=='B')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='2';
		}
		else if(p1[m]=='C')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='3';
		}
		else if(p1[m]=='D')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='4';
		}
		else if(p1[m]=='-')
		{
		   p2[2*m+11]='2'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='D';
		}
		else if(p1[m]=='E')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='5';
		}
		else if(p1[m]=='F')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='6';
		}
		else if(p1[m]=='G')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='7';
		}
		else if(p1[m]=='H')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='8';
		}
		else if(p1[m]=='I')
		{
		   p2[2*m+11]='4'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='9';
		}
		else if(p1[m]=='!')
		{
		   p2[2*m+11]='2'; //��P1������ת��ΪASC2�벢�����P2��
		   p2[2*m+11+1]='1';
		}
		else
		{
			p2[2*m+11]=((p1[m]>>4)+0x30); //��P1������ת��ΪASC2�벢�����P2��
			p2[2*m+11+1]=((p1[m]&0x0f)+0x30);
		}
	}
  p2[11+2*strlen(p1)]='"';
  p2[11+2*strlen(p1)+1]='\0';
}  	

/*���ӵ���������20130516���*/
void ConnectDNS(u8 *dns) //���ӵ�����
{
  u8 i,j,k=0,b;
	u8 ip_star,ip_end;
	u8 temp_buf[20]={'A','T','%','D','N','S','R','=','"'};	
	u8 ip_cmd[64]={'A','T','%','I','P','O','P','E','N','=','"','T','C','P','"',',','"'}; //����PC��ATָ��



	Second_AT_Command("ATE0",30);/*�ر�ע�⣬ȡ�����ԣ����������ݷ��ػ����룡����*/  //ȡ������			 ok


	for(i=0;i<strlen(dns);i++) //ƴ��������temp_buf������
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
				break;	//����while(1)ѭ��
	 	} //end of if(Hand("DNSR")) 
	}//end of while(1)

	Public_ip[k]='"';
	Public_ip[k+1]=',';	
	Public_ip[k+2]='1'; 
	Public_ip[k+3]='2';
	Public_ip[k+4]='3';
	Public_ip[k+5]='4';
	Public_ip[k+6]='5';	//�˿ں�12345							   //ok
	Public_ip[k+7]=',';
	Public_ip[k+8]=',';	//,,
	Public_ip[k+9]='4';
	Public_ip[k+10]='0';
	Public_ip[k+11]='9';
	Public_ip[k+12]='8';//4098(�̶���ʽ)
	Public_ip[k+13]='\0';
	for(b=0;b<(ip_end-ip_star-5+13);b++)    //ok	//AT%IPONEN="TCP","182.201.43.17",12345,,4098 (�˴���IP��ַֻ�Ǿ���)
	{
		ip_cmd[17+b]=Public_ip[b]; //ATָ��+����IP��ַ					  //ok
	}
 	LCD_ShowString(0,60,240,16,16,&ip_cmd[17]);	 	   
 	LCD_ShowString(0,80,240,16,16,ip_cmd);	 	   
	Second_AT_Command_ip(ip_cmd,3000); //��GPRS���͹���IP����PC��������	 //OK
	q = 1; //���ӳɹ���־λ
}
//����˵�������ж����Զ����ӵķ��ͺ���
//��������������ERROR�����������������IP��ַ
//2013016��������
//����OK���ֺ�Second_AT_Command(u8 *b,u16 wait_time)����һ����������ERROR����ִ�з��������������Ӻ���
void SecondDataOnline(u8 *b,u16 wait_time) //����ӣ����ж����Զ����ӵķ��ͺ���--����ERROR����������IP��ַ
{
	u8 i;
	u8 *c;
	c = b; //�����ַ�����ַ��c
	CLR_Buf0(); 
    i = 0;
	
	while(i == 0)                    
	{
	//USART2_CommandRoute();//ʹ���µķ�ʽ��ѯ�������λ������������
//		if(Hand("IPDATA:"))  //�յ�����
//		{
////			Timer0_Start = 0;
//			//��ȡ���͵�����
//GetCommandFrPC(Find_Str_Pos("IPDATA:"));//�������
////			i = 1;	 //����whileѭ��
//		}

		if(Hand("ERROR: 14")||(Hand("IPCLOSE:"))) //��λ���Ϲرջ᷵��IPCLOSE:���ٴη��ͣ��᷵�ش���"ERROR: 14"
		{
			POINT_COLOR=RED;
		 	LCD_ShowString(120,140,200,16,16,"ReConnect");	 	   
			POINT_COLOR=BLUE;
			ConnectDNS("pengxiangqi1991.eicp.net"); //���ӵ�����
			i = 1;	 //����whileѭ��
			Timer0_Start = 0;
		}
		else if(Hand("OK")) 
		{
		 	LCD_ShowString(120,140,200,16,16,"         ");	 	   
			i = 1;	 //����whileѭ��
			Timer0_Start = 0;
        }
 	    else
		{
				if(Timer0_Start == 0)
				{
					b = c; //���ַ�����ַ��b
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

/*******************************��λ�ִ�"IPDATA:"��λ��*****************************/
u16 Find_Str_Pos(u8 *str)   //���ؽ��յ������ݡ����ڵ�����  ���� %IPDATA:3,"120ABC" ����11
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

///* ����ʮ����������   */
//u8 GetCommandFrPC(u16 data_pos)// 
//{
//	u16 temp_i,temp_j = 0;
//	u8 j = 0;		 //�ر�ע�⣬j���������ֵ����������������ִ��󣡣�������
//	u8 temp_buf[50];//�洢��������ʱ����
//	u8 data_buf[30];//������ʮ���������ݣ�A55A....AA У��ʹ����ڴ����������
//	u8 temp_num1,temp_num2;
///*������UART2Buffer[]����ȡ��data_pos������,�����ʼ�洢��data_buf[]��*/
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
//	for(temp_i=0;temp_i<DATALENGTH;temp_i++)  //���յ������ݺϲ����������ֽںϳ�һ�����֡�
//	{
//		if(temp_buf[temp_i]<':')   //���ݵĸ���λ
//			temp_num1 = temp_buf[temp_i]-'0';
//		else
//			temp_num1 = temp_buf[temp_i]-'A' + 10;
//
//		temp_i++;
//		
//		if(temp_buf[temp_i]<':')	//���ݵĵ���λ
//			temp_num2 = temp_buf[temp_i]-'0';
//		else
//			temp_num2 = temp_buf[temp_i]-'A' + 10;
//
//	data_buf[j++] = 16*temp_num1 + temp_num2; //�ϲ�����
//	}//end of for
//	data_buf[j] = '\0';
//	/*���յ������ݽ���У�鲢��ֵ����Ӧ��ȫ�ֱ�������GoalCompsDri*/
//	if(data_buf[3]==0xa6) // 0xA6����
//	{
//		LCD_ShowString(120,200,200,16,16,"Check ");	 	   
//		if(CommandFrPC_Sum_check(data_buf))	//A55A06A6015F02AA
//		{
//			LCD_ShowString(120,220,200,16,16,"sus");	 	   
//			GetCmd_PC_Compass(data_buf);
//		}
//		return 1; 	//�ɹ���������
//	}
//
//
//	return 0;	 //��������ʧ��
//}
//
/* �����ַ� */
u8 GetCommandFrPC(u16 data_pos)// 
{
	u16 temp_i,temp_j = 0;
	u8 j = 0;		 //�ر�ע�⣬j���������ֵ����������������ִ��󣡣�������
	u8 temp_buf[50];//�洢��������ʱ����
	u8 data_buf[30];//������ʮ���������ݣ�A55A....AA У��ʹ����ڴ����������
	u8 temp_num1,temp_num2;
/*������UART2Buffer[]����ȡ��data_pos������,�����ʼ�洢��data_buf[]��*/
	for(temp_i=data_pos;temp_i<data_pos+DATALENGTH;temp_i++)//;UART2Buffer[temp_i]!='"' 
	{
		temp_buf[temp_j++]=UART2Buffer[temp_i];
	}	
	temp_buf[temp_j] = '\0';


	LCD_ShowString(0,120,200,16,16,temp_buf);	 	   


	for(temp_i=0;temp_i<DATALENGTH;temp_i++)  //���յ������ݺϲ����������ֽںϳ�һ�����֡�
	{
		if(temp_buf[temp_i]<':')   //���ݵĸ���λ
			temp_num1 = temp_buf[temp_i]-'0';
		else
			temp_num1 = temp_buf[temp_i]-'A' + 10;

		temp_i++;
		
		if(temp_buf[temp_i]<':')	//���ݵĵ���λ
			temp_num2 = temp_buf[temp_i]-'0';
		else
			temp_num2 = temp_buf[temp_i]-'A' + 10;

	data_buf[j++] = 16*temp_num1 + temp_num2; //�ϲ�����
	}//end of for
	data_buf[j] = '\0';
	/*���յ������ݽ���У�鲢��ֵ����Ӧ��ȫ�ֱ�������GoalCompsDri*/
	if(data_buf[0]=='A')
	{
		GetCmd_PC_Compass(data_buf);
		return 1;
	}
	return 0;	 //��������ʧ��
}

u16 GoalCompsDri; //A4λ
u16 GoalSpeed; 	  //B4λ
u16 GoalSteer; 	  //C4λ

void GetCmd_PC_Compass(u8 *buf)
{
	GoalCompsDri = 1000*(buf[1]-'0')+100*(buf[2]-'0')+10*(buf[3]-'0')+(buf[4]-'0'); //Ŀ�꺽�򣺷�
	if(buf[5]=='B')
		GoalSpeed = 1000*(buf[6]-'0')+100*(buf[7]-'0')+10*(buf[8]-'0')+(buf[9]-'0'); //�ٶ�
	if(buf[10]=='C')
		GoalSteer = 1000*(buf[11]-'0')+100*(buf[12]-'0')+10*(buf[13]-'0')+(buf[14]-'0'); //���
}

 //--У�鵱ǰ���յ���һ֡�����Ƿ� ��֡У���ֽ�һ��
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






/**************************20130618���GPRS"״̬��"��ʽ������********************************/
//����˵����״̬�ķ�ʽ����GPRSָ�ͨ����ѯGprsState������Ӧ��ָ��
//����������
//20130618��������
//
void GprsCommand(void)
{
	switch (GprsState)
	{
		case WAIT_REG_S:
			Send_ASCII("AT+CREG?");//��ѯGPRS�Ƿ�ע��
			Send_LR();
			break;
		case CGDCONT_S:	////����APN���߽����	  //OK
			Send_ASCII("AT+CGDCONT=1,\"IP\",\"CMNET\"");
			Send_LR();
			break;
		case ETCPIP_S: //����PPP����					 //OK
			Send_ASCII("AT%ETCPIP=\"\",\"\"");
			Send_LR();
			break;
		case ATE0_S:
			Send_ASCII("ATE0");///*�ر�ע�⣬ȡ�����ԣ����������ݷ��ػ����룡����*/  //ȡ������
			Send_LR();
			break;
//		case ATE0_S:
//			break;
		default:
			break;
	}

}
//����˵�������GPRSӦ��״̬
//����������
//#define CGDCONT 1
//#define ETCPIP 2
//
//20130618��������
void CheckGprsAck(void)
{
	switch (GprsState)
	{
		case WAIT_REG_S:
			CheckRegS(); //����Ƿ��յ�ע���Ӧ��
			break;
		case CGDCONT_S:
			CheckOk(); //
			break;
		case ETCPIP_S:
			CheckOk();
			break;
		case ATE0_S:  //ȡ�����Լ��
			CheckOk();
			break;
		default:
			break;
	}
	
}




//����˵����CheckRegS(); //����Ƿ��յ�ע���Ӧ��
//����������
//20130618��������
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
				GprsState++;//GPRSע��ɹ�,���л�����һ״̬
				CLR_Buf0();
			   	break;
			}
		}
	}
}
//����˵����CheckOk(); //����Ƿ��յ�OK
//����������
//20130618��������
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
				GprsState++;//GPRSע��ɹ�,���л�����һ״̬
				CLR_Buf0();
			   	break;
			}
		}
	}
	
}
//����˵����/*���ӵ���������20130516���*/
//����������
//20130618��������
//
void ConnectDNS_S(u8 *dns) //���ӵ�����
{
	u8 i,j,k=0,b;
	u8 ip_star,ip_end;
	u8 temp_buf[20]={'A','T','%','D','N','S','R','=','"'};	
	u8 ip_cmd[64]={'A','T','%','I','P','O','P','E','N','=','"','T','C','P','"',',','"'}; //����PC��ATָ��
	for(i=0;i<strlen(dns);i++) //ƴ��������temp_buf������
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
				break;	//����while(1)ѭ��

		Public_ip[k]='"';
		Public_ip[k+1]=',';	
		Public_ip[k+2]='1'; 
		Public_ip[k+3]='2';
		Public_ip[k+4]='3';
		Public_ip[k+5]='4';
		Public_ip[k+6]='5';	//�˿ں�12345							   //ok
		Public_ip[k+7]=',';
		Public_ip[k+8]=',';	//,,
		Public_ip[k+9]='4';
		Public_ip[k+10]='0';
		Public_ip[k+11]='9';
		Public_ip[k+12]='8';//4098(�̶���ʽ)
		Public_ip[k+13]='\0';
		for(b=0;b<(ip_end-ip_star-5+13);b++)    //ok	//AT%IPONEN="TCP","182.201.43.17",12345,,4098 (�˴���IP��ַֻ�Ǿ���)
		{
			ip_cmd[17+b]=Public_ip[b]; //ATָ��+����IP��ַ					  //ok
		}
	 	LCD_ShowString(0,60,240,16,16,&ip_cmd[17]);	 	   
	 	LCD_ShowString(0,80,240,16,16,ip_cmd);	 	   
		Second_AT_Command_ip(ip_cmd,3000); //��GPRS���͹���IP����PC��������	 //OK
		q = 1; //���ӳɹ���־λ
 	} //end of if(Hand("DNSR")) 

}


/*********************************************************************************/




