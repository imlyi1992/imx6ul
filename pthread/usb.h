#ifndef __usb_lyi 
#define __usb_lyi

/*ep0指令集合*/
/*配置相关指令*/
#define  CMD_INI 							0x01
#define  SET_INI                            0x02
#define  GET_INI                            0x03


/*病例相关指令*/
#define  CMD_FILE									0x02
#define  GET_FILE_ALL_NAME					0x00




/*大文件相关指令*/
#define  CMD_PICTURE 			0x03



/*实时传输数据*/
#define  CMD_REAL_DATA 			0x04






int fd_ep0;  //usb_ep0文件描述符


/*usb处理主函数*/
int usb_main(int fd_usb);


/*指令检测函数，检测ep0发送过来的指令*/
int check_cmd(unsigned char *cmd);


/******************************************/
/*读写配置函数*/
int cmd_ini(unsigned char *cmd);

/******************************************/
/*读写病例函数*/
int cmd_file(unsigned char *cmd);
/*获取所有病例,并发回到pc*/
int cmd_file_get_all_name(void);


/*****************************************/
/*读图片函数*/
int cmd_picture(unsigned char *cmd);




/*****************************************/
/*实时采集图片信息*/
int cmd_real_data(unsigned char *cmd);



/*****************************************/
/*配置信息读写*/
int cmd_set_ini(unsigned char *cmd);
int cmd_get_ini(unsigned char *cmd);





#endif // 宏结束行