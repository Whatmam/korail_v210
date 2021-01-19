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
#include <plat/gpio-cfg.h>

#include <linux/bcd.h>
#include <asm/mach/time.h>
#include <plat/regs-rtc.h>
#include <linux/rtc.h>
#include <linux/uaccess.h>

#define EXT_RTC_MAJOR 		239
#define EXT_RTC_MINOR		0
#define EXT_RTC_NAME     	"NV_RAM"

#define EXT_RTC_I2C_ADDR			0x68 //0xD0

#define _IOCTL_EXT_OFFSET_READ		1000
#define _IOCTL_EXT_NORMAL_READ		2000

#define _IOCTL_WATCHDOG_EN_HIGH		3000
#define _IOCTL_WATCHDOG_EN_LOW		4000
#define _IOCTL_WATCHDOG_CLK_HIGH	5000
#define _IOCTL_WATCHDOG_CLK_LOW		6000

//EXT RTC Register
#define EXT_DS1338_RTCSEC			0x00
#define EXT_DS1338_RTCMIN			0x01
#define EXT_DS1338_RTCHOUR			0x02
#define EXT_DS1338_RTCDAY			0x03
#define EXT_DS1338_RTCDATE			0x04
#define EXT_DS1338_RTCMON			0x05		
#define EXT_DS1338_RTCYEAR			0x06
#define EXT_DS1338_RTC_CONTROL		0x07
#define EXT_DS1338_RTC_RAM			0x08 //~ 0x3F // => 8bit 56ea.

static int noffset_read_flag;

void Misc_Comon_Delay( void );
void Misc_EXT_RTC_SET_I2C_VALUE( unsigned char Clk_Set, unsigned char Data_Set );
int Misc_EXT_RTC_GET_DATA_VALUE( void );
void Misc_EXT_RTC_I2C_PIN_INOUT( unsigned char CLK_Cfg, unsigned char DATA_Cfg );
void Misc_Start_Condtion( void );
void Misc_Stop_Condtion( void );
int Misc_Snd_Write( unsigned char Send_Data );
unsigned char Misc_Snd_Rad( unsigned char Last_Data );
int Misc_SEND_EXT_RTC_WRITE( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt );
int Misc_SEND_EXT_RTC_READ( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt );
int Misc_SEND_EXT_RTC_READ_BYTE( unsigned char Address, unsigned char* data, unsigned int cnt );

//////////////////////////////////////////////////////////////////////////////////////

void Misc_Comon_Delay( void )
{
	unsigned long ldDS1338_delay = 0;
	
	ndelay(ldDS1338_delay);
}

void Misc_EXT_RTC_SET_I2C_VALUE( unsigned char Clk_Set, unsigned char Data_Set )
{
	if (Clk_Set)
	{
		//gpio_request(S5PV210_GPD1(5), "GPD15");
		//gpio_direction_output(S5PV210_GPD1(5), 1);
		gpio_set_value( S5PV210_GPD1(5), 1);//s3c_gpio_setpin(S5PV210_GPD1(5), 1);
		//gpio_free(S5PV210_GPD1(5));
	}
	else
	{		
		//gpio_request(S5PV210_GPD1(5), "GPD15");
		//gpio_direction_output(S5PV210_GPD1(5), 0);
		gpio_set_value( S5PV210_GPD1(5), 0);//s3c_gpio_setpin(S5PV210_GPD1(5), 0);
		//gpio_free(S5PV210_GPD1(5));
	}

	if(Data_Set)
	{
		//gpio_request(S5PV210_GPD1(4), "GPD14");
		//gpio_direction_output(S5PV210_GPD1(4), 1);
		gpio_set_value( S5PV210_GPD1(4), 1);//s3c_gpio_setpin(S5PV210_GPD1(4), 1);
		//gpio_free(S5PV210_GPD1(4));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(4), "GPD14");
		//gpio_direction_output(S5PV210_GPD1(4), 0);
		gpio_set_value( S5PV210_GPD1(4), 0);//s3c_gpio_setpin(S5PV210_GPD1(4), 0);
		//gpio_free(S5PV210_GPD1(4));
	}
	
	//Misc_Comon_Delay();
}

