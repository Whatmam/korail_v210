/* linux/arch/arm/mach-s5pv210/mach-smdkv210.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/gpio.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/regulator/max8698.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/usb/ch9.h>
#include <linux/pwm_backlight.h>
#include <linux/spi/spi.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/setup.h>
#include <asm/mach-types.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-mem.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>
#include <mach/ts.h>
#include <mach/adc.h>

#include <media/s5k3ba_platform.h>
#include <media/s5k4ba_platform.h>
#include <media/s5k4ea_platform.h>
#include <media/s5k6aa_platform.h>

#include <plat/regs-serial.h>
#include <plat/s5pv210.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/fb.h>
#include <plat/gpio-cfg.h>
#include <plat/iic.h>
#include <plat/spi.h>
#include <plat/fimc.h>
#include <plat/csis.h>
#include <plat/mfc.h>
#include <plat/sdhci.h>
#include <plat/ide.h>
#include <plat/regs-otg.h>
#include <plat/clock.h>
#include <mach/gpio-bank-b.h>
#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#include <plat/media.h>
#endif

#if defined(CONFIG_PM)
#include <plat/pm.h>
#endif

#include <linux/serial_8250.h> //jay Serial add.
#include <linux/interrupt.h>
//#include <asm/irq.h>
#include <linux/irq.h>

// jay 140109 - QT Backlight control - add
//////
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <mach/pd.h>
#include <plat/regs-timer.h>

//#define KERNEL_SW_VERSION			"version 1.5"

//PWM 0
//#define TIMER_0_START_STOP				0
//#define TIMER_0_MANUAL_UPDATE		1
//#define TIMER_0_OUTPUT_INVERTER		2
//#define TIMER_0_AUTO_RELOAD			3

//PWM 1
#define TIMER_1_START_STOP			8		
#define TIMER_1_MANUAL_UPDATE		9
#define TIMER_1_OUTPUT_INVERTER		10
#define TIMER_1_AUTO_RELOAD			11

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define S5PV210_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define S5PV210_ULCON_DEFAULT	S3C2410_LCON_CS8

#define S5PV210_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

//jay serial add.
#define DW8         0
#define DW16        1
#define AM_WORD     0
#define AM_BYTE     1
#define _BIT1(x)    ((x)&0x1)

#define BF_SROM_BW(cs,be,we,am,dw)  ( ( (_BIT1(be)<<3) | (_BIT1(we)<<2) | (_BIT1(am)<<1) | (_BIT1(dw)<<0) ) << (cs*5) )

#define VALUE_SROM_BW   (           /* CS,BE,WE, AM     , DW  */ \
                            BF_SROM_BW( 0, 0, 0, AM_BYTE, DW16) |\
                            BF_SROM_BW( 1, 1, 0, AM_BYTE, DW8) |\
                            BF_SROM_BW( 2, 0, 0, AM_BYTE, DW16) |\
                            BF_SROM_BW( 3, 1, 0, AM_BYTE, DW8) |\
                            BF_SROM_BW( 4, 0, 0, AM_BYTE, DW16) |\
                            BF_SROM_BW( 5, 0, 0, AM_BYTE, DW16) |\
                            BF_SROM_BW( 6, 0, 0, AM_BYTE, DW16)  \
                        )

#define BF_TACS(x)      (((x)&0x0f)<<28)
#define BF_TCOS(x)      (((x)&0x0f)<<24)
#define BF_TACC(x)      (((x)&0x1f)<<16)
#define BF_TCOH(x)      (((x)&0x0f)<<12)
#define BF_TCAH(x)      (((x)&0x0f)<<8 )
#define BF_TACP(x)      (((x)&0x0f)<<4 )

#define VALUE_SROM_BC5  ( BF_TACS(0) | BF_TCOS(4) | BF_TACC(0xd) | BF_TCOH(1) | BF_TCAH(4) | BF_TACP(6) )


extern void s5pv210_reserve_bootmem(void);
extern void s3c_sdhci_set_platdata(void);

static void kernel_version_view(void)
{
#if 0
	printk("\r\n");
	printk("\r\n");
	printk("##  ##    ###     ######         #       ###### ###      \r\n");
	printk("## ##   ##   ##   ##    ##     ## ##       ##   ###      \r\n");
	printk("####  ##       ## ##    ##    ##   ##      ##   ###      \r\n");
	printk("##### ##       ## ######     #########     ##   ###    ##\r\n");
	printk("## ##   ##   ##   ##   ##   ##       ##    ##   #########\r\n");
	printk("##  ##    ###     ##    ## ##         ## ###### ######## \r\n");
#endif
	printk("\r\n");
	printk("               ** Solidtek kernel %s **                \r\n", KERNEL_SW_VERSION);
	printk("\r\n");	
}

static struct s3c2410_uartcfg smdkv210_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= S5PV210_UCON_DEFAULT,
		.ulcon		= S5PV210_ULCON_DEFAULT,
		.ufcon		= S5PV210_UFCON_DEFAULT,
	},
};

/* PMIC */
#ifdef CONFIG_REGULATOR_MAX8698
static struct regulator_consumer_supply buck1_consumers[] = {
	{
		.supply		= "vddarm",
	},
};

static struct regulator_init_data max8698_buck1_data = {
	.constraints	= {
		.name		= "VCC_ARM",
		.min_uV		=  750000,
		.max_uV		= 1500000,
		.always_on	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE,
		.state_mem	= {
			.uV	= 1250000,
			.mode	= REGULATOR_MODE_NORMAL,
			.enabled = 0,
		},
	},
	.num_consumer_supplies	= ARRAY_SIZE(buck1_consumers),
	.consumer_supplies	= buck1_consumers,
};

static struct regulator_consumer_supply buck2_consumers[] = {
	{
		.supply		= "vddint",
	},
};

static struct regulator_init_data max8698_buck2_data = {
	.constraints	= {
		.name		= "VCC_INTERNAL",
		.min_uV		= 950000,
		.max_uV		= 1200000,
		.always_on	= 1,
		.valid_ops_mask	= REGULATOR_CHANGE_VOLTAGE,
		.state_mem	= {
			.uV	= 1100000,
			.mode	= REGULATOR_MODE_NORMAL,
			.enabled = 0,
		},
	},
	.num_consumer_supplies	= ARRAY_SIZE(buck2_consumers),
	.consumer_supplies	= buck2_consumers,
};

static struct regulator_init_data max8698_buck3_data = {
	.constraints	= {
		.name		= "VCC_MEM",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.state_mem	= {
			.uV	= 1800000,
			.mode	= REGULATOR_MODE_NORMAL,
			.enabled = 1,
		},
		/* .initial_state	= PM_SUSPEND_MEM, */
	},
};

static struct regulator_init_data max8698_ldo2_data = {
	.constraints	= {
		.name		= "VALIVE_1.1V",
		.min_uV		= 1100000,
		.max_uV		= 1100000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.uV	= 1100000,
			.mode	= REGULATOR_MODE_NORMAL,
			.enabled = 1,
		},
	},
};

static struct regulator_init_data max8698_ldo3_data = {
	.constraints	= {
		.name		= "VUOTG_D_1.1V/VUHOST_D_1.1V",
		.min_uV		= 1100000,
		.max_uV		= 1100000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 1100000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,	/* LDO3 should be OFF in sleep mode */
		},
	},
};

static struct regulator_init_data max8698_ldo4_data = {
	.constraints	= {
		.name		= "V_MIPI_1.8V",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.boot_on	= 1,
		.state_mem = {
			.uV		= 1800000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 0,	/* LDO4 should be OFF in sleep mode */
		},
	},
};

static struct regulator_init_data max8698_ldo5_data = {
	.constraints	= {
		.name		= "VMMC_2.8V/VEXT_2.8V",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 2800000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,
		},
	},
};

static struct regulator_init_data max8698_ldo6_data = {
	.constraints	= {
		.name		= "VCC_2.6V",
		.min_uV		= 2600000,
		.max_uV		= 2600000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 2600000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,
		},
	},
};

static struct regulator_init_data max8698_ldo7_data = {
	.constraints	= {
		.name		= "VDAC_2.8V",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 2800000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,
		},
	},
};

