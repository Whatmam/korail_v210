/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2007 Hybus co., Ltd
 * All Rights Reserved
 * 
 */

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

#include <asm/io.h>
#include <asm/delay.h>
#include <asm/irq.h>

#include <mach/map.h>
#include <mach/gpio.h>

#include <asm/gpio.h>
#include <mach/regs-gpio.h>


#define HW_ID_MAJOR 		248
#define HW_ID_MINOR		0
#define HW_ID_NAME     	"HW_ID"

#define _IOCTL_READ_HW_ID		0x1000

#if defined(CONFIG_KORAIL_SPINLOCK)
#include <linux/spinlock.h>
        static spinlock_t mr_lock = SPIN_LOCK_UNLOCKED;
        static unsigned long flags;
#endif

u16 *hw_id_addr;

static int HW_ID_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int HW_ID_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t HW_ID_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	return 0;
}

ssize_t HW_ID_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int HW_ID_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned char strValue = 0x00;

	switch(cmd)
	{

		case _IOCTL_READ_HW_ID:

#if defined(CONFIG_KORAIL_SPINLOCK)
			spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
			strValue =  __raw_readb(hw_id_addr);
#if defined(CONFIG_KORAIL_SPINLOCK)
			spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)

			 //printk(" HW ID char : %x \r\n", strValue );
			  
			 copy_to_user((unsigned long*)arg , &strValue, sizeof(strValue));
			break;

		default:
			break;

	}
	
	return 0;
}
int HW_ID_Check(void){
	unsigned char hwid_val;

	hwid_val = __raw_readb(hw_id_addr);
	return hwid_val;
}EXPORT_SYMBOL(HW_ID_Check);

//#define DIV_Y0_DM9000		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 )
//#define DIV_Y1_DIP			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11)
//#define DIV_Y2_RFM			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12)
//#define DIV_Y3_RFMALE		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
//#define DIV_Y4_GATEIO		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_C_ADDR_13)
//#define DIV_Y5_FND2			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_C_ADDR_13)
//#define DIV_Y6_FND1			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
//#define DIV_Y7_HWID			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12 | DIV_C_ADDR_13)

static int tmp_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
        return 0;
}
static int tmp_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos){
        return 0;
}

static struct file_operations HW_ID_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   HW_ID_interface_read,
   //.read       =   tmp_read,
   //.write      =   tmp_write,
   .write      =   HW_ID_interface_write,
   .open       =   HW_ID_interface_open,
   .release    =   HW_ID_interface_release,
   .ioctl      =   HW_ID_interface_ioctl,
};

static int __init HW_ID_interface_init(void)
{
  	int result;

    hw_id_addr = ioremap(DIV_Y7_HWID , 0x0040);

	result = register_chrdev(HW_ID_MAJOR, HW_ID_NAME, &HW_ID_interface_fops);


 	//printk("%s Driver MAJOR %d\n", HW_ID_NAME, HW_ID_MAJOR );

	

	return 0;
}

static void __exit HW_ID_interface_exit(void)
{
	unregister_chrdev(HW_ID_MAJOR, HW_ID_NAME);
	//printk("Release %s Module MAJOR %d\n", HW_ID_NAME, HW_ID_MAJOR);
}

module_init(HW_ID_interface_init);
module_exit(HW_ID_interface_exit);//mknod /dev/HW_ID c 248 0 