int Misc_EXT_RTC_GET_DATA_VALUE( void )
{
	if( gpio_get_value( S5PV210_GPD1(4) ) ) //if( gpio_get_value( S5PV210_GPD1(2) ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
	//Comon_Delay();
}
	
void Misc_EXT_RTC_I2C_PIN_INOUT( unsigned char CLK_Cfg, unsigned char DATA_Cfg )
{	
	
	if (CLK_Cfg)
	{
		//gpio_request(S5PV210_GPD1(5), "GPD15");	
		s3c_gpio_cfgpin(S5PV210_GPD1(5)	, S3C_GPIO_OUTPUT);
		//gpio_free(S5PV210_GPD1(5));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(5), "GPD15");	
		s3c_gpio_cfgpin(S5PV210_GPD1(5)	, S3C_GPIO_INPUT);
		//gpio_free(S5PV210_GPD1(5));
	}
	
	if (DATA_Cfg)
	{
		//gpio_request(S5PV210_GPD1(4), "GPD14");
		s3c_gpio_cfgpin(S5PV210_GPD1(4), S3C_GPIO_OUTPUT);
		//gpio_free(S5PV210_GPD1(4));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(4), "GPD14");
		s3c_gpio_cfgpin(S5PV210_GPD1(4), S3C_GPIO_INPUT);
		//gpio_free(S5PV210_GPD1(4));
	}
}

void Misc_Start_Condtion( void )
{
	Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
	
	Misc_EXT_RTC_SET_I2C_VALUE(1, 0);
	
	Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
}

void Misc_Stop_Condtion( void )
{
	Misc_EXT_RTC_SET_I2C_VALUE(0, 0);

	Misc_EXT_RTC_SET_I2C_VALUE(1, 0);

	Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
}

int Misc_Snd_Write( unsigned char Send_Data )
{
	unsigned char Trance_Data = Send_Data;
	
	int i = 0;
	
	int bACK = 0;

	for( i=7; i>=0; i-- )
	{
		if( Trance_Data & (1 << i) )
		{
			Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
			Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
			//Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
		}
		else
		{
			Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
			Misc_EXT_RTC_SET_I2C_VALUE(1, 0);
			//Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
		}
	}
	
	Misc_EXT_RTC_SET_I2C_VALUE(0, 0);

	Misc_EXT_RTC_I2C_PIN_INOUT(1, 0);

	Misc_EXT_RTC_SET_I2C_VALUE(1, 0);

	bACK = Misc_EXT_RTC_GET_DATA_VALUE();

	Misc_EXT_RTC_I2C_PIN_INOUT(1, 1);

	Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
	

	if( bACK )
	{
		//printk("[DRV EXT RTC] A2C ACK Fail----\r\n");
	}
	
	return bACK;
}

unsigned char Misc_Snd_Rad( unsigned char Last_Data )
{
	unsigned char Read_Data = 0x00;
	int i = 0;
	int bACK = 0;

	Misc_EXT_RTC_I2C_PIN_INOUT(1, 0);

	Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
	
	for( i=7; i>=0; i-- )
	{
		Misc_EXT_RTC_SET_I2C_VALUE(1, 0);
		
		if( Misc_EXT_RTC_GET_DATA_VALUE() )
		{
			//printk("1");
			Read_Data |= (1<<i);
		}
		else
		{
			//printk("0");
		}

		Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
	}
	//printk("\r\n");
	//printk("%x\r\n", Read_Data);

	Misc_EXT_RTC_I2C_PIN_INOUT(1, 1);

#if 0	
	if( Last_Data )
	{
		Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
		Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
		Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
	}
	else
	{
		Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
		Misc_EXT_RTC_SET_I2C_VALUE(1, 0);
		Misc_EXT_RTC_SET_I2C_VALUE(0, 0);
	}
#endif
	
	return Read_Data;
}

int Misc_SEND_EXT_RTC_WRITE( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt )
{
	int data_cnt = 0;
	
	Misc_Start_Condtion();

	if( Misc_Snd_Write( ( Address << 1 ) ) )
	{
		goto Fr_Error;
	}

	if( Misc_Snd_Write( Reg ) )
	{
		goto Fr_Error;
	}

	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		if( Misc_Snd_Write( data[data_cnt] ) )
		{
			goto Fr_Error;
		}
	}

	Misc_Stop_Condtion();

	return 1;