static struct regulator_init_data max8698_ldo8_data = {
	.constraints	= {
		.name		= "VUOTG_A_3.3V/VUHOST_A_3.3V",
		.min_uV		= 3300000,
		.max_uV		= 3300000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 0,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,	/* LDO8 should be OFF in sleep mode */
		},
	},
};

static struct regulator_init_data max8698_ldo9_data = {
	.constraints	= {
		.name		= "{VADC/VSYS/VKEY}_2.8V",
		.min_uV		= 3000000,
		.max_uV		= 3000000,
		.apply_uV	= 1,
		.state_mem = {
			.uV		= 3000000,
			.mode		= REGULATOR_MODE_NORMAL,
			.enabled	= 1,
		},
	},
};

static struct max8698_subdev_data smdkc110_regulators[] = {
	{ MAX8698_LDO2, &max8698_ldo2_data },
	{ MAX8698_LDO3, &max8698_ldo3_data },
	{ MAX8698_LDO4, &max8698_ldo4_data },
	{ MAX8698_LDO5, &max8698_ldo5_data },
	{ MAX8698_LDO6, &max8698_ldo6_data },
	{ MAX8698_LDO7, &max8698_ldo7_data },
	{ MAX8698_LDO8, &max8698_ldo8_data },
	{ MAX8698_LDO9, &max8698_ldo9_data },
	{ MAX8698_BUCK1, &max8698_buck1_data },
	{ MAX8698_BUCK2, &max8698_buck2_data },
	{ MAX8698_BUCK3, &max8698_buck3_data },
};

#if 0
/* 800MHz default voltage */
static struct max8698_platform_data max8698_platform_data_0 = {
	.num_regulators	= ARRAY_SIZE(smdkc110_regulators),
	.regulators	= smdkc110_regulators,

	.set1		= S5PV210_GPH1(6),
	.set2		= S5PV210_GPH1(7),
	.set3		= S5PV210_GPH0(4),

	.dvsarm1	= 0x8,
	.dvsarm2	= 0x6,
	.dvsarm3	= 0x5,
	.dvsarm4	= 0x5,

	.dvsint1	= 0x9,
	.dvsint2	= 0x5,
};
#endif
/* 1Ghz default voltage */
static struct max8698_platform_data max8698_platform_data = {
	.num_regulators = ARRAY_SIZE(smdkc110_regulators),
	.regulators     = smdkc110_regulators,

	.set1           = S5PV210_GPH1(6),
	.set2           = S5PV210_GPH1(7),
	.set3           = S5PV210_GPH0(4),
	.dvsarm1        = 0xa,  // 1.25v
	.dvsarm2        = 0x9,  // 1.20V
	.dvsarm3        = 0x6,  // 1.05V
	.dvsarm4        = 0x4,  // 0.95V

	.dvsint1        = 0x7,  // 1.10v
	.dvsint2        = 0x5,  // 1.00V
};
#endif

#ifdef CONFIG_TOUCHSCREEN_S3C
static struct s3c_ts_mach_info s3c_ts_platform __initdata = {
	.delay                  = 30000,
	.presc                  = 49,
	.oversampling_shift     = 2,
	.resol_bit              = 12,
	.s3c_adc_con            = ADC_TYPE_2,
};
#endif

#ifdef CONFIG_S5PV210_ADC
static struct s3c_adc_mach_info s3c_adc_platform __initdata = {
	/* s5pc100 supports 12-bit resolution */
	.delay  = 10000,
	.presc  = 49,
	.resolution = 12,
};
#endif

// jay serial add.
#ifdef CONFIG_SERIAL_8250 //16000000 //3686400 //14745600
#define UART_SETUP( _addr, _irq, _regshift )   \
{                                              \
    .membase    = 0,                           \
    .mapbase    = (_addr),                     \
    .irq        = (_irq),                      \
    .irqflags   = IRQF_TRIGGER_RISING,         \
    .flags      = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_IOREMAP, \
    .iotype     = UPIO_MEM,                    \
    .regshift   = (_regshift),                 \
    .uartclk    = 14745600,                    \
}

#define UART_SIM_SETUP( _addr, _irq, _regshift )   \
{                                              \
    .membase    = 0,                           \
    .mapbase    = (_addr),                     \
    .irq        = (_irq),                      \
    .irqflags   = IRQF_TRIGGER_RISING,         \
    .flags      = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_IOREMAP, \
    .iotype     = UPIO_MEM,                    \
    .regshift   = (_regshift),                 \
    .uartclk    = 3600000,                    	\
}

static struct plat_serial8250_port korial_8250_data[] = {
    UART_SETUP(DIV_Y0_EUA, IRQ_EINT7, 1),
    UART_SETUP(DIV_Y1_EUB, IRQ_EINT8, 1),
    UART_SETUP(DIV_Y2_EUC, IRQ_EINT9, 1),
    UART_SETUP(DIV_Y3_EUD, IRQ_EINT10, 1),
    UART_SIM_SETUP(DIV_UART_NCSA, IRQ_EINT(26), 1),
    UART_SIM_SETUP(DIV_UART_NCSB, IRQ_EINT(27), 1),
    {(struct plat_serial8250_port *)NULL}
};

static struct platform_device korial_8250_device = {
    .name   = "serial8250",
    .id = 0,
    .dev    = {
        .platform_data  = &korial_8250_data,
    },
};
#endif

#ifdef CONFIG_VIDEO_FIMC
/*
 * External camera reset
 * Because the most of cameras take i2c bus signal, so that
 * you have to reset at the boot time for other i2c slave devices.
 * This function also called at fimc_init_camera()
 * Do optimization for cameras on your platform.
*/
static int smdkv210_cam0_power(int onoff)
{
	int err;
	
	//printk("[MMM] CAM0 Power \r\n");
	
	/* Camera A */
	err = gpio_request(S5PV210_GPH0(2), "GPH0");
	if (err)
		printk(KERN_ERR "#### failed to request GPH0 for CAM_2V8\n");

	s3c_gpio_setpull(S5PV210_GPH0(2), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV210_GPH0(2), 0);
	gpio_direction_output(S5PV210_GPH0(2), 1);
	gpio_free(S5PV210_GPH0(2));

	return 0;
}

static int smdkv210_cam1_power(int onoff)
{
	int err;
	
	//printk("[MMM] CAM1 Power \r\n");

	/* Camera B */
	err = gpio_request(S5PV210_GPH0(3), "GPH0");
	if (err)
		printk(KERN_ERR "#### failed to request GPH0 for CAM_2V8\n");

	s3c_gpio_setpull(S5PV210_GPH0(3), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV210_GPH0(3), 0);
	gpio_direction_output(S5PV210_GPH0(3), 1);
	gpio_free(S5PV210_GPH0(3));

	return 0;
}

static int smdkv210_mipi_cam_reset(void)
{
	int err;
	
	//printk("[MMM] MIPI Power \r\n");

	err = gpio_request(S5PV210_GPH0(3), "GPH0");
	if (err)
		printk(KERN_ERR "#### failed to reset(GPH0) for MIPI CAM\n");

	s3c_gpio_setpull(S5PV210_GPH0(3), S3C_GPIO_PULL_NONE);
	gpio_direction_output(S5PV210_GPH0(3), 0);
	gpio_direction_output(S5PV210_GPH0(3), 1);
	gpio_free(S5PV210_GPH0(3));

	return 0;
}

/*
 * Guide for Camera Configuration for smdkv210
 * ITU channel must be set as A or B
 * ITU CAM CH A: S5K3BA only
 * ITU CAM CH B: one of S5K3BA and S5K4BA
 * MIPI: one of S5K4EA and S5K6AA
 *
 * NOTE1: if the S5K4EA is enabled, all other cameras must be disabled
 * NOTE2: currently, only 1 MIPI camera must be enabled
 * NOTE3: it is possible to use both one ITU cam and
 * 	  one MIPI cam except for S5K4EA case
 *
*/
#undef CAM_ITU_CH_A
#undef WRITEBACK_ENABLED

