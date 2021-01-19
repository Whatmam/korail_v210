#define MT9D111_COMPLETE
//#undef MT9D111_COMPLETE
/*
 * Driver for MT9D111 (UXGA camera) from Samsung Electronics
 * 
 * 1/4" 2.0Mp CMOS Image Sensor SoC with an Embedded Image Processor
 *
 * Copyright (C) 2009, Jinsung Yang <jsgood.yang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __MT9D111_H__
#define __MT9D111_H__

struct mt9d111_reg{
        unsigned char page;
        unsigned char subaddr;
        unsigned short value;
};


struct mt9d111_regset_type {
	unsigned char *regset;
	int len;
};

/*
 * Macro
 */
#define REGSET_LENGTH(x)	(sizeof(x)/sizeof(struct mt9d111_reg))

/*
 * User defined commands
 */
/* S/W defined features for tune */
#define REG_DELAY	0xFF00	/* in ms */
#define REG_CMD		0xFFFF	/* Followed by command */

/* Following order should not be changed */
#ifdef CONFIG_ARCH_S3C64XX
enum image_size_mt9d111 {
	VGA,	/* 640*480 */
	SVGA,	/* 320*320*/
};
#else
enum image_size_mt9d111 {
	/* This SoC supports upto UXGA (1600*1200) */
#if 0
	QQVGA,	/* 160*120*/
	QCIF,	/* 176*144 */
	QVGA,	/* 320*240 */
	SQVGA,	/* 320*320*/
	CIF,	/* 352*288 */
#endif
	VGA,	/* 640*480 */

	SVGA,	/* 800*600 */
#if 0
	HD720P,	/* 1280*720 */
	SXGA,	/* 1280*1024 */
	UXGA,	/* 1600*1200 */
#endif
};
#endif
/*
 * Following values describe controls of camera
 * in user aspect and must be match with index of mt9d111_regset[]
 * These values indicates each controls and should be used
 * to control each control
 */
enum mt9d111_control {
	MT9D111_INIT,
	MT9D111_EV,
	MT9D111_AWB,
	MT9D111_MWB,
	MT9D111_EFFECT,
	MT9D111_CONTRAST,
	MT9D111_SATURATION,
	MT9D111_SHARPNESS,
};

#define MT9D111_REGSET(x)	{	\
	.regset = x,			\
	.len = sizeof(x)/sizeof(mt9d111_reg),}


/*
 * User tuned register setting values
 */
#define MT9D111_DELAY 0xF0
#define MT9D111_TERM 0xFF

