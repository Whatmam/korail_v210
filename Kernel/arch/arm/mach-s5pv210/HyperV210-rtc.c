/* drivers/rtc/rtc-s3c.c
 *
 * Copyright (c) 2004,2006 Simtec Electronics
 *	Ben Dooks, <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * S3C/S5P - Internal RTC Driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach/time.h>

#include <plat/regs-rtc.h>

// gpio control header add
#include <linux/kernel.h>
#include <linux/types.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/delay.h>

#ifdef CONFIG_KORAIL_DEVICE_DS1338
#define EXT_RTC_ENABLE	1
#else
#define EXT_RTC_ENABLE	0
#endif

#define RTC_CHK_FLAG	0

#if EXT_RTC_ENABLE

#define EXT_RTC_I2C_ADDR			0x68 //0xD0

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

#endif

static struct resource *hyperv210_rtc_mem;

static void __iomem *hyperv210_rtc_base;
static int hyperv210_rtc_alarmno = NO_IRQ;

static unsigned int nWdog_value = 0;

static void hyperv210_rtc_enable(struct device *dev, int en);
static int hyperv210_rtc_settime(struct device *dev, struct rtc_time *tm);

#if EXT_RTC_ENABLE

void Comon_Delay( void );
void EXT_RTC_SET_I2C_VALUE( unsigned char Clk_Set, unsigned char Data_Set );
int EXT_RTC_GET_DATA_VALUE( void );
void EXT_RTC_I2C_PIN_INOUT( unsigned char CLK_Cfg, unsigned char DATA_Cfg );
void Start_Condtion( void );
void Stop_Condtion( void );
int Snd_Write( unsigned char Send_Data );
unsigned char Snd_Rad( unsigned char Last_Data );
int SEND_EXT_RTC_WRITE( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt );
int SEND_EXT_RTC_READ( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt );
int SEND_EXT_RTC_READ_BYTE( unsigned char Address, unsigned char* data, unsigned int cnt );

//////////////////////////////////////////////////////////////////////////////////////

void Comon_Delay( void )
{
	unsigned long ldDS1338_delay = 0;
	
	ndelay(ldDS1338_delay);
}

void EXT_RTC_SET_I2C_VALUE( unsigned char Clk_Set, unsigned char Data_Set )
{
	if (Clk_Set)
	{
		//gpio_request(S5PV210_GPD1(3), "GPD13");
		//gpio_direction_output(S5PV210_GPD1(3), 1);
		//s3c_gpio_setpin(S5PV210_GPD1(3), 1);
		//gpio_free(S5PV210_GPD1(3));

		gpio_request(S5PV210_GPD1(5), "GPD15");
		//gpio_direction_output(S5PV210_GPD1(5), 1);
		s3c_gpio_setpin(S5PV210_GPD1(5), 1);
		gpio_free(S5PV210_GPD1(5));
	}
	else
	{		
		//gpio_request(S5PV210_GPD1(3), "GPD13");
		//gpio_direction_output(S5PV210_GPD1(3), 0);
		//s3c_gpio_setpin(S5PV210_GPD1(3), 0);
		//gpio_free(S5PV210_GPD1(3));

		gpio_request(S5PV210_GPD1(5), "GPD15");
		//gpio_direction_output(S5PV210_GPD1(5), 0);
		s3c_gpio_setpin(S5PV210_GPD1(5), 0);
		gpio_free(S5PV210_GPD1(5));
	}

	if(Data_Set)
	{
		//gpio_request(S5PV210_GPD1(2), "GPD12");
		//gpio_direction_output(S5PV210_GPD1(2), 1);
		//s3c_gpio_setpin(S5PV210_GPD1(2), 1);
		//gpio_free(S5PV210_GPD1(2));

		gpio_request(S5PV210_GPD1(4), "GPD14");
		//gpio_direction_output(S5PV210_GPD1(4), 1);
		s3c_gpio_setpin(S5PV210_GPD1(4), 1);
		gpio_free(S5PV210_GPD1(4));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(2), "GPD12");
		//gpio_direction_output(S5PV210_GPD1(2), 0);
		//s3c_gpio_setpin(S5PV210_GPD1(2), 0);
		//gpio_free(S5PV210_GPD1(2));

		gpio_request(S5PV210_GPD1(4), "GPD14");
		//gpio_direction_output(S5PV210_GPD1(4), 0);
		s3c_gpio_setpin(S5PV210_GPD1(4), 0);
		gpio_free(S5PV210_GPD1(4));
	}
	
	//Comon_Delay();
}

int EXT_RTC_GET_DATA_VALUE( void )
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
	
void EXT_RTC_I2C_PIN_INOUT( unsigned char CLK_Cfg, unsigned char DATA_Cfg )
{	
	
	if (CLK_Cfg)
	{
		//gpio_request(S5PV210_GPD1(3), "GPD13");	
		//s3c_gpio_cfgpin(S5PV210_GPD1(3)	, S3C_GPIO_OUTPUT);
		//gpio_free(S5PV210_GPD1(3));

		gpio_request(S5PV210_GPD1(5), "GPD15");	
		s3c_gpio_cfgpin(S5PV210_GPD1(5)	, S3C_GPIO_OUTPUT);
		gpio_free(S5PV210_GPD1(5));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(3), "GPD13");	
		//s3c_gpio_cfgpin(S5PV210_GPD1(3)	, S3C_GPIO_INPUT);
		//gpio_free(S5PV210_GPD1(3));

		gpio_request(S5PV210_GPD1(5), "GPD15");	
		s3c_gpio_cfgpin(S5PV210_GPD1(5)	, S3C_GPIO_INPUT);
		gpio_free(S5PV210_GPD1(5));
	}
	
	if (DATA_Cfg)
	{
		//gpio_request(S5PV210_GPD1(2), "GPD12");
		//s3c_gpio_cfgpin(S5PV210_GPD1(2), S3C_GPIO_OUTPUT);
		//gpio_free(S5PV210_GPD1(2));

		gpio_request(S5PV210_GPD1(4), "GPD14");
		s3c_gpio_cfgpin(S5PV210_GPD1(4), S3C_GPIO_OUTPUT);
		gpio_free(S5PV210_GPD1(4));
	}
	else
	{
		//gpio_request(S5PV210_GPD1(2), "GPD12");
		//s3c_gpio_cfgpin(S5PV210_GPD1(2), S3C_GPIO_INPUT);
		//gpio_free(S5PV210_GPD1(2));

		gpio_request(S5PV210_GPD1(4), "GPD14");
		s3c_gpio_cfgpin(S5PV210_GPD1(4), S3C_GPIO_INPUT);
		gpio_free(S5PV210_GPD1(4));
	}
}

void Start_Condtion( void )
{
	EXT_RTC_SET_I2C_VALUE(1, 1);
	
	EXT_RTC_SET_I2C_VALUE(1, 0);
	
	EXT_RTC_SET_I2C_VALUE(0, 0);
}

void Stop_Condtion( void )
{
	EXT_RTC_SET_I2C_VALUE(0, 0);

	EXT_RTC_SET_I2C_VALUE(1, 0);

	EXT_RTC_SET_I2C_VALUE(1, 1);
}

int Snd_Write( unsigned char Send_Data )
{
	unsigned char Trance_Data = Send_Data;
	
	int i = 0;
	
	int bACK = 0;

	for( i=7; i>=0; i-- )
	{
		if( Trance_Data & (1 << i) )
		{
			EXT_RTC_SET_I2C_VALUE(0, 1);
			EXT_RTC_SET_I2C_VALUE(1, 1);
			//EXT_RTC_SET_I2C_VALUE(0, 1);
		}
		else
		{
			EXT_RTC_SET_I2C_VALUE(0, 0);
			EXT_RTC_SET_I2C_VALUE(1, 0);
			//EXT_RTC_SET_I2C_VALUE(0, 0);
		}
	}
	
	EXT_RTC_SET_I2C_VALUE(0, 0);

	EXT_RTC_I2C_PIN_INOUT(1, 0);

	EXT_RTC_SET_I2C_VALUE(1, 0);

	bACK = EXT_RTC_GET_DATA_VALUE();

	EXT_RTC_I2C_PIN_INOUT(1, 1);
	
	EXT_RTC_SET_I2C_VALUE(0, 1);
	

	if( bACK )
	{
		//printk("[EXT RTC] A2C ACK Fail----\r\n");
	}
	
	return bACK;
}

unsigned char Snd_Rad( unsigned char Last_Data )
{
	unsigned char Read_Data = 0x00;
	int i = 0;
	int bACK = 0;

	EXT_RTC_I2C_PIN_INOUT(1, 0);

	EXT_RTC_SET_I2C_VALUE(0, 0);

	for( i=7; i>=0; i-- )
	{
		EXT_RTC_SET_I2C_VALUE(1, 0);
		
		if( EXT_RTC_GET_DATA_VALUE() )
		{
			Read_Data |= (1<<i);
		}

		EXT_RTC_SET_I2C_VALUE(0, 0);
	}

	EXT_RTC_I2C_PIN_INOUT(1, 1);
#if 0	
	if( Last_Data )
	{
		EXT_RTC_SET_I2C_VALUE(0, 1);
		EXT_RTC_SET_I2C_VALUE(1, 1);
		EXT_RTC_SET_I2C_VALUE(0, 1);
	}
	else
	{
		EXT_RTC_SET_I2C_VALUE(0, 0);
		EXT_RTC_SET_I2C_VALUE(1, 0);
		EXT_RTC_SET_I2C_VALUE(0, 0);
	}
#endif
	
	return Read_Data;
}

int SEND_EXT_RTC_WRITE( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt )
{
	int data_cnt = 0;
	
	Start_Condtion();

	if( Snd_Write( ( Address << 1 ) ) )
	{
		goto Fr_Error;
	}

	if( Snd_Write( Reg ) )
	{
		goto Fr_Error;
	}

	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		if( Snd_Write( data[data_cnt] ) )
		{
			goto Fr_Error;
		}
	}

	Stop_Condtion();

	return 1;

Fr_Error:
	return 0;
}

int SEND_EXT_RTC_READ( unsigned char Address, unsigned char Reg, unsigned char* data, unsigned int cnt )
{
	int  ReValue = 0;
	int data_cnt = 0;
	
	Start_Condtion();

	if( Snd_Write( ( Address << 1 ) | 0x00 ) )
	{
		goto Fr_Error;
	}

	if( Snd_Write( Reg ) )
	{
		goto Fr_Error;
	}

	Start_Condtion();

	if( Snd_Write( ( Address << 1 ) | 0x01 ) )
	{
		goto Fr_Error;
	}
	
	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		data[data_cnt] = Snd_Rad(0);
	}
	
	//data[data_cnt] = Snd_Rad(1);
	EXT_RTC_SET_I2C_VALUE(0, 1);
	EXT_RTC_SET_I2C_VALUE(1, 1);
	EXT_RTC_SET_I2C_VALUE(0, 1);

	Stop_Condtion();
	
	return 1;

Fr_Error:
	return 0;
}


int SEND_EXT_RTC_READ_BYTE( unsigned char Address, unsigned char* data, unsigned int cnt )
{
	int ReValue = 0;
	int data_cnt = 0;

	Start_Condtion();

	if( Snd_Write( ( Address << 1 ) | 0x01 ) )
	{
		goto Fr_Error;
	}
	
	for( data_cnt = 0; data_cnt < cnt; data_cnt++ )
	{
		data[data_cnt] = Snd_Rad(0);
	}
	
	//data[data_cnt] = Snd_Rad(1);
	EXT_RTC_SET_I2C_VALUE(0, 1);
	EXT_RTC_SET_I2C_VALUE(1, 1);
	EXT_RTC_SET_I2C_VALUE(0, 1);

	Stop_Condtion();
	
	return 1;

Fr_Error:
	return 0;
}

#endif

/* Update control registers */
static void hyperv210_rtc_setaie(int to)
{
	unsigned int tmp;

	pr_debug("%s: aie=%d\n", __func__, to);

	tmp = readb(hyperv210_rtc_base + S3C2410_RTCALM) & ~S3C2410_RTCALM_ALMEN;

	if (to)
		tmp |= S3C2410_RTCALM_ALMEN;

	writeb(tmp, hyperv210_rtc_base + S3C2410_RTCALM);

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 00 : 0x%x ] \r\n", nWdog_value );
#endif

}

