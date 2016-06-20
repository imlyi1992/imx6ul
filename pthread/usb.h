#ifndef __usb_lyi 
#define __usb_lyi

/*ep0ָ���*/
/*�������ָ��*/
#define  CMD_INI 							0x01
#define  SET_INI                            0x02
#define  GET_INI                            0x03


/*�������ָ��*/
#define  CMD_FILE									0x02
#define  GET_FILE_ALL_NAME					0x00




/*���ļ����ָ��*/
#define  CMD_PICTURE 			0x03



/*ʵʱ��������*/
#define  CMD_REAL_DATA 			0x04






int fd_ep0;  //usb_ep0�ļ�������


/*usb����������*/
int usb_main(int fd_usb);


/*ָ���⺯�������ep0���͹�����ָ��*/
int check_cmd(unsigned char *cmd);


/******************************************/
/*��д���ú���*/
int cmd_ini(unsigned char *cmd);

/******************************************/
/*��д��������*/
int cmd_file(unsigned char *cmd);
/*��ȡ���в���,�����ص�pc*/
int cmd_file_get_all_name(void);


/*****************************************/
/*��ͼƬ����*/
int cmd_picture(unsigned char *cmd);




/*****************************************/
/*ʵʱ�ɼ�ͼƬ��Ϣ*/
int cmd_real_data(unsigned char *cmd);



/*****************************************/
/*������Ϣ��д*/
int cmd_set_ini(unsigned char *cmd);
int cmd_get_ini(unsigned char *cmd);





#endif // �������