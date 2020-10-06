#include <linux/device.h>   // class_create, device_create()
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	// printk()
#include <linux/slab.h>		// kmalloc()
#include <linux/fs.h>		// everything...
#include <linux/errno.h>	// error codes
#include <linux/types.h>	// size_t
#include <linux/cdev.h>
#include <linux/sysfs.h>
#include <asm/uaccess.h>	// copy_*_user

//Board specific

#include <linux/serio.h>
#include <linux/clk.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <mach/regs-clock.h>
#include <plat/regs-adc.h>
#include <mach/regs-gpio.h>

#include "s3c24xx-adc.h"




////////////////////////// structs  ///////////////////////////////
typedef struct {
	wait_queue_head_t wait;
	int channel;
	int prescale;
}ADC_DEV;

////////////////////////// Globals  ///////////////////////////////

unsigned long virtual_count[4]={0,0,0,0};
int major_num = -1;
int minor_num =  0;
//int count = 0;

/* Device variables */
static struct class* ADC_class = NULL;
static struct device* onchip_A2D_device = NULL;


static void __iomem *base_addr;

//mutex
struct mutex my_adc_lock; 

//spinlock
static DEFINE_SPINLOCK (my_adc_spinlock);

static int OwnADC = 0;
static ADC_DEV adcdev;
static volatile int ev_adc = 0;
static int adc_data;

static struct clk	*adc_clock;
struct cdev cdev;	  /* Char device structure		*/
int irq_count = 0;

////////////////////////// defines  ///////////////////////////////
#define DEVICE_NAME "onchip_A2D"
#define CLASS_NAME  "ADC"

/* We'll use our own macros for printk */
#define dbg(format, arg...) do { if (debug) pr_info(CLASS_NAME ": %s: " format, __FUNCTION__, ## arg); } while (0)
#define err(format, arg...) pr_err(CLASS_NAME ": " format, ## arg)
#define info(format, arg...) pr_info(CLASS_NAME ": " format, ## arg)
#define warn(format, arg...) pr_warn(CLASS_NAME ": " format, ## arg)


#define ADCCON      (*(volatile unsigned long *)(base_addr + S3C2410_ADCCON))	//ADC control
#define ADCTSC      (*(volatile unsigned long *)(base_addr + S3C2410_ADCTSC))	//ADC touch screen control
#define ADCDLY      (*(volatile unsigned long *)(base_addr + S3C2410_ADCDLY))	//ADC start or Interval Delay
#define ADCDAT0     (*(volatile unsigned long *)(base_addr + S3C2410_ADCDAT0))	//ADC conversion data 0
#define ADCDAT1     (*(volatile unsigned long *)(base_addr + S3C2410_ADCDAT1))	//ADC conversion data 1
#define ADCUPDN     (*(volatile unsigned long *)(base_addr + 0x14))	//Stylus Up/Down interrupt status

#define PRESCALE_DIS        (0 << 14)
#define PRESCALE_EN         (1 << 14)
#define PRSCVL(x)           ((x) << 6)
#define ADC_INPUT(x)        ((x) << 3)
#define ADC_START           (1 << 0)
#define ADC_ENDCVT          (1 << 15)

#define START_ADC_AIN(ch, prescale) \
	do{ \
		ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) ; \
		ADCCON |= ADC_START; \
	}while(0)
	
///////////////////////////////////////////////////////////////////////////////

MODULE_AUTHOR("Benny Cohen");
MODULE_LICENSE("GPL");

///////////////////////////////////////////////////////////////////////////////
static ssize_t show_func(struct device *dev, struct device_attribute *attr, char *buf)
{
	int len;
	
	mutex_lock(&my_adc_lock);
	printk("show_func() show func called:\n");
	len = snprintf(buf, PAGE_SIZE,
			"VC0: %ld\n"
			"VC1: %ld\n"
			"VC2: %ld\n"
			"VC3: %ld\n",
			virtual_count[0],
			virtual_count[1],
			virtual_count[2],
			virtual_count[3]);
	//printk("show_func() %s  length=%d", buf,len);
	mutex_unlock(&my_adc_lock);		
	return len;
}
///////////////////////////////////////////////////////////////////////////////
/* Declare the sysfs entries. 
 * The macros create the instance of dev_attr_file, 
 * the "/sys/class/ADC/ADC_onchip_A2D_device/file" to read from */

static DEVICE_ATTR(file, 0666, show_func /*show func*/, NULL /*store func*/);

