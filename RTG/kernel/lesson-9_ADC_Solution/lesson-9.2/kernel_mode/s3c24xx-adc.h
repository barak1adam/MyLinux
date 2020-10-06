#ifndef _S3C2410_ADC_H_
#define _S3C2410_ADC_H_

#ifndef MAJOR_NUMBER
#define MAJOR_NUMBER 0   /* dynamic major by default */
#endif

#define ADC_WRITE(ch, prescale)	((ch)<<16|(prescale))

#define ADC_WRITE_GETCH(data)	(((data)>>16)&0x7)
#define ADC_WRITE_GETPRE(data)	((data)&0xff)

static int __init dev_init(void);
static void __exit dev_exit(void);
static int s3c2410_adc_release(struct inode *inode, struct file *filp);
static int s3c2410_adc_open(struct inode *inode, struct file *filp);
static ssize_t s3c2410_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos);
static void setup_cdev(void);

#endif /* _S3C2410_ADC_H_ */
