#ifndef __common_lyi 
#define __common_lyi
#include <semaphore.h>



/*ģ��ͨ�ñ�����Ľṹ���*/
typedef struct Pass_Info
{
	sem_t	s_thread;
	sem_t	s_main;
	int			logo;
	unsigned char data1;
	unsigned char data2;
	unsigned char data3;
	unsigned char data4;
	unsigned char data5;
} PassInfo;



/*�豸�ڵ��ļ�������*/
int fd_i2c, fd_usb, fd_eim, fd_mem;   







void print_test(void);  //���Ժ�������ӡһ������

int sysUsecTime(void);//��ȡʱ�䣬������




#endif // �������