////////////////////////// adcdone_int_handler  ///////////////////////////////
static irqreturn_t adcdone_int_handler(int irq, void *dev_id)
{
	unsigned long flags;
	spin_lock_irqsave(&my_adc_spinlock, flags);
	if (OwnADC) {
		adc_data = ADCDAT0 & 0x3ff;

		ev_adc = 1;
		wake_up_interruptible(&adcdev.wait);
	}
	printk( KERN_WARNING  "adcdone_int_handler() INT took place\n");

	spin_unlock_irqrestore(&my_adc_spinlock, flags);
	
	return IRQ_HANDLED;
}

////////////////////////// s3c2410_adc_read  ///////////////////////////////
static ssize_t s3c2410_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	char str[20];
	int value;
	size_t len;
	int ret_val;
	unsigned long flags;
	
	spin_lock_irqsave(&my_adc_spinlock, flags); //<<<<<<<<<<<<<<<<<<<<<
	printk( KERN_WARNING  "s3c2410_adc_read() started\n");
	OwnADC = 1; //we own the ADC
	START_ADC_AIN(adcdev.channel, adcdev.prescale);
	spin_unlock_irqrestore(&my_adc_spinlock, flags);//<<<<<<<<<<<<<<<<<<<<<

	//Puts the current process to sleep on the wait queue
	printk( KERN_WARNING  "s3c2410_adc_read() going to sleep, waiting on event\n");
	wait_event_interruptible(adcdev.wait, ev_adc);
	printk( KERN_WARNING  "s3c2410_adc_read() just woke up from event\n");
		
	//init the event for next time
	spin_lock_irqsave(&my_adc_spinlock, flags);//<<<<<<<<<<<<<<<<<<<<<
	ev_adc = 0; 
	value = adc_data;
	OwnADC = 0;
	spin_unlock_irqrestore(&my_adc_spinlock, flags);//<<<<<<<<<<<<<<<<<<<<<

	

	mutex_lock(&my_adc_lock);
	printk( KERN_WARNING  "AIN[%d] = 0x%04x, %d\n", adcdev.channel, adc_data,ADCCON & 0x80 ? 1:0);	
	virtual_count[3] = virtual_count[2];
	virtual_count[2] = virtual_count[1];
	virtual_count[1] = virtual_count[0];
	virtual_count[0] = value;
	printk( KERN_NOTICE  "s3c2410_adc_read() mutex locked\n");
	mutex_unlock(&my_adc_lock);


	len = sprintf(str, "%d\n", value);
	if (count >= len) 
	{
		printk( KERN_WARNING  "s3c2410_adc_read() got the required data, ...copying to user mode\n");
		ret_val = copy_to_user(buffer, str, len);
		if(ret_val==0)	
			return  count;
		else 
			return  -EFAULT;
	 } 
	else 
	{
		printk( KERN_WARNING  "s3c2410_adc_read() didnt get the required amount of data\n");
		return -EINVAL;
	}
}
/************************************************************************************************************************/
/*
 * Open and close
 */
/************************************************************************************************************************/
////////////////////////// s3c2410_adc_open  ///////////////////////////////
static int s3c2410_adc_open(struct inode *inode, struct file *filp)
{
	int irq_ret = -1;
	
	printk( KERN_WARNING  "s3c2410_adc_open() started\n");
	
	mutex_lock(&my_adc_lock);
	if (!(irq_count++ > 0))
	{
		printk( KERN_WARNING  "s3c2410_adc_open() alloacting irq-request\n");
		irq_ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, DEVICE_NAME, &adcdev);
		if (irq_ret)
        {
			iounmap(base_addr);
			printk( KERN_ERR  "Requesting IRQ for ADC failed irq_ret=%d\n",irq_ret);
			return irq_ret;
		}
	}
	mutex_unlock(&my_adc_lock);

	printk( KERN_NOTICE  "s3c2410_adc_open() ADC opened\n");
	return 0;
}
////////////////////////// s3c2410_adc_release  ///////////////////////////////
static int s3c2410_adc_release(struct inode *inode, struct file *filp)
{
	int irq_ret = -1;
	
	mutex_lock(&my_adc_lock);
	printk( KERN_WARNING "s3c2410_adc_release() started\n");
	if (!(--irq_count > 0))
	{
		free_irq(IRQ_ADC, &adcdev);
		irq_ret=-1; //for next allocation
		printk( KERN_WARNING  "s3c2410_adc_release() irq-request de-allocated\n");
	}
	printk( KERN_WARNING "s3c2410_adc_release() adc closed\n");
	mutex_unlock(&my_adc_lock);
	return 0;
}

////////////////////////// struct file_operations  ///////////////////////////////
static struct file_operations dev_fops = {
	owner:	THIS_MODULE,
	open:	s3c2410_adc_open,
	read:	s3c2410_adc_read,	
	release:s3c2410_adc_release,
};
/************************************************************************************************************************/
/*
 * Set up the char_dev structure for this device.
 */
