#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <plat/gpio-cfg.h>

#define	DEV_NAME	"TTL_IO"
#define DEV_MAJOR	238

#define OPEN_GPIO	S5PV210_GPE0(4)
#define EINT_GPIO	S5PV210_GPH3(1)

struct input_dev *ttlio_input_dev;

irqreturn_t ttlio_irq(int irq, void *dev_id)
{
	//printk("[KERN] ttlio interrupt!!\r\n");
	input_report_key(ttlio_input_dev, KEY_F5, 1);
	input_report_key(ttlio_input_dev, KEY_F5, 0);
	input_sync(ttlio_input_dev);


	return IRQ_HANDLED;
}

static ssize_t ttlio_read(struct file *f, char __user *u, size_t s, loff_t *t)
{
	unsigned char value=0;

	value = gpio_get_value(S5PV210_GPE0(4));
	
	//printk("[KERN] read!! value=%x\r\n", value);

	copy_to_user(u, &value, sizeof(value));

	return sizeof(value);	
}

static ssize_t ttlio_write(struct file *f, const char __user *u, size_t s, loff_t *t)
{
	unsigned char value=0;
	//printk("[KERN] write!! ");

	copy_from_user(&value, u, sizeof(value));

	//printk("%x to %x\r\n", *u, value);

	if(value)
	{
		gpio_direction_output( OPEN_GPIO, 1);
		
		//gpio_set_value( S5PV210_GPE0(4), 1);
		//s3c_gpio_setpin(OPEN_GPIO, 1);
	}
	else
	{
		gpio_direction_output( OPEN_GPIO, 0);

		//gpio_set_value( S5PV210_GPE0(4), 0); 
		//s3c_gpio_setpin(OPEN_GPIO, 0);
	}

	//value = gpio_get_value( S5PV210_GPE0(4) );
	
	//printk("[KERN] read!! value=%x\r\n", value);

	return sizeof(value);
}

static int ttlio_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int ttlio_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations ttlio_driver = {
	.owner		= THIS_MODULE,
	.open		= ttlio_open,
	.release	= ttlio_release,
	.write		= ttlio_write,
	.read		= ttlio_read,	
};

static int __init ttlio_init(void)
{
	int code=0, res=0;

	struct class *ttlio_cls;
	struct device *dev;

	//request GPIO
	//gpio_request( OPEN_GPIO, "GPE04");
	//gpio_request( EINT_GPIO, "GPH31");

	//set pin to GPIO
	//s3c_gpio_cfgpin( OPEN_GPIO, S3C_GPIO_OUTPUT );
	//s3c_gpio_cfgpin( EINT_GPIO, S3C_GPIO_INPUT );

	gpio_direction_output( OPEN_GPIO, 1);
	//gpio_set_value( OPEN_GPIO, 1);

	//gpio_set_value( S5PV210_GPE0(4), 1);
	//s3c_gpio_setpin(OPEN_GPIO, 1);

	//input device
	ttlio_input_dev = input_allocate_device();
	
	if(!ttlio_input_dev)
	{
		//printk("TTLIO input_allocate_device fail\r\n");
	}
	else
	{
		//printk("TTLIO input_allocate_device success\r\n");
	}

	ttlio_input_dev->name = "ttlio";
	ttlio_input_dev->evbit[0] = BIT(EV_KEY);
	ttlio_input_dev->phys = "ttlio/input0";
	ttlio_input_dev->id.bustype = BUS_HOST;
	ttlio_input_dev->id.vendor = 0x0001;
	ttlio_input_dev->id.product = 0x0001;
	ttlio_input_dev->id.version = 0x0001;

	code = KEY_F5;
	set_bit(code & KEY_MAX, ttlio_input_dev->keybit);

	res = input_register_device(ttlio_input_dev);
	if(res)
	{
		//printk("TTLIO input device register error\r\n");
	}
	else
	{
		//printk("TTLIO input device register success\r\n");	
	}

	//irq setting.
	res =
		request_irq(IRQ_EINT(25), ttlio_irq, IRQF_TRIGGER_FALLING, DEV_NAME, &ttlio_input_dev);
//		request_irq(IRQ_EINT(25), ttlio_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, DEV_NAME, &input_dev);

	if(res)
	{
		//printk("TTLIO irq fail\r\n");
	}
	else
	{
		//printk("TTLIO irq success\r\n");
	}

	//printk("TTLIO Driver init\r\n");

	ttlio_cls = class_create(THIS_MODULE, DEV_NAME);
	dev = device_create(ttlio_cls, NULL, MKDEV(238, 0), NULL, DEV_NAME);

	if(!register_chrdev(DEV_MAJOR, DEV_NAME, &ttlio_driver))
	{
		//printk("TTLIO Driver registered\r\n");
	}
	else
	{
		//printk("TTLIO Driver fail\r\n");
		return 0;
	}


	return 0;
}

static void __exit ttlio_exit(void)
{
	//printk("TTL IO EXIT\r\n");
	input_free_device(ttlio_input_dev);

	free_irq(IRQ_EINT(25), NULL);
}

module_init(ttlio_init); //mknod /dev/TTL_IO c 238 0
module_exit(ttlio_exit);