static int hyperv210_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	unsigned int have_retried = 0;

 #if EXT_RTC_ENABLE	
	unsigned char hx_get_rtc_buffer[66];
	
	unsigned char hx_get_rtc_sec = 0x00;
	unsigned char hx_get_rtc_min = 0x00;
	unsigned char hx_get_rtc_hour = 0x00;
	unsigned char hx_get_rtc_day = 0x00;
	unsigned char hx_get_rtc_date = 0x00;
	unsigned char hx_get_rtc_mon = 0x00;
	unsigned char hx_get_rtc_year = 0x00;

	int n_get_parser_sec = 0;
	int n_get_parser_min = 0;
	int n_get_parser_hour = 0;
	int n_get_parser_day = 0;
	int n_get_parser_date = 0;
	int n_get_parser_mon = 0;
	int n_get_parser_year = 0;
	
 #endif
	
	void __iomem *base = hyperv210_rtc_base;
	
	hyperv210_rtc_enable(dev, 1);
	
retry_get_time:
#if EXT_RTC_ENABLE	
 	memset( hx_get_rtc_buffer, 0x00, sizeof(hx_get_rtc_buffer) );

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCSEC, &hx_get_rtc_sec, 1 ) )
 	{
		n_get_parser_sec = (int)( hx_get_rtc_sec & 0x7F );
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCMIN, &hx_get_rtc_min, 1 ) )
 	{
		n_get_parser_min = (int)( hx_get_rtc_min & 0x7F );
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCHOUR, &hx_get_rtc_hour, 1 ) )
 	{
		n_get_parser_hour = (int)( hx_get_rtc_hour & 0x7F );

		//printk("[EXT RTC Hour] 0x%x \r\n", n_get_parser_hour );

		n_get_parser_hour = n_get_parser_hour & 0x3F;
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCDAY, &hx_get_rtc_day, 1 ) )
 	{
		n_get_parser_day = (int)( hx_get_rtc_day & 0x07 );
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCDATE, &hx_get_rtc_date, 1 ) )
 	{
		n_get_parser_date = (int)( hx_get_rtc_date & 0x3F );
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCMON, &hx_get_rtc_mon, 1 ) )
 	{
		n_get_parser_mon = (int)( hx_get_rtc_mon & 0x1F );
 	}

 	if( SEND_EXT_RTC_READ( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCYEAR, &hx_get_rtc_year, 1 ) ) 
 	{
 		// 1970 + 153 ( 0x99 ) = 2123 - 2014 = 109 => default setting value : 0x2C = 44 , 1970 + 44 = 2014
 		
		n_get_parser_year = (int)( ( hx_get_rtc_year & 0xFF ) + 0x100 );
 	}
 	
	rtc_tm->tm_sec  = n_get_parser_sec;
	rtc_tm->tm_min  = n_get_parser_min;
	rtc_tm->tm_hour = n_get_parser_hour;
	rtc_tm->tm_mday = n_get_parser_date;
	rtc_tm->tm_mon  = n_get_parser_mon;
	//rtc_tm->tm_year = n_get_parser_year + 2000;
	rtc_tm->tm_year = n_get_parser_year;
