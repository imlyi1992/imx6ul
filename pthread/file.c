#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include<sys/mman.h>
#include "common.h"
#include "file.h"


#define SEND_FILE_BUFF_LENGTH (1024 * 7)

/*��ȡdir·���������ļ����ƣ��洢��file_name����
*����ֵ���ļ�����
*file��ʽ��ÿ100���ֽڴ洢һ���ļ��������ļ������ȡ�
*/
int get_filename(unsigned char *dir, unsigned char *file_name)
{
	DIR *dp;
	struct dirent *dirp;

	//��ָ����Ŀ¼
	dp = opendir(dir);
	if (dp== NULL) {
		printf("can't open the file\n");
	}

	int file_num = 0;
	//����Ŀ¼
	while ((dirp = readdir(dp)) != NULL) {
		file_name[file_num * 100] = strlen(dirp->d_name);
		memcpy(file_name + file_num * 100 + 1, dirp->d_name, strlen(dirp->d_name));
		file_num++;
	}

	//�ر�Ŀ¼
	closedir(dp);
	return file_num;
}


/*�����Ѵ洢�õĴ��ļ�
*dir���ļ�����·��
*/
int send_picture_file(unsigned char *dir)
{

	int file_fd;  //usb�ļ�������

	char SEND_BUF[SEND_FILE_BUFF_LENGTH] = {0}; //���ͻ�����

	int write_num = 0;
	int write_all = 0;
	int file_read_bytes = 0;
	
	printf("filename = %s\n",dir);

	if ((file_fd = open(dir, O_RDONLY)) == -1) {
		fprintf(stderr, "Open %s Error��%s\n", dir, strerror(errno));
		return -1;
	}

	char *ptr = NULL;
	int time_out_times = 0;
	while (file_read_bytes = read(file_fd, SEND_BUF, SEND_FILE_BUFF_LENGTH)) {
		/* һ�������Ĵ������� */
		if ((file_read_bytes == -1) && (errno != EINTR)) break;
		else if (file_read_bytes > 0) {
			ptr = SEND_BUF;
			while (		write_num = send_data(fd_usb, ptr, file_read_bytes, 1000)) {
				/* һ�������������� */
				if ((write_num == -1) && (errno != EINTR))break;
				/* д�������ж����ֽ� */
				else if (write_num == file_read_bytes) {
					write_all = write_all + write_num;
					break;
				}
				/* ֻд��һ����,����д */
				else if (write_num > 0) {
					printf("ֻд��һ����,����д\n");
					ptr += write_num;
					file_read_bytes -= write_num;
					write_all = write_all + write_num;
				} else { //��ʱ
					time_out_times++;
					if(time_out_times >= 4) {
						printf("��ʱ\n");
						break;
					}

				}
			}
			/* д��ʱ�������������� */
			if (write_num == -1)break;
		}
	}

	printf("write all length = %d\n",write_all);

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
