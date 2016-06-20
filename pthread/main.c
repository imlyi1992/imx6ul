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
#include<sys/mman.h>

#include "main.h"
#include "common.h"
#include "usb.h"
#include "file.h"
#include "eim.h"






PassInfo* P_info = NULL;
pthread_t ntid_usb, ntid_eim;

sem_t	semtest;

int main(int argc, char **argv)
{

	/*初始化设备描述符和一些变量*/
	init_fd_data();
	
	/*线程创建*/
	int err;
	err = pthread_create(&ntid_usb, NULL, thr_fn_usb, P_info);
	if(err != 0)
		printf("cant't creat thread\n");

	err = pthread_create(&ntid_eim, NULL, thr_fn_eim, P_info);
	if(err != 0)
		printf("cant't creat thread\n");

	/*主要逻辑*/
	main_thread();

	return 0;
}

int  init_fd_data(void)//初始化设备描述符和一些变量
{

	/*初始化设备描述符*/

	fd_usb = open("/dev/ttyGS0", O_RDWR);
	if (fd_usb < 0) {
		printf("can't open fd_usb!\n");
	}


	fd_i2c = open("/dev/i2c-0", O_RDWR);
	if (fd_i2c < 0) {
		printf("can't open fd_i2c!\n");
	}

	

	/*初始化变量*/
	P_info = (PassInfo* )malloc (sizeof (PassInfo));
	if (P_info == NULL) {
		printf ("Failed to malloc P_info\n");
		return -1;
	}

	sem_init (&P_info->s_thread, 0, 1);
	sem_init (&P_info->s_main, 0, 0);
	
	sem_init (&semtest, 0, 1);

	return 0;
}

#define dir_path  "/home/root"
int main_thread()
{
	
	/*
	unsigned char file_name[100* 10] = {0};
	int file_num = get_filename(dir_path, file_name);
	int i = 0;
	for(i = 0; i < file_num; i++)
	{
		unsigned char file_name_tmp[100] = {0};
		
		memcpy(file_name_tmp, file_name + i * 100 + 1, (file_name + i * 100)[0]);
		

		printf("filename=   %s",file_name_tmp);
			
		printf("\n");
		
	}
	printf("file_num = %d\n",file_num);
	*/
	while(1) {
		sleep(100);
	}
}



void * thr_fn_usb(void *arg)
{
	printf("thr_fn_usb\n");
//	sem_test();
	PassInfo* P_info = arg;
	while(1) {
		/*usb处理主函数*/
		usb_main(fd_usb);
	}
	return ((void *)0);
}



void * thr_fn_eim(void *arg)
{
	printf("thr_fn_eim\n");
//	sem_test();
	PassInfo* P_info = arg;
	while(1) {
	eim_main();//配置eim回调函数什么的
	}
	return ((void *)0);
}




void sem_test()//信号量原子操作测试
{
	
	sem_wait (&semtest);  // 等待线程写操作结束
	printf("sem_test\n");
	sleep(4);
	printf("sem_test end\n");
    sem_post (&semtest);   //主线程读操作结束,写线程可以开始操作了
	
}
