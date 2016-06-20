#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/mach-types.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/io.h>

#include <linux/input.h>
#include <linux/pm.h>


#include <linux/pci.h>


#include <linux/ioport.h>


#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/list.h>

#include <linux/ioctl.h>

#include <linux/dmapool.h>
#include <linux/moduleparam.h>


#include <linux/prefetch.h>

#include <asm/byteorder.h>
#include <asm/unaligned.h>



#include <asm/device.h> //下面这三个头文件是由于动态创建需要加的
#include <linux/device.h>
#include <linux/cdev.h>


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>


MODULE_AUTHOR("Kozo");
MODULE_LICENSE("GPL");
static pid_t thread_id;


/*内核空间操作文件*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
static char buf[] = "你好";
static char buf1[10];
 




#define DEVICE_NAME "weim_gpio"
static struct class *cdev_class;



dev_t dev = 0;//这里是动态分配设备号和动态创建设备结点需要用到的
struct cdev dev_c;


void file_ops(void);//内核操作文件

void file_ops(void)//内核操作文件
{
	
	struct file *fp;
    mm_segment_t fs;
    loff_t pos;
    printk("hello enter\n");
    fp = filp_open("/home/root/kernel_file", O_RDWR | O_CREAT, 0644);
    if (IS_ERR(fp)) {
        printk("create file error\n");
        return -1;
    }
    fs = get_fs();
    set_fs(KERNEL_DS);
    pos = 0;
    vfs_write(fp, buf, sizeof(buf), &pos);
    pos = 0;
    vfs_read(fp, buf1, sizeof(buf), &pos);
    printk("read: %s\n", buf1);
    filp_close(fp, NULL);
    set_fs(fs);
	
	
	
}


int my_fuction(void *arg)
{
    int i = 0;
    daemonize("mythread");  /*使用kernel_thread创建的线程，需要调用该函数*/

	file_ops();//内核操作文件
	

    return 0;
}

int wei_gpio_open(struct inode *inode, struct file *file)
{
	return 0;
}

ssize_t wei_gpio_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}



struct file_operations only_test = {
	.owner  =   THIS_MODULE,
	.open   =   wei_gpio_open,
	.read   =   wei_gpio_read,


};


static int first_drv_init(void)
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

	cdev_init(&dev_c, &only_test);

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
	
	//thread_id = kernel_thread(my_fuction, NULL, CLONE_FS | CLONE_FILES);
	thread_id =  kernel_thread(my_fuction, NULL, CLONE_KERNEL|SIGCHLD);
	printk("file_ok\n");

	return ret;
}

static void first_drv_exit(void)
{
	//注销设备驱动
	device_destroy(cdev_class, dev);
	class_destroy(cdev_class);
	unregister_chrdev_region(dev,1);
	printk("globalvar_exit \n");
}

module_init(first_drv_init);
module_exit(first_drv_exit);
