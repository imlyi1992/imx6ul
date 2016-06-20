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
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>



#include "usb.h"
#include "common.h"
#include "file.h"
#include "eim.h"


#define FILE_DIR "/home/root/"    //病例文件路径
#define PICTURE_DIR "/home/root/"    //图片文件路径
#define FILE_NAME_LENGTH (100 * 20 + 1)
#define BUF_LENGTH 1024 * 20


pthread_t ntid_picture, ntid_real_data;

unsigned char  FILE_NAME[FILE_NAME_LENGTH] = {0};
unsigned char USB_EP0_REC_BUF[BUF_LENGTH] = {0};   //ep0接收缓冲区





/*ep0回调函数,异步通知,表示ep0有指令过来*/
void signal_fun(int signum)
{
	int read_num = 0;
	read_num = read(fd_ep0, USB_EP0_REC_BUF, BUF_LENGTH);
//	printf("ep0 data coming, length = %d\n", read_num);
	if(0 != read_num) { //数据接收

		check_cmd(USB_EP0_REC_BUF);//测试ep0指令

		/*
				int i = 0;
				for(i = 0; i < read_num; i++) {
					if(USB_EP0_REC_BUF[i] != i % 4) {
						printf("data err\n");
						break;
					}
				}
				printf("data right, num = %d\n",read_num);

				for(i = 0; i < read_num; i++) {
					USB_EP0_REC_BUF[i] = i % 5;
				}
				read_num = write(fd_ep0, USB_EP0_REC_BUF, read_num);
				printf("write num = %d\n",read_num);
		*/
	}
}


/*usb处理主函数*/
int usb_main(int fd_usb)
{

	int Oflags;
	signal(SIGUSR1, signal_fun);
	fd_ep0 = open("/dev/tty_ep0", O_RDWR|O_SYNC);
	if (fd_ep0 < 0) {
		printf("can't open /dev/fd_ep0\n");
		return -1;
	}
	fcntl(fd_ep0, F_SETSIG, SIGUSR1);
	fcntl(fd_ep0, F_SETOWN, getpid());
	Oflags = fcntl(fd_ep0, F_GETFL);
	fcntl(fd_ep0, F_SETFL, Oflags | FASYNC);

	while(1) {
		sleep(2000);
	}
	return 0;
}



int check_cmd(unsigned char *cmd)
{

	switch (cmd[0]) {
	case CMD_INI:   //配置相关的指令
		cmd_ini(cmd);
		break;
	case CMD_FILE:  // 病例相关的指令
		cmd_file(cmd);
		break;
	case CMD_PICTURE:    //大数据图片相关的指令
		cmd_picture(cmd);
		break;
	case CMD_REAL_DATA:    //大数据图片相关的指令
		cmd_real_data(cmd);
		break;
	default:
		printf("check_cmd :udefine the cmd\n");
		break;
	}

}

/*读写配置函数*/
int cmd_ini(unsigned char *cmd)
{
//	printf("cmd_ini\n");
	switch (cmd[1]) {
	case SET_INI:   //配置相关的指令
		cmd_set_ini(cmd);
		break;
	case GET_INI:  // 病例相关的指令
		cmd_get_ini(cmd);
		break;
	default:
		printf("cmd_ini :udefine the cmd\n");
		break;
	}

	return 0;
}


/*配置信息读写*/
int cmd_set_ini(unsigned char *cmd)
{
//	printf("cmd_set_ini\n");
//	printf("cmd2 = %x , cmd3 = %x\n",cmd[2],*(cmd + 3));
	unsigned char val = cmd[3];
	eim_write_data(cmd[2], 1, cmd + 3) ; //写数据
	return 0;
}
int cmd_get_ini(unsigned char *cmd)
{	
//	printf("cmd_get_ini\n");
	eim_read_data(cmd[2], 1, cmd + 3)  ;//读数据
//	printf("cmd3 = %x \n",cmd[3]);
	write(fd_ep0,  cmd + 3, 1);  //返回数据
	return 0;
}



/*读写病例函数*/
int cmd_file(unsigned char *cmd)
{
	switch (cmd[1]) {
	case GET_FILE_ALL_NAME:   //配置相关的指令
		cmd_file_get_all_name();  //获取所有病例信息并发送回pc
		break;
	default:
		printf("cmd_file :udefine the cmd\n");
		break;
	}
	return 0;
}

/*
*利用线程来解决大文件传输
*/
void * thr_cmd_picture(void *arg)
{

	unsigned char* cmd = arg;
	unsigned char full_dir[1024] = {0};
	memcpy(full_dir, PICTURE_DIR, sizeof(PICTURE_DIR) - 1);
	memcpy(full_dir + sizeof(PICTURE_DIR) - 1, cmd + 3, cmd[2]);
	send_picture_file(full_dir);

	return ((void *)0);
}



/*
*利用线程来解决大文件传输
*/
void * thr_cmd_real_data(void *arg)
{


		unsigned char	val = 0x25;
	eim_write_data(0x47, 1, &val) ; //写数据源
	
	return ((void *)0);
}

/*读图片函数,大数据*/
/*发送文件，cmd格式:
*cmd[2]:文件长度
*cmd[3+]：文件名称
*/
int cmd_picture(unsigned char *cmd)
{


	int err;
	err = pthread_create(&ntid_picture, NULL, thr_cmd_picture, cmd);
	if(err != 0)
		printf("cant't creat thread\n");


	return 0;
}


/*获取所有病例,并发回到pc*/
int cmd_file_get_all_name(void)
{
	/*文件格式：
	*0位为文件个数
	*后面每100位第一个为名称长度
	*/
	int file_num = get_filename(FILE_DIR, FILE_NAME + 1);
	FILE_NAME[0] = file_num;

	write(fd_ep0, FILE_NAME, file_num * 100 + 1);

	return 0;
}



/*实时采集图片信息*/
int cmd_real_data(unsigned char *cmd)
{

	int err;
	err = pthread_create(&ntid_real_data, NULL, thr_cmd_real_data, cmd);
	if(err != 0)
		printf("cant't creat thread\n");

	return 0;
}















