#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>



int read_word(unsigned char command_code);
int read_block(unsigned char command_code, int read_num);
int i2c_fd;   //i2c文件描述符
unsigned char tmp_buf[100] = {0};   //读写指令与数据缓存区
int   WHO_AM_I();
float Temperature();
unsigned char ACT_THS();
unsigned char CTRL_REG4();

 



int main()
{
    i2c_fd = open("/dev/i2c-0",O_RDWR);
    if(!i2c_fd) {
        printf("can't open i2c\n");
        return 0;
    }
    ioctl(i2c_fd ,I2C_SLAVE ,0x6a);
    ioctl(i2c_fd ,I2C_TIMEOUT,1);
    ioctl(i2c_fd ,I2C_RETRIES,1);
	printf("WHO_AM_I 	: %x\n",WHO_AM_I());

	printf("Temperature : %0.3f\n",Temperature());

	printf("ACT_THS 	: %x\n",ACT_THS());

	printf("CTRL_REG4 	: %x\n",CTRL_REG4());

    close(i2c_fd);

    return 0;
}



int read_block(unsigned char command_code, int read_num)
{
    tmp_buf[0] = command_code;
    write(i2c_fd, tmp_buf, 1);
    read(i2c_fd, tmp_buf, read_num);
    return 0;
}

int read_word(unsigned char command_code)
{
    tmp_buf[0] = command_code;
    write(i2c_fd, tmp_buf, 1);
    read(i2c_fd, tmp_buf, 1);
    return tmp_buf[0];
}

int   WHO_AM_I()
{
	return read_word(0xf);
}

float Temperature()
{
	unsigned char H_t = read_word(0x16);
	unsigned char L_t = read_word(0x15);
	printf("tmp:%x%x\n",H_t, L_t);
	return 0;
	
}

unsigned char ACT_THS()
{
	return read_word(0x04);
}
unsigned char CTRL_REG4()
{
	return read_word(0x1e);
}




