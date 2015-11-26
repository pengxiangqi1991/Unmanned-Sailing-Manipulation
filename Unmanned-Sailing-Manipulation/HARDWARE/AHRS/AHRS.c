
/* 
��д�ߣ�lisn3188
��ַ��www.chiplab7.com
����E-mail��lisn3188@163.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2012-05-25
���ԣ� ���������ڵ���ʵ���ҵ�mini IMU����ɲ���
���ܣ�
ʹ�ô����жϳ������mini IMU �����ݡ�
��ֲ�������Լ���д��Ӧ�Ĵ��ڳ�ʼ�����򣬲����������жϡ�
���ж��ӳ����� ��Ҫ����Ӧ�ĸĶ�

Ϊ��ʹ�������������Ҫ��
1.��ʼ����Ĵ��ڣ�����������ģ��Ĳ��������ó�һ�µ�
2.�������ڵĽ����жϣ����ο����³����д�ж��ӳ���
  �ο� void USART2_IRQHandler(void) 
3.���������ѭ�����У���ʱ�����ӳ���
  void UART1_CommandRoute(void)
  �Լ���Ƿ��յ��µ�����֡
  ���е���̬���ݺ�ADCֵ������ UART1_CommandRoute
  �н��и��¡�
4.ʹ�� ����ֵ���Զ����£�

float 	yaw,  //ƫ����
		pitch,//����
		roll, //��ת
		alt,  //�߶�
		tempr,//�¶�
		press;//��ѹ
int16_t ax, ay, az;//���ٶȼ�
int16_t gx, gy, gz;//������
int16_t hx, hy, hz;//������
------------------------------------
*/
 
#include "ahrs.h"
#include "led.h"
//#include "usart2.h"		



//uart reicer flag
#define b_uart_head  0x80  //�յ�A5 ͷ ��־λ
#define b_rx_over    0x40  //�յ�������֡��־
// USART Receiver buffer
#define RX_BUFFER_SIZE 100 //���ջ������ֽ���

unsigned char rx_buffer[RX_BUFFER_SIZE]; //�������ݻ�����
volatile unsigned char rx_wr_index; //����дָ��
volatile unsigned char RC_Flag;  //����״̬��־�ֽ�


float 	yaw,  //ƫ����
		pitch,//����
		roll, //��ת
		alt,  //�߶�
		tempr,//�¶�
		press;//��ѹ
//�ڽ�����һ֡IMU��̬����󣬵�������ӳ�����ȡ����̬����
void UART1_Get_IMU(void)
{
	int16_t temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	yaw=(float)temp / 10.0f; //ƫ����
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	pitch=(float)temp / 10.0f;//����
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	roll=(float)temp / 10.0f;//��ת

	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	alt=(float)temp / 10.0f;//�߶�
	
	temp = 0;
	temp = rx_buffer[10];
	temp <<= 8;
	temp |= rx_buffer[11];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	tempr=(float)temp / 10.0f;//�¶�
	
	temp = 0;
	temp = rx_buffer[12];
	temp <<= 8;
	temp |= rx_buffer[13];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	press=(float)temp * 10.0f;//��ѹ

}

//ADCֵ
int16_t ax, ay, az;//���ٶȼ�
int16_t gx, gy, gz;//������
int16_t hx, hy, hz;//������
//�ڽ���һ֡ReportMotion ���������ӳ�����ȡ��ADC����
void UART1_Get_Motion(void)
{
	int16_t temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ax=temp;//���ٶȼ� X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ay=temp;//���ٶȼ� Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	az=temp;//���ٶȼ� Z���ADCֵ
	
	temp = 0;
	temp = rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gx=temp;//������ X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[10];
	temp <<= 8;
	temp |= rx_buffer[11];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gy=temp;//������ Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[12];
	temp <<= 8;
	temp |= rx_buffer[13];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gz=temp;//������ Z���ADCֵ
	
	temp = 0;
	temp = rx_buffer[14];
	temp <<= 8;
	temp |= rx_buffer[15];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hx=temp;//������ X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[16];
	temp <<= 8;
	temp |= rx_buffer[17];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hy=temp;//������ Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[18];
	temp <<= 8;
	temp |= rx_buffer[19];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hz=temp;//������ Z���ADCֵ
}

 //�ڽ�����һ֡IMU��̬����󣬵�������ӳ�����ȡ��/*GPS*/����
float longitude,latitude; //���ܵľ�γ�ȣ���λ���ȣ�
void UART1_Get_GPS(void)
{
	u32 temp;
	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	temp <<= 8;
	temp |= rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	longitude=(float)temp / 100000.0f; //longitude����
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	temp <<= 8;
	temp |= rx_buffer[8];
	temp <<= 8;
	temp |= rx_buffer[9];
	latitude=(float)temp / 100000.0f; //latitudeγ��
	
}
	u8 svnum;					//�ɼ�������
	float speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