#ifdef CONFIG_VIDEO_S5K4EA
#define S5K4EA_ENABLED
/* undef : 3BA, 4BA, 6AA */
#else
#ifdef CONFIG_VIDEO_S5K6AA
#define S5K6AA_ENABLED
/* undef : 4EA */
#else
#ifdef CONFIG_VIDEO_S5K3BA
#define S5K3BA_ENABLED
/* undef : 4BA */
#elif CONFIG_VIDEO_S5K4BA
#define S5K4BA_ENABLED
/* undef : 3BA */
#endif
#endif
#endif

/* External camera module setting */
/* 2 ITU Cameras */
#ifdef S5K3BA_ENABLED
static struct s5k3ba_platform_data s5k3ba_plat = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_VYUY,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k3ba_i2c_info = {
	I2C_BOARD_INFO("S5K3BA", 0x2d),
	.platform_data = &s5k3ba_plat,
};

static struct s3c_platform_camera s5k3ba = {
#ifdef CAM_ITU_CH_A
	.id		= CAMERA_PAR_A,
#else
	.id		= CAMERA_PAR_B,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CRYCBY,
	.i2c_busnum	= 0,
	.info		= &s5k3ba_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_VYUY,
	.srclk_name	= "mout_epll",
	.clk_name	= "sclk_cam1",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
#ifdef CAM_ITU_CH_A
	.cam_power	= smdkv210_cam0_power,
#else
	.cam_power	= smdkv210_cam1_power,
#endif
};
#endif

#ifdef S5K4BA_ENABLED
static struct s5k4ba_platform_data s5k4ba_plat = {
	.default_width = 800,
	.default_height = 600,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 44000000,
	.is_mipi = 0,
};

static struct i2c_board_info  s5k4ba_i2c_info = {
	I2C_BOARD_INFO("S5K4BA", 0x2d),
	.platform_data = &s5k4ba_plat,
};

static struct s3c_platform_camera s5k4ba = {
	.id		= CAMERA_PAR_B,
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &s5k4ba_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_cam1",
	.clk_rate	= 44000000,
	.line_length	= 1920,
	.width		= 800,
	.height		= 600,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 800,
		.height	= 600,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
#ifdef CAM_ITU_CH_A
	.cam_power	= smdkv210_cam0_power,
#else
	.cam_power	= smdkv210_cam1_power,
#endif
};
#endif

/* 2 MIPI Cameras */
#ifdef S5K4EA_ENABLED
static struct s5k4ea_platform_data s5k4ea_plat = {
	.default_width = 1920,
	.default_height = 1080,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 1,
};

static struct i2c_board_info  s5k4ea_i2c_info = {
	I2C_BOARD_INFO("S5K4EA", 0x2d),
	.platform_data = &s5k4ea_plat,
};

static struct s3c_platform_camera s5k4ea = {
	.id		= CAMERA_CSI_C,
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &s5k4ea_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "mout_mpll",
	.clk_name	= "sclk_cam0",
	.clk_rate	= 48000000,
	.line_length	= 1920,
	.width		= 1920,
	.height		= 1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1920,
		.height	= 1080,
	},

	.mipi_lanes	= 2,
	.mipi_settle	= 12,
	.mipi_align	= 32,

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
	.cam_power	= smdkv210_mipi_cam_power,
};
#endif

#ifdef S5K6AA_ENABLED
static struct s5k6aa_platform_data s5k6aa_plat = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 1,
};

static struct i2c_board_info  s5k6aa_i2c_info = {
	I2C_BOARD_INFO("S5K6AA", 0x3c),
	.platform_data = &s5k6aa_plat,
};

static struct s3c_platform_camera s5k6aa = {
	.id		= CAMERA_CSI_C,
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &s5k6aa_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "xusbxti",
	.clk_name	= "sclk_cam",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	/* default resol for preview kind of thing */
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	.mipi_lanes	= 1,
	.mipi_settle	= 6,
	.mipi_align	= 32,

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,

	.initialized	= 0,
	.cam_power	= smdkv210_mipi_cam_power,
};
#endif

#ifdef WRITEBACK_ENABLED
static struct i2c_board_info  writeback_i2c_info = {
	I2C_BOARD_INFO("WriteBack", 0x0),
};

static struct s3c_platform_camera writeback = {
	.id		= CAMERA_WB,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &writeback_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUV444,
	.line_length	= 800,
	.width		= 480,
	.height		= 800,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 480,
		.height	= 800,
	},

	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_MT9D111
#include <media/mt9d111_platform.h>
/*SDKIM*/
static struct mt9d111_platform_data mt9d111 = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 0,
};
static struct i2c_board_info  __initdata camera_info[] = {
	{
		I2C_BOARD_INFO("MT9D111", 0x5d),
		.platform_data = &mt9d111,
	},
};

static struct s3c_platform_camera camera_a = {
	.id     = CAMERA_PAR_A,     /* FIXME */
	.type       = CAM_TYPE_ITU,     /* 2.0M ITU */
	.fmt        = ITU_601_YCBCR422_8BIT,
	.order422   = CAM_ORDER422_8BIT_CBYCRY,/* SDKIM */
	.i2c_busnum = 0,
	.info       = &camera_info[0],
	.pixelformat    = V4L2_PIX_FMT_YUYV,
	.srclk_name = "mout_mpll",
	.clk_name   = "sclk_cam0",
	.clk_rate   = 24000000,     /* 44MHz -> 24MHz SDKIM*/
	.line_length    = 1920,         /* 1280*1024 */
	/* default resol for preview kind of thing */
	.width      = 640,
	.height     = 480,
	.window     = {
		.left   = 0,
		.top    = 0,
		.width  = 640,
		.height = 480,
	},
	
	/* Polarity */
	.inv_pclk   = 0,
	.inv_vsync  = 1,/* SDKIM */
	.inv_href   = 0,
	.inv_hsync  = 0,
	.initialized = 0,
};
#endif
/* Interface setting */
static struct s3c_platform_fimc fimc_plat = {
	.srclk_name	= "mout_mpll",
	.clk_name	= "fimc",
	.clk_rate	= 166000000,
#if defined(S5K4EA_ENABLED) || defined(S5K6AA_ENABLED)
	.default_cam	= CAMERA_CSI_C,
#else

#ifdef WRITEBACK_ENABLED
	.default_cam	= CAMERA_WB,
#elif CAM_ITU_CH_A
	.default_cam	= CAMERA_PAR_A,
#else
	.default_cam	= CAMERA_PAR_B,
#endif

#endif
	.camera		= {
#ifdef S5K3BA_ENABLED
		&s5k3ba,
#endif
#ifdef S5K4BA_ENABLED
		&s5k4ba,
#endif
#ifdef S5K4EA_ENABLED
		&s5k4ea,
#endif
#ifdef S5K6AA_ENABLED
		&s5k6aa,
#endif
#ifdef WRITEBACK_ENABLED
		&writeback,
#endif
#ifdef CONFIG_VIDEO_MT9D111
		&camera_a,
#endif
	},
	.hw_ver		= 0x43,
};
#endif

#if defined(CONFIG_SPI_CNTRLR_0) || defined(CONFIG_SPI_CNTRLR_1) || defined(CONFIG_SPI_CNTRLR_2)
static void s3c_cs_suspend(int pin, pm_message_t pm)
{
        /* Whatever need to be done */
}

static void s3c_cs_resume(int pin)
{
        /* Whatever need to be done */
}

static void s3c_cs_set(int pin, int lvl)
{
        if(lvl == CS_HIGH)
           s3c_gpio_setpin(pin, 1);
        else
           s3c_gpio_setpin(pin, 0);
}
static void s3c_cs_config(int pin, int mode, int pull)
{
        s3c_gpio_cfgpin(pin, mode);

        if(pull == CS_HIGH){
           s3c_gpio_setpull(pin, S3C_GPIO_PULL_UP);
		   s3c_gpio_setpin(pin, 0);
		}
        else{
           s3c_gpio_setpull(pin, S3C_GPIO_PULL_DOWN);
		   s3c_gpio_setpin(pin, 1);
		}
}
#endif

