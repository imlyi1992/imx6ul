#ifndef __eim_lyi 
#define __eim_lyi

//#define debug_out 123
#define map_base_length  0x1000000
#define phy_addr  0x50100000






unsigned char * map_base;


int eim_read_data(unsigned long addr, int length, unsigned char* buf);  //读数据
int eim_write_data(unsigned long addr, int length, unsigned char* buf);  //写数据

int save_data(char* buf, int buf_length); //存储图片数据

//eim主处理函数
int eim_main();





#endif // 宏结束行