#include<stdio.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



#define BUF_LENGTH 1024 * 100
int ep0_fd;
unsigned char rec_buf[BUF_LENGTH] = {0};
void signal_fun(int signum)  //底层读取到一张图片
{
	int read_num = 0;
	read_num = read(ep0_fd, rec_buf, BUF_LENGTH);
	if(read_num == 0)//数据发送
		{
			printf("数据发送\n");
			rec_buf[0] = 11;
			rec_buf[1024 * 10 - 1] = 12;
			read_num = write(ep0_fd, rec_buf, 1024 * 10);

		}
	else
		{
			printf("read_num = %d  \n rec_buf[last] = %d\n",read_num, rec_buf[read_num - 1]);
		}
}


int main()
{

	int Oflags;
	signal(SIGIO, signal_fun);
	ep0_fd = open("/dev/tty_ep0", O_RDWR|O_SYNC);
	if (ep0_fd < 0)
		{
			printf("can't open /dev/weim_gpio\n");
			return -1;
		}

	fcntl(ep0_fd, F_SETOWN, getpid());
	Oflags = fcntl(ep0_fd, F_GETFL);
	fcntl(ep0_fd, F_SETFL, Oflags | FASYNC);
	while(1)
		{
			sleep(2000);
		}

	return (1);
}
