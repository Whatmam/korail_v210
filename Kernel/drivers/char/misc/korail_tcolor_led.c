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

#define TCOLOR_LED_MAJOR 		247
#define TCOLOR_LED_MINOR		0
#define TCOLOR_LED_NAME     	"TLED"

#define _IOCTL_TCOLOR_LED_RED		0x1000
#define _IOCTL_TCOLOR_LED_GREEN		0x1001
#define _IOCTL_TCOLOR_LED_BLUE		0x1010
#define _IOCTL_TCOLOR_LED_RG		0x1100
#define _IOCTL_TCOLOR_LED_RB		0x2000
#define _IOCTL_TCOLOR_LED_GB		0x2001
#define _IOCTL_TCOLOR_LED_RGB		0x2010
#define _IOCTL_TCOLOR_LED_ROFF		0x2100
#define _IOCTL_TCOLOR_LED_GOFF		0x3000
#define _IOCTL_TCOLOR_LED_BOFF		0x3001
#define _IOCTL_TCOLOR_LED_ALLOFF	0x3010

void TCOLOR_CTL_RED( int nOnOFFlag );
void TCOLOR_CTL_GREEN( int nOnOFFlag );
void TCOLOR_CTL_BLUE( int nOnOFFlag );

void TCOLOR_CTL_RED( int nOnOFFlag )
{
	if( nOnOFFlag )
	{
		//gpio_request(S5PV210_GPG3(5), "GPH35");
		//gpio_direction_output(S5PV210_GPG3(5), 1);
	    //gpio_free(S5PV210_GPG3(5));

	    gpio_set_value( S5PV210_GPG3(5), 1);//s3c_gpio_setpin(S5PV210_GPG3(5), 1);
	}
	else
	{
		//gpio_request(S5PV210_GPG3(5), "GPH35");
		//gpio_direction_output(S5PV210_GPG3(5), 0);
	    //gpio_free(S5PV210_GPG3(5));

	    gpio_set_value( S5PV210_GPG3(5), 0);//s3c_gpio_setpin(S5PV210_GPG3(5), 0);
	}
}

void TCOLOR_CTL_GREEN( int nOnOFFlag )
{
	if( nOnOFFlag )
	{
		//gpio_request(S5PV210_GPG3(4), "GPH34");
		//gpio_direction_output(S5PV210_GPG3(4), 1);
	    //gpio_free(S5PV210_GPG3(4));

	    gpio_set_value( S5PV210_GPG3(4), 1);//s3c_gpio_setpin(S5PV210_GPG3(4), 1);
	}
	else
	{
		//gpio_request(S5PV210_GPG3(4), "GPH34");
		//gpio_direction_output(S5PV210_GPG3(4), 0);
	    //gpio_free(S5PV210_GPG3(4));

	    gpio_set_value( S5PV210_GPG3(4), 0);//s3c_gpio_setpin(S5PV210_GPG3(4), 0);
	}
}

void TCOLOR_CTL_BLUE( int nOnOFFlag )
{
	if( nOnOFFlag )
	{
		//gpio_request(S5PV210_GPG3(3), "GPH33");
		//gpio_direction_output(S5PV210_GPG3(3), 1);
	    //gpio_free(S5PV210_GPG3(3));

	    gpio_set_value( S5PV210_GPG3(3), 1);//s3c_gpio_setpin(S5PV210_GPG3(3), 1);
	}
	else
	{
		//gpio_request(S5PV210_GPG3(3), "GPH33");
		//gpio_direction_output(S5PV210_GPG3(3), 0);
	    //gpio_free(S5PV210_GPG3(3));

	    gpio_set_value( S5PV210_GPG3(3), 0);//s3c_gpio_setpin(S5PV210_GPG3(3), 0);
	}
}

static int TCOLOR_LED_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int TCOLOR_LED_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t TCOLOR_LED_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	return 0;
}

ssize_t TCOLOR_LED_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int TCOLOR_LED_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{

		case _IOCTL_TCOLOR_LED_RED:
			TCOLOR_CTL_RED(1);
			TCOLOR_CTL_GREEN(0);
			TCOLOR_CTL_BLUE(0);
			break;
			
		case _IOCTL_TCOLOR_LED_GREEN:
			TCOLOR_CTL_RED(0);
			TCOLOR_CTL_GREEN(1);
			TCOLOR_CTL_BLUE(0);
			break;
			
		case _IOCTL_TCOLOR_LED_BLUE:
			TCOLOR_CTL_RED(0);
			TCOLOR_CTL_GREEN(0);
			TCOLOR_CTL_BLUE(1);
			break;

		case _IOCTL_TCOLOR_LED_RG:
			TCOLOR_CTL_RED(1);
			TCOLOR_CTL_GREEN(1);
			TCOLOR_CTL_BLUE(0);			
			break;

		case _IOCTL_TCOLOR_LED_RB:
			TCOLOR_CTL_RED(1);
			TCOLOR_CTL_GREEN(0);
			TCOLOR_CTL_BLUE(1); 
			break;

		case _IOCTL_TCOLOR_LED_GB:
			TCOLOR_CTL_RED(0);
			TCOLOR_CTL_GREEN(1);
			TCOLOR_CTL_BLUE(1); 
			break;

		case _IOCTL_TCOLOR_LED_RGB:
			TCOLOR_CTL_RED(1);
			TCOLOR_CTL_GREEN(1);
			TCOLOR_CTL_BLUE(1); 
			break;

		case _IOCTL_TCOLOR_LED_ROFF:
			TCOLOR_CTL_RED(0);
			break;
			
		case _IOCTL_TCOLOR_LED_GOFF:
			TCOLOR_CTL_GREEN(0);
			break;
			
		case _IOCTL_TCOLOR_LED_BOFF:
			TCOLOR_CTL_BLUE(0); 
			break;
			
		case _IOCTL_TCOLOR_LED_ALLOFF:
			TCOLOR_CTL_RED(0);
			TCOLOR_CTL_GREEN(0);
			TCOLOR_CTL_BLUE(0); 
			break;

		default:
			break;

	}
	
	return 0;
}

static struct file_operations TCOLOR_LED_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   TCOLOR_LED_interface_read,
   .write      =   TCOLOR_LED_interface_write,
   .open       =   TCOLOR_LED_interface_open,
   .release    =   TCOLOR_LED_interface_release,
   .ioctl      =   TCOLOR_LED_interface_ioctl,
};

static int __init TCOLOR_LED_interface_init(void)
{
  	int result;

	result = register_chrdev(TCOLOR_LED_MAJOR, TCOLOR_LED_NAME, &TCOLOR_LED_interface_fops);

 	//printk("%s Driver MAJOR %d\n", TCOLOR_LED_NAME, TCOLOR_LED_MAJOR );	

	return 0;
}

static void __exit TCOLOR_LED_interface_exit(void)
{
	unregister_chrdev(TCOLOR_LED_MAJOR, TCOLOR_LED_NAME);
	//printk("Release %s Module MAJOR %d\n", TCOLOR_LED_NAME, TCOLOR_LED_MAJOR);
}

module_init(TCOLOR_LED_interface_init);
module_exit(TCOLOR_LED_interface_exit);//mknod /dev/TLED c 247 0 


