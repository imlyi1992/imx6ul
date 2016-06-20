#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>



//#define debug_out 123
#define map_base_length 0x100000
#define phy_addr 0x50000000

int read_data(unsigned long addr, int length, char* buf);  //读数据
int write_data(unsigned long addr, int length, char* buf);  //写数据
int check_data(char* buf);  //检测数据
int printf_data(char* buf);  //打印数据

unsigned char * map_base;
int main()
{

	FILE *f;
	int n, fd;



	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		return (-1);
	}

	map_base = mmap(NULL, map_base_length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr);

	if (map_base == 0) {
		printf("NULL pointer!\n");
	} else {
		printf("Successfull!\n");
	}

	int i = 0;
	int write_length = 32;
	char send_buf[map_base_length] = {0};
	char rec_buf[map_base_length] = {0};
	for(i = 0; i < map_base_length; i++)
		send_buf[i] = i;

	for(i = 0; i < 0x8FFF; i++) {
		write_data(i, 32, send_buf) ; //写数据源
		read_data(i, 32, rec_buf)  ;//读数据
		if(0 == check_data(rec_buf)){  //数据读取失败
		   printf("第%d次读取错误\n",i + 1);
		   printf_data(rec_buf) ;
		   break;
		}	   
		memset(rec_buf, 0, map_base_length); //清空接收函数
	}
    printf("测试成功次数：%04x\n",i );

	close(fd);
	
	munmap(map_base, map_base_length);

	return (1);
}

int printf_data(char* buf)  //打印数据
{
	
	int i = 0;
	for(i = 0; i < 32; i++){
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
	for(i = 0; i < 32; i++){
		if(i != buf[i]) //数据读取错误，返回0
				return 0;
	}
	return 1;
}
int read_data(unsigned long addr, int length, char* buf)  //读数据
{
	buf[0] = 0x55;  //指令标志位
	buf[1] = 0xAA;  //指令标志位
	buf[2] = (addr >> 4) | 0x80;    //高八位，最高位置1表示读
	buf[3] = addr & 0xFF;
	buf[4] = length;
#ifdef debug_out
	printf("read :\n");
	printf("flag:%02x %02x \naddr:%02x %02x \n length:%02x\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
#endif
	memcpy(map_base, buf, 5)  ;//write addr
	memcpy(buf, map_base, length) ;   //read

	return 0;
}
int write_data(unsigned long addr, int length, char* buf)  //写数据源
{
	char tmp[5] = {0};
	tmp[0] = 0x55;  //指令标志位
	tmp[1] = 0xAA;  //指令标志位
	tmp[2] = (addr >> 4) & 0x7F;  //高八位，最高位置0表示写
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