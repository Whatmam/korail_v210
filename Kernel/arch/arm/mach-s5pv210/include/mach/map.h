/* linux/arch/arm/mach-s5pv210/include/mach/map.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5PV210 - Memory map definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_MAP_H
#define __ASM_ARCH_MAP_H __FILE__

#include <plat/map-base.h>
#include <plat/map-s5p.h>

#define KERNEL_SW_VERSION	"version 1.8.0(Ramdisk)"

#define S5PV210_PA_WDT		(0xE2700000)
#define S5P_PA_WDT		S5PV210_PA_WDT

#define S5PV210_PA_DM9000	(0xA8000000)
#define S5P_PA_DM9000		S5PV210_PA_DM9000

#define S5PV210_PA_CHIPID	(0xE0000000)
#define S5P_PA_CHIPID		S5PV210_PA_CHIPID

#define S5PV210_PA_SYSCON	(0xE0100000)
#define S5P_PA_SYSCON		S5PV210_PA_SYSCON

#define S5PV210_PA_GPIO		(0xE0200000)
#define S5P_PA_GPIO		S5PV210_PA_GPIO

#define S5PV210_PA_PDMA		(0xE0900000)
#define S5P_PA_PDMA		S5PV210_PA_PDMA

/* PL330 New driver */
#define S5PC110_PA_MDMA		(0xFA200000)
#define S5PC110_PA_PDMA0	(0xE0900000)
#define S5PC110_PA_PDMA1	(0xE0A00000)

#define S5PV210_PA_IIC0		(0xE1800000)
#define S5PV210_PA_IIC1		(0xFAB00000)
#define S5PV210_PA_IIC2		(0xE1A00000)

#define S5PV210_PA_TIMER	(0xE2500000)
#define S5P_PA_TIMER		S5PV210_PA_TIMER

#define S5PV210_PA_SYSTIMER	(0xE2600000)

#define S5PV210_PA_RTC		(0xE2800000)
#define S5P_PA_RTC		S5PV210_PA_RTC

#define S5PV210_PA_UART		(0xE2900000)

#define S5P_PA_UART0		(S5PV210_PA_UART + 0x0)
#define S5P_PA_UART1		(S5PV210_PA_UART + 0x400)
#define S5P_PA_UART2		(S5PV210_PA_UART + 0x800)
#define S5P_PA_UART3		(S5PV210_PA_UART + 0xC00)

#define S5P_SZ_UART		SZ_256

#define S5PV210_PA_SROMC	(0xE8000000)

#define S5PV210_PA_CFCON	(0xE8200000)
#define S5PV210_SZ_CFCON	SZ_1M

#define S5PV210_PA_OTG		(0xEC000000)
#define S5P_PA_OTG		S5PV210_PA_OTG
#define S5P_SZ_OTG		SZ_1M

#define S5PV210_PA_OTGPHY	(0xEC100000)
#define S5P_PA_OTGPHY		S5PV210_PA_OTGPHY

#define S5PV210_PA_USB_EHCI	(0xEC200000)
#define S5P_PA_USB_EHCI		S5PV210_PA_USB_EHCI
#define S5P_SZ_USB_EHCI		SZ_1M

#define S5PV210_PA_USB_OHCI	(0xEC300000)
#define S5P_PA_USB_OHCI		S5PV210_PA_USB_OHCI
#define S5P_SZ_USB_OHCI     	SZ_1M

#define S5PV210_VA_RTC		S3C_ADDR(0x00c00000)

#define S5PV210_PA_SPI0         (0xE1300000)
#define S5PV210_PA_SPI1         (0xE1400000)
#define S5PV210_PA_SPI2         (0xE1500000)


#define S5PV210_PA_IIS1 	(0xE2100000)
#define S5P_PA_IIS1		S5PV210_PA_IIS1
#define S5PV210_PA_IIS2 	(0xE2A00000)
#define S5P_PA_IIS2		S5PV210_PA_IIS2
#define S5PV210_PA_IIS0 	(0xEEE30000)
#define S5P_PA_IIS0		S5PV210_PA_IIS0

#define S5PV210_PA_PCM1 	(0xE1200000)
#define S5PV210_PA_PCM0 	(0xE2300000)
#define S5PV210_PA_PCM2 	(0xE2B00000)

#define S5PV210_PA_AC97 	(0xE2200000)

#define S5PV210_PA_SPDIF	(0xE1100000)

#define S5PV210_PA_HSMMC(x)     (0xEB000000 + ((x) * 0x100000))

#define S5PV210_PA_AUDSS	(0xEEE10000)

#define S5PV210_PA_VIC0         (0xF2000000)
#define S5P_PA_VIC0		S5PV210_PA_VIC0