#if defined(CONFIG_SPI_CNTRLR_0)
static struct s3c_spi_pdata s3c_slv_pdata_0[] __initdata = {
        [0] = { /* Slave-0 */
                .cs_level     = CS_FLOAT,
                .cs_pin       = S5PV210_GPB(1),
                .cs_mode      = S5PV210_GPB_OUTPUT(1),
                .cs_set       = s3c_cs_set,
                .cs_config    = s3c_cs_config,
                .cs_suspend   = s3c_cs_suspend,
                .cs_resume    = s3c_cs_resume,
        },
        #if 0
        [1] = { /* Slave-1 */
                .cs_level     = CS_FLOAT,
                .cs_pin       = S5PV210_GPA1(1),
                .cs_mode      = S5PV210_GPA1_OUTPUT(1),
                .cs_set       = s3c_cs_set,
                .cs_config    = s3c_cs_config,
                .cs_suspend   = s3c_cs_suspend,
                .cs_resume    = s3c_cs_resume,
        },
        #endif
};
#endif

#if defined(CONFIG_SPI_CNTRLR_1)
static struct s3c_spi_pdata s3c_slv_pdata_1[] __initdata = {
        [0] = { /* Slave-0 */
                .cs_level     = CS_FLOAT,
                .cs_pin       = S5PV210_GPB(5),
                .cs_mode      = S5PV210_GPB_OUTPUT(5),
                .cs_set       = s3c_cs_set,
                .cs_config    = s3c_cs_config,
                .cs_suspend   = s3c_cs_suspend,
                .cs_resume    = s3c_cs_resume,
        },
		#if 0
        [1] = { /* Slave-1 */
                .cs_level     = CS_FLOAT,
                .cs_pin       = S5PV210_GPA1(3),
                .cs_mode      = S5PV210_GPA1_OUTPUT(3),
                .cs_set       = s3c_cs_set,
                .cs_config    = s3c_cs_config,
                .cs_suspend   = s3c_cs_suspend,
                .cs_resume    = s3c_cs_resume,
        },
		#endif
};
#endif

static struct spi_board_info s3c_spi_devs[] __initdata = {
#if defined(CONFIG_SPI_CNTRLR_0)
        [0] = {
                .modalias        = "spidev", /* Test Interface */
                .mode            = SPI_MODE_0,  /* CPOL=0, CPHA=0 */
                .max_speed_hz    = 100000,
                /* Connected to SPI-0 as 1st Slave */
                .bus_num         = 0,
                .irq             = IRQ_SPI0,
                .chip_select     = 0,
        },
		#if 0
        [1] = {
                .modalias        = "spidev", /* Test Interface */
                .mode            = SPI_MODE_0,  /* CPOL=0, CPHA=0 */
                .max_speed_hz    = 100000,
                /* Connected to SPI-0 as 2nd Slave */
                .bus_num         = 0,
                .irq             = IRQ_SPI0,
                .chip_select     = 1,
        },
		#endif
#endif

#if defined(CONFIG_SPI_CNTRLR_1)
        [1] = {
                .modalias        = "spidev", /* Test Interface */
                .mode            = SPI_MODE_0,  /* CPOL=0, CPHA=0 */
                .max_speed_hz    = 100000,
                /* Connected to SPI-1 as 1st Slave */
                .bus_num         = 1,
                .irq             = IRQ_SPI1,
                .chip_select     = 0,
        },
		#if 0
        [3] = {
                .modalias        = "spidev", /* Test Interface */
                .mode            = SPI_MODE_0 | SPI_CS_HIGH,    /* CPOL=0, CPHA=0 & CS is Active High */
                .max_speed_hz    = 100000,
                /* Connected to SPI-1 as 3rd Slave */
                .bus_num         = 1,
                .irq             = IRQ_SPI1,
                .chip_select     = 1,
        },
		#endif
#endif
};


#if defined(CONFIG_HAVE_PWM)
static struct platform_pwm_backlight_data smdk_backlight_data = {
	.pwm_id  = 1,
	.max_brightness = 255,
	.dft_brightness = 255,
	.pwm_period_ns  = 10000000, //3330000 (298) //5000000 (200) //6666667 (150) //10000000 (100 ) // 12000000 (80~) //14000000 (70~) // 20000000 (49.5) //25000000 (39.7)
};

static struct platform_device smdk_backlight_device = {
	.name      = "pwm-backlight",
	.id        = -1,
	.dev        = {
		.parent = &s3c_device_timer[1].dev,
		//.parent = &s3c_device_timer[0].dev,
		.platform_data = &smdk_backlight_data,
	},
};
static void __init smdk_backlight_register(void)
{
	int ret = platform_device_register(&smdk_backlight_device);
	if (ret)
		printk(KERN_ERR "smdk: failed to register backlight device: %d\n", ret);
}
#endif

#if defined(CONFIG_BLK_DEV_IDE_S3C)
static struct s3c_ide_platdata smdkv210_ide_pdata __initdata = {
	.setup_gpio     = s3c_ide_setup_gpio,
};
#endif

/* I2C0 */
static struct i2c_board_info i2c_devs0[] __initdata = {
#ifdef CONFIG_SND_SOC_WM8580
	{
		I2C_BOARD_INFO("wm8580", 0x1b),
	},
#endif

#ifdef CONFIG_SND_SOC_WM8960
    {
		I2C_BOARD_INFO("wm8960", (0x34>>1)),
    },
#endif
};

/* I2C1 */
static struct i2c_board_info i2c_devs1[] __initdata = {
#ifdef CONFIG_VIDEO_TV20
	{
		I2C_BOARD_INFO("s5p_ddc", (0x74>>1)),
	},
#endif

#ifdef CONFIG_RTC_DRV_DS1307
	{
		I2C_BOARD_INFO("ds1338", (0xD0>>1)),
	},
#endif
};

/* I2C2 */
static struct i2c_board_info i2c_devs2[] __initdata = {
#ifdef CONFIG_REGULATOR_MAX8698
	{
		/* The address is 0xCC used since SRAD = 0 */
		I2C_BOARD_INFO("max8698", (0xCC >> 1)),
		.platform_data = &max8698_platform_data,
	},
#endif
};

void korail_srom_cs_timing(char channel, unsigned int tmp){
	//volatile unsigned int tmp;
	unsigned long sromc_base = (unsigned long)S5P_SROM_BW;
	unsigned int addr_inc;
	
	addr_inc = channel * 4;

//	tmp =((0x5<<28)|(0x5<<24)|(0x8<<16)|(0x5<<12)|(0x5<<8)|(0x5<<4)|(0<<0));
	__raw_writel(tmp, sromc_base + addr_inc);

	/* SROM CS Setting */
	tmp = __raw_readl(S5PV210_MP01CON);
	tmp &= ~(0xf<<addr_inc);	//clr pin configuration
	tmp |=(0x2<<addr_inc);		//cs set pin configuration
	__raw_writel(tmp,(S5PV210_MP01CON));
}
EXPORT_SYMBOL(korail_srom_cs_timing);

