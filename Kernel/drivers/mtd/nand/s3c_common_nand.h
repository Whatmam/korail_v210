#ifndef _S3C_COMMON_NAND_H
#define _S3C_COMMON_NAND_H

enum s3c_cpu_type {
	TYPE_S3C2450,	/* including s3c2416 */
	TYPE_S3C6400,
	TYPE_S3C6410,	/* including s3c6430/31 */
	TYPE_S5P6440,
	TYPE_S5PC100,
	TYPE_S5PC110,
	TYPE_S5PV210,
};

/* Nand flash definition values by jsgood */
#define S3C_NAND_TYPE_UNKNOWN	0x0
#define S3C_NAND_TYPE_SLC	0x1
#define S3C_NAND_TYPE_MLC	0x2

/* Select the chip by setting nCE to low */
#define KORAIL_LOW_NAND_NCE		0x01
/* Select the command latch by setting CLE to high */
#define KORAIL_LOW_NAND_CLE		0x02
/* Select the address latch by setting ALE to high */
#define KORAIL_LOW_NAND_ALE		0x04
/* Always High signal */
#define KORAIL_LOW_NAND_NWP		0x08

/* Select the chip by setting nCE to low */
#define KORAIL_HIGH_NAND_NCE	0x10
/* Select the command latch by setting CLE to high */
#define KORAIL_HIGH_NAND_CLE	0x20
/* Select the address latch by setting ALE to high */
#define KORAIL_HIGH_NAND_ALE	0x40
/* Always High signal */
#define KORAIL_HIGH_NAND_NWP	0x80

#define KORAIL_HWCTL_LOW_INIT	(~0x0F)
#define KORAIL_HWCTL_HIGH_INIT	(~0xF0)


#define NUM_PARTITIONS 1


#endif /* _S3C_COMMON_NAND_H */
