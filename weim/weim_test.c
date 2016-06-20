#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>





int sysUsecTime();


//#define debug_out 123
#define map_base_length 0x1000000
#define phy_addr 0x50100000

#define FILE_NAME  "/home/root/rec_data.dat"

int read_data(unsigned long addr, int length, char* buf);  //读数据
int write_data(unsigned long addr, int length, char* buf);  //写数据
int check_data(char* buf);  //检测数据
int printf_data(char* buf);  //打印数据
int save_data(char* buf, int buf_length); //存储图片数据
void test_1(void); //测试读写数据是否正确。
void test_2(void); //测试读取数据速率
void test_3(void); //实际设备读取测试


unsigned char * map_base;
char send_buf[map_base_length] = {0};
char rec_buf[map_base_length] = {0};
char rec_buf_round[map_base_length] = {0};
int weim_fd;  //底层驱动文件描述符
int picture_data = 0;
int data_tmp = 0;

void signal_fun(int signum)  //底层读取到一张图片
{
	int read_num = 0;
	read_num = read(weim_fd, rec_buf, map_base_length);
	save_data(rec_buf, read_num); //存储图片数据
	picture_data = picture_data + read_num;
	if( data_tmp <= (picture_data / (1024 * 100)) )
	{
		printf("数据接收%0.3fK\n",(float)picture_data / 1024);
		data_tmp++;
	}
	
}

int main()
{

	int n, fd;

	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		return (-1);
	}


	int Oflags;
	signal(SIGIO, signal_fun);
	weim_fd = open("/dev/weim_gpio", O_RDWR);
	if (weim_fd < 0) {
		printf("can't open /dev/weim_gpio\n");
		return -1;
	}

	fcntl(weim_fd, F_SETOWN, getpid());
	Oflags = fcntl(weim_fd, F_GETFL);
	fcntl(weim_fd, F_SETFL, Oflags | FASYNC);




	map_base = mmap(NULL, map_base_length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr);

	if (map_base == 0) {
		printf("NULL pointer!\n");
	} else {
		printf("Successfull!\n");
	}

	memcpy(map_base, send_buf, 8)  ;//首次随便发几个东西使后面正常指令跳过初始启动时间。

	//test_1();//测试读写数据是否正确。
	//test_2();  //测试读取数据速率
	test_3(); //实际设备读取测试

	while(1)
	{
		sleep(10);
	}
	
	close(weim_fd);
	close(fd);

	munmap(map_base, map_base_length);

	return (1);
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

void test_3(void) //实际设备读取测试
{
	char val = 0;

	read_data(0x47, 1, &val)  ;//读数据
	printf("读取0x47地址：%x\n",val);


	val = 0x25;
	write_data(0x47, 1, &val) ; //写数据源
	printf("0x47地址写入值：%x\n",val);


	read_data(0x47, 1, &val)  ;//读数据
	printf("再次读取0x47地址：%x\n",val);


}

void test_2(void)
{

	int i = 0;
	char* rec_buf_tmp = NULL;
	rec_buf_tmp = rec_buf;

	for(i = 0; i < 6400; i++) {
		read_data(i, 32, rec_buf_tmp)  ;//读数据
		rec_buf_tmp = rec_buf_tmp + 32;
	}
	memcpy(rec_buf_round, rec_buf, 32 * 6400)  ;//write addr

}

void test_1(void)
{

	int i = 0;

	for(i = 0; i < 0x8FFF; i++) {
		write_data(i, 32, send_buf) ; //写数据源

		int j = 0;
		for(j = 0; j < 5; j++);  //读写间隔

		read_data(i, 32, rec_buf)  ;//读数据
		if(0 == check_data(rec_buf)) { //数据读取失败
			printf("第%d次读取错误\n",i + 1);
			printf_data(rec_buf) ;
			break;
		}
		memset(rec_buf, 0, map_base_length); //清空接收函数

	}
	printf("测试成功次数：%04x\n",i );


}


int printf_data(char* buf)  //打印数据
{

	int i = 0;
	for(i = 0; i < 32; i++) {
		printf("%02x ",buf[i]);
		if(0 == (i + 1) % 8)
			printf("\n");
	}

	printf("\n");
	return 0;
}
int check_data(char* buf)  //检测数据
{
	int i = 0;
	for(i = 0; i < 32; i++) {
		if(i != buf[i]) //数据读取错误，返回0
			return 0;
	}
	return 1;
}
int read_data(unsigned long addr, int length, char* buf)  //读数据
{
	char tmp[5] = {0};
	tmp[0] = 0x55;  //指令标志位
	tmp[1] = 0xAA;  //指令标志位
	tmp[2] = (addr >> 8) | 0x80;    //高八位，最高位置1表示读
	tmp[3] = addr & 0xFF;
	tmp[4] = length;
#ifdef debug_out
	printf("read :\n");
	printf("flag:%02x %02x \naddr:%02x %02x \n length:%02x\n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4]);
#endif
	memcpy(map_base, tmp, 5)  ;//write addr
	memcpy(buf, map_base, length) ;   //read

	return 0;
}
int write_data(unsigned long addr, int length, char* buf)  //写数据源
{
	char tmp[5] = {0};
	tmp[0] = 0x55;  //指令标志位
	tmp[1] = 0xAA;  //指令标志位
	tmp[2] = (addr >> 8) & 0x7F;  //高八位，最高位置0表示写
	tmp[3] = addr & 0xFF;
	tmp[4] = length;

#ifdef debug_out
	printf("write :\n");
	printf("flag:%02x %02x \naddr:%02x %02x \n length:%02x\n",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4]);
#endif
	memcpy(map_base, tmp, 5)  ;//write addr and length
	memcpy(map_base, buf, length)  ;//write data
	return 0;
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