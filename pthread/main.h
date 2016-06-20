#ifndef __main_lyi 
#define __main_lyi


int main_thread();   //literal meaning
void * thr_fn_usb(void *arg);   //new thread
void * thr_fn_eim(void *arg);   //new thread

void sem_test();//信号量原子操作测试

int init_fd_data(void); //初始化设备描述符和一些变量



#endif // 宏结束行