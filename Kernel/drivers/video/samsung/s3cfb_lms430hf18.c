/* linux/drivers/video/samsung/s3cfb_aa121xn01.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * AA121XN01  Landscape LCD module driver for the SMDK
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "s3cfb.h"

static struct s3cfb_lcd lms430hf18 = {
	.width = 480,
	.height = 272,
	.bpp = 24,
	.freq = 60,

	.timing = {
		.h_fp = 8,
		.h_bp = 45,
		.h_sw = 1,
		.v_fp = 4,
		.v_fpe = 1,
		.v_bp = 12,
		.v_bpe = 1,
		.v_sw = 1,
	},

	.polarity = {
		.rise_vclk = 0,
		.inv_hsync = 1,
		.inv_vsync = 1,
		.inv_vden = 0,
	},
};

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info_lms430hf18(struct s3cfb_global *ctrl)
{
	lms430hf18.init_ldi = NULL;
	ctrl->lcd = &lms430hf18;
}