#if defined(CONFIG_DM9000) || defined(CONFIG_SMSC911X)
void smdkv210_dm9000_set(void)
{
	volatile unsigned int tmp;
    unsigned long sromc_base = (unsigned long)S5P_SROM_BW;

    /* SROM Bus Width & Wait Contorol register setting */
    tmp = __raw_readl(sromc_base);
    tmp &= ~(0xf<<20);
    tmp |= (0x0<<23) | (0x0<<22) | (0x1<<21) | (0x1<<20);/* 16bit */
    __raw_writel(tmp, sromc_base);

#ifdef CONFIG_KORAIL_MAIN_BOARD
    /* CS5 Timming */
    //tmp = ((0<<28)|(0<<24)|(5<<16)|(0<<12)|(0<<8)|(0<<4)|(0<<0));
    tmp =((0x5<<28)|(0x5<<24)|(0x8<<16)|(0x5<<12)|(0x5<<8)|(0x5<<4)|(0<<0));
//    tmp =((0xA<<28)|(0xA<<24)|(0xA<<16)|(0xA<<12)|(0xA<<8)|(0xA<<4)|(0<<0));
    __raw_writel(tmp, sromc_base + 0x18);
#else
    tmp =((0x5<<28)|(0x5<<24)|(0x8<<16)|(0x5<<12)|(0x5<<8)|(0x5<<4)|(0<<0));
    __raw_writel(tmp, sromc_base + 0x18);
#endif

    /* CS5 Pullup */
    tmp = 0xAAAA;
    __raw_writel(tmp,(S5PV210_MP01PUD));

#if 1
    /* SROM OEn Setting */
    tmp = 0xffff;
    __raw_writel(tmp,(S5PV210_MP01DRV));
    udelay(100);
#endif
    /* SROM ADDR Strength Setting */
    tmp = 0xffff;
    __raw_writel(tmp, (S5PV210_MP04DRV));
    __raw_writel(tmp, (S5PV210_MP05DRV));

    /* SROM DATA Strength Setting */
    tmp = 0xffff;
    __raw_writel(tmp, (S5PV210_MP06DRV));
    __raw_writel(tmp, (S5PV210_MP07DRV));

    /* SROM CS5 Setting */
    tmp = __raw_readl(S5PV210_MP01CON);
    tmp &= ~(0xf<<20);
    tmp |=(0x2<<20);/*EBI_WEn EBI_OEn SROM_CS[5] */
    __raw_writel(tmp,(S5PV210_MP01CON));

    /* DM9000 Interrupt pin */
    s3c_gpio_cfgpin(S5PV210_GPH0(6), S3C_GPIO_SFN(0xf));
}
EXPORT_SYMBOL(smdkv210_dm9000_set);
#endif
#ifdef CONFIG_ANDROID_PMEM
static struct android_pmem_platform_data pmem_pdata = {
	.name = "pmem",
	.no_allocator = 1,
	.cached = 1,
	.start = 0, // will be set during proving pmem driver.
	.size = 0 // will be set during proving pmem driver.
};

static struct android_pmem_platform_data pmem_gpu1_pdata = {
   .name = "pmem_gpu1",
   .no_allocator = 1,
   .cached = 1,
   .buffered = 1,
   .start = 0,
   .size = 0,
};

static struct android_pmem_platform_data pmem_adsp_pdata = {
   .name = "pmem_adsp",
   .no_allocator = 1,
   .cached = 1,
   .buffered = 1,
   .start = 0,
   .size = 0,
};

static struct platform_device pmem_device = {
   .name = "android_pmem",
   .id = 0,
   .dev = { .platform_data = &pmem_pdata },
};

static struct platform_device pmem_gpu1_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &pmem_gpu1_pdata },
};

static struct platform_device pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &pmem_adsp_pdata },
};

static void __init android_pmem_set_platdata(void)
{
	pmem_pdata.start = (u32)s3c_get_media_memory_bank(S3C_MDEV_PMEM, 0);
	pmem_pdata.size = (u32)s3c_get_media_memsize_bank(S3C_MDEV_PMEM, 0);

	pmem_gpu1_pdata.start = (u32)s3c_get_media_memory_bank(S3C_MDEV_PMEM_GPU1, 0);
	pmem_gpu1_pdata.size = (u32)s3c_get_media_memsize_bank(S3C_MDEV_PMEM_GPU1, 0);

	pmem_adsp_pdata.start = (u32)s3c_get_media_memory_bank(S3C_MDEV_PMEM_ADSP, 0);
	pmem_adsp_pdata.size = (u32)s3c_get_media_memsize_bank(S3C_MDEV_PMEM_ADSP, 0);
}
#endif
struct platform_device sec_device_battery = {
	.name	= "sec-fake-battery",
	.id		= -1,
};

static void init_gpio_set(void)
{
	u32  gpio, irq;

    unsigned int nUART1_UMCON_Data;
	void __iomem *pUART1_UMCON_Addr;
	
	// sd card power enable
	//gpio_request(S5PV210_GPH0(7), "GPH07");
	//gpio_direction_output(S5PV210_GPH0(7), 1);
	//gpio_free(S5PV210_GPH0(7));

	//korail RF Module Reset
	gpio_request(S5PV210_GPG1(2), "GPG12");
	gpio_direction_output(S5PV210_GPG1(2), 0); 
    s3c_gpio_setpull(S5PV210_GPG1(2), S3C_GPIO_PULL_DOWN);
	//gpio_free(S5PV210_GPG1(2));

	gpio_request(S5PV210_GPG3(2), "GPG32");
	gpio_direction_output(S5PV210_GPG3(2), 1); 
    s3c_gpio_setpull(S5PV210_GPG1(2), S3C_GPIO_PULL_UP);
	//gpio_free(S5PV210_GPG3(2));
   
    gpio = S5PV210_GPH0(7);
    irq  = IRQ_EINT7;
    gpio_request(gpio, "GPH07");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    gpio = S5PV210_GPH1(0);
    irq  = IRQ_EINT8;
    gpio_request(gpio, "GPH10");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    gpio = S5PV210_GPH1(1);
    irq  = IRQ_EINT9;
    gpio_request(gpio, "GPH11");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    gpio = S5PV210_GPH1(2);
    irq  = IRQ_EINT10;
    gpio_request(gpio, "GPH12");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    gpio = S5PV210_GPH3(2);
    irq  = IRQ_EINT(26);
    gpio_request(gpio, "GPH32");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    gpio = S5PV210_GPH3(3);
    irq  = IRQ_EINT(27);
    gpio_request(gpio, "GPH33");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0xf));
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    set_irq_type(irq , IRQF_TRIGGER_RISING);
    //gpio_free(gpio);

    //uart1 CTSn. //485
	gpio = S5PV210_GPA0(6);
	gpio_request(gpio, "GPA06");
	s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT); //UART_1_CTSn( 2 ) //Output Mode(1) //Imput (0)
	s3c_gpio_setpin(gpio, 0);	
	//gpio_free(gpio);

	//uart1 RTSn.
	//gpio = S5PV210_GPA0(7);
	//gpio_request(gpio, "GPA07");
	//s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(0x02)); //UART_1_RTSn
	//gpio_free(gpio);	
	
	//uart AFC Enable
    //pUART1_UMCON_Addr = ioremap(S5P_PA_UART1_UMCON , 0x0040);
	//nUART1_UMCON_Data = __raw_readl(pUART1_UMCON_Addr);
	//nUART1_UMCON_Data |= ( 1 << 4 ); 
	//__raw_writel( nUART1_UMCON_Data , pUART1_UMCON_Addr );

	gpio = S5PV210_GPG3(1); 		//Latent watch dog EN
    gpio_request(gpio, "GPG31");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
    s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPE0(0); 		//Latent watch dog CLK
    gpio_request(gpio, "GPE00");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
    s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPD1(4); //gpio = S5PV210_GPD1(2); //ds1338 rtc i2c
    gpio_request(gpio, "GPD14"); //gpio_request(gpio, "GPE12");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
    s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPD1(5); //gpio = S5PV210_GPD1(3); //ds1338 rtc i2c
    gpio_request(gpio, "GPD15"); //gpio_request(gpio, "GPE13");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
    s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);


    gpio = S5PV210_GPE0(4); //ttl io en
    gpio_request(gpio, "GPE04");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpull(gpio, S3C_GPIO_PULL_DOWN);
    s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);


    //---------------------------------------------------------------------

    gpio = S5PV210_GPG3(5);      //Latent LED IF
    gpio_request(gpio, "GPG35");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPG3(4); 	//Latent LED IF
    gpio_request(gpio, "GPG34");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPG3(3); 	//Latent LED IF
    gpio_request(gpio, "GPG33");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPE1(1); 
    gpio_request(gpio, "GPE11");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPE1(2); 
    gpio_request(gpio, "GPE12");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
    //s3c_gpio_setpin(gpio, 0);
    //gpio_free(gpio);

    gpio = S5PV210_GPH2(0); 
    gpio_request(gpio, "GPH20");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPH2(1); 
    gpio_request(gpio, "GPH21");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPH2(2); 
    gpio_request(gpio, "GPH22");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPH2(3); 
    gpio_request(gpio, "GPH23");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPH3(1); 
    gpio_request(gpio, "GPH31");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPH1(3); 
    gpio_request(gpio, "GPH13");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);

    gpio = S5PV210_GPG3(6);      //Latent LCD Mode Select // HW - PULL DOWN
    gpio_request(gpio, "GPG36");
    s3c_gpio_cfgpin(gpio, S3C_GPIO_INPUT);
    //s3c_gpio_setpin(gpio, 1);
    //gpio_free(gpio);
}