Fr_Error:
	return 0;
}

int Misc_SEND_EXT_RTC_READ( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt )
{
	int  ReValue = 0;
	int data_cnt = 0;
	
	Misc_Start_Condtion();

	if( Misc_Snd_Write( ( Address << 1 ) | 0x00 ) )
	{
		goto Fr_Error;
	}

	if( Misc_Snd_Write( Reg ) )
	{
		goto Fr_Error;
	}

	Misc_Start_Condtion();

	if( Misc_Snd_Write( ( Address << 1 ) | 0x01 ) )
	{
		goto Fr_Error;
	}

	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		data[data_cnt] = Misc_Snd_Rad(0);
	}
	
	//data[data_cnt] = Misc_Snd_Rad(1);
	Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
	Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
	Misc_EXT_RTC_SET_I2C_VALUE(0, 1);

	Misc_Stop_Condtion();
	
	return 1;

Fr_Error:
	return 0;
}


int Misc_SEND_EXT_RTC_READ_BYTE( unsigned char Address, unsigned char* data, unsigned int cnt )
{
	int ReValue = 0;
	int data_cnt = 0;

	Misc_Start_Condtion();

	if( Misc_Snd_Write( ( Address << 1 ) | 0x01 ) )
	{
		goto Fr_Error;
	}
	
	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		data[data_cnt] = Misc_Snd_Rad(0);
	}
	
	//data[data_cnt] = Misc_Snd_Rad(1);
	Misc_EXT_RTC_SET_I2C_VALUE(0, 1);
	Misc_EXT_RTC_SET_I2C_VALUE(1, 1);
	Misc_EXT_RTC_SET_I2C_VALUE(0, 1);

	Misc_Stop_Condtion();
	
	return 1;

Fr_Error:
	return 0;
}

static int EXT_RTC_interface_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int EXT_RTC_interface_release(struct inode *inode, struct file *filp)
{
	return 0;
}                         

ssize_t EXT_RTC_interface_write(struct file *filp, const char *buf, size_t length, loff_t *f_pos)
{
	unsigned char hx_write_info_buffer[100];

	int nwr_size = (int)length;

	int nLoop_w_count = 0;

	memset( hx_write_info_buffer, 0x00, sizeof(hx_write_info_buffer) );

	copy_from_user( hx_write_info_buffer, buf, sizeof(length));

	for( nLoop_w_count = 0; nLoop_w_count < length; nLoop_w_count++ )
	{
		//printk("[DRV MSG] EXT RTC WRITE [ %d  :  0x%x ] \r\n", nLoop_w_count, hx_write_info_buffer[nLoop_w_count] );
	}

	Misc_SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, hx_write_info_buffer[0], &hx_write_info_buffer[1], nwr_size );
	
	return 0;
}

ssize_t EXT_RTC_interface_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	unsigned char hx_read_info_buffer[100];

	int nre_size = (int)count;

	int nLoop_r_count = 0;

	unsigned char hx_read_offset = 0x00;

	memset( hx_read_info_buffer, 0x00, sizeof(hx_read_info_buffer) );

	if( noffset_read_flag )
	{
		copy_from_user( hx_read_info_buffer, buf, 1);

		hx_read_offset = hx_read_info_buffer[0];

		memset( hx_read_info_buffer, 0x00, sizeof(hx_read_info_buffer) );		
		
		Misc_SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, hx_read_offset, hx_read_info_buffer, nre_size );
	}
	else
	{
		Misc_SEND_EXT_RTC_READ_BYTE( EXT_RTC_I2C_ADDR, hx_read_info_buffer, nre_size );
	}

	for( nLoop_r_count = 0; nLoop_r_count < nre_size; nLoop_r_count++ )
	{
		//printk("[DRV MSG] EXT RTC OFFSET READ [ %d  :  0x%x ] \r\n", nLoop_r_count, hx_read_info_buffer[nLoop_r_count] );
	}

	copy_to_user( buf , hx_read_info_buffer, nre_size );
	
	return 0;
}

