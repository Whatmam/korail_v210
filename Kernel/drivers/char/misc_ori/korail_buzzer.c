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

#include <linux/err.h>
#include <mach/pd.h>
#include <plat/regs-timer.h>

#define BUZZER_MAJOR 				246
#define BUZZER_MINOR				0
#define BUZZER_NAME     			"BUZZER"

#define PWM_BUZZER_0_START_STOP			0		
#define PWM_BUZZER_0_MANUAL_UPDATE		1
#define PWM_BUZZER_0_OUTPUT_INVERTER	2
#define PWM_BUZZER_0_AUTO_RELOAD		3

// for BLK Test
#define PWM_BLK_1_START_STOP			8		
#define PWM_BLK_1_MANUAL_UPDATE			9
#define PWM_BLK_1_OUTPUT_INVERTER		10
#define PWM_BLK_1_AUTO_RELOAD			11

#define _IOCTL_BUZZER_ON			0x1000
#define _IOCTL_BUZZER_ON_100		0x1100
#define _IOCTL_BUZZER_OFF			0x2000
#define _IOCTL_BUZZER_OFF_100		0x2100
#define _IOCTL_BUZZER_PANIC			0x3000
#define _IOCTL_TEST_VALUE_TCNTB		0x4000
#define _IOCTL_TEST_VALUE_TCMPB		0x5000
#define _IOCTL_TEST_VALUE_TCNTO		0x6000
#define _IOCTL_BUZZER_ALRAM			0x7000

#define _IOCTL_BLK_TEST_TCNTB		0x8000
#define _IOCTL_BLK_TEST_TCMPB		0x8100
#define _IOCTL_BLK_TEST_TCNTO		0x8200

#define _BUZZER_INIT_FREQUENCY		16384
#define _BUZZER_INIT_DUTYRATIO		8192
#define _BUZZER_INIT_DUTY100		164
#define _BUZZER_INIT_TIMECOUNT		0

//#define _BKL_INIT_FREQUENCY			// 41920 => FREQUENCY 100 Hz   // 20960 => FREQUENCY 200 Hz // 10480 => FREQUENCY 400 Hz
//#define _BKL_INIT_DUTYRATIO			// 419 => 100% // 8192 => 82% // 21440 => 50% // 16384 => 62% // 32768 => 22% // 41919 => 0% // 41920 => High
// 210 => 100% // 10500 => 50% // 20959 => 0% 

static struct timer_list control_timer_list;
static int nlife_timer;

static unsigned long buzzer_ctl_time = 0;
static unsigned long buzzer_ctl_level = 0;	

static int nOnOff_flagvalue = 0;

void buzzer_alram_off( void );

void timer_set(unsigned long buzz_time)
{
	init_timer(&control_timer_list);

	control_timer_list.function =&buzzer_alram_off;
	//control_timer_list.expires = ( jiffies + (buzz_time/10) )*100;
	//control_timer_list.expires = jiffies + (buzz_time*HZ);
	control_timer_list.expires = jiffies + (buzz_time/10);

	add_timer(&control_timer_list);

	if( nlife_timer == 0 )
		nlife_timer++;
}

void buzzer_init_level( void )
{

	int wtcon;
#if 0
	printk("\r\n");
	printk("CHECK 00. TCON : %x \r\n", readl(S3C2410_TCON) );
	printk("CHECK 00. TCNTB0 : %x \r\n", readl(S3C2410_TCNTB(0)) );
	printk("CHECK 00. TCMPB0 : %x \r\n", readl(S3C2410_TCMPB(0)) );
	printk("CHECK 00. TCNTO0 : %x \r\n", readl(S3C2410_TCNTO(0)) );
	printk("\r\n");
#endif

	wtcon = readl(S3C2410_TCON);

	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_START_STOP))) | (0<<PWM_BUZZER_0_START_STOP);
	writel(wtcon,S3C2410_TCON);
#if 0
	wtcon = readl(S3C2410_TCON);
	printk("02. wtcon : %x \r\n", wtcon );
	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_OUTPUT_INVERTER))) | (0<<PWM_BUZZER_0_OUTPUT_INVERTER);
	writel(wtcon,S3C2410_TCON);
#endif
	wtcon = readl(S3C2410_TCON);

	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_AUTO_RELOAD))) | (1<<PWM_BUZZER_0_AUTO_RELOAD);
	writel(wtcon,S3C2410_TCON);

	writel(_BUZZER_INIT_FREQUENCY, S3C2410_TCNTB(0));
	writel(_BUZZER_INIT_DUTYRATIO, S3C2410_TCMPB(0));
	writel(_BUZZER_INIT_TIMECOUNT, S3C2410_TCNTO(0));

	wtcon = readl(S3C2410_TCON);

	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);

	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);

	writel(wtcon,S3C2410_TCON);
