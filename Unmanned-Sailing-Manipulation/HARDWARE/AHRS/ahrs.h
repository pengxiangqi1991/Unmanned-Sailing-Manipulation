#ifndef __AHRS_H
#define __AHRS_H	 
#include "sys.h"  

//uart reicer flag
#define b_uart_head  0x80  //�յ�A5 ͷ ��־λ
#define b_rx_over    0x40  //�յ�������֡��־
// USART Receiver buffer
#define RX_BUFFER_SIZE 100 //���ջ������ֽ���

typedef signed short int int16_t;  //�ڴ��ļ��м���Ķ���  20130414

extern unsigned char rx_buffer[RX_BUFFER_SIZE]; //�������ݻ�����
extern volatile unsigned char rx_wr_index; //����дָ��
extern volatile unsigned char RC_Flag;  //����״̬��־�ֽ�

//�ڽ�����һ֡IMU��̬����󣬵�������ӳ�����ȡ����̬����
extern void UART1_Get_IMU(void);

//--����ӳ�����Ҫ���������� ��ʱ����,�Լ�� �����Ƿ������һ֡����
void UART1_CommandRoute(void);
extern float 	yaw,  //ƫ����
		pitch,//����
		roll, //��ת
		alt,  //�߶�
		tempr,//�¶�
		press;//��ѹ
extern int16_t ax, ay, az;//���ٶȼ�
extern int16_t gx, gy, gz;//������
extern int16_t hx, hy, hz;//������

extern float longitude,latitude;
extern u8 svnum;					//�ɼ�������
extern float speed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	 










#endif









