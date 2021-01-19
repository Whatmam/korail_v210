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


#define DIP_SW_MAJOR 		249
#define DIP_SW_MINOR		0
#define DIP_SW_NAME     	"DIPSWITCH"

#define _IOCTL_READ_DIP_SWITCH		0x1000

u16 *hw_dip_sw_addr;

static int DIP_SW_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int DIP_SW_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t DIP_SW_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	return 0;
}

ssize_t DIP_SW_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int DIP_SW_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
#if defined(CONFIG_KORAIL_MAIN_BOARD)
	unsigned short dip_read_value = 0;
#else
	unsigned char dip_read_value = 0x00;
#endif
	
	switch(cmd)
	{

		case _IOCTL_READ_DIP_SWITCH:
#if defined(CONFIG_KORAIL_MAIN_BOARD)
			 dip_read_value =  __raw_readw(hw_dip_sw_addr);	

			 //printk(" DIP Switch 16 bit : %x \r\n", dip_read_value );
			
#else
			  dip_read_value =  __raw_readb(hw_dip_sw_addr);

			 //printk(" DIP Switch 8 bit : %x \r\n", dip_read_value );		 
#endif			  
			 copy_to_user((unsigned long*)arg , &dip_read_value, sizeof(dip_read_value));
			break;

		default:
			break;

	}
	
	return 0;
}


//#define DIV_Y0_DM9000		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 )
//#define DIV_Y1_DIP			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11)
//#define DIV_Y2_RFM			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12)
//#define DIV_Y3_RFMALE		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
//#define DIV_Y4_GATEIO		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_C_ADDR_13)
//#define DIV_Y5_FND2			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_C_ADDR_13)
//#define DIV_Y6_FND1			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
//#define DIV_Y7_HWID			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12 | DIV_C_ADDR_13)


static struct file_operations DIP_SW_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   DIP_SW_interface_read,
   .write      =   DIP_SW_interface_write,
   .open       =   DIP_SW_interface_open,
   .release    =   DIP_SW_interface_release,
   .ioctl      =   DIP_SW_interface_ioctl,
};

static int __init DIP_SW_interface_init(void)
{
  	int result;

    hw_dip_sw_addr = ioremap(DIV_Y1_DIP , 0x0040);

	result = register_chrdev(DIP_SW_MAJOR, DIP_SW_NAME, &DIP_SW_interface_fops);


 	//printk("%s Driver MAJOR %d\n", DIP_SW_NAME, DIP_SW_MAJOR );

	

	return 0;
}

static void __exit DIP_SW_interface_exit(void)
{
	unregister_chrdev(DIP_SW_MAJOR, DIP_SW_NAME);
	//printk("Release %s Module MAJOR %d\n", DIP_SW_NAME, DIP_SW_MAJOR);
}

module_init(DIP_SW_interface_init);
module_exit(DIP_SW_interface_exit);//mknod /dev/DIPSWITCH c 249 0 







