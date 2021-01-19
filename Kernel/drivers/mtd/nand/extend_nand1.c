/*
 * drivers/mtd/nand/extend_nand1.c
 *
 * Copyright (C) 2004 Technologic Systems (support@embeddedARM.com)
 *
 * Derived from drivers/mtd/nand/edb7312.c
 *   Copyright (C) 2004 Marius Gröger (mag@sysgo.de)
 *
 * Derived from drivers/mtd/nand/autcpu12.c
 *   Copyright (c) 2001 Thomas Gleixner (gleixner@autronix.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Overview:
 *   This is a device driver for the NAND flash device found on the
 *   TS-7250 board which utilizes a Samsung 32 Mbyte part.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/jiffies.h>
#include <linux/sched.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include <plat/regs-nand.h>
#include <plat/nand.h>

#include <mach/map.h>
#include <mach/regs-mem.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>

#include "s3c_common_nand.h"

static struct mtd_info *extend_nand1_mtd = NULL;



u32 *nand1_cs_base; 
u32 *nand1_cwe_base;


static const struct mtd_partition partition_info[] = {
    {
     	.name = "NAND 1 flash partition",
     	.offset = 0,
     	.size = MTDPART_SIZ_FULL, //512 * 1024 * 1024
     	.mask_flags	= MTD_CAP_ROM,
    }, //2012.10.16 jay sim board nand 1G : 512 -> 1024 change
};



/*
 *	hardware specific access to control-lines
 *
 *	ctrl:
 *	NAND_NCE: bit 0 -> bit 2
 *	NAND_CLE: bit 1 -> bit 1
 *	NAND_ALE: bit 2 -> bit 0
 */
static void extend_nand1_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;

	if (ctrl & NAND_CTRL_CHANGE) {
		unsigned long addr = nand1_cwe_base;
		unsigned char bits;

                bits = ( ~ctrl & NAND_NCE );
                bits |= ( ctrl & NAND_CLE );
                bits |= ( ctrl & NAND_ALE );
                bits |= NAND_NWP;

		//__raw_writeb((__raw_readb(addr) & (~0x7)) | bits, addr);
		__raw_writeb((__raw_readb(addr) & (~0xF)) | bits, addr);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}

extern int HW_ID_Check(void);
/*
 * Main initialization routine
 */
static int __init extend_nand1_init(void)
{
	struct nand_chip *this;
	int hwid_val;
	unsigned int nCS4_Value = 0;
    unsigned long ld_Sromc_nCSBase = (unsigned long)S5P_SROM_BW;

hwid_val = HW_ID_Check();
if(hwid_val == 0x0a){
    nCS4_Value = __raw_readl(ld_Sromc_nCSBase);

    nCS4_Value &= ~(0xf << 16); //CS4

    nCS4_Value |= (1<<19) | (0<<18) | (1<<17) | (0<<16);

    __raw_writel(nCS4_Value, ld_Sromc_nCSBase);

    nCS4_Value = ((0<<28)|(1<<24)|(5<<16)|(1<<12)|(4<<8)|(6<<4)|(0<<0));

    __raw_writel(nCS4_Value, ld_Sromc_nCSBase + 0x14);

    nCS4_Value = 0x22222222;
	__raw_writel(nCS4_Value,(S5PV210_MP04CON));
	__raw_writel(nCS4_Value,(S5PV210_MP05CON));
	__raw_writel(nCS4_Value,(S5PV210_MP06CON));
	__raw_writel(nCS4_Value,(S5PV210_MP07CON));

    nCS4_Value = __raw_readl(S5PV210_MP01CON);
    
    nCS4_Value &= ~(0xfff<<16);
	nCS4_Value |=(0x222<<16);
	
	__raw_writel(nCS4_Value,(S5PV210_MP01CON));

	/* Allocate memory for MTD device structure and private data */
	extend_nand1_mtd = kmalloc(sizeof(struct mtd_info) + sizeof(struct nand_chip), GFP_KERNEL);
	if (!extend_nand1_mtd) {
		printk("Unable to allocate extend_nand1 NAND MTD device structure.\n");
		return -ENOMEM;
	}

	/* Get pointer to private data */
	this = (struct nand_chip *)(&extend_nand1_mtd[1]);

	/* Initialize structures */
	memset(extend_nand1_mtd, 0, sizeof(struct mtd_info));
	memset(this, 0, sizeof(struct nand_chip));

	/* Link the private data with the MTD structure */
	extend_nand1_mtd->priv = this;
	extend_nand1_mtd->owner = THIS_MODULE;


    nand1_cs_base= ioremap(KORAIL_DIV_Y0_EXT_ND_NCS0, 0x0040);
    nand1_cwe_base= ioremap(KORAIL_DIV_Y2_EXT_ND_CWE0, 0x0040);

	//NFCONT_REG &= ~NFCONT_WP;
	
	/* insert callbacks */
	this->IO_ADDR_R = (void *)nand1_cs_base;
	this->IO_ADDR_W = (void *)nand1_cs_base;
	this->cmd_ctrl = extend_nand1_hwcontrol;
	this->chip_delay = 50;
	this->ecc.mode = NAND_ECC_SOFT;

	/* Scan to find existence of the device */
	if (nand_scan(extend_nand1_mtd, 1)) {
		kfree(extend_nand1_mtd);
		return -ENXIO;
	}
	/* Register the partitions */
	add_mtd_partitions(extend_nand1_mtd, partition_info, NUM_PARTITIONS);
	}
	/* Return happy */
	return 0;
}

module_init(extend_nand1_init);

/*
 * Clean up routine
 */
static void __exit extend_nand1_cleanup(void)
{
	/* Unregister the device */
	del_mtd_device(extend_nand1_mtd);

	/* Free the MTD device structure */
	kfree(extend_nand1_mtd);
}

module_exit(extend_nand1_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JAY <d3ix@hybus.net>");
MODULE_DESCRIPTION("MTD map driver for Solidetech board of korial");