#else
	rtc_tm->tm_sec  = readb(base + S3C2410_RTCSEC);
	rtc_tm->tm_min  = readb(base + S3C2410_RTCMIN);
	rtc_tm->tm_hour = readb(base + S3C2410_RTCHOUR);
	rtc_tm->tm_mday = readb(base + S3C2410_RTCDATE);
	rtc_tm->tm_mon  = readb(base + S3C2410_RTCMON);
	rtc_tm->tm_year = readl(base + S3C2410_RTCYEAR);
#endif

#if RTC_CHK_FLAG
	printk("[H-RTC Before 00] read time %02x.%02x.%02x %02x/%02x/%02x\n",
		 rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		 rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
#endif

	/* the only way to work out wether the system was mid-update
	 * when we read it is to check the second counter, and if it
	 * is zero, then we re-try the entire read
	 */

	if (rtc_tm->tm_sec == 0 && !have_retried) {
		have_retried = 1;
		goto retry_get_time;
	}
	
	hyperv210_rtc_enable(dev, 0);
	
#if RTC_CHK_FLAG
	printk("[H-RTC Before 01] read time %02x.%02x.%02x %02x/%02x/%02x\n",
		 rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		 rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
#endif

	rtc_tm->tm_sec = bcd2bin(rtc_tm->tm_sec);
	rtc_tm->tm_min = bcd2bin(rtc_tm->tm_min);
	rtc_tm->tm_hour = bcd2bin(rtc_tm->tm_hour);
	rtc_tm->tm_mday = bcd2bin(rtc_tm->tm_mday);
	rtc_tm->tm_mon = bcd2bin(rtc_tm->tm_mon);
	rtc_tm->tm_year = bcd2bin(rtc_tm->tm_year & 0xff) +
			  bcd2bin(rtc_tm->tm_year >> 8) * 100;

	rtc_tm->tm_mon -= 1;

#if RTC_CHK_FLAG
	printk("[H-RTC After] read time %02x.%02x.%02x %02x/%02x/%02x\n",
		 rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		 rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
#endif

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 01 : 0x%x ] \r\n", nWdog_value );
#endif

	return 0;
}

static int hyperv210_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	void __iomem *base = hyperv210_rtc_base;
	int year = tm->tm_year;
	int year100;

	unsigned char hx_set_parser_sec = 0x00;
	unsigned char hx_set_parser_min = 0x00;
	unsigned char hx_set_parser_hour = 0x00;
	unsigned char hx_set_parser_date = 0x00;
	unsigned char hx_set_parser_mon = 0x00;
	
#if EXT_RTC_ENABLE	
	unsigned char hx_ext_rtc_time_buffer[9];

	memset( hx_ext_rtc_time_buffer, 0x00, sizeof(hx_ext_rtc_time_buffer) );
#endif

#if RTC_CHK_FLAG
	printk("[H-RTC WRITE 00 ] set time %02d.%02d.%02d %02d/%02d/%02d\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
#endif

	/* we get around y2k by simply not supporting it */
	if (year < 0 || year >= 1000) {
		dev_err(dev, "rtc only supports 0~999 years\n");
		return -EINVAL;
	}

	hyperv210_rtc_enable(dev, 1);

	hx_set_parser_sec = bin2bcd(tm->tm_sec);
	hx_set_parser_min = bin2bcd(tm->tm_min);
	hx_set_parser_hour = bin2bcd(tm->tm_hour);
	hx_set_parser_date = bin2bcd(tm->tm_mday);
	hx_set_parser_mon = bin2bcd(tm->tm_mon + 1);

#if RTC_CHK_FLAG
	printk("[H-RTC WRITE 01 ] set time %02d.%02d %02d/%02d/%02d\n",
		 hx_set_parser_mon, hx_set_parser_date,
		 hx_set_parser_hour, hx_set_parser_min, hx_set_parser_sec);
#endif

#if EXT_RTC_ENABLE	

	hx_ext_rtc_time_buffer[0] = (char)hx_set_parser_sec;
	hx_ext_rtc_time_buffer[1] = (char)hx_set_parser_min;
	hx_ext_rtc_time_buffer[2] = (char)hx_set_parser_hour;

	hx_ext_rtc_time_buffer[3] = 0x00;
	hx_ext_rtc_time_buffer[4] = (char)hx_set_parser_date;
	hx_ext_rtc_time_buffer[5] = (char)hx_set_parser_mon;
	
	//SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCSEC, &hx_set_parser_sec, 1 );
	//SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCMIN, &hx_set_parser_min, 1 );
	//SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCHOUR, &hx_set_parser_hour, 1 );
	//SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCDATE, &hx_set_parser_date, 1 );
	//SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCMON, &hx_set_parser_mon, 1 );

	SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCSEC, hx_ext_rtc_time_buffer, 6 );
#endif

	writeb(hx_set_parser_sec,  base + S3C2410_RTCSEC);
	writeb(hx_set_parser_min,  base + S3C2410_RTCMIN);
	writeb(hx_set_parser_hour, base + S3C2410_RTCHOUR);
	writeb(hx_set_parser_date, base + S3C2410_RTCDATE);
	writeb(hx_set_parser_mon, base + S3C2410_RTCMON);

	year100 = year/100;
	year = year%100;
	year = bin2bcd(year) | ((bin2bcd(year100)) << 8);
	
#if EXT_RTC_ENABLE	
	year = (0x00000ff & year);

	hx_ext_rtc_time_buffer[6] = (char)year;
#else
	year = (0x00000fff & year);
#endif

	pr_debug("year %x\n", year);

#if RTC_CHK_FLAG
	printk("[H-RTC WRITE 01 ] set time year %02d\n", year);
#endif

#if EXT_RTC_ENABLE	
	SEND_EXT_RTC_WRITE( EXT_RTC_I2C_ADDR, EXT_DS1338_RTCYEAR, &hx_ext_rtc_time_buffer[6], 1 );
#endif

	writel(year, base + S3C2410_RTCYEAR);
	

	hyperv210_rtc_enable(dev, 0);

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 02 : 0x%x ] \r\n", nWdog_value );
#endif

	return 0;
}