static struct platfrom_device *korail_devices[] __initdata ={
#ifdef CONFIG_SERIAL_8250 //jay serial add.
	&korial_8250_device,
#endif
};

static struct platfrom_device *latent_devices[] __initdata ={
#ifndef CONFIG_KORAIL_DEVICE_DS1338 //CONFIG_KORAIL_DEVICE
        &s3c_device_i2c1,
#endif
};
static struct platform_device *smdkv210_devices[] __initdata = {
#ifdef CONFIG_MTD_ONENAND
	&s3c_device_onenand,
#endif
#ifdef CONFIG_MTD_NAND
	&s3c_device_nand,
#endif
#ifdef CONFIG_FB_S3C
	&s3c_device_fb,
#endif
	&s3c_device_keypad,
#ifdef CONFIG_TOUCHSCREEN_S3C
	&s3c_device_ts,
#endif
#ifdef CONFIG_S5PV210_ADC
	&s3c_device_adc,
#endif
/*
#ifdef CONFIG_SERIAL_8250 //jay serial add.
    &korial_8250_device,
#endif
*/
#ifdef CONFIG_DM9000
	&s5p_device_dm9000,
#endif

#if defined(CONFIG_SMSC911X)
    &s5p_device_smsc911x,
#endif

#ifdef CONFIG_S3C2410_WATCHDOG
	&s3c_device_wdt,
#endif
#if defined(CONFIG_BLK_DEV_IDE_S3C)
	&s3c_device_cfcon,
#endif
#ifdef CONFIG_RTC_DRV_S3C
	&s5p_device_rtc,
#endif
#ifdef CONFIG_HAVE_PWM
	&s3c_device_timer[0],
	&s3c_device_timer[1],
	&s3c_device_timer[2],
	&s3c_device_timer[3],
#endif
#ifdef CONFIG_SND_S3C24XX_SOC
	&s3c64xx_device_iis0,
#endif
#ifdef CONFIG_SND_S3C_SOC_AC97
	&s3c64xx_device_ac97,
#endif
#ifdef CONFIG_SND_S3C_SOC_PCM
	&s3c64xx_device_pcm1,
#endif
#ifdef CONFIG_SND_S5P_SOC_SPDIF
	&s5p_device_spdif,
#endif
#ifdef CONFIG_VIDEO_FIMC
	&s3c_device_fimc0,
	&s3c_device_fimc1,
	&s3c_device_fimc2,
	&s3c_device_csis,
	&s3c_device_ipc,
#endif
#ifdef CONFIG_VIDEO_MFC50
	&s3c_device_mfc,
#endif

#ifdef CONFIG_VIDEO_JPEG_V2
	&s3c_device_jpeg,
#endif

#ifdef CONFIG_VIDEO_ROTATOR
	&s5p_device_rotator,
#endif
	&s3c_device_i2c0,
//#ifndef CONFIG_KORAIL_DEVICE_DS1338 //CONFIG_KORAIL_DEVICE
//	&s3c_device_i2c1,
//#endif
	&s3c_device_i2c2,

#ifdef CONFIG_USB
	&s3c_device_usb_ehci,
	&s3c_device_usb_ohci,
#endif
#ifdef CONFIG_USB_GADGET
	&s3c_device_usbgadget,
#endif
#ifdef CONFIG_USB_ANDROID
	&s3c_device_android_usb,
	&s3c_device_usb_mass_storage,
#endif

#ifdef CONFIG_S3C_DEV_HSMMC
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	&s3c_device_hsmmc1,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	&s3c_device_hsmmc2,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	&s3c_device_hsmmc3,
#endif

#ifdef CONFIG_SPI_CNTRLR_0
        &s3c_device_spi0,
#endif
#ifdef CONFIG_SPI_CNTRLR_1
        &s3c_device_spi1,
#endif

#ifdef CONFIG_VIDEO_TV20
	&s5p_device_tvout,
	&s5p_device_cec,
	&s5p_device_hpd,
#endif

#ifdef CONFIG_ANDROID_PMEM
	&pmem_device,
	&pmem_gpu1_device,
	&pmem_adsp_device,
#endif
	&sec_device_battery,
#ifdef CONFIG_VIDEO_G2D
	&s5p_device_g2d,
#endif
};

static void __init smdkv210_fixup(struct machine_desc *desc,
			struct tag *tags, char **cmdline,
			struct meminfo *mi)
{
	mi->bank[0].start = 0x20000000;
	mi->bank[0].size = 256 * SZ_1M;
	mi->bank[0].node = 0;
	
	mi->bank[1].start = 0x40000000;
	mi->bank[1].size = 256 * SZ_1M;
	mi->bank[1].node = 1;
	
	mi->nr_banks = 2;
}

static void mkj_test_i2c(void)
{
	void __iomem * gpio_reg;
	int reg_dat = 0x222222;
	
	//printk("[MMM] Test i2c dat\r\n");
	
	gpio_reg = ioremap (S5PV210_PA_GPIO, 0x400);
	
	writel(reg_dat, gpio_reg + 0xc0);
	
	//printk("[MMM] Gpio GPD1CON = 0x%x \r\n", readl(gpio_reg + 0xc0));
	
}

static void __init smdkv210_map_io(void)
{
	s5p_init_io(NULL, 0, S5P_VA_CHIPID);
	s3c24xx_init_clocks(24000000);
	s3c24xx_init_uarts(smdkv210_uartcfgs, ARRAY_SIZE(smdkv210_uartcfgs));
	s5pv210_reserve_bootmem();

#ifdef CONFIG_MTD_ONENAND
	s3c_device_onenand.name = "s5pc110-onenand";
#endif
#ifdef CONFIG_MTD_NAND
	s3c_device_nand.name = "s5pv210-nand";
#endif
}

#ifdef CONFIG_S3C_SAMSUNG_PMEM
static void __init s3c_pmem_set_platdata(void)
{
	pmem_pdata.start = s3c_get_media_memory_bank(S3C_MDEV_PMEM, 1);
	pmem_pdata.size = s3c_get_media_memsize_bank(S3C_MDEV_PMEM, 1);
}
#endif

#if defined(CONFIG_FB_S3C_LTE480WV)
static struct s3c_platform_fb lte480wv_fb_data __initdata = {
	.hw_ver	= 0x62,
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_WORD | FB_SWAP_HWORD,
};

#elif defined(CONFIG_FB_S3C_LB070WV6)
static struct s3c_platform_fb lb070wv6_fb_data __initdata = {
	.hw_ver	= 0x62,
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_WORD | FB_SWAP_HWORD,
};
#elif defined(CONFIG_FB_S3C_LMS430HF18)
static struct s3c_platform_fb lms430hf18_fb_data __initdata = {
	.hw_ver	= 0x62,
	.nr_wins = 5,
	.default_win = CONFIG_FB_S3C_DEFAULT_WINDOW,
	.swap = FB_SWAP_WORD | FB_SWAP_HWORD,
};
#endif
/* this function are used to detect s5pc110 chip version temporally */

int s5pc110_version ;

void _hw_version_check(void)
{
	void __iomem * phy_address ;
	int temp; 

	phy_address = ioremap (0x40,1);

	temp = __raw_readl(phy_address);


	if (temp == 0xE59F010C)
	{
		s5pc110_version = 0;
	}
	else
	{
		s5pc110_version=1 ;
	}
	//printk("S5PC110 Hardware version : EVT%d \n",s5pc110_version);
	
	iounmap(phy_address);
}

/* Temporally used
 * return value 0 -> EVT 0
 * value 1 -> evt 1
 */

int hw_version_check(void)
{
	return s5pc110_version ;
}
EXPORT_SYMBOL(hw_version_check);

