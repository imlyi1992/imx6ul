#ifndef __common_lyi 
#define __common_lyi
#include <semaphore.h>



/*模拟通用变量表的结构体的*/
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



/*设备节点文件描述符*/
int fd_i2c, fd_usb, fd_eim, fd_mem;   







void print_test(void);  //测试函数，打印一个数据

int sysUsecTime(void);//获取时间，总秒数




#endif // 宏结束行