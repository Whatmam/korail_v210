/*	@(#)sim.c	1.0b	2014/07/04	(SOLIDTEK)	*/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2007 Hybus co., Ltd
 * All Rights Reserved
 * 
 * 2012.11.08 HyBus Engineer MKJ Make or Change Source.
 * 2014.07.04 SOLIDTEK@hskim Changed.
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

#include <linux/err.h>
#include <mach/pd.h>
#include <plat/regs-timer.h>

#include "korail_sim.h"

#define SIM_IF_MAJOR 		244
#define SIM_IF_NAME     	"SAM_IF"

//latch_2
#define DA_SIM_SEL_0		0
#define DA_SIM_SEL_1		1
#define DA_SIM_SEL_2		2
#define DA_NE_TXB		3
#define DA_NE_RXB		4
#define DA_SAM_SEL_1		5
#define DA_SAM_SEL_0		6
#define DA_NE_SAM		7

//latch_0
#define DA_SIM_1_PWR		0
#define DA_SIM_1_RST		1
#define DA_SIM_2_PWR		2
#define DA_SIM_2_RST		3
#define DA_SIM_3_PWR		4
#define DA_SIM_3_RST		5
#define DA_SIM_4_PWR		6
#define DA_SIM_4_RST		7

//latch_1
#define DA_SIM_5_PWR		0
#define DA_SIM_5_RST		1
#define DA_SIM_6_PWR		2
#define DA_SIM_6_RST		3
#define DA_SIM_7_PWR		4
#define DA_SIM_7_RST		5
#define DA_SIM_8_PWR		6
#define DA_SIM_8_RST		7

#if defined(CONFIG_KORAIL_SPINLOCK)
#include <linux/spinlock.h>
        static spinlock_t mr_lock = SPIN_LOCK_UNLOCKED;
        static unsigned long flags;
#endif

u32 *korail_uart_ncsa_addr;  
u32 *korail_uart_ncsb_addr; 
u32 *korail_simsam_latch0_addr;  
u32 *korail_simsam_latch1_addr;
u32 *korail_simsam_latch2_addr;

unsigned int nSIM_IF_FlowNumber;
unsigned int latchData[3];

static int SIM_IF_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int SIM_IF_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}    

ssize_t SIM_IF_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	char hxSIM_IF_WriteBuffer[100];
	char hxSIM_IF_WriteData = 0x00;

	int nSIM_IF_WriteCount = 0;

	int nSIM_IF_WriteLoop = 0;

	u32 *korail_simsam_w_addr = NULL;

	memset( hxSIM_IF_WriteBuffer, 0x00, sizeof(hxSIM_IF_WriteBuffer) );
	
	copy_from_user( hxSIM_IF_WriteBuffer, buf, sizeof(length));

	nSIM_IF_WriteCount = length;

	switch( nSIM_IF_FlowNumber )
	{
		case _IOCTL_SIM1_WRITE_MODE:
		case _IOCTL_SIM2_WRITE_MODE:
		case _IOCTL_SIM3_WRITE_MODE:
		case _IOCTL_SIM4_WRITE_MODE:
		case _IOCTL_SIM5_WRITE_MODE:
		case _IOCTL_SIM6_WRITE_MODE:
		case _IOCTL_SIM7_WRITE_MODE:
		case _IOCTL_SIM8_WRITE_MODE:
			korail_simsam_w_addr = korail_uart_ncsb_addr;
			break;
		case _IOCTL_SAM_1_UART_ENABLE:
		case _IOCTL_SAM_2_UART_ENABLE:
			korail_simsam_w_addr = korail_uart_ncsa_addr;
			break;
		default:
			break;
	}

	for( nSIM_IF_WriteLoop = 0; nSIM_IF_WriteLoop < nSIM_IF_WriteCount; nSIM_IF_WriteLoop++ )
	{	
		hxSIM_IF_WriteData = hxSIM_IF_WriteBuffer[nSIM_IF_WriteLoop];
#if defined(CONFIG_KORAIL_SPINLOCK)
		spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
		__raw_writeb( hxSIM_IF_WriteData, korail_simsam_w_addr );
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
	}

	printk("[ SIM Kernel Write %x ] \r\n", hxSIM_IF_WriteData );
	
	return 0;
}