int mv_ps_hold_enable(void){
	//printk("%s : setting GPIO_PDA_PS_HOLD Enable.[0x%x]\n", __func__, readl(S5P_PSHOLD_CONTROL));
	writel(0x5331, S5P_PSHOLD_CONTROL);
}
EXPORT_SYMBOL(mv_ps_hold_enable);

static void smdkv210_power_off(void)
{
	/* PS_HOLD --> Output Low */
	//printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);
	/* PS_HOLD output High --> Low  PS_HOLD_CONTROL, R/W, 0xE010_E81C */
	writel(readl(S5P_PSHOLD_CONTROL) & 0xFFFFFEFF, S5P_PSHOLD_CONTROL);

	while(1);

	//printk(KERN_EMERG "%s : should not reach here!\n", __func__);
}

//#define _BKL_INIT_FREQUENCY			// 41920 => FREQUENCY 100 Hz   // 20960 => FREQUENCY 200 Hz // 10480 => FREQUENCY 400 Hz
//#define _BKL_INIT_DUTYRATIO			// 419 => 100% // 8192 => 82% // 21440 => 50% // 16384 => 62% // 32768 => 22% // 41919 => 0% // 41920 => High
										// 210 => 100% // 10500 => 50% // 20959 => 0%
										
// jay 140109 - QT Backlight control
static void backlight_init_level( void )
{
	int wtcon;
	int pwm_freq = 20960; //145
	int pwm_level = 10500; //75
	
	//printk("%s >> In \r\n", __func__);
		
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_START_STOP))) | (0<<TIMER_1_START_STOP);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_OUTPUT_INVERTER))) | (0<<TIMER_1_OUTPUT_INVERTER);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_AUTO_RELOAD))) | (1<<TIMER_1_AUTO_RELOAD);
	writel(wtcon,S3C2410_TCON);
	
	writel(pwm_freq, S3C2410_TCNTB(1));
	writel(pwm_level, S3C2410_TCMPB(1));
	
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_MANUAL_UPDATE))) | (1<<TIMER_1_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_MANUAL_UPDATE))) | (0<<TIMER_1_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
	mdelay(1);
	
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_START_STOP))) | (1<<TIMER_1_START_STOP);
	writel(wtcon,S3C2410_TCON);
	
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_MANUAL_UPDATE))) | (1<<TIMER_1_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
	wtcon = readl(S3C2410_TCON);
	wtcon = (wtcon & (~(1<<TIMER_1_MANUAL_UPDATE))) | (0<<TIMER_1_MANUAL_UPDATE);
	writel(wtcon,S3C2410_TCON);
}

static void korail_watchdog_enable( void )
{
	u16* fp_rtc_addr;
	
	unsigned int ldRTC_Read_ConData = 0;
	unsigned int ldRTC_Write_ConData = 0;

	//gpio_request(S5PV210_ETC2(4), "ETC24");
	//s3c_gpio_cfgpin(S5PV210_ETC2(4), S3C_GPIO_SFN(0x0));
	//gpio_direction_output(S5PV210_ETC2(4), 1);
	//gpio_free(S5PV210_ETC2(4));

	fp_rtc_addr = ioremap( (S5P_PA_RTC|0x40) , 0x0040);

	ldRTC_Read_ConData =  readl(fp_rtc_addr);

	//printk("[ JAY RTC Read : 0x%x ]\r\n",  ldRTC_Read_ConData );

	ldRTC_Write_ConData = ( ldRTC_Read_ConData | 0x100 ); // 302

	//printk("[ JAY RTC Write : 0x%x ]\r\n",  ldRTC_Write_ConData );

	writel( ldRTC_Write_ConData, fp_rtc_addr);
}

static int hw_id_check(void)
{
        void __iomem * phy_address ;
        int temp;
        phy_address = ioremap (DIV_Y7_HWID,0x0040);
        temp = __raw_readl(phy_address);
        iounmap(phy_address);
	return temp;
}

#define MAIN_ID		0x0A
#define LATENT_ID	0x0B
static void __init smdkv210_machine_init(void)
{
	volatile unsigned int tmp;

	int hwid_val;
	kernel_version_view();
	/* Find out S5PC110 chip version */
	_hw_version_check();
	
	init_gpio_set();

	/* OneNAND */
#ifdef CONFIG_MTD_ONENAND
	//s3c_device_onenand.dev.platform_data = &s5p_onenand_data;
#endif
#ifdef CONFIG_MTD_NAND
	//s3c_device_nand.dev.platform_data = &s5p_nand_data;
#endif

#ifdef CONFIG_SERIAL_8250 // jay serial add.
	korail_watchdog_enable();
#endif

#if defined(CONFIG_DM9000) || defined(CONFIG_SMSC911X)
	smdkv210_dm9000_set();
#endif

#if defined(CONFIG_KORAIL_CHIPSELECT_DIVISION)
/*	      Tacs 	Tcos 	  Tacc	    Tcoh      Tcah	Tacp	PMC*/
	tmp =((0x5<<28)|(0x5<<24)|(0x8<<16)|(0x5<<12)|(0x5<<8)|(0x5<<4)|(0<<0));
        korail_srom_cs_timing(1, tmp);

	tmp =((0x5<<28)|(0x5<<24)|(0x8<<16)|(0x5<<12)|(0x5<<8)|(0x5<<4)|(0<<0));
	korail_srom_cs_timing(3, tmp);

	tmp =((0x0<<28)|(0x0<<24)|(0x5<<16)|(0x0<<12)|(0x0<<8)|(0x0<<4)|(0<<0));
	korail_srom_cs_timing(5, tmp);
#endif

#ifdef CONFIG_ANDROID_PMEM
	android_pmem_set_platdata();
#endif

    //2014.01.09 jay
	backlight_init_level();
	
#ifdef CONFIG_KORAIL_COMBINE_KERNEL
        hwid_val = hw_id_check();
        hwid_val &= 0xff;
        printk("Read HWID is %02x\n", hwid_val);
        if(hwid_val == MAIN_ID){
                printk("------------------------\n");
                printk(" Korail Main board init \n");
                printk("------------------------\n");
                platform_add_devices(korail_devices, ARRAY_SIZE(korail_devices));
        }
        else if(hwid_val == LATENT_ID){
                printk("--------------------------\n");
                printk(" Korail Latent board init \n");
                printk("--------------------------\n");
                platform_add_devices(latent_devices, ARRAY_SIZE(latent_devices));
        }
        else{
                printk("--------------------------\n");
                printk(" Unkwnown      board init \n");
                printk("--------------------------\n");
        }
#endif

	/* i2c */
	s3c_i2c0_set_platdata(NULL);
	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
	
#ifndef CONFIG_KORAIL_DEVICE_DS1338
	if(hwid_val != MAIN_ID){
		s3c_i2c1_set_platdata(NULL);
		i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
	}
#endif
	s3c_i2c2_set_platdata(NULL);
	i2c_register_board_info(2, i2c_devs2, ARRAY_SIZE(i2c_devs2));

	/* to support system shut down */
	pm_power_off = smdkv210_power_off;
#if defined(CONFIG_SPI_CNTRLR_0)
        s3cspi_set_slaves(BUSNUM(0), ARRAY_SIZE(s3c_slv_pdata_0), s3c_slv_pdata_0);
#endif
#if defined(CONFIG_SPI_CNTRLR_1)
        s3cspi_set_slaves(BUSNUM(1), ARRAY_SIZE(s3c_slv_pdata_1), s3c_slv_pdata_1);
#endif
#if defined(CONFIG_SPI_CNTRLR_2)
        s3cspi_set_slaves(BUSNUM(2), ARRAY_SIZE(s3c_slv_pdata_2), s3c_slv_pdata_2);
#endif
        spi_register_board_info(s3c_spi_devs, ARRAY_SIZE(s3c_spi_devs));

#ifdef CONFIG_FB_S3C_LTE480WV
	s3cfb_set_platdata(&lte480wv_fb_data);
#endif

#ifdef CONFIG_FB_S3C_LB070WV6
	s3cfb_set_platdata(&lb070wv6_fb_data);
#endif

#ifdef CONFIG_FB_S3C_LMS430HF18
	s3cfb_set_platdata(&lms430hf18_fb_data);	
#endif

#if defined(CONFIG_BLK_DEV_IDE_S3C)
	s3c_ide_set_platdata(&smdkv210_ide_pdata);
#endif

#if defined(CONFIG_TOUCHSCREEN_S3C)
	s3c_ts_set_platdata(&s3c_ts_platform);
#endif

#if defined(CONFIG_S5PV210_ADC)
	s3c_adc_set_platdata(&s3c_adc_platform);
#endif

#if defined(CONFIG_PM)
	s3c_pm_init();
#endif
#ifdef CONFIG_VIDEO_FIMC
	/* fimc */
	s3c_fimc0_set_platdata(&fimc_plat);
	s3c_fimc1_set_platdata(&fimc_plat);
	s3c_fimc2_set_platdata(&fimc_plat);
	s3c_csis_set_platdata(NULL);
	//smdkv210_cam0_power(1);
	//smdkv210_cam1_power(1);
	//smdkv210_mipi_cam_reset();
#endif

#ifdef CONFIG_VIDEO_MFC50
	/* mfc */
	s3c_mfc_set_platdata(NULL);
#endif

#ifdef CONFIG_S3C_DEV_HSMMC
	s5pv210_default_sdhci0();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	s5pv210_default_sdhci1();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	s5pv210_default_sdhci2();
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	s5pv210_default_sdhci3();
#endif
#ifdef CONFIG_S5PV210_SETUP_SDHCI
	s3c_sdhci_set_platdata();
#endif
	platform_add_devices(smdkv210_devices, ARRAY_SIZE(smdkv210_devices));

#if defined(CONFIG_HAVE_PWM)
#if 0
	printk("\r\n");
	printk("PWM CHECK 00. TCON : %x \r\n", readl(S3C2410_TCON) );
	printk("PWM CHECK 00. TCNTB1 : %x \r\n", readl(S3C2410_TCNTB(1)) );
	printk("PWM CHECK 00. TCMPB1 : %x \r\n", readl(S3C2410_TCMPB(1)) );
	printk("PWM CHECK 00. TCNTO1 : %x \r\n", readl(S3C2410_TCNTO(1)) );
	printk("\r\n");
#endif
	smdk_backlight_register();
#if 0
	printk("\r\n");
	printk("PWM CHECK 01. TCON : %x \r\n", readl(S3C2410_TCON) );
	printk("PWM CHECK 01. TCNTB1 : %x \r\n", readl(S3C2410_TCNTB(1)) );
	printk("PWM CHECK 01. TCMPB1 : %x \r\n", readl(S3C2410_TCMPB(1)) );
	printk("PWM CHECK 01. TCNTO1 : %x \r\n", readl(S3C2410_TCNTO(1)) );
	printk("\r\n");
#endif
#endif
}

