
#ifndef __MT9D111_H__
#define __MT9D111_H__

/*******************************************************************************************
 #  Display resolution standards #
	QCIF	: 176 x 144
	CIF		: 352 x 288
	QVGA	: 320 x 240
	VGA		: 640 x 480 
	SVGA	: 800 x 600 
	XGA		: 1024 x 768 
	WXGA	: 1280 x 800 
	QVGA	: 1280 x 960 
	SXGA	: 1280 x 1024 
	SXGA+	: 1400 x 1050 
	WSXGA+	: 1680 x 1050 
	UXGA	: 1600 x 1200 
	WUXGA	: 1920 x 1200 
	QXGA	: 2048 x 1536
********************************************************************************************/

#define MT9D111_DELAY 0xF0
#define MT9D111_TERM 0xFF
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
	HD720P,	/* 1280*720 */
	SXGA,	/* 1280*1024 */
	UXGA,	/* 1600*1200 */
};
#endif
                                                                               
struct mt9d111_reg{
        unsigned char page;
        unsigned char subaddr;
        unsigned short value;
};

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
	{1, 0xC6, 0x104C 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0000 	}, // MCU_DATA_0
	{1, 0xC6, 0x0400 	}, // MCU_ADDRESS
	{1, 0xC8, 0x3736 	}, // MCU_DATA_0
	{1, 0xC9, 0x3C3C 	}, // MCU_DATA_1
	{1, 0xCA, 0x3C3C 	}, // MCU_DATA_2
	{1, 0xCB, 0x30EC 	}, // MCU_DATA_3
	{1, 0xCC, 0x08BD 	}, // MCU_DATA_4
	{1, 0xCD, 0xA1B6 	}, // MCU_DATA_5
	{1, 0xCE, 0xD6A9 	}, // MCU_DATA_6
	{1, 0xCF, 0xF102 	}, // MCU_DATA_7
	{1, 0xC6, 0x0410 	}, // MCU_ADDRESS
	{1, 0xC8, 0xBD22 	}, // MCU_DATA_0
	{1, 0xC9, 0x6DF6 	}, // MCU_DATA_1
	{1, 0xCA, 0x02BE 	}, // MCU_DATA_2
	{1, 0xCB, 0x4F30 	}, // MCU_DATA_3
	{1, 0xCC, 0xED06 	}, // MCU_DATA_4
	{1, 0xCD, 0xF602 	}, // MCU_DATA_5
	{1, 0xCE, 0xC1ED 	}, // MCU_DATA_6
	{1, 0xCF, 0x04EC 	}, // MCU_DATA_7
	{1, 0xC6, 0x0420 	}, // MCU_ADDRESS
	{1, 0xC8, 0x06A3 	}, // MCU_DATA_0
	{1, 0xC9, 0x04ED 	}, // MCU_DATA_1
	{1, 0xCA, 0x02D6 	}, // MCU_DATA_2
	{1, 0xCB, 0xA94F 	}, // MCU_DATA_3
	{1, 0xCC, 0xED00 	}, // MCU_DATA_4
	{1, 0xCD, 0xF602 	}, // MCU_DATA_5
	{1, 0xCE, 0xBDBD 	}, // MCU_DATA_6
	{1, 0xCF, 0xD268 	}, // MCU_DATA_7
	{1, 0xC6, 0x0430 	}, // MCU_ADDRESS
	{1, 0xC8, 0xF602 	}, // MCU_DATA_0
	{1, 0xC9, 0xBE30 	}, // MCU_DATA_1
	{1, 0xCA, 0xE003 	}, // MCU_DATA_2
	{1, 0xCB, 0xF702 	}, // MCU_DATA_3
	{1, 0xCC, 0xC7F6 	}, // MCU_DATA_4
	{1, 0xCD, 0x02BF 	}, // MCU_DATA_5
	{1, 0xCE, 0x4FED 	}, // MCU_DATA_6
	{1, 0xCF, 0x06F6 	}, // MCU_DATA_7
	{1, 0xC6, 0x0440 	}, // MCU_ADDRESS
	{1, 0xC8, 0x02C2 	}, // MCU_DATA_0
	{1, 0xC9, 0xED04 	}, // MCU_DATA_1
	{1, 0xCA, 0xEC06 	}, // MCU_DATA_2
	{1, 0xCB, 0xA304 	}, // MCU_DATA_3
	{1, 0xCC, 0xED02 	}, // MCU_DATA_4
	{1, 0xCD, 0xD6A9 	}, // MCU_DATA_5
	{1, 0xCE, 0x4FED 	}, // MCU_DATA_6
	{1, 0xCF, 0x00F6 	}, // MCU_DATA_7
	{1, 0xC6, 0x0450 	}, // MCU_ADDRESS
	{1, 0xC8, 0x02BD 	}, // MCU_DATA_0
	{1, 0xC9, 0xBDD2 	}, // MCU_DATA_1
	{1, 0xCA, 0x68F6 	}, // MCU_DATA_2
	{1, 0xCB, 0x02BF 	}, // MCU_DATA_3
	{1, 0xCC, 0x30E0 	}, // MCU_DATA_4
	{1, 0xCD, 0x03F7 	}, // MCU_DATA_5
	{1, 0xCE, 0x02C8 	}, // MCU_DATA_6
	{1, 0xCF, 0xF602 	}, // MCU_DATA_7
	{1, 0xC6, 0x0460 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC04F 	}, // MCU_DATA_0
	{1, 0xC9, 0xED06 	}, // MCU_DATA_1
	{1, 0xCA, 0xF602 	}, // MCU_DATA_2
	{1, 0xCB, 0xC3ED 	}, // MCU_DATA_3
	{1, 0xCC, 0x04EC 	}, // MCU_DATA_4
	{1, 0xCD, 0x06A3 	}, // MCU_DATA_5
	{1, 0xCE, 0x04ED 	}, // MCU_DATA_6
	{1, 0xCF, 0x02D6 	}, // MCU_DATA_7
	{1, 0xC6, 0x0470 	}, // MCU_ADDRESS
	{1, 0xC8, 0xA94F 	}, // MCU_DATA_0
	{1, 0xC9, 0xED00 	}, // MCU_DATA_1
	{1, 0xCA, 0xF602 	}, // MCU_DATA_2
	{1, 0xCB, 0xBDBD 	}, // MCU_DATA_3
	{1, 0xCC, 0xD268 	}, // MCU_DATA_4
	{1, 0xCD, 0xF602 	}, // MCU_DATA_5
	{1, 0xCE, 0xC07E 	}, // MCU_DATA_6
	{1, 0xC6, 0x0480 	}, // MCU_ADDRESS
	{1, 0xC8, 0xF602 	}, // MCU_DATA_0
	{1, 0xC9, 0xC44F 	}, // MCU_DATA_1
	{1, 0xCA, 0x30ED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06F6 	}, // MCU_DATA_3
	{1, 0xCC, 0x02C1 	}, // MCU_DATA_4
	{1, 0xCD, 0xED04 	}, // MCU_DATA_5
	{1, 0xCE, 0xEC06 	}, // MCU_DATA_6
	{1, 0xCF, 0xA304 	}, // MCU_DATA_7
	{1, 0xC6, 0x0490 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED02 	}, // MCU_DATA_0
	{1, 0xC9, 0xD6A8 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06D6 	}, // MCU_DATA_3
	{1, 0xCC, 0xA9ED 	}, // MCU_DATA_4
	{1, 0xCD, 0x04EC 	}, // MCU_DATA_5
	{1, 0xCE, 0x06A3 	}, // MCU_DATA_6
	{1, 0xCF, 0x04ED 	}, // MCU_DATA_7
	{1, 0xC6, 0x04A0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x00F6 	}, // MCU_DATA_0
	{1, 0xC9, 0x02BD 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FBD 	}, // MCU_DATA_2
	{1, 0xCB, 0xD268 	}, // MCU_DATA_3
	{1, 0xCC, 0xF602 	}, // MCU_DATA_4
	{1, 0xCD, 0xC430 	}, // MCU_DATA_5
	{1, 0xCE, 0xE003 	}, // MCU_DATA_6
	{1, 0xCF, 0xF702 	}, // MCU_DATA_7
	{1, 0xC6, 0x04B0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC7F6 	}, // MCU_DATA_0
	{1, 0xC9, 0x02C5 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06F6 	}, // MCU_DATA_3
	{1, 0xCC, 0x02C2 	}, // MCU_DATA_4
	{1, 0xCD, 0xED04 	}, // MCU_DATA_5
	{1, 0xCE, 0xEC06 	}, // MCU_DATA_6
	{1, 0xCF, 0xA304 	}, // MCU_DATA_7
	{1, 0xC6, 0x04C0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED02 	}, // MCU_DATA_0
	{1, 0xC9, 0xD6A8 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06D6 	}, // MCU_DATA_3
	{1, 0xCC, 0xA9ED 	}, // MCU_DATA_4
	{1, 0xCD, 0x04EC 	}, // MCU_DATA_5
	{1, 0xCE, 0x06A3 	}, // MCU_DATA_6
	{1, 0xCF, 0x04ED 	}, // MCU_DATA_7
	{1, 0xC6, 0x04D0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x00F6 	}, // MCU_DATA_0
	{1, 0xC9, 0x02BD 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FBD 	}, // MCU_DATA_2
	{1, 0xCB, 0xD268 	}, // MCU_DATA_3
	{1, 0xCC, 0xF602 	}, // MCU_DATA_4
	{1, 0xCD, 0xC530 	}, // MCU_DATA_5
	{1, 0xCE, 0xE003 	}, // MCU_DATA_6
	{1, 0xCF, 0xF702 	}, // MCU_DATA_7
	{1, 0xC6, 0x04E0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC8F6 	}, // MCU_DATA_0
	{1, 0xC9, 0x02C6 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06F6 	}, // MCU_DATA_3
	{1, 0xCC, 0x02C3 	}, // MCU_DATA_4
	{1, 0xCD, 0xED04 	}, // MCU_DATA_5
	{1, 0xCE, 0xEC06 	}, // MCU_DATA_6
	{1, 0xCF, 0xA304 	}, // MCU_DATA_7
	{1, 0xC6, 0x04F0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED02 	}, // MCU_DATA_0
	{1, 0xC9, 0xD6A8 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FED 	}, // MCU_DATA_2
	{1, 0xCB, 0x06D6 	}, // MCU_DATA_3
	{1, 0xCC, 0xA9ED 	}, // MCU_DATA_4
	{1, 0xCD, 0x04EC 	}, // MCU_DATA_5
	{1, 0xCE, 0x06A3 	}, // MCU_DATA_6
	{1, 0xCF, 0x04ED 	}, // MCU_DATA_7
	{1, 0xC6, 0x0500 	}, // MCU_ADDRESS
	{1, 0xC8, 0x00F6 	}, // MCU_DATA_0
	{1, 0xC9, 0x02BD 	}, // MCU_DATA_1
	{1, 0xCA, 0x4FBD 	}, // MCU_DATA_2
	{1, 0xCB, 0xD268 	}, // MCU_DATA_3
	{1, 0xCC, 0xF602 	}, // MCU_DATA_4
	{1, 0xCD, 0xC630 	}, // MCU_DATA_5
	{1, 0xCE, 0xE003 	}, // MCU_DATA_6
	{1, 0xCF, 0xF702 	}, // MCU_DATA_7
	{1, 0xC6, 0x0510 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC9EE 	}, // MCU_DATA_0
	{1, 0xC9, 0x08EC 	}, // MCU_DATA_1
	{1, 0xCA, 0x0030 	}, // MCU_DATA_2
	{1, 0xCB, 0xED02 	}, // MCU_DATA_3
	{1, 0xCC, 0xF602 	}, // MCU_DATA_4
	{1, 0xCD, 0xC74F 	}, // MCU_DATA_5
	{1, 0xCE, 0xED00 	}, // MCU_DATA_6
	{1, 0xCF, 0xCC00 	}, // MCU_DATA_7
	{1, 0xC6, 0x0520 	}, // MCU_ADDRESS
	{1, 0xC8, 0x80BD 	}, // MCU_DATA_0
	{1, 0xC9, 0xD268 	}, // MCU_DATA_1
	{1, 0xCA, 0x301A 	}, // MCU_DATA_2
	{1, 0xCB, 0xEE08 	}, // MCU_DATA_3
	{1, 0xCC, 0xEC02 	}, // MCU_DATA_4
	{1, 0xCD, 0x18ED 	}, // MCU_DATA_5
	{1, 0xCE, 0x00EE 	}, // MCU_DATA_6
	{1, 0xCF, 0x08EC 	}, // MCU_DATA_7
	{1, 0xC6, 0x0530 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0230 	}, // MCU_DATA_0
	{1, 0xC9, 0xED02 	}, // MCU_DATA_1
	{1, 0xCA, 0xF602 	}, // MCU_DATA_2
	{1, 0xCB, 0xC84F 	}, // MCU_DATA_3
	{1, 0xCC, 0xED00 	}, // MCU_DATA_4
	{1, 0xCD, 0xCC00 	}, // MCU_DATA_5
	{1, 0xCE, 0x80BD 	}, // MCU_DATA_6
	{1, 0xCF, 0xD268 	}, // MCU_DATA_7
	{1, 0xC6, 0x0540 	}, // MCU_ADDRESS
	{1, 0xC8, 0x301A 	}, // MCU_DATA_0
	{1, 0xC9, 0xEE08 	}, // MCU_DATA_1
	{1, 0xCA, 0xEC02 	}, // MCU_DATA_2
	{1, 0xCB, 0x18ED 	}, // MCU_DATA_3
	{1, 0xCC, 0x02EE 	}, // MCU_DATA_4
	{1, 0xCD, 0x08EC 	}, // MCU_DATA_5
	{1, 0xCE, 0x0430 	}, // MCU_DATA_6
	{1, 0xCF, 0xED02 	}, // MCU_DATA_7
	{1, 0xC6, 0x0550 	}, // MCU_ADDRESS
	{1, 0xC8, 0xF602 	}, // MCU_DATA_0
	{1, 0xC9, 0xC94F 	}, // MCU_DATA_1
	{1, 0xCA, 0xED00 	}, // MCU_DATA_2
	{1, 0xCB, 0xCC00 	}, // MCU_DATA_3
	{1, 0xCC, 0x80BD 	}, // MCU_DATA_4
	{1, 0xCD, 0xD268 	}, // MCU_DATA_5
	{1, 0xCE, 0x301A 	}, // MCU_DATA_6
	{1, 0xCF, 0xEE08 	}, // MCU_DATA_7
	{1, 0xC6, 0x0560 	}, // MCU_ADDRESS
	{1, 0xC8, 0xEC02 	}, // MCU_DATA_0
	{1, 0xC9, 0x18ED 	}, // MCU_DATA_1
	{1, 0xCA, 0x0430 	}, // MCU_DATA_2
	{1, 0xCB, 0xC60A 	}, // MCU_DATA_3
	{1, 0xCC, 0x3A35 	}, // MCU_DATA_4
	{1, 0xCD, 0x3930 	}, // MCU_DATA_5
	{1, 0xCE, 0x8FC3 	}, // MCU_DATA_6
	{1, 0xCF, 0xFFE5 	}, // MCU_DATA_7
	{1, 0xC6, 0x0570 	}, // MCU_ADDRESS
	{1, 0xC8, 0x8F35 	}, // MCU_DATA_0
	{1, 0xC9, 0x6F1A 	}, // MCU_DATA_1
	{1, 0xCA, 0xDC8C 	}, // MCU_DATA_2
	{1, 0xCB, 0x2C02 	}, // MCU_DATA_3
	{1, 0xCC, 0x6C1A 	}, // MCU_DATA_4
	{1, 0xCD, 0xDC8E 	}, // MCU_DATA_5
	{1, 0xCE, 0x2C06 	}, // MCU_DATA_6
	{1, 0xCF, 0xE61A 	}, // MCU_DATA_7
	{1, 0xC6, 0x0580 	}, // MCU_ADDRESS
	{1, 0xC8, 0xCB02 	}, // MCU_DATA_0
	{1, 0xC9, 0xE71A 	}, // MCU_DATA_1
	{1, 0xCA, 0xDC90 	}, // MCU_DATA_2
	{1, 0xCB, 0x2C06 	}, // MCU_DATA_3
	{1, 0xCC, 0xE61A 	}, // MCU_DATA_4
	{1, 0xCD, 0xCB04 	}, // MCU_DATA_5
	{1, 0xCE, 0xE71A 	}, // MCU_DATA_6
	{1, 0xCF, 0xDC94 	}, // MCU_DATA_7
	{1, 0xC6, 0x0590 	}, // MCU_ADDRESS
	{1, 0xC8, 0x2C06 	}, // MCU_DATA_0
	{1, 0xC9, 0xE61A 	}, // MCU_DATA_1
	{1, 0xCA, 0xCB08 	}, // MCU_DATA_2
	{1, 0xCB, 0xE71A 	}, // MCU_DATA_3
	{1, 0xCC, 0xDC96 	}, // MCU_DATA_4
	{1, 0xCD, 0x2C06 	}, // MCU_DATA_5
	{1, 0xCE, 0xE61A 	}, // MCU_DATA_6
	{1, 0xCF, 0xCB10 	}, // MCU_DATA_7
	{1, 0xC6, 0x05A0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xE71A 	}, // MCU_DATA_0
	{1, 0xC9, 0xDC98 	}, // MCU_DATA_1
	{1, 0xCA, 0x2C06 	}, // MCU_DATA_2
	{1, 0xCB, 0xE61A 	}, // MCU_DATA_3
	{1, 0xCC, 0xCB20 	}, // MCU_DATA_4
	{1, 0xCD, 0xE71A 	}, // MCU_DATA_5
	{1, 0xCE, 0x5F4F 	}, // MCU_DATA_6
	{1, 0xCF, 0xED0D 	}, // MCU_DATA_7
	{1, 0xC6, 0x05B0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED0B 	}, // MCU_DATA_0
	{1, 0xC9, 0xC608 	}, // MCU_DATA_1
	{1, 0xCA, 0xE709 	}, // MCU_DATA_2
	{1, 0xCB, 0x4FE6 	}, // MCU_DATA_3
	{1, 0xCC, 0x092A 	}, // MCU_DATA_4
	{1, 0xCD, 0x0143 	}, // MCU_DATA_5
	{1, 0xCE, 0x058F 	}, // MCU_DATA_6
	{1, 0xCF, 0xEC8A 	}, // MCU_DATA_7
	{1, 0xC6, 0x05C0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x30ED 	}, // MCU_DATA_0
	{1, 0xC9, 0x0F2C 	}, // MCU_DATA_1
	{1, 0xCA, 0x065F 	}, // MCU_DATA_2
	{1, 0xCB, 0x4FA3 	}, // MCU_DATA_3
	{1, 0xCC, 0x0FED 	}, // MCU_DATA_4
	{1, 0xCD, 0x0FC6 	}, // MCU_DATA_5
	{1, 0xCE, 0x04E7 	}, // MCU_DATA_6
	{1, 0xCF, 0x0A20 	}, // MCU_DATA_7
	{1, 0xC6, 0x05D0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x07EC 	}, // MCU_DATA_0
	{1, 0xC9, 0x0F04 	}, // MCU_DATA_1
	{1, 0xCA, 0xED0F 	}, // MCU_DATA_2
	{1, 0xCB, 0x6A0A 	}, // MCU_DATA_3
	{1, 0xCC, 0x6D0F 	}, // MCU_DATA_4
	{1, 0xCD, 0x2704 	}, // MCU_DATA_5
	{1, 0xCE, 0x6D0A 	}, // MCU_DATA_6
	{1, 0xCF, 0x26F1 	}, // MCU_DATA_7
	{1, 0xC6, 0x05E0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x4FE6 	}, // MCU_DATA_0
	{1, 0xC9, 0x092A 	}, // MCU_DATA_1
	{1, 0xCA, 0x0143 	}, // MCU_DATA_2
	{1, 0xCB, 0xED06 	}, // MCU_DATA_3
	{1, 0xCC, 0x8FC3 	}, // MCU_DATA_4
	{1, 0xCD, 0x0011 	}, // MCU_DATA_5
	{1, 0xCE, 0x30E3 	}, // MCU_DATA_6
	{1, 0xCF, 0x0618 	}, // MCU_DATA_7
	{1, 0xC6, 0x05F0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x8FE6 	}, // MCU_DATA_0
	{1, 0xC9, 0x1018 	}, // MCU_DATA_1
	{1, 0xCA, 0xE700 	}, // MCU_DATA_2
	{1, 0xCB, 0xE609 	}, // MCU_DATA_3
	{1, 0xCC, 0xC104 	}, // MCU_DATA_4
	{1, 0xCD, 0x2F10 	}, // MCU_DATA_5
	{1, 0xCE, 0xEC0B 	}, // MCU_DATA_6
	{1, 0xCF, 0x0505 	}, // MCU_DATA_7
	{1, 0xC6, 0x0600 	}, // MCU_ADDRESS
	{1, 0xC8, 0x05ED 	}, // MCU_DATA_0
	{1, 0xC9, 0x0BE6 	}, // MCU_DATA_1
	{1, 0xCA, 0x0A4F 	}, // MCU_DATA_2
	{1, 0xCB, 0xE30B 	}, // MCU_DATA_3
	{1, 0xCC, 0xED0B 	}, // MCU_DATA_4
	{1, 0xCD, 0x200E 	}, // MCU_DATA_5
	{1, 0xCE, 0xEC0D 	}, // MCU_DATA_6
	{1, 0xCF, 0x0505 	}, // MCU_DATA_7
	{1, 0xC6, 0x0610 	}, // MCU_ADDRESS
	{1, 0xC8, 0x05ED 	}, // MCU_DATA_0
	{1, 0xC9, 0x0DE6 	}, // MCU_DATA_1
	{1, 0xCA, 0x0A4F 	}, // MCU_DATA_2
	{1, 0xCB, 0xE30D 	}, // MCU_DATA_3
	{1, 0xCC, 0xED0D 	}, // MCU_DATA_4
	{1, 0xCD, 0x6A09 	}, // MCU_DATA_5
	{1, 0xCE, 0x6D09 	}, // MCU_DATA_6
	{1, 0xCF, 0x2C96 	}, // MCU_DATA_7
	{1, 0xC6, 0x0620 	}, // MCU_ADDRESS
	{1, 0xC8, 0xCC01 	}, // MCU_DATA_0
	{1, 0xC9, 0x60ED 	}, // MCU_DATA_1
	{1, 0xCA, 0x00EC 	}, // MCU_DATA_2
	{1, 0xCB, 0x0DBD 	}, // MCU_DATA_3
	{1, 0xCC, 0xD350 	}, // MCU_DATA_4
	{1, 0xCD, 0xCC01 	}, // MCU_DATA_5
	{1, 0xCE, 0x6130 	}, // MCU_DATA_6
	{1, 0xCF, 0xED00 	}, // MCU_DATA_7
	{1, 0xC6, 0x0630 	}, // MCU_ADDRESS
	{1, 0xC8, 0xEC0B 	}, // MCU_DATA_0
	{1, 0xC9, 0xBDD3 	}, // MCU_DATA_1
	{1, 0xCA, 0x5030 	}, // MCU_DATA_2
	{1, 0xCB, 0xE612 	}, // MCU_DATA_3
	{1, 0xCC, 0x4F17 	}, // MCU_DATA_4
	{1, 0xCD, 0x5FED 	}, // MCU_DATA_5
	{1, 0xCE, 0x0FE6 	}, // MCU_DATA_6
	{1, 0xCF, 0x114F 	}, // MCU_DATA_7
	{1, 0xC6, 0x0640 	}, // MCU_ADDRESS
	{1, 0xC8, 0xE30F 	}, // MCU_DATA_0
	{1, 0xC9, 0xED0F 	}, // MCU_DATA_1
	{1, 0xCA, 0xCC01 	}, // MCU_DATA_2
	{1, 0xCB, 0x62ED 	}, // MCU_DATA_3
	{1, 0xCC, 0x00EC 	}, // MCU_DATA_4
	{1, 0xCD, 0x0FBD 	}, // MCU_DATA_5
	{1, 0xCE, 0xD350 	}, // MCU_DATA_6
	{1, 0xCF, 0x30E6 	}, // MCU_DATA_7
	{1, 0xC6, 0x0650 	}, // MCU_ADDRESS
	{1, 0xC8, 0x144F 	}, // MCU_DATA_0
	{1, 0xC9, 0x175F 	}, // MCU_DATA_1
	{1, 0xCA, 0xED0F 	}, // MCU_DATA_2
	{1, 0xCB, 0xE613 	}, // MCU_DATA_3
	{1, 0xCC, 0x4FE3 	}, // MCU_DATA_4
	{1, 0xCD, 0x0FED 	}, // MCU_DATA_5
	{1, 0xCE, 0x0FCC 	}, // MCU_DATA_6
	{1, 0xCF, 0x0163 	}, // MCU_DATA_7
	{1, 0xC6, 0x0660 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED00 	}, // MCU_DATA_0
	{1, 0xC9, 0xEC0F 	}, // MCU_DATA_1
	{1, 0xCA, 0xBDD3 	}, // MCU_DATA_2
	{1, 0xCB, 0x5030 	}, // MCU_DATA_3
	{1, 0xCC, 0xE616 	}, // MCU_DATA_4
	{1, 0xCD, 0x4F17 	}, // MCU_DATA_5
	{1, 0xCE, 0x5FED 	}, // MCU_DATA_6
	{1, 0xCF, 0x0FE6 	}, // MCU_DATA_7
	{1, 0xC6, 0x0670 	}, // MCU_ADDRESS
	{1, 0xC8, 0x154F 	}, // MCU_DATA_0
	{1, 0xC9, 0xE30F 	}, // MCU_DATA_1
	{1, 0xCA, 0xED0F 	}, // MCU_DATA_2
	{1, 0xCB, 0xCC01 	}, // MCU_DATA_3
	{1, 0xCC, 0x64ED 	}, // MCU_DATA_4
	{1, 0xCD, 0x00EC 	}, // MCU_DATA_5
	{1, 0xCE, 0x0FBD 	}, // MCU_DATA_6
	{1, 0xCF, 0xD350 	}, // MCU_DATA_7
	{1, 0xC6, 0x0680 	}, // MCU_ADDRESS
	{1, 0xC8, 0x30E6 	}, // MCU_DATA_0
	{1, 0xC9, 0x184F 	}, // MCU_DATA_1
	{1, 0xCA, 0x175F 	}, // MCU_DATA_2
	{1, 0xCB, 0xED0F 	}, // MCU_DATA_3
	{1, 0xCC, 0xE617 	}, // MCU_DATA_4
	{1, 0xCD, 0x4FE3 	}, // MCU_DATA_5
	{1, 0xCE, 0x0FED 	}, // MCU_DATA_6
	{1, 0xCF, 0x0FCC 	}, // MCU_DATA_7
	{1, 0xC6, 0x0690 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0165 	}, // MCU_DATA_0
	{1, 0xC9, 0xED00 	}, // MCU_DATA_1
	{1, 0xCA, 0xEC0F 	}, // MCU_DATA_2
	{1, 0xCB, 0xBDD3 	}, // MCU_DATA_3
	{1, 0xCC, 0x5030 	}, // MCU_DATA_4
	{1, 0xCD, 0xE61A 	}, // MCU_DATA_5
	{1, 0xCE, 0x4F17 	}, // MCU_DATA_6
	{1, 0xCF, 0x5FED 	}, // MCU_DATA_7
	{1, 0xC6, 0x06A0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0FE6 	}, // MCU_DATA_0
	{1, 0xC9, 0x194F 	}, // MCU_DATA_1
	{1, 0xCA, 0xE30F 	}, // MCU_DATA_2
	{1, 0xCB, 0xED0F 	}, // MCU_DATA_3
	{1, 0xCC, 0xCC01 	}, // MCU_DATA_4
	{1, 0xCD, 0x66ED 	}, // MCU_DATA_5
	{1, 0xCE, 0x00EC 	}, // MCU_DATA_6
	{1, 0xCF, 0x0FBD 	}, // MCU_DATA_7
	{1, 0xC6, 0x06B0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xD350 	}, // MCU_DATA_0
	{1, 0xC9, 0x13AB 	}, // MCU_DATA_1
	{1, 0xCA, 0x0406 	}, // MCU_DATA_2
	{1, 0xCB, 0xCC7F 	}, // MCU_DATA_3
	{1, 0xCC, 0x8030 	}, // MCU_DATA_4
	{1, 0xCD, 0x207A 	}, // MCU_DATA_5
	{1, 0xCE, 0xDC9C 	}, // MCU_DATA_6
	{1, 0xCF, 0x30ED 	}, // MCU_DATA_7
	{1, 0xC6, 0x06C0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x02D6 	}, // MCU_DATA_0
	{1, 0xC9, 0xA44F 	}, // MCU_DATA_1
	{1, 0xCA, 0xED00 	}, // MCU_DATA_2
	{1, 0xCB, 0xCC00 	}, // MCU_DATA_3
	{1, 0xCC, 0x80BD 	}, // MCU_DATA_4
	{1, 0xCD, 0xD268 	}, // MCU_DATA_5
	{1, 0xCE, 0x30E6 	}, // MCU_DATA_6
	{1, 0xCF, 0x03E7 	}, // MCU_DATA_7
	{1, 0xC6, 0x06D0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x11DC 	}, // MCU_DATA_0
	{1, 0xC9, 0x9EED 	}, // MCU_DATA_1
	{1, 0xCA, 0x02D6 	}, // MCU_DATA_2
	{1, 0xCB, 0xA64F 	}, // MCU_DATA_3
	{1, 0xCC, 0xED00 	}, // MCU_DATA_4
	{1, 0xCD, 0xCC00 	}, // MCU_DATA_5
	{1, 0xCE, 0x80BD 	}, // MCU_DATA_6
	{1, 0xCF, 0xD268 	}, // MCU_DATA_7
	{1, 0xC6, 0x06E0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x30E6 	}, // MCU_DATA_0
	{1, 0xC9, 0x03E7 	}, // MCU_DATA_1
	{1, 0xCA, 0x12E6 	}, // MCU_DATA_2
	{1, 0xCB, 0x11A6 	}, // MCU_DATA_3
	{1, 0xCC, 0x123D 	}, // MCU_DATA_4
	{1, 0xCD, 0xED0F 	}, // MCU_DATA_5
	{1, 0xCE, 0x6F0A 	}, // MCU_DATA_6
	{1, 0xCF, 0x2007 	}, // MCU_DATA_7
	{1, 0xC6, 0x06F0 	}, // MCU_ADDRESS
	{1, 0xC8, 0xEC0F 	}, // MCU_DATA_0
	{1, 0xC9, 0x04ED 	}, // MCU_DATA_1
	{1, 0xCA, 0x0F6C 	}, // MCU_DATA_2
	{1, 0xCB, 0x0AEC 	}, // MCU_DATA_3
	{1, 0xCC, 0x0F1A 	}, // MCU_DATA_4
	{1, 0xCD, 0x8300 	}, // MCU_DATA_5
	{1, 0xCE, 0x2024 	}, // MCU_DATA_6
	{1, 0xCF, 0xF183 	}, // MCU_DATA_7
	{1, 0xC6, 0x0700 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0010 	}, // MCU_DATA_0
	{1, 0xC9, 0xED0F 	}, // MCU_DATA_1
	{1, 0xCA, 0xC3E8 	}, // MCU_DATA_2
	{1, 0xCB, 0xFB8F 	}, // MCU_DATA_3
	{1, 0xCC, 0xE600 	}, // MCU_DATA_4
	{1, 0xCD, 0x30E7 	}, // MCU_DATA_5
	{1, 0xCE, 0x08E6 	}, // MCU_DATA_6
	{1, 0xCF, 0x0A86 	}, // MCU_DATA_7
	{1, 0xC6, 0x0710 	}, // MCU_ADDRESS
	{1, 0xC8, 0x203D 	}, // MCU_DATA_0
	{1, 0xC9, 0xEB08 	}, // MCU_DATA_1
	{1, 0xCA, 0xC040 	}, // MCU_DATA_2
	{1, 0xCB, 0xE708 	}, // MCU_DATA_3
	{1, 0xCC, 0xD6BA 	}, // MCU_DATA_4
	{1, 0xCD, 0x4FED 	}, // MCU_DATA_5
	{1, 0xCE, 0x06E6 	}, // MCU_DATA_6
	{1, 0xCF, 0x08ED 	}, // MCU_DATA_7
	{1, 0xC6, 0x0720 	}, // MCU_ADDRESS
	{1, 0xC8, 0x04E3 	}, // MCU_DATA_0
	{1, 0xC9, 0x06ED 	}, // MCU_DATA_1
	{1, 0xCA, 0x0F17 	}, // MCU_DATA_2
	{1, 0xCB, 0x5FED 	}, // MCU_DATA_3
	{1, 0xCC, 0x0FD6 	}, // MCU_DATA_4
	{1, 0xCD, 0xB94F 	}, // MCU_DATA_5
	{1, 0xCE, 0xED06 	}, // MCU_DATA_6
	{1, 0xCF, 0xE608 	}, // MCU_DATA_7
	{1, 0xC6, 0x0730 	}, // MCU_ADDRESS
	{1, 0xC8, 0xED04 	}, // MCU_DATA_0
	{1, 0xC9, 0xE306 	}, // MCU_DATA_1
	{1, 0xCA, 0xE30F 	}, // MCU_DATA_2
	{1, 0xCB, 0xED0F 	}, // MCU_DATA_3
	{1, 0xCC, 0xCC01 	}, // MCU_DATA_4
	{1, 0xCD, 0x52ED 	}, // MCU_DATA_5
	{1, 0xCE, 0x00EC 	}, // MCU_DATA_6
	{1, 0xCF, 0x0FBD 	}, // MCU_DATA_7
	{1, 0xC6, 0x0740 	}, // MCU_ADDRESS
	{1, 0xC8, 0xD350 	}, // MCU_DATA_0
	{1, 0xC9, 0x30C6 	}, // MCU_DATA_1
	{1, 0xCA, 0x1B3A 	}, // MCU_DATA_2
	{1, 0xCB, 0x3539 	}, // MCU_DATA_3
	{1, 0xCC, 0x3C3C 	}, // MCU_DATA_4
	{1, 0xCD, 0x34C6 	}, // MCU_DATA_5
	{1, 0xCE, 0x18F7 	}, // MCU_DATA_6
	{1, 0xCF, 0x02BD 	}, // MCU_DATA_7
	{1, 0xC6, 0x0750 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC6B4 	}, // MCU_DATA_0
	{1, 0xC9, 0xF702 	}, // MCU_DATA_1
	{1, 0xCA, 0xBEC6 	}, // MCU_DATA_2
	{1, 0xCB, 0x96F7 	}, // MCU_DATA_3
	{1, 0xCC, 0x02BF 	}, // MCU_DATA_4
	{1, 0xCD, 0xC680 	}, // MCU_DATA_5
	{1, 0xCE, 0xF702 	}, // MCU_DATA_6
	{1, 0xCF, 0xC0F7 	}, // MCU_DATA_7
	{1, 0xC6, 0x0760 	}, // MCU_ADDRESS
	{1, 0xC8, 0x02C1 	}, // MCU_DATA_0
	{1, 0xC9, 0xF702 	}, // MCU_DATA_1
	{1, 0xCA, 0xC2F7 	}, // MCU_DATA_2
	{1, 0xCB, 0x02C3 	}, // MCU_DATA_3
	{1, 0xCC, 0xF702 	}, // MCU_DATA_4
	{1, 0xCD, 0xC4F7 	}, // MCU_DATA_5
	{1, 0xCE, 0x02C5 	}, // MCU_DATA_6
	{1, 0xCF, 0xF702 	}, // MCU_DATA_7
	{1, 0xC6, 0x0770 	}, // MCU_ADDRESS
	{1, 0xC8, 0xC6CC 	}, // MCU_DATA_0
	{1, 0xC9, 0x02AB 	}, // MCU_DATA_1
	{1, 0xCA, 0x30ED 	}, // MCU_DATA_2
	{1, 0xCB, 0x02FE 	}, // MCU_DATA_3
	{1, 0xCC, 0x1050 	}, // MCU_DATA_4
	{1, 0xCD, 0xEC06 	}, // MCU_DATA_5
	{1, 0xCE, 0xFD02 	}, // MCU_DATA_6
	{1, 0xCF, 0xA7FE 	}, // MCU_DATA_7
	{1, 0xC6, 0x0780 	}, // MCU_ADDRESS
	{1, 0xC8, 0x02A7 	}, // MCU_DATA_0
	{1, 0xC9, 0xEC00 	}, // MCU_DATA_1
	{1, 0xCA, 0xFD02 	}, // MCU_DATA_2
	{1, 0xCB, 0xA930 	}, // MCU_DATA_3
	{1, 0xCC, 0x6F04 	}, // MCU_DATA_4
	{1, 0xCD, 0xE604 	}, // MCU_DATA_5
	{1, 0xCE, 0x4F05 	}, // MCU_DATA_6
	{1, 0xCF, 0xF302 	}, // MCU_DATA_7
	{1, 0xC6, 0x0790 	}, // MCU_ADDRESS
	{1, 0xC8, 0xA98F 	}, // MCU_DATA_0
	{1, 0xC9, 0xEC00 	}, // MCU_DATA_1
	{1, 0xCA, 0x30ED 	}, // MCU_DATA_2
	{1, 0xCB, 0x00E6 	}, // MCU_DATA_3
	{1, 0xCC, 0x044F 	}, // MCU_DATA_4
	{1, 0xCD, 0x05E3 	}, // MCU_DATA_5
	{1, 0xCE, 0x0218 	}, // MCU_DATA_6
	{1, 0xCF, 0x8FEC 	}, // MCU_DATA_7
	{1, 0xC6, 0x07A0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x0018 	}, // MCU_DATA_0
	{1, 0xC9, 0xED00 	}, // MCU_DATA_1
	{1, 0xCA, 0x6C04 	}, // MCU_DATA_2
	{1, 0xCB, 0xE604 	}, // MCU_DATA_3
	{1, 0xCC, 0xC109 	}, // MCU_DATA_4
	{1, 0xCD, 0x25DE 	}, // MCU_DATA_5
	{1, 0xCE, 0xEE02 	}, // MCU_DATA_6
	{1, 0xCF, 0xCC05 	}, // MCU_DATA_7
	{1, 0xC6, 0x07B0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x6BED 	}, // MCU_DATA_0
	{1, 0xC9, 0x0830 	}, // MCU_DATA_1
	{1, 0xCA, 0xEE02 	}, // MCU_DATA_2
	{1, 0xCB, 0xCC04 	}, // MCU_DATA_3
	{1, 0xCC, 0x00ED 	}, // MCU_DATA_4
	{1, 0xCD, 0x0ECC 	}, // MCU_DATA_5
	{1, 0xCE, 0x02AB 	}, // MCU_DATA_6
	{1, 0xCF, 0xDD58 	}, // MCU_DATA_7
	{1, 0xC6, 0x07C0 	}, // MCU_ADDRESS
	{1, 0xC8, 0x3838 	}, // MCU_DATA_0
	{1, 0xC9, 0x3139 	}, // MCU_DATA_1
	{1, 0xC6, 0x2003	}, //MON_ARG1
	{1, 0xC8, 0x0748	}, //MON_ARG1
	{1, 0xC6, 0xA002	}, //MON_CMD
	{1, 0xC8, 0x0001	}, //MON_CMD
	{MT9D111_DELAY, 0x00, 0x01F4 }, //Delay = 500ms
	{1, 0xC6, 0xA361	}, //AWB_TG_MIN0
	{1, 0xC8, 0x00E2	}, //AWB_TG_MIN0
	{1, 0x1F, 0x0018 	}, // RESERVED_SOC1_1F
	{1, 0x51, 0x7F40 	}, // RESERVED_SOC1_51
	{MT9D111_DELAY, 0x00, 0x3E8  }, //Delay = 1000ms

	{0, 0x33, 0x0343 	}, // RESERVED_CORE_33
	{0, 0x38, 0x0868 	}, // RESERVED_CORE_38
	{1, 0xC6, 0xA10F	}, //SEQ_RESET_LEVEL_TH
	{1, 0xC8, 0x0042	}, //SEQ_RESET_LEVEL_TH
	{1, 0x1F, 0x0020 	}, // RESERVED_SOC1_1F
	{1, 0xC6, 0xAB04	}, //HG_MAX_DLEVEL
	{1, 0xC8, 0x0008	}, //HG_MAX_DLEVEL
	{1, 0xC6, 0xA120	}, //SEQ_CAP_MODE
	{1, 0xC8, 0x0002	}, //SEQ_CAP_MODE
	{1, 0xC6, 0xA103	}, //SEQ_CMD
	{1, 0xC8, 0x0001	}, //SEQ_CMD
	{MT9D111_DELAY, 0x00, 0x03E8 }, // Delay =1000ms
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
	{0, 0x20, 0x0303 	}, // READ_MODE_B (Image flip settings)
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
	{1, 0xC8, 0x0140	}, //MODE_OUTPUT_WIDTH_B			// CYM 320x240
	{1, 0xC6, 0x2709	}, //MODE_OUTPUT_HEIGHT_B
	{1, 0xC8, 0x00F0	}, //MODE_OUTPUT_HEIGHT_B			// CYM 320x240
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

	{1, 0xC6, 0x276B	}, //MODE_FIFO_CONF0_A
	{1, 0xC8, 0x0027	}, //MODE_FIFO_CONF0_A
	{1, 0xC6, 0x276D	}, //MODE_FIFO_CONF1_A
	{1, 0xC8, 0xE1E1	}, //MODE_FIFO_CONF1_A
	{1, 0xC6, 0xA76F	}, //MODE_FIFO_CONF2_A
	{1, 0xC8, 0x00E1	}, //MODE_FIFO_CONF2_A

	{1, 0xC6, 0x2772	}, //MODE_FIFO_CONF0_B
	{1, 0xC8, 0x0027	}, //MODE_FIFO_CONF0_B
	{1, 0xC6, 0x2774	}, //MODE_FIFO_CONF1_B
	{1, 0xC8, 0xE1E1	}, //MODE_FIFO_CONF1_B
	{1, 0xC6, 0xA776	}, //MODE_FIFO_CONF2_B
	{1, 0xC8, 0x00E1	}, //MODE_FIFO_CONF2_B

	{1, 0xC6, 0xA20E	}, //AE_MAX_INDEX
	{1, 0xC8, 0x0004	}, //AE_MAX_INDEX

	{1, 0xC6, 0xA20D	}, //AE_MAX_INDEX
	{1, 0xC8, 0x0004	}, //AE_MAX_INDEX

	{1, 0xC6, 0x9078	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9079	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9070	},
	{1, 0xC8, 0x0000	},
	{1, 0xC6, 0x9071	},
	{1, 0xC8, 0x0000	},

	{1, 0xC6, 0xA743	}, // MODE_GAM_CONT_A
	
	{1, 0xC8, 0x0042	}, // MODE_GAM_CONT_A
	{1, 0xC6, 0xA744	}, // MODE_GAM_CONT_B
	
	{1, 0xC8, 0x0042	}, // MODE_GAM_CONT_B
	{MT9D111_DELAY, 0x00, 0x01F4 }, // Delay =500m
	{MT9D111_TERM, 0, 0 }
};

const static struct mt9d111_reg mt9d111_pll[] ={
#if 1
	{0, 0x66, 0x4805	},	// 24MHz -> 36MHz(C100)
	{0, 0x67, 0x0503	},
#else
	{0, 0x66, 0x0c01},//24M->80M 
	{0, 0x67, 0x0500}, //
#endif
	{0, 0x65, 0xA000	},
	{MT9D111_DELAY, 0x00, 0x01F4	},
	{0, 0x65, 0x2000	},
	{MT9D111_TERM, 0, 0 }
};

// For VGA ( 640 x 480) on 3BA module
struct mt9d111_reg mt9d111_vga[] = 
{
	{1, 0xC6, 0x2703	}, //MODE_OUTPUT_WIDTH_A
	{1, 0xC8, 0x0280	}, //MODE_OUTPUT_WIDTH_A
	{1, 0xC6, 0x2705	}, //MODE_OUTPUT_HEIGHT_A
	{1, 0xC8, 0x01E0	}, //MODE_OUTPUT_HEIGHT_A
	{1, 0xC6, 0xA103	}, //SEQ_CMD
	{1, 0xC8, 0x0005	}, //SEQ_CMD
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

#endif

