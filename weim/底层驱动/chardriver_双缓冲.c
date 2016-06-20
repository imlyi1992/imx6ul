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

#define IMX_GPIO_NR(bank, nr)   (((bank) - 1) * 32 + (nr))
#define VPU_POW_INT_PIN         IMX_GPIO_NR(4, 20)
#define VPU_POW_INT             gpio_to_irq(VPU_POW_INT_PIN)

static DECLARE_WAIT_QUEUE_HEAD(vpu_pow_read_wait);
static int wait_flag = 0;
static struct timer_list weim_timer;


#define DEVICE_NAME "weim_gpio"
static struct class *cdev_class;

static volatile int ev_press = 0;
char key_val = 1; //key vaul

dev_t dev = 0;//这里是动态分配设备号和动态创建设备结点需要用到的
struct cdev dev_c;

#define map_base_length 0x100000
static volatile unsigned long *map_base = NULL;
static unsigned char send_buf[map_base_length] = {0};


static unsigned char rec_buf[map_base_length] = {0}; //接收缓冲区
//临时接收缓冲区（当应用端读取缓冲区的时候，避免数据冲突，发送异步信号之后使用临时缓冲区）
static unsigned char rec_buf_2[map_base_length] = {0};  
static volatile int buf_change = 0;  //接收缓冲区切换标志位
    
static unsigned long rec_buf_num = 0;   //数据读取个数
static unsigned char *rec_buf_tmp = NULL;   //读取地址缓冲区
static unsigned long return_rec_buf_num = 0;   //数据读取个数
static unsigned char *return_rec_buf_tmp = NULL;   //返回读取地址缓冲区

static unsigned long read_addr = 0x100;


void enable_gpio(void);   //使能gpio功能
int read_data(unsigned long addr, int length, char* buf);  //读数据
int write_data(unsigned long addr, int length, char* buf);  //写数据
void deal_data(void); //处理数据缓冲区切换

static struct fasync_struct *wei_gpio_async;  //异步通知


int wei_gpio_open(struct inode *inode, struct file *file)
{
	return 0;
}

ssize_t wei_gpio_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
//	printk("读取到数据个数：%d\n",return_rec_buf_num);
	copy_to_user(buf,return_rec_buf_tmp,return_rec_buf_num);
	return return_rec_buf_num;
}

static unsigned wei_gpio_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;

	if (ev_press)
		mask |= POLLIN | POLLRDNORM;
	return mask;
}

static int weim_fasync (int fd, struct file *filp, int on)
{
	//printk("driver: weim_fasync\n");
	return fasync_helper (fd, filp, on, &wei_gpio_async);
}

struct file_operations only_test = {
	.owner  =   THIS_MODULE,
	.open   =   wei_gpio_open,
	.read   =   wei_gpio_read,
	.poll   =   wei_gpio_poll,
	.fasync =   weim_fasync,
};



static irqreturn_t weim_irq(int irq, void *dev_id)
{
	
	read_data(read_addr, 32, rec_buf_tmp);//读数据
	rec_buf_num = rec_buf_num + 32;  	//接收数据个数+32
	if(rec_buf_num >= map_base_length) {
		deal_data(); //缓冲区溢出，说明数据接收不正常直接丢弃
		printk("缓冲区溢出，请检查中断脚是否异常拉低");  //缓冲区溢出，说明很长世间内都不存在
	} else
		rec_buf_tmp = rec_buf_tmp + 32; 	//每次读取一次都往后移动32.
	mod_timer(&weim_timer, jiffies + HZ/10);    //100ms延时s
	return IRQ_HANDLED;
}


/*
*延时触发函数
*如果10ms内被重置，说明当前图片没有被接收完毕
*如果10ms内没有被重置，说明当前图片接收完毕，通知应用层将图片读取出去
*/
static void weim_timer_function(unsigned long data)
{
//	printk("happen kill_fasync\n");

	deal_data(); 
	kill_fasync (&wei_gpio_async, SIGIO, POLL_IN);  //异步通知
}

void deal_data(void) //处理数据缓冲区切换
{
	
	return_rec_buf_num = rec_buf_num;
	return_rec_buf_tmp = rec_buf_tmp;
	rec_buf_num = 0;
    buf_change++;
	buf_change = buf_change % 2;
	if(0 == buf_change) 	
		rec_buf_tmp	= rec_buf;
	else
		rec_buf_tmp	= rec_buf_2;
	printk("buf_change = %d\n",buf_change);
}



void enable_gpio(void)   //使能gpio功能
{

	init_timer(&weim_timer);
	weim_timer.function = weim_timer_function;
	weim_timer.data = 1;
	add_timer(&weim_timer);

	map_base = (volatile unsigned long *)ioremap(0x50000000, 16);    // 映射实际物理地址
	printk("底层weim物理地址映射成功\n");
	rec_buf_tmp = rec_buf;  //绑定接收缓冲区收地址
	rec_buf_num = 0;  //接收数据个数初始化为0

	int result;
	result = gpio_request(VPU_POW_INT_PIN, "Leds");
	if (result < 0)
		printk("gpio_request failed!\n");

	result = gpio_direction_input(VPU_POW_INT_PIN);
	if (result < 0) {
		printk("gpio_direction failed!\n");
		gpio_free(VPU_POW_INT_PIN);   //如果不成功就释放这个管脚。
	}

	request_irq(VPU_POW_INT, weim_irq, 0, "keys_test", NULL);
	irq_set_irq_type(VPU_POW_INT, IRQF_TRIGGER_LOW);  //下降触发

}

int read_data(unsigned long addr, int length, char* buf)  //读数据
{
	char tmp[5] = {0};
	tmp[0] = 0x55;  //指令标志位
	tmp[1] = 0xAA;  //指令标志位
	tmp[2] = (addr >> 8) | 0x80;    //高八位，最高位置1表示读
	tmp[3] = addr & 0xFF;
	tmp[4] = length;


	int i = 0;
	for(i = 0; i < 5; i++)
		writeb(tmp[i],map_base)  ;//write addr

	for(i = 0; i < length; i++)
		buf[i] = readb(map_base) ;   //read
	/*

		memcpy_toio(map_base, tmp, 5);
		memcpy_fromio(buf, map_base, length);
		*/
	return 0;
}
int write_data(unsigned long addr, int length, char* buf)  //写数据源
{
	char tmp[5] = {0};
	tmp[0] = 0x55;  //指令标志位
	tmp[1] = 0xAA;  //指令标志位
	tmp[2] = (addr >> 8) & 0x7F;  //高八位，最高位置0表示写
	tmp[3] = addr & 0xFF;
	tmp[4] = length;

	memcpy(map_base, tmp, 5)  ;//write addr and length
	memcpy(map_base, buf, length)  ;//write data

	int i = 0;
	for(i = 0; i < 5; i++)
		writeb(tmp[i],map_base)	;//write addr

	for(i = 0; i < length; i++)
		writeb(buf[i],map_base)	;//write addr

	/*
	memcpy_toio(map_base, tmp, 5);
	memcpy_toio(map_base, buf, length);
	*/
	return 0;
}


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

	enable_gpio();   //使能gpio功能

	return ret;
}

static void first_drv_exit(void)
{
	//注销设备驱动
	free_irq(VPU_POW_INT, NULL);
	gpio_free(VPU_POW_INT_PIN);

	device_destroy(cdev_class, dev);
	class_destroy(cdev_class);
	unregister_chrdev_region(dev,1);
	iounmap(map_base);
	printk("globalvar_exit \n");
}

MODULE_LICENSE("GPL");
module_init(first_drv_init);
module_exit(first_drv_exit);