#ifdef CONFIG_USB_SUPPORT
/* Initializes OTG Phy. */
void otg_phy_init(void)
{
	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL)
		|(0x1<<0), S5P_USB_PHY_CONTROL); /*USB PHY0 Enable */
	__raw_writel((__raw_readl(S3C_USBOTG_PHYPWR)
		&~(0x3<<3)&~(0x1<<0))|(0x1<<5), S3C_USBOTG_PHYPWR);
	__raw_writel((__raw_readl(S3C_USBOTG_PHYCLK)
		&~(0x5<<2))|(0x3<<0), S3C_USBOTG_PHYCLK);
	__raw_writel((__raw_readl(S3C_USBOTG_RSTCON)
		&~(0x3<<1))|(0x1<<0), S3C_USBOTG_RSTCON);
	udelay(10);
	__raw_writel(__raw_readl(S3C_USBOTG_RSTCON)
		&~(0x7<<0), S3C_USBOTG_RSTCON);
	udelay(10);
}
EXPORT_SYMBOL(otg_phy_init);

/* USB Control request data struct must be located here for DMA transfer */
//struct usb_ctrlrequest usb_ctrl __attribute__((aligned(8)));

/* OTG PHY Power Off */
void otg_phy_off(void)
{
	__raw_writel(__raw_readl(S3C_USBOTG_PHYPWR)
		|(0x3<<3), S3C_USBOTG_PHYPWR);
	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL)
		&~(1<<0), S5P_USB_PHY_CONTROL);
}
EXPORT_SYMBOL(otg_phy_off);

void usb_host_phy_init(void)
{
	struct clk *otg_clk;

	otg_clk = clk_get(NULL, "usbotg");
	clk_enable(otg_clk);

	if (readl(S5P_USB_PHY_CONTROL) & (0x1<<1))
		return;

	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL)
		|(0x1<<1), S5P_USB_PHY_CONTROL);
	__raw_writel((__raw_readl(S3C_USBOTG_PHYPWR)
		&~(0x1<<7)&~(0x1<<6))|(0x1<<8)|(0x1<<5), S3C_USBOTG_PHYPWR);
	__raw_writel((__raw_readl(S3C_USBOTG_PHYCLK)
		&~(0x1<<7))|(0x3<<0), S3C_USBOTG_PHYCLK);
	__raw_writel((__raw_readl(S3C_USBOTG_RSTCON))
		|(0x1<<4)|(0x1<<3), S3C_USBOTG_RSTCON);
	__raw_writel(__raw_readl(S3C_USBOTG_RSTCON)
		&~(0x1<<4)&~(0x1<<3), S3C_USBOTG_RSTCON);
}
EXPORT_SYMBOL(usb_host_phy_init);

void usb_host_phy_off(void)
{
	__raw_writel(__raw_readl(S3C_USBOTG_PHYPWR)
		|(0x1<<7)|(0x1<<6), S3C_USBOTG_PHYPWR);
	__raw_writel(__raw_readl(S5P_USB_PHY_CONTROL)
		&~(1<<1), S5P_USB_PHY_CONTROL);
}
EXPORT_SYMBOL(usb_host_phy_off);
#endif

#if defined(CONFIG_KEYPAD_S3C) || defined(CONFIG_KEYPAD_S3C_MODULE)
#if defined(CONFIG_KEYPAD_S3C_MSM)
void s3c_setup_keypad_cfg_gpio(void)
{
	unsigned int gpio;
	unsigned int end;

	/* gpio setting for KP_COL0 */
	s3c_gpio_cfgpin(S5PV210_GPJ1(5), S3C_GPIO_SFN(3));
	s3c_gpio_setpull(S5PV210_GPJ1(5), S3C_GPIO_PULL_NONE);

	/* gpio setting for KP_COL1 ~ KP_COL7 and KP_ROW0 */
	end = S5PV210_GPJ2(8);
	for (gpio = S5PV210_GPJ2(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	/* gpio setting for KP_ROW1 ~ KP_ROW8 */
	end = S5PV210_GPJ3(8);
	for (gpio = S5PV210_GPJ3(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}

	/* gpio setting for KP_ROW9 ~ KP_ROW13 */
	end = S5PV210_GPJ4(5);
	for (gpio = S5PV210_GPJ4(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}
}
#else
void s3c_setup_keypad_cfg_gpio(int rows, int columns)
{
	unsigned int gpio;
	unsigned int end;

	end = S5PV210_GPH3(rows);

	/* Set all the necessary GPH2 pins to special-function 0 */
	for (gpio = S5PV210_GPH3(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
	}

	end = S5PV210_GPH2(columns);

	/* Set all the necessary GPK pins to special-function 0 */
	for (gpio = S5PV210_GPH2(0); gpio < end; gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
	}
}
#endif /* if defined(CONFIG_KEYPAD_S3C_MSM)*/
EXPORT_SYMBOL(s3c_setup_keypad_cfg_gpio);
#endif

MACHINE_START(SMDKV210, "SMDKV210")
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.phys_io	= S3C_PA_UART & 0xfff00000,
	.io_pg_offst	= (((u32)S3C_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S5P_PA_SDRAM + 0x100,
	//.fixup			= smdkv210_fixup,
	.init_irq	= s5pv210_init_irq,
	.map_io		= smdkv210_map_io,
	.init_machine	= smdkv210_machine_init,
	.timer		= &s5p_systimer,
MACHINE_END
