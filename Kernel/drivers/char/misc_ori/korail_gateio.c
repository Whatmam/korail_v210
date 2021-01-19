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

#define GATE_IO_MAJOR 		241
#define GATE_IO_MINOR		0
#define GATE_IO_NAME     	"GATE_IO"

u16 *hw_gateio_y0_gmode1;
u16 *hw_gateio_y1_gmode2;
u16 *hw_gateio_y2_gmode3;
u16 *hw_gateio_y3_gmode4;
u16 *hw_gateio_y4_gsel1;
u16 *hw_gateio_y5_gsel2;
u16 *hw_gateio_y6_gsel3;
u16 *hw_gateio_y7_gspare;

static int GATE_IO_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int GATE_IO_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}  

ssize_t GATE_IO_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	char hxGate_IOW_buffer[2];
	
	memset( hxGate_IOW_buffer, 0x00, sizeof(hxGate_IOW_buffer) );
		
	copy_from_user( &hxGate_IOW_buffer, buf, 2);
	
	printk("[KERN] smk test print buffer0 : 0x%x %d\r\n", hxGate_IOW_buffer[0], hxGate_IOW_buffer[0]);

	switch( hxGate_IOW_buffer[0] )
	{
		case 0:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y0_gmode1);
			break;
		case 1:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y1_gmode2);
			break;
		case 2:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y2_gmode3);
			break;
		case 3:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y3_gmode4);
			break;
		case 4:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y4_gsel1);
			break;
		case 5:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y5_gsel2);
			break;
		case 6:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y6_gsel3);
			break;
		case 7:
			__raw_writeb(hxGate_IOW_buffer[1], hw_gateio_y7_gspare);
			break;
		default:
			break;		
	}
	
	return 0;
}


ssize_t GATE_IO_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char hxGate_IOR_buffer[2];
	
	memset( hxGate_IOR_buffer, 0x00, sizeof(hxGate_IOR_buffer) );
	
	copy_from_user( &hxGate_IOR_buffer, buf, 2);
	
	switch( hxGate_IOR_buffer[0] )
	{
		case 0:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y0_gmode1);
			break;
		case 1:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y1_gmode2);
			break;
		case 2:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y2_gmode3);
			break;
		case 3:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y3_gmode4);
			break;
		case 4:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y4_gsel1);
			break;
		case 5:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y5_gsel2);
			break;
		case 6:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y6_gsel3);
			break;
		case 7:
			hxGate_IOR_buffer[1] =  __raw_readb(hw_gateio_y7_gspare);
			break;
		default:
			break;		
	}

	copy_to_user( buf , &hxGate_IOR_buffer, 2);
	
	return 0;
}

int GATE_IO_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static struct file_operations GATE_IO_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   GATE_IO_interface_read,
   .write      =   GATE_IO_interface_write,
   .open       =   GATE_IO_interface_open,
   .release    =   GATE_IO_interface_release,
   .ioctl      =   GATE_IO_interface_ioctl,
};

static int __init GATE_IO_interface_init(void)
{
  	int result;

		hw_gateio_y0_gmode1 = ioremap(KORAIL_GATEIO_Y0_GMODE1 , 0x0040);
		hw_gateio_y1_gmode2 = ioremap(KORAIL_GATEIO_Y1_GMODE2 , 0x0040);
		hw_gateio_y2_gmode3 = ioremap(KORAIL_GATEIO_Y2_GMODE3 , 0x0040);
		hw_gateio_y3_gmode4 = ioremap(KORAIL_GATEIO_Y3_GMODE4 , 0x0040);
		hw_gateio_y4_gsel1 = ioremap(KORAIL_GATEIO_Y4_GSEL1 , 0x0040);
		hw_gateio_y5_gsel2 = ioremap(KORAIL_GATEIO_Y5_GSEL2 , 0x0040);
		hw_gateio_y6_gsel3 = ioremap(KORAIL_GATEIO_Y6_GSEL3 , 0x0040);
		hw_gateio_y7_gspare = ioremap(KORAIL_GATEIO_Y7_GSPARE , 0x0040);

	result = register_chrdev(GATE_IO_MAJOR, GATE_IO_NAME, &GATE_IO_interface_fops);

 	//printk("%s Driver MAJOR %d\n", GATE_IO_NAME, GATE_IO_MAJOR );	

	return 0;
}

static void __exit GATE_IO_interface_exit(void)
{
	unregister_chrdev(GATE_IO_MAJOR, GATE_IO_NAME);
	//printk("Release %s Module MAJOR %d\n", GATE_IO_NAME, GATE_IO_MAJOR);
}

module_init(GATE_IO_interface_init);
module_exit(GATE_IO_interface_exit);//mknod /dev/GATE_IO c 241 0 