struct mt9d111_reg mt9d111Init[] = 
{
	{0, 0x33, 0x0343 	}, // RESERVED_CORE_33
	{1, 0xC6, 0xA115	}, //SEQ_LLMODE
	{1, 0xC8, 0x0020	}, //SEQ_LLMODE
	{0, 0x38, 0x0866 	}, // RESERVED_CORE_38
	{MT9D111_DELAY, 0x00, 0x0064 }, // Delay =100ms
	{2, 0x80, 0x0168 	}, // LENS_CORRECTION_CONTROL
	{2, 0x81, 0x6432 	}, // ZONE_BOUNDS_X1_X2
	{2, 0x82, 0x3296 	}, // ZONE_BOUNDS_X0_X3
	{2, 0x83, 0x9664 	}, // ZONE_BOUNDS_X4_X5
	{2, 0x84, 0x5028 	}, // ZONE_BOUNDS_Y1_Y2
	{2, 0x85, 0x2878 	}, // ZONE_BOUNDS_Y0_Y3
	{2, 0x86, 0x7850 	}, // ZONE_BOUNDS_Y4_Y5
	{2, 0x87, 0x0000 	}, // CENTER_OFFSET
	{2, 0x88, 0x0152 	}, // FX_RED
	{2, 0x89, 0x015C 	}, // FX_GREEN
	{2, 0x8A, 0x00F4 	}, // FX_BLUE
	{2, 0x8B, 0x0108 	}, // FY_RED
	{2, 0x8C, 0x00FA 	}, // FY_GREEN
	{2, 0x8D, 0x00CF 	}, // FY_BLUE
	{2, 0x8E, 0x09AD 	}, // DF_DX_RED
	{2, 0x8F, 0x091E 	}, // DF_DX_GREEN
	{2, 0x90, 0x0B3F 	}, // DF_DX_BLUE
	{2, 0x91, 0x0C85 	}, // DF_DY_RED
	{2, 0x92, 0x0CFF 	}, // DF_DY_GREEN
	{2, 0x93, 0x0D86 	}, // DF_DY_BLUE
	{2, 0x94, 0x163A 	}, // SECOND_DERIV_ZONE_0_RED
	{2, 0x95, 0x0E47 	}, // SECOND_DERIV_ZONE_0_GREEN
	{2, 0x96, 0x103C 	}, // SECOND_DERIV_ZONE_0_BLUE
	{2, 0x97, 0x1D35 	}, // SECOND_DERIV_ZONE_1_RED
	{2, 0x98, 0x173E 	}, // SECOND_DERIV_ZONE_1_GREEN
	{2, 0x99, 0x1119 	}, // SECOND_DERIV_ZONE_1_BLUE
	{2, 0x9A, 0x1663 	}, // SECOND_DERIV_ZONE_2_RED
	{2, 0x9B, 0x1569 	}, // SECOND_DERIV_ZONE_2_GREEN
	{2, 0x9C, 0x104C 	}, // SECOND_DERIV_ZONE_2_BLUE
	{2, 0x9D, 0x1015 	}, // SECOND_DERIV_ZONE_3_RED
	{2, 0x9E, 0x1010 	}, // SECOND_DERIV_ZONE_3_GREEN
	{2, 0x9F, 0x0B0A 	}, // SECOND_DERIV_ZONE_3_BLUE
	{2, 0xA0, 0x0D53 	}, // SECOND_DERIV_ZONE_4_RED
	{2, 0xA1, 0x0D51 	}, // SECOND_DERIV_ZONE_4_GREEN
	{2, 0xA2, 0x0A44 	}, // SECOND_DERIV_ZONE_4_BLUE
	{2, 0xA3, 0x1545 	}, // SECOND_DERIV_ZONE_5_RED
	{2, 0xA4, 0x1643 	}, // SECOND_DERIV_ZONE_5_GREEN
	{2, 0xA5, 0x1231 	}, // SECOND_DERIV_ZONE_5_BLUE
	{2, 0xA6, 0x0047 	}, // SECOND_DERIV_ZONE_6_RED
	{2, 0xA7, 0x035C 	}, // SECOND_DERIV_ZONE_6_GREEN
	{2, 0xA8, 0xFE30 	}, // SECOND_DERIV_ZONE_6_BLUE
	{2, 0xA9, 0x4625 	}, // SECOND_DERIV_ZONE_7_RED
	{2, 0xAA, 0x47F3 	}, // SECOND_DERIV_ZONE_7_GREEN
	{2, 0xAB, 0x5859 	}, // SECOND_DERIV_ZONE_7_BLUE
	{2, 0xAC, 0x0000 	}, // X2_FACTORS
	{2, 0xAD, 0x0000 	}, // GLOBAL_OFFSET_FXY_FUNCTION
	{2, 0xAE, 0x0000 	}, // K_FACTOR_IN_K_FX_FY
	{1, 0x08, 0x01FC 	}, // COLOR_PIPELINE_CONTROL
	{MT9D111_DELAY, 0x00, 0x0064 }, // Delay =100ms
	{1, 0xBE, 0x0004 	}, // YUV_YCBCR_CONTROL
	{0, 0x65, 0xA000 	}, // CLOCK_ENABLING
	{MT9D111_DELAY, 0x00, 0x0064 }, // Delay =100ms
	{1, 0xC6, 0xA102	}, //SEQ_MODE
	{1, 0xC8, 0x001F	}, //SEQ_MODE
	{1, 0x08, 0x01FC 	}, // COLOR_PIPELINE_CONTROL
	{1, 0x08, 0x01EC 	}, // COLOR_PIPELINE_CONTROL
	{1, 0x08, 0x01FC 	}, // COLOR_PIPELINE_CONTROL
	{1, 0x36, 0x0F08 	}, // APERTURE_PARAMETERS
	{1, 0xC6, 0x270B	}, //MODE_CONFIG
	{1, 0xC8, 0x0030	}, //MODE_CONFIG, JPEG disabled for A and B
	{1, 0xC6, 0xA121	}, //SEQ_CAP_MODE
	{1, 0xC8, 0x007f	}, //SEQ_CAP_MODE (127 frames before switching to Preview)
	{0, 0x05, 0x011E 	}, // HORZ_BLANK_B
	{0, 0x06, 0x006F 	}, // VERT_BLANK_B
	{0, 0x07, 0xFE 		}, // HORZ_BLANK_A
	{0, 0x08, 19 		}, // VERT_BLANK_A
	{0, 0x20, 0x0300 	}, // READ_MODE_B (Image flip settings)
	{0, 0x21, 0x8400 	}, // READ_MODE_A (1ADC)
	{1, 0xC6, 0x2717	}, //MODE_SENSOR_X_DELAY_A
	{1, 0xC8, 792		}, //MODE_SENSOR_X_DELAY_A
	{1, 0xC6, 0x270F	}, //MODE_SENSOR_ROW_START_A
	{1, 0xC8, 0x001C	}, //MODE_SENSOR_ROW_START_A
	{1, 0xC6, 0x2711	}, //MODE_SENSOR_COL_START_A
	{1, 0xC8, 0x003C	}, //MODE_SENSOR_COL_START_A
	{1, 0xC6, 0x2713	}, //MODE_SENSOR_ROW_HEIGHT_A
	{1, 0xC8, 0x04B0	}, //MODE_SENSOR_ROW_HEIGHT_A
	{1, 0xC6, 0x2715	}, //MODE_SENSOR_COL_WIDTH_A
	{1, 0xC8, 0x0640	}, //MODE_SENSOR_COL_WIDTH_A
	{1, 0xC6, 0x2719	}, //MODE_SENSOR_ROW_SPEED_A
	{1, 0xC8, 0x0011	}, //MODE_SENSOR_ROW_SPEED_A
	{1, 0xC6, 0x2707	}, //MODE_OUTPUT_WIDTH_B
	{1, 0xC8, 0x0640	}, //MODE_OUTPUT_WIDTH_B
	{1, 0xC6, 0x2709	}, //MODE_OUTPUT_HEIGHT_B
	{1, 0xC8, 0x04B0	}, //MODE_OUTPUT_HEIGHT_B
	{1, 0xC6, 0x271B	}, //MODE_SENSOR_ROW_START_B
	{1, 0xC8, 0x001C	}, //MODE_SENSOR_ROW_START_B
	{1, 0xC6, 0x271D	}, //MODE_SENSOR_COL_START_B
	{1, 0xC8, 0x003C	}, //MODE_SENSOR_COL_START_B
	{1, 0xC6, 0x271F	}, //MODE_SENSOR_ROW_HEIGHT_B
	{1, 0xC8, 0x04B0	}, //MODE_SENSOR_ROW_HEIGHT_B
	{1, 0xC6, 0x2721	}, //MODE_SENSOR_COL_WIDTH_B
	{1, 0xC8, 0x0640	}, //MODE_SENSOR_COL_WIDTH_B
	{1, 0xC6, 0x2723	}, //MODE_SENSOR_X_DELAY_B
	{1, 0xC8, 0x0716	}, //MODE_SENSOR_X_DELAY_B
	{1, 0xC6, 0x2725	}, //MODE_SENSOR_ROW_SPEED_B
	{1, 0xC8, 0x0011	}, //MODE_SENSOR_ROW_SPEED_B
	{1, 0xC6, 0x2703    }, //MODE_OUTPUT_WIDTH_A
	{1, 0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_A
	{1, 0xC6, 0x2705    }, //MODE_OUTPUT_HEIGHT_A
	{1, 0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_A
	{1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
	{1, 0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_B
	{1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
	{1, 0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_B
	{1, 0xC6, 0x2779    }, //Spoof Frame Width
	{1, 0xC8, 0x0280    }, //Spoof Frame Width
	{1, 0xC6, 0x277B    }, //Spoof Frame Height
	{1, 0xC8, 0x01E0    }, //Spoof Frame Height
	{1, 0xC6, 0xA103    }, //SEQ_CMD
	{1, 0xC8, 0x0005    }, //SEQ_CMD
	//Maximum Slew-Rate on IO-Pads (for Mode A)
	{1, 0xC6, 0x276B	}, //MODE_FIFO_CONF0_A
	{1, 0xC8, 0x0027	}, //MODE_FIFO_CONF0_A
	{1, 0xC6, 0x276D	}, //MODE_FIFO_CONF1_A
	{1, 0xC8, 0xE1E1	}, //MODE_FIFO_CONF1_A
	{1, 0xC6, 0xA76F	}, //MODE_FIFO_CONF2_A
	{1, 0xC8, 0x00E1	}, //MODE_FIFO_CONF2_A
	//Maximum Slew-Rate on IO-Pads (for Mode B)
	{1, 0xC6, 0x2772	}, //MODE_FIFO_CONF0_B
	{1, 0xC8, 0x0027	}, //MODE_FIFO_CONF0_B
	{1, 0xC6, 0x2774	}, //MODE_FIFO_CONF1_B
	{1, 0xC8, 0xE1E1	}, //MODE_FIFO_CONF1_B
	{1, 0xC6, 0xA776	}, //MODE_FIFO_CONF2_B
	{1, 0xC8, 0x00E1	}, //MODE_FIFO_CONF2_B
	//Set maximum integration time to get a minimum of 15 fps at 45MHz
	{1, 0xC6, 0xA20E	}, //AE_MAX_INDEX
	{1, 0xC8, 0x0004	}, //AE_MAX_INDEX
	//Set minimum integration time to get a maximum of 15 fps at 45MHz
	{1, 0xC6, 0xA20D	}, //AE_MAX_INDEX
	{1, 0xC8, 0x0004	}, //AE_MAX_INDEX
	// Configue all GPIO for output and set low to save power
	{1, 0xC6, 0x9078	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9079	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9070	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9071	},
	{1, 0xC8, 0x0000	},
	// gamma and contrast
	{1, 0xC6, 0xA743	}, // MODE_GAM_CONT_A
	{1, 0xC8, 0x0003	}, // MODE_GAM_CONT_A
	{1, 0xC6, 0xA744	}, // MODE_GAM_CONT_B
	{1, 0xC8, 0x0003	}, // MODE_GAM_CONT_B
	{MT9D111_DELAY, 0x00, 0x01F4 }, // Delay =500m
//	Set PLL
	{0, 0xF2, 0x0000	}, 	
	{0, 0x65, 0xE000	}, 	
	{0, 0x66, 0x4805	}, 	// PLL_REG
	{0, 0x67, 0x0305	}, 	// PLL2_REG
	{0, 0x65, 0xA000	}, 	// CLOCK_ENABLING
	{MT9D111_DELAY, 0x00, 0x01F4 }, // Delay =500ms
	{0, 0x65, 0x2000	}, 	// CLOCK_ENABLING
	{MT9D111_DELAY, 0x00, 0x01F4 }, // Delay =500ms
	{MT9D111_TERM, 0, 0 }
};

// For VGA ( 640 x 480) on 3BA module
struct mt9d111_reg mt9d111_vga[] =
{
    {1, 0xC6, 0x2703    }, //MODE_OUTPUT_WIDTH_A
    {1, 0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_A
    {1, 0xC6, 0x2705    }, //MODE_OUTPUT_HEIGHT_A
    {1, 0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_A
    {1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC8, 0x0280    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC8, 0x01E0    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC6, 0x2779    }, //Spoof Frame Width
    {1, 0xC8, 0x0280    }, //Spoof Frame Width
    {1, 0xC6, 0x277B    }, //Spoof Frame Height
    {1, 0xC8, 0x01E0    }, //Spoof Frame Height
    {1, 0xC6, 0xA103    }, //SEQ_CMD
    {1, 0xC8, 0x0005    }, //SEQ_CMD
    {MT9D111_TERM, 0, 0 }
};

// For SVGA (800 x 600) on 3BA module
struct mt9d111_reg mt9d111_svga[] =
{
    {1, 0xC6, 0x2703    }, //MODE_OUTPUT_WIDTH_A
    {1, 0xC8, 0x0320    }, //MODE_OUTPUT_WIDTH_A
    {1, 0xC6, 0x2705    }, //MODE_OUTPUT_HEIGHT_A
    {1, 0xC8, 0x0258    }, //MODE_OUTPUT_HEIGHT_A
    {1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC8, 0x0320    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC8, 0x0258    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC6, 0x2779    }, //Spoof Frame Width
    {1, 0xC8, 0x0320    }, //Spoof Frame Width
    {1, 0xC6, 0x277B    }, //Spoof Frame Height
    {1, 0xC8, 0x0258    }, //Spoof Frame Height
    {1, 0xC6, 0xA103    }, //SEQ_CMD
    {1, 0xC8, 0x0005    }, //SEQ_CMD
    {MT9D111_TERM, 0, 0 }
};

// For SXGA (1280 x 1024 = 1.3M) on 3BA module
struct mt9d111_reg mt9d111_sxga[] =
{
    {1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC8, 0x0500    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC8, 0x0400    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC6, 0x2779    }, //Spoof Frame Width
    {1, 0xC8, 0x0500    }, //Spoof Frame Width
    {1, 0xC6, 0x277B    }, //Spoof Frame Height
    {1, 0xC8, 0x0400    }, //Spoof Frame Height
    {1, 0xC6, 0xA103    }, //SEQ_CMD
    {1, 0xC8, 0x0005    }, //SEQ_CMD
    {MT9D111_TERM, 0, 0 }
};

// For UXGA (1600 x 1200 = 2M) on 3BA module
struct mt9d111_reg mt9d111_uxga[] =
{
    {1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC8, 0x0640    }, //MODE_OUTPUT_WIDTH_B
    {1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC8, 0x04B0    }, //MODE_OUTPUT_HEIGHT_B
    {1, 0xC6, 0x2779    }, //Spoof Frame Width
    {1, 0xC8, 0x0640    }, //Spoof Frame Width
    {1, 0xC6, 0x277B    }, //Spoof Frame Height
    {1, 0xC8, 0x04B0    }, //Spoof Frame Height
    {1, 0xC6, 0xA103    }, //SEQ_CMD
    {1, 0xC8, 0x0005    }, //SEQ_CMD
    {MT9D111_TERM, 0, 0 }
};
#if 0
// For SVGA ( 800 x 600) on 4BA module
static unsigned char mt9d111_svga_reg[][2] = {
	{0xfc,0x02},
	{0x2d,0x48},
	{0x44,0x63},

	{0xfc,0x03},
	{0x02,0x04},
	{0xfc,0x20},
	{0x14,0x70},

	{0xfc,0x00},
	{0x03,0x4b},		// AE/AWB On
	{0x7e,0xf4},		// Suppress On
	{0x89,0x03},		// Edge Suppress On

	{0xfc,0x02},
	{0x02,0x0e},//sensor BPRoff

	{0xfc,0x20},
	{0x16,0x60},		// Frame AE Start

	{0xfc,0x02},
	{0x30,0x90},		// Analog offset
	{0x37,0x0d},		// Global Gain
	{0x60,0x00},		// Blank_Adrs
	{0x45,0x0e},		// CDS Timing for Average Sub_Sampling
	{0x47,0x2f},

	{0xfc,0x01},
	{0x9F,0x05},		//B
	{0xA0,0x18},
	{0xA1,0x42},
	{0xA2,0xd7}, 		
	{0xA3,0x00},
	
	{0xA4,0xB6},
	{0xA5,0x3b},
	{0xA6,0x88},
	{0xA7,0xC8},
	{0xA8,0x6A},

	{0xfc,0x05},
	{0x34,0x20},	// APTCLP
	{0x35,0x08},	//9 //APTSC

	{0xfc,0x00},  // flash 0821
	{0x32,0x04},  // AWB moving average 8 frame

	{0xfc,0x01},
	{0x01,0x01}, // Pclk inversion

	{0xfc,0x00},
	{0x02,0x09},		// 800 x 600


	{0xFF,0xFF} // REGISTER END
};

/* SQVGA */
static unsigned char mt9d111_qsvga_reg[][2] =
{
	// Pclk inversion
	{0xfc,0x01},
	{0x01,0x01},

	// To setting CbCr selection on Table 14h
	{0xfc, 0x14},
	{0x5c, 0x00},

	// To load table_11 H4V4
	{0xfc, 0x00},
	{0x02, 0x0B}
};
#endif
#define MT9D111_INIT_REGS	(sizeof(mt9d111_init_reg) / sizeof(mt9d111_init_reg[0]))
#define MT9D111_SVGA_REGS	(sizeof(mt9d111_svga_reg) / sizeof(mt9d111_svga_reg[0]))
#define MT9D111_QSVGA_REGS	(sizeof(mt9d111_qsvga_reg) / sizeof(mt9d111_qsvga_reg[0]))

/*
 * EV bias
 */

static const struct mt9d111_reg mt9d111_ev_m6[] = {
};

static const struct mt9d111_reg mt9d111_ev_m5[] = {
};

static const struct mt9d111_reg mt9d111_ev_m4[] = {
};

static const struct mt9d111_reg mt9d111_ev_m3[] = {
};

static const struct mt9d111_reg mt9d111_ev_m2[] = {
};

static const struct mt9d111_reg mt9d111_ev_m1[] = {
};

static const struct mt9d111_reg mt9d111_ev_default[] = {
};

static const struct mt9d111_reg mt9d111_ev_p1[] = {
};

static const struct mt9d111_reg mt9d111_ev_p2[] = {
};

static const struct mt9d111_reg mt9d111_ev_p3[] = {
};

static const struct mt9d111_reg mt9d111_ev_p4[] = {
};

static const struct mt9d111_reg mt9d111_ev_p5[] = {
};

static const struct mt9d111_reg mt9d111_ev_p6[] = {
};

#ifdef MT9D111_COMPLETE
/* Order of this array should be following the querymenu data */
static const unsigned char *mt9d111_regs_ev_bias[] = {
	(unsigned char *)mt9d111_ev_m6, (unsigned char *)mt9d111_ev_m5,
	(unsigned char *)mt9d111_ev_m4, (unsigned char *)mt9d111_ev_m3,
	(unsigned char *)mt9d111_ev_m2, (unsigned char *)mt9d111_ev_m1,
	(unsigned char *)mt9d111_ev_default, (unsigned char *)mt9d111_ev_p1,
	(unsigned char *)mt9d111_ev_p2, (unsigned char *)mt9d111_ev_p3,
	(unsigned char *)mt9d111_ev_p4, (unsigned char *)mt9d111_ev_p5,
	(unsigned char *)mt9d111_ev_p6,
};

/*
 * Auto White Balance configure
 */
#if !defined CONFIG_ARCH_S3C64XX
static const struct mt9d111_reg mt9d111_awb_off[] = {
};
#endif
static const struct mt9d111_reg mt9d111_awb_on[] = {	
	{0xfc, 0x00},
	{0x30, 0x00}
};

#if !defined CONFIG_ARCH_S3C64XX
static const unsigned char *mt9d111_regs_awb_enable[] = {
	(unsigned char *)mt9d111_awb_off,
	(unsigned char *)mt9d111_awb_on,
};
#endif

/*
 * Manual White Balance (presets)
 */
 
#if defined CONFIG_ARCH_S3C64XX
static const struct mt9d111_reg mt9d111_wb_outdoor_3001[] = {
	{0xfc, 0x00},
	{0x30, 0x01}
};

static const struct mt9d111_reg mt9d111_wb_outdoor_5100[] = {
	{0xfc, 0x00},
	{0x30, 0x02}
};

static const struct mt9d111_reg mt9d111_wb_indoor_2000[] = {
	{0xfc, 0x00},
	{0x30, 0x03}
};

static const struct mt9d111_reg mt9d111_wb_outdoor_halt[] = {
	{0xfc, 0x00},
	{0x30, 0x04}
};

static const struct mt9d111_reg mt9d111_wb_cloudy[] = {
	{0xfc, 0x00},
	{0x30, 0x05}
};

static const struct mt9d111_reg mt9d111_wb_sunny[] = {
	{0xfc, 0x00},
	{0x30, 0x06}
};

/* Order of this array should be following the querymenu data */
static const unsigned char *mt9d111_regs_wb_preset[] = {
	(unsigned char *)mt9d111_awb_on,
	(unsigned char *)mt9d111_wb_outdoor_3001,
	(unsigned char *)mt9d111_wb_outdoor_5100,
	(unsigned char *)mt9d111_wb_indoor_2000,
	(unsigned char *)mt9d111_wb_outdoor_halt,
	(unsigned char *)mt9d111_wb_cloudy,
	(unsigned char *)mt9d111_wb_sunny,
};

#else
static const struct mt9d111_reg mt9d111_wb_tungsten[] = {

};

static const struct mt9d111_reg mt9d111_wb_fluorescent[] = {

};

static const struct mt9d111_reg mt9d111_wb_sunny[] = {

};

static const struct mt9d111_reg mt9d111_wb_cloudy[] = {

};

/* Order of this array should be following the querymenu data */
static const unsigned char *mt9d111_regs_wb_preset[] = {
	(unsigned char *)mt9d111_wb_tungsten,
	(unsigned char *)mt9d111_wb_fluorescent,
	(unsigned char *)mt9d111_wb_sunny,
	(unsigned char *)mt9d111_wb_cloudy,
};
#endif
/*
 * Color Effect (COLORFX)
 */
static const struct mt9d111_reg mt9d111_color_sepia[] = {
};

static const struct mt9d111_reg mt9d111_color_aqua[] = {
};

static const struct mt9d111_reg mt9d111_color_monochrome[] = {
};

static const struct mt9d111_reg mt9d111_color_negative[] = {
};

static const struct mt9d111_reg mt9d111_color_sketch[] = {
};

/* Order of this array should be following the querymenu data */
static const unsigned char *mt9d111_regs_color_effect[] = {
	(unsigned char *)mt9d111_color_sepia,
	(unsigned char *)mt9d111_color_aqua,
	(unsigned char *)mt9d111_color_monochrome,
	(unsigned char *)mt9d111_color_negative,
	(unsigned char *)mt9d111_color_sketch,
};

/*
 * Contrast bias
 */
static const struct mt9d111_reg mt9d111_contrast_m2[] = {
};

static const struct mt9d111_reg mt9d111_contrast_m1[] = {
};

static const struct mt9d111_reg mt9d111_contrast_default[] = {
};

static const struct mt9d111_reg mt9d111_contrast_p1[] = {
};

static const struct mt9d111_reg mt9d111_contrast_p2[] = {
};

static const unsigned char *mt9d111_regs_contrast_bias[] = {
	(unsigned char *)mt9d111_contrast_m2,
	(unsigned char *)mt9d111_contrast_m1,
	(unsigned char *)mt9d111_contrast_default,
	(unsigned char *)mt9d111_contrast_p1,
	(unsigned char *)mt9d111_contrast_p2,
};

/*
 * Saturation bias
 */
static const struct mt9d111_reg mt9d111_saturation_m2[] = {
};

static const struct mt9d111_reg mt9d111_saturation_m1[] = {
};

static const struct mt9d111_reg mt9d111_saturation_default[] = {
};

static const struct mt9d111_reg mt9d111_saturation_p1[] = {
};

static const struct mt9d111_reg mt9d111_saturation_p2[] = {
};

static const unsigned char *mt9d111_regs_saturation_bias[] = {
	(unsigned char *)mt9d111_saturation_m2,
	(unsigned char *)mt9d111_saturation_m1,
	(unsigned char *)mt9d111_saturation_default,
	(unsigned char *)mt9d111_saturation_p1,
	(unsigned char *)mt9d111_saturation_p2,
};

/*
 * Sharpness bias
 */
static const struct mt9d111_reg mt9d111_sharpness_m2[] = {
};

static const struct mt9d111_reg mt9d111_sharpness_m1[] = {
};

static const struct mt9d111_reg mt9d111_sharpness_default[] = {
};

static const struct mt9d111_reg mt9d111_sharpness_p1[] = {
};

static const struct mt9d111_reg mt9d111_sharpness_p2[] = {
};

static const unsigned char *mt9d111_regs_sharpness_bias[] = {
	(unsigned char *)mt9d111_sharpness_m2,
	(unsigned char *)mt9d111_sharpness_m1,
	(unsigned char *)mt9d111_sharpness_default,
	(unsigned char *)mt9d111_sharpness_p1,
	(unsigned char *)mt9d111_sharpness_p2,
};
#endif /* MT9D111_COMPLETE */

#endif
