#ifndef __file_lyi 
#define __file_lyi

/*��ȡdir·���������ļ����ƣ��洢��file_name����
*����ֵ���ļ�����
*file��ʽ��ÿ100���ֽڴ洢һ���ļ��������ļ������ȡ�
*/
int get_filename(unsigned char *dir, unsigned char *file_name);






/*�����Ѵ洢�õĴ��ļ�
*dir���ļ�����·��
*/
int send_picture_file(unsigned char *dir);
int send_data(int fd, char* send_buf, int length, int timeout_ms); //��������


#endif // �������