static int hyperv210_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *alm_tm = &alrm->time;
	void __iomem *base = hyperv210_rtc_base;
	unsigned int alm_en;
	
	hyperv210_rtc_enable(dev, 1);

	alm_tm->tm_sec  = readb(base + S3C2410_ALMSEC);
	alm_tm->tm_min  = readb(base + S3C2410_ALMMIN);
	alm_tm->tm_hour = readb(base + S3C2410_ALMHOUR);
	alm_tm->tm_mon  = readb(base + S3C2410_ALMMON);
	alm_tm->tm_mday = readb(base + S3C2410_ALMDATE);
	alm_tm->tm_year = readl(base + S3C2410_ALMYEAR);
	alm_tm->tm_year = (0x00000fff & alm_tm->tm_year);
	
	hyperv210_rtc_enable(dev, 0);

	alm_en = readb(base + S3C2410_RTCALM);

	alrm->enabled = (alm_en & S3C2410_RTCALM_ALMEN) ? 1 : 0;

	pr_debug("read alarm %02x %02x.%02x.%02x %02x/%02x/%02x\n",
		 alm_en,
		 alm_tm->tm_year, alm_tm->tm_mon, alm_tm->tm_mday,
		 alm_tm->tm_hour, alm_tm->tm_min, alm_tm->tm_sec);

	/* decode the alarm enable field */

	if (alm_en & S3C2410_RTCALM_SECEN)
		alm_tm->tm_sec = bcd2bin(alm_tm->tm_sec);
	else
		alm_tm->tm_sec = 0xff;

	if (alm_en & S3C2410_RTCALM_MINEN)
		alm_tm->tm_min = bcd2bin(alm_tm->tm_min);
	else
		alm_tm->tm_min = 0xff;

	if (alm_en & S3C2410_RTCALM_HOUREN)
		alm_tm->tm_hour = bcd2bin(alm_tm->tm_hour);
	else
		alm_tm->tm_hour = 0xff;

	if (alm_en & S3C2410_RTCALM_DAYEN)
		alm_tm->tm_mday = bcd2bin(alm_tm->tm_mday);
	else
		alm_tm->tm_mday = 0xff;

	if (alm_en & S3C2410_RTCALM_MONEN) {
		alm_tm->tm_mon = bcd2bin(alm_tm->tm_mon);
		alm_tm->tm_mon -= 1;
	} else {
		alm_tm->tm_mon = 0xff;
	}

	if (alm_en & S3C2410_RTCALM_YEAREN) {
		alm_tm->tm_year = bcd2bin(alm_tm->tm_year & 0xff) +
			  bcd2bin(alm_tm->tm_year >> 8) * 100;
	} else {
		alm_tm->tm_year = 0xffff;
	}

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 03 : 0x%x ] \r\n", nWdog_value );
#endif

	return 0;
}

