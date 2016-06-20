#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>
#define BUFFER_SIZE (1024)


int send_fd, usb_fd;
int bytes_read, bytes_write;
char buffer[BUFFER_SIZE];
char buffer_1[BUFFER_SIZE];
char *read_buf;
volatile int read_over = 0;
int sysUsecTime();

/*线程相关*/
typedef struct Pass_Info {
	sem_t s_send;
	sem_t s_read;
} PassInfo;
PassInfo* P_info = NULL;
pthread_t ntid_file_read;
pthread_t ntid_file_send;

int main_thread();   //literal meaning
void * file_read(void *arg);   //new thread
void * file_send(void *arg);   //new thread

volatile int exit_info = 0;


int main(int argc, char **argv)
{

	usb_fd = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd == -1) {
		printf("can't open usb\n");
		return (-1);
	}


	int time  = 0;

	P_info = (PassInfo* )malloc (sizeof (PassInfo));
	if (P_info == NULL) {
		printf ("Failed to malloc P_info\n");
		return -1;
	}
	int err;
	read_over = 0;
	sem_init(&P_info->s_send, 0, 1);
	sem_init(&P_info->s_read, 0, 0);

	err = pthread_create(&ntid_file_send, NULL, file_send, P_info);
	if(err != 0)
		printf("cant't creat thread2\n");

	err = pthread_create(&ntid_file_read, NULL, file_read, P_info);
	if(err != 0)
		printf("cant't creat thread\n");


	main_thread();   //主要处理函数，包括串口处理与数据判断
}

int main_thread()
{
	printf("main thread run\n");
	while(1) {

		if(1 == exit_info)
			break;
	}
	printf("main thread out\n");
	return 0;
}


void * file_send(void *arg)
{
   int i = 0;
   for(i = 0; i < 1024 * 100; i++){
	   write(usb_fd,buffer,BUFFER_SIZE);
   }
	printf("closedown\n");
   exit_info = 1;
	return ((void *)0);
}


void * file_read(void *arg)
{
	sleep(10);
	close(usb_fd);
	printf("close the usb\n");
	return ((void *)0);
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