#if 0
	mdelay(1);

	wtcon = readl(S3C2410_TCON);
	printk("06. wtcon : %x \r\n", wtcon );
	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_START_STOP))) | (1<<PWM_BUZZER_0_START_STOP);
	writel(wtcon,S3C2410_TCON);

	wtcon = readl(S3C2410_TCON);
	printk("07. wtcon : %x \r\n", wtcon );
	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);
	printk("08. wtcon : %x \r\n", wtcon );
	wtcon = (wtcon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);

	printk("\r\n");
	printk("CHECK 01. TCON : %x \r\n", readl(S3C2410_TCON) );
	printk("CHECK 01. TCNTB0 : %x \r\n", readl(S3C2410_TCNTB(0)) );
	printk("CHECK 01. TCMPB0 : %x \r\n", readl(S3C2410_TCMPB(0)) );
	printk("CHECK 01. TCNTO0 : %x \r\n", readl(S3C2410_TCNTO(0)) );
	printk("\r\n");
#endif
}

void buzzer_alram_on( void )
{
	int nbuzzer_on_basecon = 0;

	nbuzzer_on_basecon = readl(S3C2410_TCON);
	nbuzzer_on_basecon = (nbuzzer_on_basecon & (~(1<<PWM_BUZZER_0_START_STOP))) | (1<<PWM_BUZZER_0_START_STOP);
	writel(nbuzzer_on_basecon,S3C2410_TCON);

	nbuzzer_on_basecon = readl(S3C2410_TCON);
	nbuzzer_on_basecon = (nbuzzer_on_basecon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(nbuzzer_on_basecon,S3C2410_TCON);
	nbuzzer_on_basecon = readl(S3C2410_TCON);
	nbuzzer_on_basecon = (nbuzzer_on_basecon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(nbuzzer_on_basecon,S3C2410_TCON);
}

void buzzer_alram_off( void )
{
	int nbuzzer_off_basecon = 0;

	nbuzzer_off_basecon = readl(S3C2410_TCON);
	nbuzzer_off_basecon = (nbuzzer_off_basecon & (~(1<<PWM_BUZZER_0_START_STOP))) | (0<<PWM_BUZZER_0_START_STOP);
	writel(nbuzzer_off_basecon,S3C2410_TCON);

	nbuzzer_off_basecon = readl(S3C2410_TCON);
	nbuzzer_off_basecon = (nbuzzer_off_basecon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(nbuzzer_off_basecon,S3C2410_TCON);
	nbuzzer_off_basecon = readl(S3C2410_TCON);
	nbuzzer_off_basecon = (nbuzzer_off_basecon & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
	writel(nbuzzer_off_basecon,S3C2410_TCON);

	nlife_timer--;

	if( nlife_timer )
	{
		if( nOnOff_flagvalue )
		{
			nOnOff_flagvalue = 0;
		}
		else
		{
			nOnOff_flagvalue = 1;
		}
		
		del_timer(&control_timer_list);	

		if( nOnOff_flagvalue )
		{
			buzzer_alram_on();		
		}

		timer_set( buzzer_ctl_time );
	}
	else
	{
		nOnOff_flagvalue = 0;
		buzzer_ctl_time = 0;
		buzzer_ctl_level = 0;
		nlife_timer = 0;
	}
}

void buzzer_ctl_dutyratio( unsigned long ldBuzzerDutyratio )
{
	writel(ldBuzzerDutyratio, S3C2410_TCMPB(0));
}

static int BUZZER_interface_open(struct inode *inode, struct file *filp)
{	
	return 0;
}

static int BUZZER_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t BUZZER_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	return 0;
}

ssize_t BUZZER_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int BUZZER_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long pwm_ctl_level = 0; //75

	unsigned long pwm_test_value = 0;
	unsigned long pwm_blk_setting_read_value = 0;
	int ntest_read_value = 0;

	copy_from_user(&pwm_ctl_level, arg, sizeof(pwm_ctl_level));

	pwm_test_value = pwm_ctl_level;

	buzzer_ctl_time = ( pwm_ctl_level & 0xFFFF0000 ) >> 16;

	buzzer_ctl_level = ( pwm_ctl_level & 0xFF00 ) >> 8; 


	//printk("[ Buzzer > Time : %d || Level : %d ] \r\n", buzzer_ctl_time , buzzer_ctl_level );

	switch(cmd)
	{

		case _IOCTL_BUZZER_ON:
		case _IOCTL_BUZZER_ON_100:
			printk("[ Buzzer > ON ]\r\n");	
			
			if( nlife_timer )
			{
				nlife_timer = 0;
				
				del_timer(&control_timer_list);
			}

			nlife_timer = (int)( pwm_ctl_level & 0xFF );

			nlife_timer = nlife_timer*2;

			writel(_BUZZER_INIT_TIMECOUNT, S3C2410_TCNTO(0));

			writel(_BUZZER_INIT_FREQUENCY, S3C2410_TCNTB(0));
			if (cmd == _IOCTL_BUZZER_ON) 
				writel(_BUZZER_INIT_DUTYRATIO, S3C2410_TCMPB(0));
			else
				writel(_BUZZER_INIT_DUTY100, S3C2410_TCMPB(0));

			buzzer_alram_on();			

			if (cmd == _IOCTL_BUZZER_ON) 
			{
				nOnOff_flagvalue = 1;
				
				timer_set( buzzer_ctl_time );
			}

			break;

		case _IOCTL_BUZZER_OFF:
		case _IOCTL_BUZZER_OFF_100:
			printk("[ Buzzer > OFF ]\r\n");

			if( nlife_timer )
			{
				nlife_timer = 0;
				
				del_timer(&control_timer_list);
			}

			buzzer_alram_off();

			break;

		case _IOCTL_TEST_VALUE_TCNTB:
			writel(pwm_test_value, S3C2410_TCNTB(0));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		case _IOCTL_TEST_VALUE_TCMPB:
			writel(pwm_test_value, S3C2410_TCMPB(0));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		case _IOCTL_TEST_VALUE_TCNTO:
			writel(pwm_test_value, S3C2410_TCNTO(0));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (1<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BUZZER_0_MANUAL_UPDATE))) | (0<<PWM_BUZZER_0_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		case _IOCTL_BUZZER_ALRAM:

			if( nlife_timer )
			{
				del_timer(&control_timer_list);
			}

			buzzer_ctl_dutyratio( buzzer_ctl_level );

			buzzer_alram_on();

			timer_set( buzzer_ctl_time );

			break;	

		case _IOCTL_BLK_TEST_TCNTB:
			pwm_blk_setting_read_value = readl( S3C2410_TCNTB(1) );
			printk("[TEST BLK VALUE > TCNTB : %x] \r\n", pwm_blk_setting_read_value );

			writel(pwm_test_value, S3C2410_TCNTB(1));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (1<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (0<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		case _IOCTL_BLK_TEST_TCMPB:
			pwm_blk_setting_read_value = readl( S3C2410_TCMPB(1) );
			printk("[TEST BLK VALUE > TCMPB : %x] \r\n", pwm_blk_setting_read_value );

			writel(pwm_test_value, S3C2410_TCMPB(1));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (1<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (0<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		case _IOCTL_BLK_TEST_TCNTO:
			pwm_blk_setting_read_value = readl( S3C2410_TCNTO(1) );
			printk("[TEST BLK VALUE > TCNTO : %x] \r\n", pwm_blk_setting_read_value );

			writel(pwm_test_value, S3C2410_TCNTO(1));

			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (1<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);
			ntest_read_value = readl(S3C2410_TCON);
			ntest_read_value = (ntest_read_value & (~(1<<PWM_BLK_1_MANUAL_UPDATE))) | (0<<PWM_BLK_1_MANUAL_UPDATE);
			writel(ntest_read_value,S3C2410_TCON);

			break;

		default:
			break;
	}

	return 0;
}

static struct file_operations BUZZER_interface_fops = {
	.owner      =   THIS_MODULE,
	.read       =   BUZZER_interface_read,
	.write      =   BUZZER_interface_write,
	.open       =   BUZZER_interface_open,
	.release    =   BUZZER_interface_release,
	.ioctl      =   BUZZER_interface_ioctl,
};

static int __init BUZZER_interface_init(void)
{
	int result;

	result = register_chrdev(BUZZER_MAJOR, BUZZER_NAME, &BUZZER_interface_fops);

	printk("%s Driver MAJOR %d\n", BUZZER_NAME, BUZZER_MAJOR );	

	buzzer_init_level();

	return 0;
}

static void __exit BUZZER_interface_exit(void)
{
	unregister_chrdev(BUZZER_MAJOR, BUZZER_NAME);
	printk("Release %s Module MAJOR %d\n", BUZZER_NAME, BUZZER_MAJOR);
}

module_init(BUZZER_interface_init);
module_exit(BUZZER_interface_exit);//mknod /dev/BUZZER c 246 0 







