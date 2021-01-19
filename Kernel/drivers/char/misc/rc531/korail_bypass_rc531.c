//	@(#)korail_rc531_bypass.c	1.0g	2015/08/13	(SOLIDTEK)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/delay.h>

#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/ioport.h>

#include <linux/interrupt.h>
#include <linux/input.h>

#include <asm/io.h>
#include <asm/delay.h>
#include <asm/irq.h>
#include <asm/gpio.h>
#include <asm/uaccess.h>

#include <mach/map.h>
#include <mach/gpio.h>

#include <asm/gpio.h>
#include <mach/regs-gpio.h>

#include <plat/gpio-cfg.h>

#include <linux/soundcard.h>
#include <linux/spinlock.h>

#include "rfio.h"

#define RC531_INTER_GPIO	S5PV210_GPH1(3)
#define GetRegPage(adr)     	(0x80 | (adr>>3))

volatile byte *rc531_base_cs;

#define RC531_ADDR_PTR		1
#define RC531_BASE		0xE0842800

static spinlock_t rf_lock = SPIN_LOCK_UNLOCKED;
static unsigned long flags;

static int	oldPage = 0;
static int	isrFlag = 0;

void rfint_enable(void)
{
	enable_irq(IRQ_EINT11);
}

void rfint_disable(void)
{
	disable_irq(IRQ_EINT11);
}

byte rfraw_read(byte addr)
{
	byte data;

	spin_lock_irqsave(&rf_lock, flags);
	data = __raw_readb(rc531_base_cs + (addr << 1));
	spin_unlock_irqrestore(&rf_lock, flags);
	return(data);
}

void rfraw_write(byte addr, byte data)
{
	spin_lock_irqsave(&rf_lock, flags);
	__raw_writeb(data, rc531_base_cs+(addr << 1));
	spin_unlock_irqrestore(&rf_lock, flags);
}

byte rfreg_read(byte addr)
{
	int 	newpage;
	byte 	data;
	
	spin_lock_irqsave(&rf_lock, flags);
	newpage = addr / 8;
	if (oldPage == newpage) {
		data = __raw_readb(rc531_base_cs+(addr << 1));
		spin_unlock_irqrestore(&rf_lock, flags);
		return(data);
	}
	oldPage = newpage;
	__raw_writeb(GetRegPage(addr), rc531_base_cs);	
	data = __raw_readb(rc531_base_cs+(addr << 1));
	spin_unlock_irqrestore(&rf_lock, flags);
	return(data);
}

void rfreg_write(byte addr, byte data)
{
	int 	newpage;
	
	spin_lock_irqsave(&rf_lock, flags);
	newpage = addr / 8;
	if (oldPage != newpage) {
		oldPage = newpage;
		__raw_writeb(GetRegPage(addr), rc531_base_cs);
	}
	__raw_writeb(data, rc531_base_cs+(addr << 1));
	spin_unlock_irqrestore(&rf_lock, flags);
}

void rfreg_setbit(byte addr, byte mask)
{
	byte data;

	data = rfreg_read(addr);
	data |= mask;
	rfreg_write(addr, data);
}

void rfreg_clrbit(byte addr, byte mask)
{
	byte data;
	
	data = rfreg_read(addr);
	data &= ~mask;
	rfreg_write(addr, data);
}

void rfpin_setbit(byte data)
{
	if (data)
		gpio_set_value( S5PV210_GPG1(2), 1);
	else
		gpio_set_value( S5PV210_GPG1(2), 0);
}

static int RF_RC531_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int RF_RC531_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t RF_RC531_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	byte wbuf[8];	
	byte cmd;
	byte addr;
	byte data;

	copy_from_user(wbuf, buf, length);	
	
	cmd = wbuf[0];
	addr = wbuf[1];
	data = wbuf[2];

	switch (cmd) {
		case RC531_WRD_FLG_WR :
			memcpy(&isrFlag, &wbuf[1], sizeof(int));
			break;
		case RC531_WRD_RAW_WR :
			rfraw_write(addr, data); 
			break;
		case RC531_WRD_REG_WR :
			rfreg_write(addr, data);
			break;
		case RC531_WRD_CLR_BV :
			rfreg_clrbit(addr, data);
			break;
		case RC531_WRD_SET_BV :
			rfreg_setbit(addr, data);
			break;
		default : break;
	}
	return length;
}

