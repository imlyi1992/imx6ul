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
    if (argc != 2) {
		fprintf(stderr, "Usage��%s sendfile\n\a", argv[0]);
		return (1);
	}


	int usb_fd_send, usb_fd_rec, file_fd;  //usb�ļ�������
	usb_fd_send = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd_send == -1) {
		printf("cant't open usb_fd_send\n");
		return (-1);
	}
	
	usb_fd_rec = open("/dev/ttyGS0", O_RDWR);
	if (usb_fd_rec == -1) {
		printf("cant't open usb_fd_rec\n");
		return (-1);
	}
	

	char SEND_BUF[BUFF_LENGTH] = {0}; //���ͻ�����
	char REC_BUF[BUFF_LENGTH] = {0}; //���ջ�����


	int time = sysUsecTime();
	int write_num = 0;
	int write_all = 0;
	int file_read_bytes = 0;
	int i;

	
	
	while(1){
	
	int maxfdp;  //�ɲ�ѯ����ļ�������
	fd_set fds;  //select ��ѯ����
	int timeout_ms = 1000;
	struct timeval timeout;  //��ʱ�ṹ��
	timeout.tv_sec = timeout_ms / 1000;      //��
	timeout.tv_usec = timeout_ms % 1000;   //����


	FD_ZERO(&fds); //ÿ��ѭ����Ҫ��ռ��ϣ������ܼ���������仯
	FD_SET(usb_fd_send,&fds); //���������
	FD_SET(usb_fd_rec,&fds); //���������
	maxfdp=usb_fd_send>usb_fd_rec?usb_fd_send+1:usb_fd_rec+1;    //���������ֵ��1  
	switch(select(maxfdp,&fds,&fds,NULL,&timeout)) { //selectʹ��
	case -1:  //select�����˳�����
	    printf("select err\n");
		return -1;
	case 0:  //��ʱ���˳�����
		printf("timeout\n");
		return 0;
	default:
		if(FD_ISSET(usb_fd_rec,&fds)) { //�����ļ��Ƿ�ɶ�
		printf("read\n");
		printf("read num = %d\n",read(usb_fd_rec,REC_BUF,5));   //���ؽ������ݸ���
		}/*
		if(FD_ISSET(usb_fd_send,&fds)) { //�����ļ��Ƿ��д
		printf("write\n");
		write(usb_fd_send,SEND_BUF,BUFF_LENGTH);   //���ط������ݸ���
		}*/
	}// end switch
	
	
	}
	
	/*
	for(i = 0 ; i < 125 * 200; i++) {
		write_num = send_data(usb_fd_send, SEND_BUF, BUFF_LENGTH, 4000);
		switch(write_num) { //selectʹ��
		case -1:  //select�����˳�����
			printf("select err\n");
			close(usb_fd_send);
			return -1;
		case 0:  //��ʱ���˳�����
			printf("time out\n");
			close(usb_fd_send);
			return 0;
		default:
			write_all = write_all + write_num;
		}// end switch
	}
	
	*/
	printf("write all length = %d\n",write_all);
	printf("speed: %0.2fM/s\n", (float)(write_all / 1024 /1024) / (sysUsecTime() - time));
	close(usb_fd_send);
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
	FD_SET(fd,&fds); //���������
	maxfdp=fd+1;    //���������ֵ��1
	switch(select(maxfdp,NULL,&fds,NULL,&timeout)) { //selectʹ��
	case -1:  //select�����˳�����
		return -1;
	case 0:  //��ʱ���˳�����
		return 0;
	default:
		if(FD_ISSET(fd,&fds)) { //�����ļ��Ƿ��д
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