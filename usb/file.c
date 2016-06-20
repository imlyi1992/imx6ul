#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <string.h>

#include <time.h>  
#include <sys/time.h>  
#define BUFFER_SIZE (1024 * 5)

int sysUsecTime();
int main(int argc, char **argv)
{


	int send_fd, usb_fd;
	int bytes_read, bytes_write, send_all;
	char buffer[BUFFER_SIZE];
	char *ptr;
	if (argc != 2) {
		fprintf(stderr, "Usage��%s sendfile\n\a", argv[0]);
		return (1);
	}

	usb_fd = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd == -1) {
		printf("can't open usb\n");
		return (-1);
	}
	int time  = 0;
	time = sysUsecTime();
	send_all = 0;
	
	

	/* ��Դ�ļ� */
	

    int i = 0;

		
   if ((send_fd = open(argv[1], O_RDONLY)) == -1) {
		fprintf(stderr, "Open %s Error��%s\n", argv[1], strerror(errno));
		return (1);
   }

	/* ���´�����һ������Ŀ����ļ��Ĵ��� */
	while (bytes_read = read(send_fd, buffer, BUFFER_SIZE)) {
		/* һ�������Ĵ������� */
		if ((bytes_read == -1) && (errno != EINTR)) break;
		else if (bytes_read > 0) {
			ptr = buffer;
			while (bytes_write = write(usb_fd, ptr, bytes_read)) {
				/* һ�������������� */
				if ((bytes_write == -1) && (errno != EINTR))break;
				/* д�������ж����ֽ� */
				else if (bytes_write == bytes_read){ 
				send_all = send_all + bytes_read;
				break;
				}
				/* ֻд��һ����,����д */
				else if (bytes_write > 0) {
					ptr += bytes_write;
					bytes_read -= bytes_write;
				}
			}
			/* д��ʱ�������������� */
			if (bytes_write == -1)break;
		}
	}

	close(send_fd);
	
	
	printf("used time = %d s\n total send: %d byte\n speed:%0.3fM/s",sysUsecTime() - time, send_all, ((float)send_all / 1024 / 1024) / (sysUsecTime() - time));
	close(usb_fd);
	return (0);
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
