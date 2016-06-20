#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#define length (1024 )
#define phy_addr 0x50000000

int sysUsecTime();

int main()
{
	int num, fd;

	fd = open("/dev/ttyGS0", O_RDWR);
	if (fd == -1) {
		return (-1);
	}

	struct pollfd fds[1];
	int ret , ret_num;
	fds[0].fd     = fd;
	fds[0].events = POLLIN;

	char tmp_val[length] = {0};
	int i = 0;

	int time  = 0;
	time = sysUsecTime();
	for (i = 0 ; i < 100; i++) {
		ret = poll(fds, 1, 5000);
		if (ret == 0){
			printf("time out\n");
			break;
		}
		else{
			ret_num = write(fd,tmp_val,length);
		printf("i = %d, num = %d\n", i, ret_num);
		}

	}
	printf("used time = %d s\n",time - sysUsecTime());

	close(fd);

	return (1);
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
