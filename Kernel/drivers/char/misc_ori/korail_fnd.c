/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2007 Hybus co., Ltd
 * All Rights Reserved
 * 
 *			    A	
 *			--------            			  bit  |	7	|	6	|	5	|	4	|	3	|	2	|	1	|	0	|
 *          |      |              			  -----------------------------------------------------------------------    
 *          |      |     					  pin  |	DP	|	G	|	F	|	E	|	D	|	C	|	B	|	A	|
 *        F |      | B                        -----------------------------------------------------------------------
 *			|   G  |            bit Semantic Value |	8	|	4	|	2	|	1	|	8	|	4	|	2	|	1	|
 *          --------              			  -----------------------------------------------------------------------
 *          |      |            			  -----------------------------------------------------------------------	
 *          |      |                        ex (2) |	0	|	1	|	0	|	1	|	1	|	0	|	1	|	1	|	=> 0x5B
 *        E |      | C						  -----------------------------------------------------------------------
 *			|      |                          -----------------------------------------------------------------------
 *          --------	o                  ex (2.) |	1	|	1	|	0	|	1	|	1	|	0	|	1	|	1	|	=> 0xDB
 *              D        DP
 *	
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

#define FND_MAJOR 			245
#define FND_MINOR			0
#define FND_NAME     		"FND"

#define _IOCTL_READ_FND_		0x1000
#define _IOCTL_WRITE_FND_		0x2000

#define _IOCTL_FND_DP_0_MASK_			0x8000
#define _IOCTL_FND_DP_1_MASK_			0x0080
#define _IOCTL_FND_STR_0_MASK_			0x7F00
#define _IOCTL_FND_STR_1_MASK_			0x007F

u16 *fnd_one_addr;
u16 *fnd_two_addr;

#if 0
unsigned long FND_strMake( char* strParserBuffer, int nStrBufferSize  )
{
	char strSaveAsBuffer[5];

	char strChangeReturnBuffer[5];

	memset( strSaveAsBuffer, 0x00, sizeof( strSaveAsBuffer ) );

	memcpy( strSaveAsBuffer, strParserBuffer, nStrBufferSize );

	return 0;	
}

void FND_strParser( unsigned long ld_strParser )
{
	unsigned short ld_fnd_one_parserdata = 0;
	unsigned short ld_fnd_two_parserdata = 0;

	char strFND_FirstData = 0x00;
	char strFND_SecondData = 0x00;
	char strFND_ThirdData = 0x00;
	char strFND_ForthData = 0x00;

	ld_fnd_one_parserdata = (unsigned short)(( ld_strParser & 0xFFFF0000 ) >> 16);
	ld_fnd_two_parserdata = (unsigned short)( ld_strParser & 0x0000FFFF );

	strFND_FirstData = (char)(( ld_fnd_one_parserdata & _IOCTL_FND_STR_0_MASK_ ) >> 8 );
	strFND_SecondData = (char)( ld_fnd_one_parserdata & _IOCTL_FND_STR_1_MASK_ );

	strFND_ThirdData = (char)(( ld_fnd_two_parserdata & _IOCTL_FND_STR_0_MASK_ ) >> 8 );
	strFND_ForthData = (char)( ld_fnd_two_parserdata & _IOCTL_FND_STR_1_MASK_ );
	

	if( ld_fnd_one_parserdata & _IOCTL_FND_DP_0_MASK_ )
	{
		printk("%c.", strFND_FirstData);		
	}
	else
	{
		printk("%c", strFND_FirstData);
	}

	if( ld_fnd_one_parserdata & _IOCTL_FND_DP_1_MASK_ )
	{
		printk("%c.", strFND_SecondData);		
	}
	else
	{
		printk("%c", strFND_SecondData);
	}

	if( ld_fnd_two_parserdata & _IOCTL_FND_DP_0_MASK_ )
	{
		printk("%c.", strFND_ThirdData);		
	}
	else
	{
		printk("%c", strFND_ThirdData);
	}

	if( ld_fnd_two_parserdata & _IOCTL_FND_DP_1_MASK_ )
	{
		printk("%c.", strFND_ForthData);		
	}
	else
	{
		printk("%c", strFND_ForthData);
	}	
}
#endif