#define S5PV210_PA_VIC1		(0xF2100000)
#define S5P_PA_VIC1		S5PV210_PA_VIC1

#define S5PV210_PA_VIC2		(0xF2200000)
#define S5P_PA_VIC2		S5PV210_PA_VIC2

#define S5PV210_PA_VIC3		(0xF2300000)
#define S5P_PA_VIC3		S5PV210_PA_VIC3

#define S5PV210_PA_LCD	   	(0xF8000000)
#define S5P_PA_LCD		S5PV210_PA_LCD
#define S5PV210_SZ_LCD		SZ_1M
#define S5P_SZ_LCD		S5PV210_SZ_LCD

#define S5PV210_PA_FIMC0	(0xFB200000)
#define S5P_PA_FIMC0		S5PV210_PA_FIMC0
#define S5P_SZ_FIMC0		SZ_1M

#define S5PV210_PA_FIMC1	(0xFB300000)
#define S5P_PA_FIMC1		S5PV210_PA_FIMC1
#define S5P_SZ_FIMC1		SZ_1M

#define S5PV210_PA_FIMC2	(0xFB400000)
#define S5P_PA_FIMC2		S5PV210_PA_FIMC2
#define S5P_SZ_FIMC2		SZ_1M

#define S5PV210_PA_IPC		(0xFB700000)
#define S5P_PA_IPC		S5PV210_PA_IPC
#define S5P_SZ_IPC		SZ_1M

#define S5PV210_PA_DMA		(0xFA200000)
#define S5P_PA_DMA		S5PV210_PA_DMA

#define S5PV210_PA_CSIS		(0xFA600000)
#define S5P_PA_CSIS		S5PV210_PA_CSIS
#define S5P_SZ_CSIS		SZ_1M

#define S5PV210_PA_MFC          (0xF1700000)
#define S5PV210_SZ_MFC          SZ_1M

#define S5PV210_PA_JPEG		(0xFB600000)
#define S5PV210_SZ_JPEG		SZ_1M

#define S5PV210_PA_ROTATOR	(0xFA300000)
#define S5PV210_SZ_ROTATOR	SZ_1M

#if defined(CONFIG_MACH_SMDKC110)
#define S5PV210_PA_SDRAM	(0x30000000)
#else
#define S5PV210_PA_SDRAM	(0x20000000)
#endif
#define S5P_PA_SDRAM		S5PV210_PA_SDRAM

#define S5P_PA_SDRAM1		(0x40000000)/*SDKIM*/

/* KEYPAD IF */
#define S5PV2XX_SZ_KEYPAD       SZ_4K

#define S5PV2XX_PA_ADC          (0xE1700000)
#define S3C_PA_ADC              S5PV2XX_PA_ADC

#define S5PV2XX_PA_KEYPAD       (0xE1600000)
#define S3C_PA_KEYPAD           S5PV2XX_PA_KEYPAD
#define S3C_SZ_KEYPAD           S5PV2XX_SZ_KEYPAD

/* CEC */
#define S5PV210_PA_CEC		(0xE1B00000)
#define S5P_PA_CEC		S5PV210_PA_CEC
#define S5P_SZ_CEC		SZ_4K

/* compatibiltiy defines. */
#define S3C_PA_UART		S5PV210_PA_UART
#define S3C_PA_IIC		S5PV210_PA_IIC0
#define S3C_PA_IIC1		S5PV210_PA_IIC1
#define S3C_PA_IIC2		S5PV210_PA_IIC2

#define S3C_PA_SPI0             S5PV210_PA_SPI0
#define S3C_PA_SPI1             S5PV210_PA_SPI1
#define S3C_PA_SPI2             S5PV210_PA_SPI2
#define S3C_SZ_SPI0             SZ_4K
#define S3C_SZ_SPI1             SZ_4K
#define S3C_SZ_SPI2             SZ_4K

#define S5P_VA_RTC		S5PV210_VA_RTC

#define S3C_PA_HSMMC0		S5PV210_PA_HSMMC(0)
#define S3C_PA_HSMMC1		S5PV210_PA_HSMMC(1)
#define S3C_PA_HSMMC2		S5PV210_PA_HSMMC(2)
#define S3C_PA_HSMMC3		S5PV210_PA_HSMMC(3)

#define S5P_VA_VIC0		(S3C_VA_IRQ + 0x0)
#define S5P_VA_VIC1		(S3C_VA_IRQ + 0x10000)
#define S5P_VA_VIC2		(S3C_VA_IRQ + 0x20000)
#define S5P_VA_VIC3		(S3C_VA_IRQ + 0x30000)

/* TVOUT */
#define S5PV210_PA_TVENC	(0xF9000000)
#define S5P_PA_TVENC		S5PV210_PA_TVENC
#define S5P_SZ_TVENC		SZ_1M