ssize_t SIM_IF_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	char hxSIM_IF_ReadData = 0x00;

	u32 *korail_simsam_r_addr = NULL;

	switch( nSIM_IF_FlowNumber )
	{
		case _IOCTL_SIM1_READ_MODE:
		case _IOCTL_SIM2_READ_MODE:
		case _IOCTL_SIM3_READ_MODE:
		case _IOCTL_SIM4_READ_MODE:
		case _IOCTL_SIM5_READ_MODE:
		case _IOCTL_SIM6_READ_MODE:
		case _IOCTL_SIM7_READ_MODE:
		case _IOCTL_SIM8_READ_MODE:
			korail_simsam_r_addr = korail_uart_ncsb_addr;
			break;
		case _IOCTL_SAM_1_UART_ENABLE:
		case _IOCTL_SAM_2_UART_ENABLE:
			korail_simsam_r_addr = korail_uart_ncsa_addr;
			break;
		default:
			break;
	}
	
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
	hxSIM_IF_ReadData = __raw_readb( korail_simsam_r_addr );
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)

	printk("[ SIM Kernel Read %x ] \r\n", hxSIM_IF_ReadData );
	
	return 0;
}

void SIM_IF_interface_rw_mode( u32 *addr, int slot, int wmode, int rmode)
{
	
	unsigned char writedat;

	if (slot == 0 || slot > 10)
		return;

	switch (slot) {
		case 1:
			writedat = 0xf8;			
			break;
		case 2:
			writedat = 0xf9;
			break;
		case 3:
			writedat = 0xfa;
			break;
		case 4:
			writedat = 0xfb;
			break;
		case 5:
			writedat = 0xfc;
			break;
		case 6:
			writedat = 0xfd;
			break;
		case 7:
			writedat = 0xfe;
			break;
		case 8:
			writedat = 0xff;
			break;
		case 9:
			writedat = 0x9f;
			break;
		case 10:
			writedat = 0xbf;
			break;
		default: return 0;
	}

	if (slot > 8) 
		writedat &= 0x7f;	 	// plcc rx/tx mode
	else {
		if (!wmode){
			writedat &= 0xf7;	// tx mode
			udelay(100);
		}
		if (!rmode)
			writedat &= 0xef;	// rx mode
	}
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
	__raw_writeb((latchData[2] | 0x98), addr );	// sim tx, rx and plcc_en disable 
	__raw_writeb((writedat | 0x98), addr ); 	// sim or plcc select signal
	__raw_writeb( writedat , addr );		// sim tx, rx or plcc_en enable 
	latchData[2] = writedat;
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
}

void SIM_IF_interface_nE_set(int rmode, int wmode){
	u32 tmp;
	__raw_writeb((latchData[2] | 0x98), korail_simsam_latch2_addr);	
	tmp = latchData[2];
	if(!rmode){
		tmp &= ~(1<<4);
		tmp |= 0x88; 
	}
	else if(!wmode){
		tmp &= ~(1<<3);
		tmp |= 0x90;
	}
	__raw_writeb(tmp, korail_simsam_latch2_addr);
//	__raw_writeb((latchData[2] | 0x98), korail_simsam_latch2_addr);     // sim tx, rx and plcc_en disable
}
EXPORT_SYMBOL(SIM_IF_interface_nE_set);

void SIM_IF_interface_OnOFF( int port, u32 *addr, int nBitSetData , int nHLbit )
{
	//char hxInterface_OnOFF_WriteData = __raw_readb( addr );
	unsigned char writedat = latchData[port];
	

#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
	if( nHLbit )
	{
		writedat |= ( 1 << nBitSetData );
		__raw_writeb( writedat, addr );
	}
	else
	{
		writedat &= ~( 1 << nBitSetData );
		__raw_writeb( writedat, addr );
	}
#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)

	latchData[port] = writedat;
}

