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

int main()
{
printf("hello\n");
int num, fd;
	
    fd = open("/dev/ttyGS0", O_RDWR);
    if (fd == -1)
    {
		printf("open wrong fd\n");
        return (-1);
    }
	
	 num = open("/dev/ttyGS0", O_RDWR);
    if (num == -1)
    {
		printf("open wrong num\n");
        return (-1);
    }
	while(1)
	{}

return 0;

}