static int hyperv210_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *tm = &alrm->time;
	void __iomem *base = hyperv210_rtc_base;
	unsigned int alrm_en;

	int year = tm->tm_year;
	int year100;

	pr_debug("hyperv210_rtc_setalarm: %d, %02x/%02x/%02x %02x.%02x.%02x\n",
		 alrm->enabled,
		 tm->tm_mday & 0xff, tm->tm_mon & 0xff, tm->tm_year & 0xff,
		 tm->tm_hour & 0xff, tm->tm_min & 0xff, tm->tm_sec);

	hyperv210_rtc_enable(dev, 1);

	alrm_en = readb(base + S3C2410_RTCALM) & S3C2410_RTCALM_ALMEN;
	writeb(0x00, base + S3C2410_RTCALM);

	if (tm->tm_sec < 60 && tm->tm_sec >= 0) {
		alrm_en |= S3C2410_RTCALM_SECEN;
		writeb(bin2bcd(tm->tm_sec), base + S3C2410_ALMSEC);
	}

	if (tm->tm_min < 60 && tm->tm_min >= 0) {
		alrm_en |= S3C2410_RTCALM_MINEN;
		writeb(bin2bcd(tm->tm_min), base + S3C2410_ALMMIN);
	}

	if (tm->tm_hour < 24 && tm->tm_hour >= 0) {
		alrm_en |= S3C2410_RTCALM_HOUREN;
		writeb(bin2bcd(tm->tm_hour), base + S3C2410_ALMHOUR);
	}

	if (tm->tm_mday >= 0) {
		alrm_en |= S3C2410_RTCALM_DAYEN;
		writeb(bin2bcd(tm->tm_mday), base + S3C2410_ALMDATE);
	}

	if (tm->tm_mon < 13 && tm->tm_mon >= 0) {
		alrm_en |= S3C2410_RTCALM_MONEN;
		writeb(bin2bcd(tm->tm_mon + 1), base + S3C2410_ALMMON);
	}

	if (year < 1000 && year >= 0) {
		alrm_en |= S3C2410_RTCALM_YEAREN;
		year100 = year/100;
		year = year%100;
		year = bin2bcd(year) | ((bin2bcd(year100)) << 8);
		year = (0x00000fff & year);
		pr_debug("year %x\n", year);
		writel(year, base + S3C2410_ALMYEAR);
	}

	hyperv210_rtc_enable(dev, 0);

	pr_debug("setting S3C2410_RTCALM to %08x\n", alrm_en);

	writeb(alrm_en, base + S3C2410_RTCALM);

	hyperv210_rtc_setaie(alrm->enabled);

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 04 : 0x%x ] \r\n", nWdog_value );
#endif

	return 0;
}