int SIM_IF_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	nSIM_IF_FlowNumber = cmd;

	switch(cmd)
	{
		// SIM Power On
		case _IOCTL_SIM1_PWR_ON:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_1_PWR, 0 );
			break;
		case _IOCTL_SIM2_PWR_ON:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_2_PWR, 0 );
			break;
		case _IOCTL_SIM3_PWR_ON:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_3_PWR, 0 );
			break;
		case _IOCTL_SIM4_PWR_ON:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_4_PWR, 0 );
			break;
		case _IOCTL_SIM5_PWR_ON:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_5_PWR, 0 );
			break;
		case _IOCTL_SIM6_PWR_ON:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_6_PWR, 0 );
			break;
		case _IOCTL_SIM7_PWR_ON:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_7_PWR, 0 );
			break;
		case _IOCTL_SIM8_PWR_ON:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_8_PWR, 0 );
			break;
		// SIM Power Off
		case _IOCTL_SIM1_PWR_OFF:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_1_PWR, 1 );
			break;
		case _IOCTL_SIM2_PWR_OFF:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_2_PWR, 1 );
			break;
		case _IOCTL_SIM3_PWR_OFF:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_3_PWR, 1 );
			break;
		case _IOCTL_SIM4_PWR_OFF:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_4_PWR, 1 );
			break;
		case _IOCTL_SIM5_PWR_OFF:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_5_PWR, 1 );
			break;
		case _IOCTL_SIM6_PWR_OFF:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_6_PWR, 1 );
			break;
		case _IOCTL_SIM7_PWR_OFF:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_7_PWR, 1 );
			break;
		case _IOCTL_SIM8_PWR_OFF:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_8_PWR, 1 );
			break;
		// SIM Reset Active
		case _IOCTL_SIM1_RST_HIGH:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_1_RST, 1 );
			break;
		case _IOCTL_SIM2_RST_HIGH:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_2_RST, 1 );
			break;
		case _IOCTL_SIM3_RST_HIGH:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_3_RST, 1 );
			break;
		case _IOCTL_SIM4_RST_HIGH:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_4_RST, 1 );
			break;
		case _IOCTL_SIM5_RST_HIGH:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_5_RST, 1 );
			break;
		case _IOCTL_SIM6_RST_HIGH:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_6_RST, 1 );
			break;
		case _IOCTL_SIM7_RST_HIGH:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_7_RST, 1 );
			break;
		case _IOCTL_SIM8_RST_HIGH:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_8_RST, 1 );
			break;
		// SIM Reset Release
		case _IOCTL_SIM1_RST_LOW:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_1_RST, 0 );
			break;
		case _IOCTL_SIM2_RST_LOW:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_2_RST, 0 );
			break;
		case _IOCTL_SIM3_RST_LOW:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_3_RST, 0 );
			break;
		case _IOCTL_SIM4_RST_LOW:  
			SIM_IF_interface_OnOFF(0, korail_simsam_latch0_addr, DA_SIM_4_RST, 0 );
			break;
		case _IOCTL_SIM5_RST_LOW:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_5_RST, 0 );
			break;
		case _IOCTL_SIM6_RST_LOW:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_6_RST, 0 );
			break;
		case _IOCTL_SIM7_RST_LOW:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_7_RST, 0 );
			break;
		case _IOCTL_SIM8_RST_LOW:  
			SIM_IF_interface_OnOFF(1, korail_simsam_latch1_addr, DA_SIM_8_RST, 0 );
			break;
		//SIM Read Mode
		case _IOCTL_SIM1_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 1, 1, 0 );
			break;
		case _IOCTL_SIM2_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 2, 1, 0 );
			break;
		case _IOCTL_SIM3_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 3, 1, 0 );
			break;
		case _IOCTL_SIM4_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 4, 1, 0 );
			break;
		case _IOCTL_SIM5_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 5, 1, 0 );
			break;
		case _IOCTL_SIM6_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 6, 1, 0 );
			break;
		case _IOCTL_SIM7_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 7, 1, 0 );
			break;
		case _IOCTL_SIM8_READ_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 8, 1, 0 );
			break;			
		//SIM Write Mode
		case _IOCTL_SIM1_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 1, 0, 1 );		
			break;
		case _IOCTL_SIM2_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 2, 0, 1 );
			break;
		case _IOCTL_SIM3_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 3, 0, 1 );
			break;
		case _IOCTL_SIM4_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 4, 0, 1 );
			break;
		case _IOCTL_SIM5_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 5, 0, 1 );
			break;
		case _IOCTL_SIM6_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 6, 0, 1 );
			break;
		case _IOCTL_SIM7_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 7, 0, 1 );
			break;
		case _IOCTL_SIM8_WRITE_MODE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 8, 0, 1 );
			break;
		//SAM 1 UART Enable ( z )
		case _IOCTL_SAM_1_UART_ENABLE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 9, 1, 1 );
			break;
		case _IOCTL_SAM_1_RESET_HIGH:  
			//gpio_request(S5PV210_GPE1(1), "GPE11");
			//gpio_direction_output(S5PV210_GPE1(1), 1);
			//gpio_free(S5PV210_GPE1(1));

			gpio_direction_output(S5PV210_GPE1(1), 1);
			//gpio_set_value( S5PV210_GPE1(1), 1);//s3c_gpio_setpin(S5PV210_GPE1(1), 1);
			break;
		case _IOCTL_SAM_1_RESET_LOW:  
			//gpio_request(S5PV210_GPE1(1), "GPE11");
			//gpio_direction_output(S5PV210_GPE1(1), 0);
			//gpio_free(S5PV210_GPE1(1));

			gpio_direction_output(S5PV210_GPE1(1), 0);
			//gpio_set_value( S5PV210_GPE1(1), 0);//s3c_gpio_setpin(S5PV210_GPE1(1), 0);
			break;
		//SAM 2 UART Enable ( a )
		case _IOCTL_SAM_2_UART_ENABLE:  
			SIM_IF_interface_rw_mode( korail_simsam_latch2_addr, 10, 1, 1 );
			break;
		case _IOCTL_SAM_2_RESET_HIGH:  
			//gpio_request(S5PV210_GPE1(2), "GPE12");
			//gpio_direction_output(S5PV210_GPE1(2), 1);
			//gpio_free(S5PV210_GPE1(2));

			gpio_direction_output(S5PV210_GPE1(2), 1);
			//gpio_set_value( S5PV210_GPE1(2), 1);//s3c_gpio_setpin(S5PV210_GPE1(2), 1);
			break;
		case _IOCTL_SAM_2_RESET_LOW:  
			//gpio_request(S5PV210_GPE1(2), "GPE12");
			//gpio_direction_output(S5PV210_GPE1(2), 0);
			//gpio_free(S5PV210_GPE1(2));

			gpio_direction_output(S5PV210_GPE1(2), 0);
			//gpio_set_value( S5PV210_GPE1(2), 0);//s3c_gpio_setpin(S5PV210_GPE1(2), 0);
			break;
		default: 
			nSIM_IF_FlowNumber = 0;
			break;
	}
	
	return 0;
}