ssize_t RF_RC531_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	byte rbuf[4];
	byte cmd;
	byte addr;

	copy_from_user(rbuf, buf, count);
	
	cmd = rbuf[0];
	addr = rbuf[1];

	switch (cmd) {
		case RC531_WRD_FLG_RD :
			copy_to_user(buf, (byte *)&isrFlag, sizeof(int));
			break;
		case RC531_WRD_RAW_RD :
			rbuf[0] = rfraw_read(addr);
			rbuf[1] = 'R';
			copy_to_user(buf, rbuf, 2);
			break;
		case RC531_WRD_REG_RD :
			rbuf[0] = rfreg_read(addr);
			rbuf[1] = 'R';
			copy_to_user(buf, rbuf, 2);
			break;
		default : return count;
	}
	return count;
}

int RF_RC531_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int	err = 0;
	byte	rbuf[4];

	switch (cmd) {
		case RC531_IOC_INT_EN :
			rfint_enable();	
			break;
		case RC531_IOC_INT_DI :
			rfint_disable();
			break;
		case RC531_IOC_FLG_WR :
			copy_from_user((byte *)&isrFlag, (byte *)arg, sizeof(int));
			break;
		case RC531_IOC_FLG_RD :
			copy_to_user((byte *)arg, (byte *)&isrFlag, sizeof(int));
			break;
		case RC531_IOC_RAW_WR :
			copy_from_user(rbuf, (byte *)arg, 2);
			rfraw_write(rbuf[0], rbuf[1]);	// addr, data
			break;
		case RC531_IOC_RAW_RD :
			copy_from_user(rbuf, (byte *)arg, 1);
			rbuf[1] = rfraw_read(rbuf[0]);	// data = rfraw_read(addr)
			copy_to_user((byte *)arg, &rbuf[1], 1); 
			break;
		case RC531_IOC_REG_WR :
			copy_from_user(rbuf, (byte *)arg, 2);
			rfreg_write(rbuf[0], rbuf[1]);	// addr, data
			break;
		case RC531_IOC_REG_RD :
			copy_from_user(rbuf, (byte *)arg, 1);
			rbuf[1] = rfreg_read(rbuf[0]);	// data = rfreg_read(addr)
			copy_to_user((byte *)arg, &rbuf[1], 1);
			break;
		case RC531_IOC_CLR_BV :
			copy_from_user(rbuf, (byte *)arg, 2);
			rfreg_clrbit(rbuf[0], rbuf[1]);
			break;
		case RC531_IOC_SET_BV :
			copy_from_user(rbuf, (byte *)arg, 2);
			rfreg_setbit(rbuf[0], rbuf[1]);
			break;
		case RC531_IOC_RST_HI :
			rfpin_setbit(1);
			break;
		case RC531_IOC_RST_LO :
			rfpin_setbit(0);
			break;
		default:
			err = -EFAULT;
		    	break; 
	} 

	return err;  
}

irqreturn_t RC531_ISR_FUNC(int irq, void *dev_id)
{
	isrFlag++;
	return IRQ_HANDLED;
}

static struct file_operations RF_RC531_interface_fops = {
	.owner      =   THIS_MODULE,
	.read       =   RF_RC531_interface_read,
	.write      =   RF_RC531_interface_write,
	.open       =   RF_RC531_interface_open,
	.release    =   RF_RC531_interface_release,
	.ioctl      =   RF_RC531_interface_ioctl,
};


int __init rc531_reader_init(void)
{
	int err , ret;

	rc531_base_cs = ioremap(DIV_Y3_RFMALE , 0x0040);
	
	/* register device */
	if ((err = register_chrdev(READER_MAJOR, READER_NAME, &RF_RC531_interface_fops)) != 0)
	{
		/* log error and fail */
		printk(READER_NAME ": unable to register major device %d\n", READER_MAJOR);
		return(err);
	}
#if 1
	ret = request_irq(IRQ_EINT11, RC531_ISR_FUNC, IRQF_TRIGGER_FALLING, READER_NAME, NULL); 
#else
	ret = request_irq(IRQ_EINT11, RC531_ISR_FUNC, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
	                  READER_NAME, NULL); 
#endif
	if (ret) 
	{
		printk(READER_NAME": couldn't allocate IRQ (errno %d)\n", ret);
		return -EBUSY;
	}

	/* log device registration */
	printk("MF RC531 driver is registered with Major Number =%d \n", READER_MAJOR);

	/* everything OK */
	return 0;
}

void __exit rc531_reader_exit(void)
{
	/* log device unload */
	//printk(READER_NAME " unloading module\n");
	free_irq(IRQ_EINT11, NULL);
	//gpio_free(S5PV210_GPG1(2)); // rf reset pin free	
	//gpio_free(S5PV210_GPH1(3));

	/* unregister driver */
	unregister_chrdev(READER_MAJOR, READER_NAME);
}

module_init(rc531_reader_init);
module_exit(rc531_reader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HSKIM <hyungsik.kim@gmail.com>");
MODULE_DESCRIPTION("MFRC531 Contactless Card Reader driver for S5PV210");
