#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/mach-types.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/pm.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/ioctl.h>
#include <linux/dmapool.h>
#include <linux/moduleparam.h>
#include <linux/prefetch.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>

#include <asm/device.h> //下面这三个头文件是由于动态创建需要加的
#include <linux/device.h>
#include <linux/cdev.h>


#define DEVICE_NAME "tty_ep0"
static struct class *cdev_class;
dev_t dev = 0;//这里是动态分配设备号和动态创建设备结点需要用到的
struct cdev dev_c;


/*内核间通信*/
#include<linux/completion.h>
#define DATA_BUF_SIZE 1024 * 50
extern struct completion rec_comp;
extern struct completion send_comp;
//extern  volatile char data_tmp[DATA_BUF_SIZE];

struct ep0_info_to_user {
	volatile unsigned char 	info_0;
	volatile unsigned char	back_ok;
	volatile unsigned int 		data_length;
	volatile unsigned char*	data;
};

extern struct ep0_info_to_user ep0_info_to_user;

static struct fasync_struct *ep0_fasync_struct;  //异步通知

struct task_struct * my_thread = NULL;



int my_fuction(void *arg)
{
	printk("my_fuction run\n");
	while(1) {
		int ret = wait_for_completion_interruptible_timeout(&rec_comp, HZ * 2);
		if(ret > 0) {
			/*	printk("ep0_info_to_user.info_0 = %d\n",ep0_info_to_user.info_0);
				printk("ep0_info_to_user.back_ok = %d\n",ep0_info_to_user.back_ok);
				printk("ep0_info_to_user.data_length= %d\n",ep0_info_to_user.data_length);
				printk("ep0 data is coming\n");
				*/
			//	printk("ep0 data is coming\n");
			kill_fasync (&ep0_fasync_struct, SIGUSR1, POLL_IN);  //异步通知,通知应用程序有数据过来

		}
		if(ret == 0) {
		}
	}

	return 0;
}

int ep0_open(struct inode *inode, struct file *file)
{
	my_thread = kthread_run(my_fuction, NULL, "thread_name");
	if (IS_ERR(my_thread)) {
		printk("error create thread_name thread\n");
	}
	return 0;
}

/*ep0数据提交给用户空间*/
ssize_t ep0_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{

	copy_to_user(buf, ep0_info_to_user.data, ep0_info_to_user.data_length);
	
	return ep0_info_to_user.data_length;

}

ssize_t ep0_write(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	copy_from_user(ep0_info_to_user.data, buf, size);
	ep0_info_to_user.data_length = size;
	ep0_info_to_user.back_ok = 1;  //指令处理完毕，可以返回数据
	//complete(&send_comp); //通知ep0发送数据
	return size;
}

static int ep0_fasync (int fd, struct file *filp, int on)
{
	printk("driver: weim_fasync\n");
	return fasync_helper (fd, filp, on, &ep0_fasync_struct);
}

struct file_operations ep0_file_operations = {
	.owner  =   THIS_MODULE,
	.open   =   ep0_open,
	.read   =   ep0_read,
	.write  =   ep0_write,
	.fasync =   ep0_fasync,
};


static __init int ep0_demo_init(void)
{

	printk("hello module!\n");

	int ret,err;

	//注册设备驱动

	ret = alloc_chrdev_region(&dev, 0, 1,DEVICE_NAME); //动态分配设备号
	if (ret) {
		printk("globalvar register failure\n");
		unregister_chrdev_region(dev,1);
		return ret;
	} else {
		printk("globalvar register success\n");
	}

	cdev_init(&dev_c, &ep0_file_operations);

	err = cdev_add(&dev_c, dev, 1);

	if(err) {
		printk(KERN_NOTICE "error %d adding FC_dev\n",err);
		unregister_chrdev_region(dev, 1);
		return err;
	}

	cdev_class = class_create(THIS_MODULE, DEVICE_NAME);//动态创建设备结点
	if(IS_ERR(cdev_class)) {
		printk("ERR:cannot create a cdev_class\n");
		unregister_chrdev_region(dev, 1);
		return -1;
	}
	device_create(cdev_class,NULL, dev, 0, DEVICE_NAME);


	return 0;
}

static __exit void ep0_demo_exit(void)
{
	//注销设备驱动
	device_destroy(cdev_class, dev);
	class_destroy(cdev_class);
	unregister_chrdev_region(dev,1);
	kthread_stop(my_thread);
	printk("globalvar_exit \n");
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Kozo");

module_init(ep0_demo_init);
module_exit(ep0_demo_exit);