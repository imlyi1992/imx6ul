#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#define length (1024)
#define phy_addr 0x50000000


int main()
{
	int num, fd;

	fd = open("/dev/ttyGS0", O_RDWR);
	if (fd == -1) {
		return (-1);
	}


	char tmp_val[length] = {0};
	char tmp_read[length] = {0};
	int i = 0;
	for(i = 0; i < length; i++)
		tmp_val[i] = i;

  //  read(fd,tmp_val,length);
	
 //   printf("123\n");
	write(fd,tmp_val,length);
		
	close(fd);

	return (1);
}