void UART1_Get_ALL(void)
{
//void UART1_Get_IMU(void)
	int16_t temp;
	u32 temp_gps;
	u16 temp_uint;	
	temp = 0;
	temp = rx_buffer[2];
	temp <<= 8;
	temp |= rx_buffer[3];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	yaw=(float)temp / 10.0f; //ƫ����
	
	temp = 0;
	temp = rx_buffer[4];
	temp <<= 8;
	temp |= rx_buffer[5];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	pitch=(float)temp / 10.0f;//����
	
	temp = 0;
	temp = rx_buffer[6];
	temp <<= 8;
	temp |= rx_buffer[7];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	roll=(float)temp / 10.0f;//��ת

//	temp = 0;
//	temp = rx_buffer[8];
//	temp <<= 8;
//	temp |= rx_buffer[9];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	alt=(float)temp / 10.0f;//�߶�
//	
//	temp = 0;
//	temp = rx_buffer[10];
//	temp <<= 8;
//	temp |= rx_buffer[11];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	tempr=(float)temp / 10.0f;//�¶�
//	
//	temp = 0;
//	temp = rx_buffer[12];
//	temp <<= 8;
//	temp |= rx_buffer[13];
//	if(temp&0x8000){
//	temp = 0-(temp&0x7fff);
//	}else temp = (temp&0x7fff);
//	press=(float)temp * 10.0f;//��ѹ

////�ڽ���һ֡ReportMotion ���������ӳ�����ȡ��ADC����
//void UART1_Get_Motion(void)
/*IMUpersec  ռ�������ֽ�14  15��û�ý���ʹ�ã��Ժ����ӣ������Ǹ������ʣ�*/	
	temp = 0;
	temp = rx_buffer[16];
	temp <<= 8;
	temp |= rx_buffer[17];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ax=temp;//���ٶȼ� X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[18];
	temp <<= 8;
	temp |= rx_buffer[19];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	ay=temp;//���ٶȼ� Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[20];
	temp <<= 8;
	temp |= rx_buffer[21];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	az=temp;//���ٶȼ� Z���ADCֵ
	
	temp = 0;
	temp = rx_buffer[22];
	temp <<= 8;
	temp |= rx_buffer[23];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gx=temp;//������ X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[24];
	temp <<= 8;
	temp |= rx_buffer[25];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gy=temp;//������ Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[26];
	temp <<= 8;
	temp |= rx_buffer[27];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	gz=temp;//������ Z���ADCֵ
	
	temp = 0;
	temp = rx_buffer[28];
	temp <<= 8;
	temp |= rx_buffer[29];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hx=temp;//������ X���ADCֵ
	
	temp = 0;
	temp = rx_buffer[30];
	temp <<= 8;
	temp |= rx_buffer[31];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hy=temp;//������ Y���ADCֵ
	
	temp = 0;
	temp = rx_buffer[32];
	temp <<= 8;
	temp |= rx_buffer[33];
	if(temp&0x8000){
	temp = 0-(temp&0x7fff);
	}else temp = (temp&0x7fff);
	hz=temp;//������ Z���ADCֵ
// //�ڽ�����һ֡IMU��̬����󣬵�������ӳ�����ȡ��/*GPS*/����
//void UART1_Get_GPS(void)
	
	temp_gps = 0;
	temp_gps = rx_buffer[34];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[35];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[36];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[37];
	longitude=(float)temp_gps / 100000.0f; //longitude����
	
	temp_gps = 0;
	temp_gps = rx_buffer[38];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[39];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[40];
	temp_gps <<= 8;
	temp_gps |= rx_buffer[41];
	latitude=(float)temp_gps / 100000.0f; //latitudeγ��


//	u8 svnum;					//�ɼ�������
//	u16 speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 
	temp_uint = 0;
	temp_uint = rx_buffer[42];
	svnum = temp_uint;					//�ɼ�������

	temp_uint = 0;
	temp_uint = rx_buffer[43];
	temp_uint <<= 8;
	temp_uint |= rx_buffer[44];
	speed = temp_uint/1000.0;					//��������,�Ŵ���1000��,ʵ�ʳ���1000.��λ:0.001����/Сʱ

}


 //--У�鵱ǰ���յ���һ֡�����Ƿ� ��֡У���ֽ�һ��
unsigned char Sum_check(void)
{ 
  unsigned char i;
  u32 checksum=0; 
  for(i=0;i<rx_buffer[0]-2;i++)
   checksum+=rx_buffer[i];
  if((checksum%256)==rx_buffer[rx_buffer[0]-2])
   return(0x01); //Checksum successful
  else
   return(0x00); //Checksum error
}



//--����ӳ�����Ҫ���������� ��ʱ����,�Լ�� �����Ƿ������һ֡����
void UART1_CommandRoute(void)
{
	if(RC_Flag&b_rx_over)
	{  //�Ѿ�������һ֡?
		RC_Flag&=~b_rx_over; //���־��
		if(Sum_check())
		{ 
//		u2_printf(rx_buffer);
			//У��ͨ��
//			if(rx_buffer[1]==0xA1)
//			{ //UART1_ReportIMU ������
//				UART1_Get_IMU();	//ȡ����
//			}
//			if(rx_buffer[1]==0xA2)
//			{ //UART1_ReportMotion ������
//				UART1_Get_Motion();	 //ȡ����
//			} 
//			if(rx_buffer[1]==0xA4) //REGPS  ����GPS������
//			{ 
//				UART1_Get_GPS();	 //ȡGPS����
//			} 
			if(rx_buffer[1]==0xA3) //REALL  ����ALL������
			{ 
				UART1_Get_ALL();	 //ȡALL����
			} 
		
		}//У���Ƿ�ͨ��?
	}
}


