#ifndef __file_lyi 
#define __file_lyi

/*获取dir路径下所有文件名称，存储到file_name里面
*返回值：文件个数
*file格式：每100个字节存储一个文件名称与文件名长度。
*/
int get_filename(unsigned char *dir, unsigned char *file_name);






/*发送已存储好的大文件
*dir：文件绝对路径
*/
int send_picture_file(unsigned char *dir);
int send_data(int fd, char* send_buf, int length, int timeout_ms); //发送数据


#endif // 宏结束行