#define S5PV210_PA_VP		(0xF9100000)
#define S5P_PA_VP		S5PV210_PA_VP
#define S5P_SZ_VP		SZ_1M

#define S5PV210_PA_MIXER	(0xF9200000)
#define S5P_PA_MIXER		S5PV210_PA_MIXER
#define S5P_SZ_MIXER		SZ_1M

#define S5PV210_PA_HDMI		(0xFA100000)
#define S5P_PA_HDMI		S5PV210_PA_HDMI
#define S5P_SZ_HDMI		SZ_1M

#define S5PV210_I2C_HDMI_PHY	(0xFA900000)
#define S5P_I2C_HDMI_PHY	S5PV210_I2C_HDMI_PHY
#define S5P_I2C_HDMI_SZ_PHY	SZ_1K

/* G2D */
#define S5PV210_PA_G2D	   	(0xFA000000)
#define S5P_PA_G2D		S5PV210_PA_G2D

#define S5PV210_SZ_G2D		SZ_1M
#define S5P_SZ_G2D		S5PV210_SZ_G2D

/* OneNAND */
#define S5PV210_PA_ONENAND   	(0xB0000000)
#define S5P_PA_ONENAND		S5PV210_PA_ONENAND

#define S5PV210_SZ_ONENAND	SZ_8M   // redefined at device/mtd/onenand/s5pc110.c
#define S5P_SZ_ONENAND		S5PV210_SZ_ONENAND

/* NAND */
#define S5PV210_PA_NAND   	(0xB0E00000)
#define S5P_PA_NAND		S5PV210_PA_NAND

#define S5PV210_SZ_NAND		SZ_1M
#define S5P_SZ_NAND		S5PV210_SZ_NAND

/* MEMCON */
#define S5PV210_PA_DMC0		(0xF0000000)
#define S5PV210_PA_DMC1		(0xF1400000)


#define S5P_PA_UART1_UMCON	(S5PV210_PA_UART + 0x40C)

/* Korail CS5 Div... VIRTUAL ADDRESS*/
/* 74HC138 Data Map

|	G2B		G2A		G1		C		B		A		Y0		Y1		Y2		Y3		Y4		Y5		Y6		Y7
----------------------------------------------------------------------------------------------------------------
|	L		L		H		L		L		L		L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		L		L		H				L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		L		H		L						L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		L		H		H								L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		H		L		L										L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		H		L		H												L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		H		H		L														L
----------------------------------------------------------------------------------------------------------------
|	L		L		H		H		H		H																L
----------------------------------------------------------------------------------------------------------------
 15      14      13      12      11
            
Y0 : DM9000 
Y1 : DIP
Y2 : RFM
Y3 : RFMALE
Y4 : Gate IO
Y5 : FND2
Y6 : FND1
Y7 : HW ID

		|	15	14	13	12	|	11	10	9	8	|	7	6	5	4	|	3	2	1	0
		|-----------------------------------------------------------------------------	
11 (A) 		|	0	0	0	0	|	1	0	0	0	|	0	0	0	0	|	0	0	0	0	: 0x0800
			|-----------------------------------------------------------------------------	
12 (B)		|	0	0	0	1	|	0	0	0	0	|	0	0	0	0	|	0	0	0	0	: 0x1000
			|-----------------------------------------------------------------------------	
13 (C)		|	0	0	1	0	|	0	0	0	0	|	0	0	0	0	|	0	0	0	0	: 0x2000
			|-----------------------------------------------------------------------------	
14 (G2A)	|	0	1	0	0	|	0	0	0	0	|	0	0	0	0	|	0	0	0	0	: 0x4000
			|-----------------------------------------------------------------------------	
15 (G1)		|	1	0	0	0	|	0	0	0	0	|	0	0	0	0	|	0	0	0	0	: 0x8000
			|-----------------------------------------------------------------------------
*/

#define CS1_DIV_PA_BASE		(0x88000000)
#define CS3_DIV_PA_BASE		(0x98000000)
#define CS4_DIV_PA_BASE 	(0xA0000000)
#define CS5_DIV_PA_BASE 	(0xA8000000)

#define DIV_SMIF_ADDR0		(0x02)
#define DIV_SMIF_ADDR1		(0x04)
#define DIV_SMIF_ADDR2		(0x08)
#define DIV_XM0_ADDR4		(0x10)

#define DIV_A_ADDR_08		(0x100)
#define DIV_B_ADDR_09		(0x200)
#define DIV_C_ADDR_10		(0x400)

#define DIV_A_ADDR_11		(0x800)
#define DIV_B_ADDR_12		(0x1000)
#define DIV_C_ADDR_13		(0x2000)
#define DIV_G2A_ADDR_14		(0x4000)
#define DIV_G1_ADDR_15		(0x8000)

