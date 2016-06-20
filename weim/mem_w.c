#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>

#define length 0x100000
#define phy_addr 0x50000000

int main()
{
    unsigned char * map_base;
    FILE *f;
    int n, fd;
	
	

    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (fd == -1)
    {
        return (-1);
    }

    map_base = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr);

    if (map_base == 0)
    {
        printf("NULL pointer!\n");
    }
    else
    {
        printf("Successfull!\n");
    }

    unsigned long addr;
    unsigned char content;
    int i = 0;
    int write_length = 32;
    char tmp_val[length] = {0};
    for(i = 0; i < length; i++)
    	tmp_val[i] = i;
	
    tmp_val[0] = 0x7F;
    tmp_val[1] = 0xFF;
    tmp_val[2] = write_length;

 	printf("write addr:%02x%02x\n",tmp_val[0],tmp_val[1]);
	printf("write length = %d:\n",write_length);	
	
	memcpy(map_base, tmp_val, 3 + write_length)  ;//write addr and data
           
	for (i = 0 ;i < write_length; i++)
    {
        printf("%02x ",tmp_val[ i+ 3]);
	if(0 == (i+1)%9)
	printf("\n");
    }
	printf("\n");

    close(fd);

    munmap(map_base, length);

    return (1);
}