static const struct rtc_class_ops hyperv210_rtcops = {
#if EXT_RTC_ENABLE
	.read_time	= hyperv210_rtc_gettime,
	.set_time	= hyperv210_rtc_settime,
#else
	.read_time	= hyperv210_rtc_gettime,
	.set_time	= hyperv210_rtc_settime,
	.read_alarm	= hyperv210_rtc_getalarm,
	.set_alarm	= hyperv210_rtc_setalarm,
#endif
};

static void hyperv210_rtc_enable(struct device *dev, int en)
{
	void __iomem *base = hyperv210_rtc_base;
	unsigned int tmp;

	if (hyperv210_rtc_base == NULL)
		return;

	if (!en) {
		tmp = readw(base + S3C2410_RTCCON);
		writew(tmp & ~(S3C2410_RTCCON_RTCEN), base + S3C2410_RTCCON);
	} else {
		/* re-enable the device, and check it is ok */

		if ((readw(base+S3C2410_RTCCON) & S3C2410_RTCCON_RTCEN) == 0) {
			dev_info(dev, "rtc disabled, re-enabling\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp|S3C2410_RTCCON_RTCEN, base+S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CNTSEL)) {
			dev_info(dev, "removing RTCCON_CNTSEL\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp & ~S3C2410_RTCCON_CNTSEL,
				 base+S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CLKRST)) {
			dev_info(dev, "removing RTCCON_CLKRST\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp & ~S3C2410_RTCCON_CLKRST,
				     base+S3C2410_RTCCON);
		}
	}

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 05 : 0x%x ] \r\n", nWdog_value );
#endif

}