static void setup_cdev(void)
{
	int err, devno = MKDEV(major_num, minor_num);
    
	cdev_init(&cdev, &dev_fops);
	cdev.owner = THIS_MODULE;
	cdev.ops = &dev_fops;
	err = cdev_add (&cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error adding char device err=%d", err);
}
/************************************************************************************************************************/

////////////////////////// dev_init  ///////////////////////////////

/* main entry point for the driver,
 * first to be run when driver is uploaded
 */ 
static int __init dev_init(void)
{
	dev_t devno = 0;
	int err,retval = -1;
	

	/* asking for a dynamic major at load time */
	printk(KERN_NOTICE "init_module(): dont have a major so ask the kernel to allocate one");
	retval = alloc_chrdev_region(&devno, minor_num, 1,DEVICE_NAME);
	if (retval < 0) 
	{
		//printk(KERN_WARNING "init_module(): REGISTRATION FAILURE %d\n", major_num);
		err("init_module(): REGISTRATION FAILURE %d\n", major_num);
		goto reg_err;  // Make this more graceful 
	}
	
	major_num = MAJOR(devno);
	printk(KERN_NOTICE "init_module():  the kernel gave us major =  %d\n", major_num);
	
	/*  We can connect our device to a an existing class, or create a new one
	 *  We have created a "virtual" class called ADC.  
	 *  The "ADC" directory will be created:  "/sys/class/ADC"                                  	
	 */ 
	ADC_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(ADC_class)) 
	{
		err("failed to register device class '%s'\n", CLASS_NAME);
		goto reg_err;
	}
 
	devno = MKDEV(major_num, minor_num);
 
	/*  now that the class is already constructed, lets instantiate a device and attach it to the class
	 *  this will:  
	 *  1. create the directory "ADC_onchip_A2D_device" such as: "/sys/class/ADC/ADC_onchip_A2D_device"
	 *  2. a device-file under /dev will be created with the given #major #minor values: "/dev/ADC_onchip_A2D_device"                                	
	 */ 
	onchip_A2D_device = device_create(ADC_class, NULL, devno, NULL, CLASS_NAME "_" DEVICE_NAME);
	if (IS_ERR(onchip_A2D_device)) 
	{
		err("failed to create device '%s_%s'\n", CLASS_NAME, DEVICE_NAME);
		goto class_err;
	}

	base_addr=ioremap(S3C2410_PA_ADC,0x20);
	if (base_addr == NULL) {
		printk(KERN_ERR "Failed to remap register block\n");
		return -ENOMEM;
	}
	printk(KERN_WARNING "re-mapping register for virtual mem base_addr=%pk\n",base_addr);

	adc_clock = clk_get(NULL, "adc");
	if (!adc_clock) {
		printk(KERN_ERR "failed to get adc clock source\n");
		return -ENOENT;
	}
	// Dynamic initialization of the mutex
        // has to be inited before usage by open()
	mutex_init(&my_adc_lock);

	clk_enable(adc_clock);
	
	/* normal ADC */
	ADCTSC = 0;
	
	// init the wait_queue
	init_waitqueue_head(&(adcdev.wait));
	
	//default channel and pre-scale configuration
	adcdev.channel=0;
	adcdev.prescale=0xff;
	
	/* Initialize the char device. should be put last */
	setup_cdev();


	err = device_create_file(onchip_A2D_device, &dev_attr_file);
	if (err < 0)
		 //warn("failed to create write /sys endpoint - continuing without\n");
		 printk(KERN_WARNING "failed to create write /sys endpoint - continuing without\n");


	printk (DEVICE_NAME"\tinitialized\n");
	return 0; /* succeed */
		
	
class_err:
	class_destroy(ADC_class);
	
reg_err:
	unregister_chrdev(devno, DEVICE_NAME);
	return -EFAULT;
}

////////////////////////// dev_exit  ///////////////////////////////
static void __exit dev_exit(void)
{
	dev_t devno = MKDEV(major_num, minor_num);

	iounmap(base_addr);

	if (adc_clock) {
		clk_disable(adc_clock);
		clk_put(adc_clock);
		adc_clock = NULL;
	}

	/* Get rid of our char dev entries */
	cdev_del(&cdev);
	
	device_remove_file(onchip_A2D_device, &dev_attr_file);
	device_destroy(ADC_class, devno);
	class_destroy(ADC_class);
	
	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);
	printk(KERN_NOTICE "char device unregistered");
	printk(KERN_NOTICE "char device module unloaded");

}
///////////////////////////////////////////////////////////////////////////////
EXPORT_SYMBOL(my_adc_lock);
module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");