static struct file_operations SIM_IF_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   SIM_IF_interface_read,
   .write      =   SIM_IF_interface_write,
   .open       =   SIM_IF_interface_open,
   .release    =   SIM_IF_interface_release,
   .ioctl      =   SIM_IF_interface_ioctl,
};

static int __init SIM_IF_interface_init(void)
{
  	int result; 

  	nSIM_IF_FlowNumber = 0;

 	result = register_chrdev(SIM_IF_MAJOR, SIM_IF_NAME, &SIM_IF_interface_fops);
 	
 	printk("%s Driver MAJOR %d\n", SIM_IF_NAME, SIM_IF_MAJOR );

	korail_uart_ncsa_addr = ioremap(DIV_UART_NCSA, 0x0040);  
	korail_uart_ncsb_addr = ioremap(DIV_UART_NCSB, 0x0040); 
	korail_simsam_latch0_addr = ioremap(DIV_SIMAM_LATCH0, 0x0040);  
	korail_simsam_latch1_addr = ioremap(DIV_SIMAM_LATCH1, 0x0040);
	korail_simsam_latch2_addr = ioremap(DIV_SIMAM_LATCH2, 0x0040);	


#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_lock_irqsave(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)
	__raw_writeb(0xff, korail_simsam_latch0_addr);
	__raw_writeb(0xff, korail_simsam_latch1_addr);
	__raw_writeb(0xff, korail_simsam_latch2_addr);

#if defined(CONFIG_KORAIL_SPINLOCK)
        spin_unlock_irqrestore(&mr_lock, flags);
#endif //(CONFIG_KORAIL_SPINLOCK)

	latchData[0] = latchData[1] = latchData[2] = 0xff;

	return 0;
}

static void __exit SIM_IF_interface_exit(void)
{
	unregister_chrdev(SIM_IF_MAJOR, SIM_IF_NAME);
	printk("Release %s Module MAJOR %d\n", SIM_IF_NAME, SIM_IF_MAJOR);
}

module_init(SIM_IF_interface_init);
module_exit(SIM_IF_interface_exit);//mknod /dev/SAM_IF c 244 0
