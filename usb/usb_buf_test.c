#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include<sys/mman.h>

#define BUFF_LENGTH (1024 * 7)

int send_data(int fd, char* send_buf, int length, int timeout_ms); //��������
int sysUsecTime();

int main(int argc, char ** argv)
{

	int usb_fd,file_fd;  //usb�ļ�������
	usb_fd = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd == -1) {
		printf("cant't open usb\n");
		return (-1);
	}



	char SEND_BUF[BUFF_LENGTH] = {0}; //���ͻ�����
	char REC_BUF[BUFF_LENGTH] = {0}; //���ջ�����


	int time = sysUsecTime();
	int write_num = 0;
	int write_all = 0;
	int file_read_bytes = 0;
	int i;




	int time_out_times = 0;
	for(i = 0 ; i < 125 * 200; i++) {
		write_num = send_data(usb_fd, SEND_BUF, BUFF_LENGTH, 1000);
		switch(write_num) { //selectʹ��
		case -1:  //select�����˳�����
			printf("select err\n");
			close(usb_fd);
			return -1;
		case 0:  //��ʱ���˳�����
			time_out_times++;
			if(time_out_times >= 4) {
				printf("time out\n");
				close(usb_fd);
				usb_fd = open("/dev/ttyGS0", O_RDWR);
				//return 0;

			}// end switch
			break;
		default:
			time_out_times = 0;
			write_all = write_all + write_num;
		}
	}



	close(usb_fd);
	return 0;
}

/*
*author��liyi
*time:20160513
*int send_data(int fd, char* send_buf, int length, int timeout_ms);
*�������ݡ�
*������
*	fd���ļ�������
*	sendbuf�����ݻ�����
*	length�����ͳ���
*	timeout_ms:��ʱ����λms
*����ֵ��
*	�ɹ������ط��ͳ�ȥ�����ݸ���
*	ʧ�ܣ�
*		-1��select����ʧ�ܣ�
*		0����ʱ��
*/
int send_data(int fd, char* send_buf, int length, int timeout_ms)//��������
{
	int maxfdp;  //�ɲ�ѯ����ļ�������
	fd_set fds;  //select ��ѯ����
	struct timeval timeout;  //��ʱ�ṹ��
	timeout.tv_sec = timeout_ms / 1000;      //��
	timeout.tv_usec = timeout_ms % 1000;   //����


	FD_ZERO(&fds); //ÿ��ѭ����Ҫ��ռ��ϣ������ܼ���������仯
	FD_SET(fd,&fds); //����������
	maxfdp=fd+1;    //���������ֵ��1
	switch(select(maxfdp,NULL,&fds,NULL,&timeout)) { //selectʹ��
	case -1:  //select�����˳�����
		return -1;
	case 0:  //��ʱ���˳�����
		return 0;
	default:
		if(FD_ISSET(fd,&fds)) { //�����ļ��Ƿ��д
			printf("123\n");
			return write(fd,send_buf,length);   //���ط������ݸ���
		}
	}// end switch
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