static int __devexit hyperv210_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc);

	hyperv210_rtc_setaie(0);

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 06 : 0x%x ] \r\n", nWdog_value );
#endif

	iounmap(hyperv210_rtc_base);
	release_mem_region(res->start, res->end - res->start + 1);

	return 0;
}

static int __devinit hyperv210_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	struct resource *res;
	int ret;
	struct rtc_time tm;

	pr_debug("%s: probe=%p\n", __func__, pdev);

	/* find the IRQs */
	hyperv210_rtc_alarmno = platform_get_irq(pdev, 0);
	if (hyperv210_rtc_alarmno < 0) {
		dev_err(&pdev->dev, "no irq for alarm\n");
		return -ENOENT;
	}

	pr_debug("hyperv210_rtc: alarm irq %d\n", hyperv210_rtc_alarmno);

	/* get the memory region */

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get memory region resource\n");
		return -ENOENT;
	}

	hyperv210_rtc_mem = request_mem_region(res->start,
					res->end - res->start + 1,
					pdev->name);

	if (hyperv210_rtc_mem == NULL) {
		dev_err(&pdev->dev, "failed to reserve memory region\n");
		ret = -ENOENT;
		goto err_nores;
	}

	hyperv210_rtc_base = ioremap(res->start, res->end - res->start + 1);
	if (hyperv210_rtc_base == NULL) {
		dev_err(&pdev->dev, "failed ioremap()\n");
		ret = -EINVAL;
		goto err_nomap;
	}

	pr_debug("hyperv210_rtc: RTCCON=%02x\n",
		readw(hyperv210_rtc_base + S3C2410_RTCCON));

	device_init_wakeup(&pdev->dev, 1);
