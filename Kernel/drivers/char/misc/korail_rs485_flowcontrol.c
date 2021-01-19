/*	@(#)sim.c	1.0a	2013/01/03	(SOLIDTEK)	*/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2007 Hybus co., Ltd
 * All Rights Reserved
 * 
 * 2012.11.08 HyBus Engineer MKJ Make or Change Source.
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

#define RS485_IF_MAJOR 		240
#define RS485_IF_NAME     	"RS485_FLOW"

#define _IOCTL_RS485_FLOW_READ		1000
#define _IOCTL_RS485_FLOW_WRITE		2000

static void RS485_Flow_Control_Pin( int nFlowLevel )
{
	if( nFlowLevel )
	{
		//gpio_request(S5PV210_GPA0(6), "GPA06");
		//gpio_direction_output(S5PV210_GPA0(6), 1);
	    //gpio_free(S5PV210_GPA0(6));

	    gpio_set_value( S5PV210_GPA0(6), 1);//s3c_gpio_setpin(S5PV210_GPA0(6), 1);
	}
	else
	{
		//gpio_request(S5PV210_GPA0(6), "GPA06");
		//gpio_direction_output(S5PV210_GPA0(6), 0);
	    //gpio_free(S5PV210_GPA0(6));

	    gpio_set_value( S5PV210_GPA0(6), 0);//s3c_gpio_setpin(S5PV210_GPA0(6), 0);
	}
}

static int RS485_IF_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int RS485_IF_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}    

ssize_t RS485_IF_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	return 0;
}

ssize_t RS485_IF_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int RS485_IF_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	u32 value;

	switch(cmd)
	{
		case _IOCTL_RS485_FLOW_READ:  
			RS485_Flow_Control_Pin(0);
			break;

		case _IOCTL_RS485_FLOW_WRITE: 
			RS485_Flow_Control_Pin(1);
			break;
			
		default: 
			break;
	}
	
	return 0;
}

static struct file_operations RS485_IF_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   RS485_IF_interface_read,
   .write      =   RS485_IF_interface_write,
   .open       =   RS485_IF_interface_open,
   .release    =   RS485_IF_interface_release,
   .ioctl      =   RS485_IF_interface_ioctl,
};

static int __init RS485_IF_interface_init(void)
{
  	int result; 

 	result = register_chrdev(RS485_IF_MAJOR, RS485_IF_NAME, &RS485_IF_interface_fops);
 	
 	//printk("%s Driver MAJOR %d\n", RS485_IF_NAME, RS485_IF_MAJOR );

	return 0;
}

static void __exit RS485_IF_interface_exit(void)
{
	unregister_chrdev(RS485_IF_MAJOR, RS485_IF_NAME);
	//printk("Release %s Module MAJOR %d\n", RS485_IF_NAME, RS485_IF_MAJOR);
}

module_init(RS485_IF_interface_init);
module_exit(RS485_IF_interface_exit);//mknod /dev/RS485_FLOW c 240 0
