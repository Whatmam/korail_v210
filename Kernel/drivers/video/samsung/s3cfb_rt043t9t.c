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

static struct s3cfb_lcd rt043t9t = {
	.width = 480,
	.height = 272,
	.bpp = 24,
	.freq = 60,

	.timing = {
		.h_fp = 8,
		.h_bp = 43,
		.h_sw = 4,
		.v_fp = 8,
		.v_fpe = 1,
		.v_bp = 12,
		.v_bpe = 1,
		.v_sw = 41,
	},

	.polarity = {
		.rise_vclk = 0,
		.inv_hsync = 0,
		.inv_vsync = 0,
		.inv_vden = 0,
	},
};

/* name should be fixed as 's3cfb_set_lcd_info' */
void s3cfb_set_lcd_info_rt043t9t(struct s3cfb_global *ctrl)
{
	rt043t9t.init_ldi = NULL;
	ctrl->lcd = &rt043t9t;
}