int EXT_RTC_interface_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{

		case _IOCTL_EXT_OFFSET_READ:
			noffset_read_flag = 1;
			break;
			
		case _IOCTL_EXT_NORMAL_READ:
			noffset_read_flag = 0;
			break;
		case _IOCTL_WATCHDOG_EN_HIGH:
			//printk("EN HIGH \r\n");
			//gpio_request(S5PV210_GPG3(1), "GPG31");
			//s3c_gpio_cfgpin(S5PV210_GPG3(1), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPG3(1), 1);//s3c_gpio_setpin(S5PV210_GPG3(1), 1); //1 
			//gpio_free(S5PV210_GPG3(1));		
			break;
		case _IOCTL_WATCHDOG_EN_LOW:
			//printk("EN LOW \r\n");
			//gpio_request(S5PV210_GPG3(1), "GPG31");
			//s3c_gpio_cfgpin(S5PV210_GPG3(1), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPG3(1), 0);//s3c_gpio_setpin(S5PV210_GPG3(1), 0); //1 
			//gpio_free(S5PV210_GPG3(1));				
			
			break;
		case _IOCTL_WATCHDOG_CLK_HIGH:
			//printk("CLK HIGH \r\n");
			//gpio_request(S5PV210_GPE0(0), "GPE00");
			//s3c_gpio_cfgpin(S5PV210_GPE0(0), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPE0(0), 1);//s3c_gpio_setpin(S5PV210_GPE0(0), 1); //0
			//gpio_free(S5PV210_GPE0(0));			

			//gpio_request(S5PV210_GPE0(0), "GPE00");
			//s3c_gpio_cfgpin(S5PV210_GPE0(0), S3C_GPIO_OUTPUT);			
			gpio_set_value( S5PV210_GPE0(0), 0);//s3c_gpio_setpin(S5PV210_GPE0(0), 0);
			//gpio_free(S5PV210_GPE0(0));	
			break;
		case _IOCTL_WATCHDOG_CLK_LOW:
			//printk("CLK LOW \r\n");
			//gpio_request(S5PV210_GPG3(1), "GPG31");
			//s3c_gpio_cfgpin(S5PV210_GPG3(1), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPG3(1), 0);//s3c_gpio_setpin(S5PV210_GPG3(1), 0); //1 
			//gpio_free(S5PV210_GPG3(1));
			
			//gpio_request(S5PV210_GPE0(0), "GPE00");
			//s3c_gpio_cfgpin(S5PV210_GPE0(0), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPE0(0), 1);//s3c_gpio_setpin(S5PV210_GPE0(0), 1);
			//gpio_free(S5PV210_GPE0(0));			

			mdelay(1);

			//gpio_request(S5PV210_GPG3(1), "GPG31");
			//s3c_gpio_cfgpin(S5PV210_GPG3(1), S3C_GPIO_OUTPUT);
			gpio_set_value( S5PV210_GPG3(1), 1);//s3c_gpio_setpin(S5PV210_GPG3(1), 1); //1 
			//gpio_free(S5PV210_GPG3(1));			
			break;			
		default:
			break;

	}
	
	return 0;
}

static struct file_operations EXT_RTC_interface_fops = {
   .owner      =   THIS_MODULE,
   .read       =   EXT_RTC_interface_read,
   .write      =   EXT_RTC_interface_write,
   .open       =   EXT_RTC_interface_open,
   .release    =   EXT_RTC_interface_release,
   .ioctl      =   EXT_RTC_interface_ioctl,
};

static int __init EXT_RTC_interface_init(void)
{
  	int result;

    noffset_read_flag = 0;

	result = register_chrdev(EXT_RTC_MAJOR, EXT_RTC_NAME, &EXT_RTC_interface_fops);


 	//printk("%s Driver MAJOR %d\n", EXT_RTC_NAME, EXT_RTC_MAJOR );

	

	return 0;
}

static void __exit EXT_RTC_interface_exit(void)
{
	unregister_chrdev(EXT_RTC_MAJOR, EXT_RTC_NAME);
	//printk("Release %s Module MAJOR %d\n", EXT_RTC_NAME, EXT_RTC_MAJOR);
}

module_init(EXT_RTC_interface_init);
module_exit(EXT_RTC_interface_exit);//mknod /dev/NV_RAM c 239 0 







