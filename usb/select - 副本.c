#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>




#define BUFF_LENGTH (1024 * 7)

int send_data(int fd, char* send_buf, int length, int timeout_ms); //发送数据
int sysUsecTime();

int main(int argc, char ** argv)
{

	int usb_fd;  //usb文件描述符
	usb_fd = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd == -1) {
		printf("cant't open usb\n");
		return (-1);
	}
	char SEND_BUF[BUFF_LENGTH] = {0}; //发送缓冲区
	char REC_BUF[BUFF_LENGTH] = {0}; //接收缓冲区


	int time = sysUsecTime();
	int write_num = 0;
	int write_all = 0;
	int i;
	for(i = 0 ; i < 125 * 200; i++) {
		write_num = send_data(usb_fd, SEND_BUF, BUFF_LENGTH, 4000);
		switch(write_num) { //select使用
		case -1:  //select错误，退出程序
			printf("select err\n");
			close(usb_fd);
			return -1;
		case 0:  //超时，退出程序
			printf("time out\n");
			close(usb_fd);
			return 0;
		default:
			write_all = write_all + write_num;
		}// end switch
	}
	printf("write all length = %d\n",write_all);
	printf("speed: %0.2fM/s\n", (float)(write_all / 1024 /1024) / (sysUsecTime() - time));
	close(usb_fd);
	return 0;
}

/*
*author：liyi
*time:20160513
*int send_data(int fd, char* send_buf, int length, int timeout_ms);
*发送数据。
*参数：
*	fd：文件描述符
*	sendbuf：数据缓冲区
*	length：发送长度
*	timeout_ms:超时，单位ms
*返回值：
*	成功：返回发送出去的数据个数
*	失败：
*		-1（select创建失败）
*		0（超时）
*/
int send_data(int fd, char* send_buf, int length, int timeout_ms)//发送数据
{
	int maxfdp;  //可查询最大文件描述符
	fd_set fds;  //select 查询集合
	struct timeval timeout;  //超时结构体
	timeout.tv_sec = timeout_ms / 1000;      //秒
	timeout.tv_usec = timeout_ms % 1000;   //毫秒


	FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
	FD_SET(fd,&fds); //添加描述符
	maxfdp=fd+1;    //描述符最大值加1
	switch(select(maxfdp,NULL,&fds,NULL,&timeout)) { //select使用
	case -1:  //select错误，退出程序
		return -1;
	case 0:  //超时，退出程序
		return 0;
	default:
		if(FD_ISSET(fd,&fds)) { //测试文件是否可写
			return write(fd,send_buf,length);   //返回发送数据个数
		}
	}// end switch
}


int sysUsecTime()
{
	struct timeval    tv;
	struct timezone tz;
	struct tm         *p;
	gettimeofday(&tv, &tz);
	printf("tv_sec:%ld\n",tv.tv_sec);
	return tv.tv_sec;

}