#if 0 //crztech 2011 04 20
	/* Set the default time. 2011:1:1:12:0:0 */
	tm.tm_year = 111;
	tm.tm_mon = 0;
	tm.tm_mday = 1;
	tm.tm_hour = 12;
	tm.tm_min = 0;
	tm.tm_sec = 0;

	hyperv210_rtc_settime(&pdev->dev, &tm);
#endif
	/* register RTC and exit */
	rtc = rtc_device_register("s3c", &pdev->dev, &hyperv210_rtcops,
				  THIS_MODULE);

	if (IS_ERR(rtc)) {
		dev_err(&pdev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc);
		goto err_nortc;
	}

	platform_set_drvdata(pdev, rtc);

#if RTC_CHK_FLAG
	nWdog_value = readl(hyperv210_rtc_base + S3C2410_RTCCON);
	//printk("[ JAY RTC TEST 07 : 0x%x ] \r\n", nWdog_value );
#endif

	return 0;

 err_nortc:
	iounmap(hyperv210_rtc_base);

 err_nomap:
	release_mem_region(res->start, res->end - res->start + 1);

 err_nores:
	return ret;
}

#ifdef CONFIG_PM

/* RTC Power management control */
static int ticnt_save;

static int hyperv210_rtc_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* save TICNT for anyone using periodic interrupts */
	ticnt_save = readl(hyperv210_rtc_base + S3C2410_TICNT);

	if (device_may_wakeup(&pdev->dev))
		enable_irq_wake(hyperv210_rtc_alarmno);

	return 0;
}

static int hyperv210_rtc_resume(struct platform_device *pdev)
{
	writel(ticnt_save, hyperv210_rtc_base + S3C2410_TICNT);

	if (device_may_wakeup(&pdev->dev))
		disable_irq_wake(hyperv210_rtc_alarmno);

	return 0;
}
#else
#define hyperv210_rtc_suspend NULL
#define hyperv210_rtc_resume  NULL
#endif

static struct platform_driver hyperv210_rtc_driver = {
	.probe		= hyperv210_rtc_probe,
	.remove		= __devexit_p(hyperv210_rtc_remove),
	.suspend	= hyperv210_rtc_suspend,
	.resume		= hyperv210_rtc_resume,
	.driver		= {
		.name	= "hyperv210-rtc",
		.owner	= THIS_MODULE,
	},
};

static char __initdata banner[] = "Hyper-V210 RTC, (c) 2010 \n";

static int __init hyperv210_rtc_init(void)
{
	printk(banner);
	
#if 0
	gpio_request(S5PV210_GPD1(2), "GPD12");
	s3c_gpio_cfgpin(S5PV210_GPD1(2), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(S5PV210_GPD1(2), S3C_GPIO_PULL_UP);
	gpio_direction_output(S5PV210_GPD1(2), 1);
	gpio_free(S5PV210_GPD1(2));

	gpio_request(S5PV210_GPD1(3), "GPD13");
	s3c_gpio_cfgpin(S5PV210_GPD1(3), S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(S5PV210_GPD1(3), S3C_GPIO_PULL_UP);
	gpio_direction_output(S5PV210_GPD1(3), 1);
	gpio_free(S5PV210_GPD1(3));
#endif
	
	return platform_driver_register(&hyperv210_rtc_driver);
}

static void __exit hyperv210_rtc_exit(void)
{
	platform_driver_unregister(&hyperv210_rtc_driver);
}

module_init(hyperv210_rtc_init);
module_exit(hyperv210_rtc_exit);

MODULE_DESCRIPTION("Samsung HyperV210 RTC Driver");
MODULE_AUTHOR("samsung");
MODULE_LICENSE("GPL");