#define DIV_Y0_DM9000		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 )
#define DIV_Y1_DIP			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11)
#define DIV_Y2_RFM			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12)
#if defined(CONFIG_KORAIL_CHIPSELECT_DIVISION)
#define DIV_Y3_RFMALE		(CS1_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
#else
#define DIV_Y3_RFMALE		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
#endif
#define DIV_Y4_GATEIO		(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_C_ADDR_13)
#define DIV_Y5_FND2			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_C_ADDR_13)
#define DIV_Y6_FND1			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
#define DIV_Y7_HWID			(CS5_DIV_PA_BASE | DIV_G1_ADDR_15 | DIV_A_ADDR_11 | DIV_B_ADDR_12 | DIV_C_ADDR_13)

#if defined(CONFIG_KORAIL_CHIPSELECT_DIVISION)
#define DIV_Y0_EUA                      (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 )
#define DIV_Y1_EUB                      (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11)
#define DIV_Y2_EUC                      (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_B_ADDR_12)
#define DIV_Y3_EUD                      (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
#define DIV_Y4_SAM0                     (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_C_ADDR_13)
#define DIV_Y5_SAM1                     (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_C_ADDR_13)
#define DIV_Y6_SIM0                     (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
#define DIV_Y7_SIM1                     (CS3_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
#else
#define DIV_Y0_EUA			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 )
#define DIV_Y1_EUB			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11)
#define DIV_Y2_EUC			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_B_ADDR_12)
#define DIV_Y3_EUD			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_B_ADDR_12)
#define DIV_Y4_SAM0			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_C_ADDR_13)
#define DIV_Y5_SAM1			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_C_ADDR_13)
#define DIV_Y6_SIM0			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
#define DIV_Y7_SIM1			(CS5_DIV_PA_BASE | DIV_G2A_ADDR_14 | DIV_A_ADDR_11 | DIV_B_ADDR_12 | DIV_C_ADDR_13)
#endif

#define DIV_UART_NCSA 		( DIV_Y4_SAM0 ) 
#define DIV_UART_NCSB		( DIV_Y4_SAM0 | DIV_XM0_ADDR4 )
#define DIV_SIMAM_LATCH0	( DIV_Y6_SIM0 ) 
#define DIV_SIMAM_LATCH1	( DIV_Y6_SIM0 | DIV_SMIF_ADDR1 ) 
#define DIV_SIMAM_LATCH2	( DIV_Y6_SIM0 | DIV_SMIF_ADDR2 )

#define KORAIL_DIV_Y0_EXT_ND_NCS0	(CS4_DIV_PA_BASE)
#define KORAIL_DIV_Y1_EXT_ND_NCS1	(CS4_DIV_PA_BASE | DIV_C_ADDR_13)
#define KORAIL_DIV_Y2_EXT_ND_CWE0	(CS4_DIV_PA_BASE | DIV_G2A_ADDR_14)
#define KORAIL_DIV_Y3_EXT_ND_NCS2	(CS4_DIV_PA_BASE | DIV_C_ADDR_13 | DIV_G2A_ADDR_14)
#define KORAIL_DIV_Y4_EXT_ND_NCS3	(CS4_DIV_PA_BASE | DIV_G1_ADDR_15)
#define KORAIL_DIV_Y5_EXT_ND_CWE1	(CS4_DIV_PA_BASE | DIV_C_ADDR_13 | DIV_G1_ADDR_15)

//20140915 Gate IO Chip Base Address.
#define KORAIL_GATEIO_Y0_GMODE1		(DIV_Y4_GATEIO) //gateio main cs
#define KORAIL_GATEIO_Y1_GMODE2		(DIV_Y4_GATEIO | DIV_A_ADDR_08)
#define KORAIL_GATEIO_Y2_GMODE3		(DIV_Y4_GATEIO | DIV_B_ADDR_09)
#define KORAIL_GATEIO_Y3_GMODE4		(DIV_Y4_GATEIO | DIV_A_ADDR_08 | DIV_B_ADDR_09)
#define KORAIL_GATEIO_Y4_GSEL1		(DIV_Y4_GATEIO | DIV_C_ADDR_10)
#define KORAIL_GATEIO_Y5_GSEL2		(DIV_Y4_GATEIO | DIV_A_ADDR_08 | DIV_C_ADDR_10)
#define KORAIL_GATEIO_Y6_GSEL3		(DIV_Y4_GATEIO | DIV_B_ADDR_09 | DIV_C_ADDR_10)
#define KORAIL_GATEIO_Y7_GSPARE		(DIV_Y4_GATEIO | DIV_A_ADDR_08 | DIV_B_ADDR_09 | DIV_C_ADDR_10)

#endif /* __ASM_ARCH_MAP_H */