static int FND_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int FND_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t FND_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	char fnd_strSave_buffer[4];
	
	unsigned short ld_fnd_0_sromdata = 0;
	unsigned short ld_fnd_1_sromdata = 0;
	
	memset( fnd_strSave_buffer, 0x00, sizeof(fnd_strSave_buffer) );

	copy_from_user( fnd_strSave_buffer, buf, sizeof(length));

	ld_fnd_0_sromdata = ( fnd_strSave_buffer[0] << 8 ) | fnd_strSave_buffer[1];
	ld_fnd_1_sromdata = ( fnd_strSave_buffer[2] << 8 ) | fnd_strSave_buffer[3];

	__raw_writew(ld_fnd_0_sromdata, fnd_two_addr);
	__raw_writew(ld_fnd_1_sromdata, fnd_one_addr);	
	
	return 0;
}

ssize_t FND_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char fnd_strSave_buffer[4];
	
	unsigned short ld_fnd_0_sromdata = 0;
	unsigned short ld_fnd_1_sromdata = 0;

	memset( fnd_strSave_buffer, 0x00, sizeof(fnd_strSave_buffer) );

	ld_fnd_0_sromdata =  __raw_readw(fnd_two_addr);
	ld_fnd_1_sromdata =  __raw_readw(fnd_one_addr);

	fnd_strSave_buffer[0] = (char)( ld_fnd_0_sromdata & 0x00FF ); 
	fnd_strSave_buffer[1] = (char)( ( ld_fnd_0_sromdata & 0xFF00 ) >> 8 ); 

	fnd_strSave_buffer[2] = (char)( ld_fnd_1_sromdata & 0x00FF ); 
	fnd_strSave_buffer[3] = (char)( ( ld_fnd_1_sromdata & 0xFF00 ) >> 8 );

	copy_to_user( buf , fnd_strSave_buffer, sizeof(fnd_strSave_buffer));
	
	return 0;
}


int FND_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long ld_ioctl_read_data = 0;
	unsigned long ld_ioctl_write_data = 0;
	
	switch(cmd)
	{

		case _IOCTL_READ_FND_:
			 copy_to_user((unsigned long*)arg , &ld_ioctl_read_data, sizeof(ld_ioctl_read_data)); // __raw_readw();			 
			break;

		case _IOCTL_WRITE_FND_:
			copy_from_user(&ld_ioctl_write_data, arg, sizeof(ld_ioctl_write_data)); // __raw_writew();
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


static struct file_operations FND_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   FND_interface_read,
   .write      =   FND_interface_write,
   .open       =   FND_interface_open,
   .release    =   FND_interface_release,
   .ioctl      =   FND_interface_ioctl,
};

static int __init FND_interface_init(void)
{
  	int result;

    fnd_one_addr = ioremap(DIV_Y6_FND1 , 0x0040);

    fnd_two_addr = ioremap(DIV_Y5_FND2 , 0x0040);

	result = register_chrdev(FND_MAJOR, FND_NAME, &FND_interface_fops);

	__raw_writew( 0xFFFF, fnd_one_addr );
	__raw_writew( 0xFFFF, fnd_two_addr );	

 	//printk("%s Driver MAJOR %d\n", FND_NAME, FND_MAJOR );	

	return 0;
}

static void __exit FND_interface_exit(void)
{
	unregister_chrdev(FND_MAJOR, FND_NAME);
	//printk("Release %s Module MAJOR %d\n", FND_NAME, FND_MAJOR);
}

module_init(FND_interface_init);
module_exit(FND_interface_exit);//mknod /dev/FND c 245 0 







