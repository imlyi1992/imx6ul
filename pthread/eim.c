#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "common.h"
#include "eim.h"
#include "file.h"
#include "usb.h"


#define FILE_NAME   "/home/root/rec_data.dat"

char send_buf[map_base_length] = {0};
char rec_buf[map_base_length] = {0};
char rec_buf_round[map_base_length] = {0};
int picture_data = 0;
int data_tmp = 0;

void eim_signal_fun(int signum)  //底层读取到一张图片
{
	int read_num = 0;
	read_num = read(fd_eim, rec_buf, map_base_length);
	save_data(rec_buf, read_num); //存储图片数据
	send_data(fd_usb, rec_buf, read_num, 10); //实时传输数据，超时10ms。
	
	/*picture_data = picture_data + read_num;
	if( data_tmp <= (picture_data / (1024 * 100)) )
	{
		printf("数据接收%0.3fK\n",(float)picture_data / 1024);
		data_tmp++;
	}*/
}




int eim_main()
{

	int Oflags;
	signal(SIGIO , eim_signal_fun);
	
	
	fd_mem = open("/dev/mem", O_RDWR);
	if (fd_mem < 0) {
		printf("can't open fd_mem!\n");
	}

	
	fd_eim = open("/dev/weim_gpio", O_RDWR);
	if (fd_eim < 0) {
		printf("can't open fd_eim!\n");
	}
	
	
	map_base = mmap(NULL, map_base_length, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, phy_addr);

	
	if (map_base == 0) {
		printf("NULL pointer!\n");
	} else {
		printf("Successfull!\n");
	}
	
	fcntl(fd_eim, F_SETOWN, getpid());
	Oflags = fcntl(fd_eim, F_GETFL);
	fcntl(fd_eim, F_SETFL, Oflags | FASYNC);
	
	
	unsigned char tmp[10] = {0};
	memcpy(map_base, tmp, 8)  ;//首次随便发几个东西使后面正常指令跳过初始启动时间。
	
	/*
	unsigned char	val = 0x25;
	eim_write_data(0x47, 1, &val) ; //写数据源
*/
	while(1){
		
		
		sleep(1000);
	}
	

/*
	close(fd_eim);


	munmap(map_base, map_base_length);
*/
	return 1;
}

int save_data(char* buf, int buf_length) //存储图片数据
{
	FILE* file_fd;
	file_fd = fopen(FILE_NAME,"ab+");
	if(file_fd == NULL) {
		printf("errno\n");
	} else {
	//	printf("File Open successed!\n");
	}
    
	fwrite(buf, buf_length , 1, file_fd);
  //  printf("save data = %d\n",buf_length);
	fclose(file_fd);
}
/*
void test_3(void) //实际设备读取测试
{
	unsigned char val = 0;

	eim_read_data(0x47, 1, &val)  ;//读数据
//	printf("read 0x47 = %x\n",val);


	val = 0x25;
	eim_write_data(0x47, 1, &val) ; //写数据源
//	printf("write 0x47 = %x\n",val);


	eim_read_data(0x47, 1, &val)  ;//读数据
//	printf("read 0x47 = %x\n",val);

}

*/

int eim_read_data(unsigned long addr, int length, unsigned  char* buf)  //读数据
{
	unsigned  char RW = 1;
	unsigned  char val =  *buf;
	unsigned  int    cmd = (val << 8) | RW;
	unsigned long ret = ioctl(fd_eim, cmd, addr);
	*buf = (unsigned  char)(ret & 0xff);
//	printf("read = %x\n", *buf );
	return 0;
}
int eim_write_data(unsigned long addr, int length, unsigned char* buf)  //写数据源
{
//	printf("eim_write_data:\n");
	
	unsigned  char RW = 0;
	unsigned  int val =  (unsigned int)(*buf) ;
	unsigned  int    cmd = (val << 8) | RW;
//	printf("val = %d, addr = %d\n",val, addr);
    unsigned long ret = ioctl(fd_eim, cmd, addr);
	return 0;
}

