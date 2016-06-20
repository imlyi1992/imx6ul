#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include<sys/mman.h>
#include "common.h"
#include "file.h"


#define SEND_FILE_BUFF_LENGTH (1024 * 7)

/*获取dir路径下所有文件名称，存储到file_name里面
*返回值：文件个数
*file格式：每100个字节存储一个文件名称与文件名长度。
*/
int get_filename(unsigned char *dir, unsigned char *file_name)
{
	DIR *dp;
	struct dirent *dirp;

	//打开指定的目录
	dp = opendir(dir);
	if (dp== NULL) {
		printf("can't open the file\n");
	}

	int file_num = 0;
	//遍历目录
	while ((dirp = readdir(dp)) != NULL) {
		file_name[file_num * 100] = strlen(dirp->d_name);
		memcpy(file_name + file_num * 100 + 1, dirp->d_name, strlen(dirp->d_name));
		file_num++;
	}

	//关闭目录
	closedir(dp);
	return file_num;
}


/*发送已存储好的大文件
*dir：文件绝对路径
*/
int send_picture_file(unsigned char *dir)
{

	int file_fd;  //usb文件描述符

	char SEND_BUF[SEND_FILE_BUFF_LENGTH] = {0}; //发送缓冲区

	int write_num = 0;
	int write_all = 0;
	int file_read_bytes = 0;
	
	printf("filename = %s\n",dir);

	if ((file_fd = open(dir, O_RDONLY)) == -1) {
		fprintf(stderr, "Open %s Error：%s\n", dir, strerror(errno));
		return -1;
	}

	char *ptr = NULL;
	int time_out_times = 0;
	while (file_read_bytes = read(file_fd, SEND_BUF, SEND_FILE_BUFF_LENGTH)) {
		/* 一个致命的错误发生了 */
		if ((file_read_bytes == -1) && (errno != EINTR)) break;
		else if (file_read_bytes > 0) {
			ptr = SEND_BUF;
			while (		write_num = send_data(fd_usb, ptr, file_read_bytes, 1000)) {
				/* 一个致命错误发生了 */
				if ((write_num == -1) && (errno != EINTR))break;
				/* 写完了所有读的字节 */
				else if (write_num == file_read_bytes) {
					write_all = write_all + write_num;
					break;
				}
				/* 只写了一部分,继续写 */
				else if (write_num > 0) {
					printf("只写了一部分,继续写\n");
					ptr += write_num;
					file_read_bytes -= write_num;
					write_all = write_all + write_num;
				} else { //超时
					time_out_times++;
					if(time_out_times >= 4) {
						printf("超时\n");
						break;
					}

				}
			}
			/* 写的时候发生的致命错误 */
			if (write_num == -1)break;
		}
	}

	printf("write all length = %d\n",write_all);

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
