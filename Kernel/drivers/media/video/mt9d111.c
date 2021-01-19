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

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-i2c-drv.h>
#include <media/mt9d111_platform.h>

#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif

#include "mt9d111.h"

#define MT9D111_DRIVER_NAME	"MT9D111"

/* Default resolution & pixelformat. plz ref mt9d111_platform.h */
#define DEFAULT_RES		VGA	/* Index of resoultion */
#define DEFAUT_FPS_INDEX	MT9D111_15FPS
#define DEFAULT_FMT		V4L2_PIX_FMT_UYVY	/* YUV422 */

/*
 * Specification
 * Parallel : ITU-R. 656/601 YUV422, RGB565, RGB888 (Up to VGA), RAW10 
 * Serial : MIPI CSI2 (single lane) YUV422, RGB565, RGB888 (Up to VGA), RAW10
 * Resolution : 1280 (H) x 1024 (V)
 * Image control : Brightness, Contrast, Saturation, Sharpness, Glamour
 * Effect : Mono, Negative, Sepia, Aqua, Sketch
 * FPS : 15fps @full resolution, 30fps @VGA, 24fps @720p
 * Max. pixel clock frequency : 48MHz(upto)
 * Internal PLL (6MHz to 27MHz input frequency)
 */

/* Camera functional setting values configured by user concept */
struct mt9d111_userset {
	signed int exposure_bias;	/* V4L2_CID_EXPOSURE */
	unsigned int ae_lock;
	unsigned int awb_lock;
	unsigned int auto_wb;	/* V4L2_CID_AUTO_WHITE_BALANCE */
	unsigned int manual_wb;	/* V4L2_CID_WHITE_BALANCE_PRESET */
	unsigned int wb_temp;	/* V4L2_CID_WHITE_BALANCE_TEMPERATURE */
	unsigned int effect;	/* Color FX (AKA Color tone) */
	unsigned int contrast;	/* V4L2_CID_CONTRAST */
	unsigned int saturation;	/* V4L2_CID_SATURATION */
	unsigned int sharpness;		/* V4L2_CID_SHARPNESS */
	unsigned int glamour;
};

struct mt9d111_state {
	struct mt9d111_platform_data *pdata;
	struct v4l2_subdev sd;
	struct v4l2_pix_format pix;
	struct v4l2_fract timeperframe;
	struct mt9d111_userset userset;
	int freq;	/* MCLK in KHz */
	int is_mipi;
	int isize;
	int ver;
	int fps;
};

static struct mt9d111_state *g_state = NULL;

static inline struct mt9d111_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct mt9d111_state, sd);
}

/*
 * MT9D111 register structure : 2bytes address, 2bytes value
 * retry on write failure up-to 5 times
 */
static inline int mt9d111_write(struct v4l2_subdev *sd, u8 addr, u8 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msg[1];
	unsigned char reg[2];
	int err = 0;
	int retry = 0;


	if (!client->adapter)
		return -ENODEV;

again:
	msg->addr = client->addr;
	msg->flags = 0;
	msg->len = 2;
	msg->buf = reg;

	reg[0] = addr & 0xff;
	reg[1] = val & 0xff;

	err = i2c_transfer(client->adapter, msg, 1);
	if (err >= 0)
		return err;	/* Returns here on success */

	/* abnormal case: retry 5 times */
	if (retry < 5) {
		dev_err(&client->dev, "%s: addr(0x%02x)," "value(0x%02x:0x%02x)\n", __func__, reg[0], reg[1], reg[2]);
		retry++;
		goto again;
	}

	return err;
}
#if 0
static int mt9d111_i2c_write(struct v4l2_subdev *sd, unsigned char i2c_data[],
				unsigned char length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buf[length], i;
	struct i2c_msg msg = {client->addr, 0, length, buf};

	for (i = 0; i < length; i++) {
		buf[i] = i2c_data[i];
	}
	return i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
}

static int mt9d111_write_regs(struct v4l2_subdev *sd, unsigned char regs[], 
				int size)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int i, err;
	for (i = 0; i < size; i++) {
		err = mt9d111_i2c_write(sd, &regs[i], sizeof(regs[i]));
		if (err < 0)
			v4l_info(client, "%s: register set failed\n", \
			__func__);
	}

	return 0;	/* FIXME */
}
#endif
#if 1	// KSSON20100311
static int sensor_write(struct i2c_client *client, unsigned char subaddr, unsigned short val)
{
	unsigned char buf[3];
	struct i2c_msg msg = { client->addr, 0, 3, buf };
	int ret;

	buf[0] = subaddr;
	buf[1] = val >> 8;
	buf[2] = val;

	ret = i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;

	return ret;
}
static int mt9d111_set_regs(struct i2c_client *sam_client, const struct mt9d111_reg *regP)
{
	u32 curReg = 0;
	u16 regVal;
	int status = 0;
	static int curPage = -1;

	/* The list is a register number followed by the value */
	while( regP[curReg].page != MT9D111_TERM ){
		if (curPage != regP[curReg].page) {
			if (regP[curReg].page == 0) {
				regVal = 0;
				status = (sensor_write(sam_client, 0xF0, regVal) == 0) ? 0 : -EIO;
			}
			if (regP[curReg].page == 1) {
				regVal = 1;
				status = (sensor_write(sam_client, 0xF0, regVal) == 0) ? 0 : -EIO;
			}
			if (regP[curReg].page == 2) {
				regVal = 2;
				status = (sensor_write(sam_client, 0xF0, regVal) == 0) ? 0 : -EIO;
			}
			curPage = regP[curReg].page;
		}
		if (regP[curReg].page == MT9D111_DELAY) {
			mdelay(regP[curReg].value);
			curReg++;
			continue;
		}

		regVal = regP[curReg].value;
		status = sensor_write(sam_client, regP[curReg].subaddr, regP[curReg].value);

		curReg++;
	}
	return status;
}
#endif	// KSSON20100311

static const char *mt9d111_querymenu_wb_preset[] = {
	"WB Tungsten", "WB Fluorescent", "WB sunny", "WB cloudy", NULL
};

static const char *mt9d111_querymenu_effect_mode[] = {
	"Effect Sepia", "Effect Aqua", "Effect Monochrome",
	"Effect Negative", "Effect Sketch", NULL
};

static const char *mt9d111_querymenu_ev_bias_mode[] = {
	"-3EV",	"-2,1/2EV", "-2EV", "-1,1/2EV",
	"-1EV", "-1/2EV", "0", "1/2EV",
	"1EV", "1,1/2EV", "2EV", "2,1/2EV",
	"3EV", NULL
};

static struct v4l2_queryctrl mt9d111_controls[] = {
	{
		/*
		 * For now, we just support in preset type
		 * to be close to generic WB system,
		 * we define color temp range for each preset
		 */
		.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "White balance in kelvin",
		.minimum = 0,
		.maximum = 10000,
		.step = 1,
		.default_value = 0,	/* FIXME */
	},
	{
		.id = V4L2_CID_WHITE_BALANCE_PRESET,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "White balance preset",
		.minimum = 0,
		.maximum = ARRAY_SIZE(mt9d111_querymenu_wb_preset) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_AUTO_WHITE_BALANCE,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Auto white balance",
		.minimum = 0,
		.maximum = 1,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_EXPOSURE,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Exposure bias",
		.minimum = 0,
		.maximum = ARRAY_SIZE(mt9d111_querymenu_ev_bias_mode) - 2,
		.step = 1,
		.default_value = (ARRAY_SIZE(mt9d111_querymenu_ev_bias_mode) - 2) / 2,	/* 0 EV */
	},
	{
		.id = V4L2_CID_COLORFX,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Image Effect",
		.minimum = 0,
		.maximum = ARRAY_SIZE(mt9d111_querymenu_effect_mode) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_CONTRAST,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Contrast",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_SATURATION,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Saturation",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_SHARPNESS,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Sharpness",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
};

const char **mt9d111_ctrl_get_menu(u32 id)
{
	switch (id) {
	case V4L2_CID_WHITE_BALANCE_PRESET:
		return mt9d111_querymenu_wb_preset;

	case V4L2_CID_COLORFX:
		return mt9d111_querymenu_effect_mode;

	case V4L2_CID_EXPOSURE:
		return mt9d111_querymenu_ev_bias_mode;

	default:
		return v4l2_ctrl_get_menu(id);
	}
}

static inline struct v4l2_queryctrl const *mt9d111_find_qctrl(int id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mt9d111_controls); i++)
		if (mt9d111_controls[i].id == id)
			return &mt9d111_controls[i];

	return NULL;
}

static int mt9d111_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mt9d111_controls); i++) {
		if (mt9d111_controls[i].id == qc->id) {
			memcpy(qc, &mt9d111_controls[i], \
				sizeof(struct v4l2_queryctrl));
			return 0;
		}
	}

	return -EINVAL;
}

static int mt9d111_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
	struct v4l2_queryctrl qctrl;

	qctrl.id = qm->id;
	mt9d111_queryctrl(sd, &qctrl);

	return v4l2_ctrl_query_menu(qm, &qctrl, mt9d111_ctrl_get_menu(qm->id));
}

/*
 * Clock configuration
 * Configure expected MCLK from host and return EINVAL if not supported clock
 * frequency is expected
 * 	freq : in Hz
 * 	flag : not supported for now
 */
static int mt9d111_s_crystal_freq(struct v4l2_subdev *sd, u32 freq, u32 flags)
{
	int err = -EINVAL;

	return err;
}

static int mt9d111_g_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int mt9d111_s_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}
static int mt9d111_enum_framesizes(struct v4l2_subdev *sd, \
					struct v4l2_frmsizeenum *fsize)
{
	int err = 0;

	return err;
}

static int mt9d111_enum_frameintervals(struct v4l2_subdev *sd, 
					struct v4l2_frmivalenum *fival)
{
	int err = 0;

	return err;
}

static int mt9d111_enum_fmt(struct v4l2_subdev *sd, struct v4l2_fmtdesc *fmtdesc)
{
	int err = 0;

	return err;
}

static int mt9d111_try_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
{
	int err = 0;

	return err;
}

static int mt9d111_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;
				
	return err;
}

static int change_sensor_size(struct i2c_client *client, int res)
{
    struct mt9d111_reg *regP = NULL;
	
	printk("\t%s res:%d\n", __func__, res);
	switch (res) {
    case VGA:
        regP = &mt9d111_vga[0];	// mt9d111_vga;
        break;

    case SVGA:
        //regP = &mt9d111_svga[0];
        regP = &mt9d111_uxga[0];
        break;
	case UXGA:
        regP = &mt9d111_uxga[0];
        break;
    default:
        printk("%s[%d] : invalid value\n", __func__, __LINE__);
		return -1;
    }

	/* SDKIM */	
    mt9d111_set_regs(client, regP);
	//mt9d111_set_regs(client, mt9d111_pll);

    return 0;
}

static int mt9d111_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	dev_dbg(&client->dev, "%s: numerator %d, denominator: %d\n", \
		__func__, param->parm.capture.timeperframe.numerator, \
		param->parm.capture.timeperframe.denominator);

	//if (param->parm.capture.capturemode == V4L2_MODE_HIGHQUALITY) {
	if (param->parm.capture.timeperframe.denominator & 0x02) {/* SDKIM High res*/
		err = change_sensor_size(client, SVGA);
		//err = change_sensor_size(client, UXGA);
	} else {
		err = change_sensor_size(client, VGA);
	}
	
	if (err < 0)
		v4l_info(client, "%s: mt9d111_s_parm set failed\n", __func__);

	return err;
}

static int mt9d111_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct mt9d111_state *state = to_state(sd);
	struct mt9d111_userset userset = state->userset;
	int err = -EINVAL;

	switch (ctrl->id) {
	case V4L2_CID_EXPOSURE:
		ctrl->value = userset.exposure_bias;
		err = 0;
		break;

	case V4L2_CID_AUTO_WHITE_BALANCE:
		ctrl->value = userset.auto_wb;
		err = 0;
		break;

	case V4L2_CID_WHITE_BALANCE_PRESET:
		ctrl->value = userset.manual_wb;
		err = 0;
		break;

	case V4L2_CID_COLORFX:
		ctrl->value = userset.effect;
		err = 0;
		break;

	case V4L2_CID_CONTRAST:
		ctrl->value = userset.contrast;
		err = 0;
		break;

	case V4L2_CID_SATURATION:
		ctrl->value = userset.saturation;
		err = 0;
		break;

	case V4L2_CID_SHARPNESS:
		ctrl->value = userset.saturation;
		err = 0;
		break;

	default:
		dev_err(&client->dev, "%s: no such ctrl\n", __func__);
		break;
	}
	
	return err;
}

static int mt9d111_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
#ifdef MT9D111_COMPLETE
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EINVAL;

	switch (ctrl->id) {
	case V4L2_CID_EXPOSURE:
		dev_dbg(&client->dev, "%s: V4L2_CID_EXPOSURE\n", __func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_ev_bias[ctrl->value], \
			sizeof(mt9d111_regs_ev_bias[ctrl->value]));
		break;

	case V4L2_CID_AUTO_WHITE_BALANCE:
		dev_dbg(&client->dev, "%s: V4L2_CID_AUTO_WHITE_BALANCE\n", \
			__func__);
		
#if defined CONFIG_ARCH_S3C64XX
		if ((ctrl->value < 0) || (ctrl->value > 6)) {
			dev_dbg(&client->dev, "WB type is invalid:[%d]\n", \
				ctrl->value);
			break;
		}

		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_wb_preset[ctrl->value], \
			sizeof(mt9d111_regs_wb_preset[ctrl->value]));
#else
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_awb_enable[ctrl->value], \
			sizeof(mt9d111_regs_awb_enable[ctrl->value]));
#endif
		break;

	case V4L2_CID_WHITE_BALANCE_PRESET:
		dev_dbg(&client->dev, "%s: V4L2_CID_WHITE_BALANCE_PRESET\n", \
			__func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_wb_preset[ctrl->value], \
			sizeof(mt9d111_regs_wb_preset[ctrl->value]));
		break;

	case V4L2_CID_COLORFX:
		dev_dbg(&client->dev, "%s: V4L2_CID_COLORFX\n", __func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_color_effect[ctrl->value], \
			sizeof(mt9d111_regs_color_effect[ctrl->value]));
		break;

	case V4L2_CID_CONTRAST:
		dev_dbg(&client->dev, "%s: V4L2_CID_CONTRAST\n", __func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_contrast_bias[ctrl->value], \
			sizeof(mt9d111_regs_contrast_bias[ctrl->value]));
		break;

	case V4L2_CID_SATURATION:
		dev_dbg(&client->dev, "%s: V4L2_CID_SATURATION\n", __func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_saturation_bias[ctrl->value], \
			sizeof(mt9d111_regs_saturation_bias[ctrl->value]));
		break;

	case V4L2_CID_SHARPNESS:
		dev_dbg(&client->dev, "%s: V4L2_CID_SHARPNESS\n", __func__);
		err = mt9d111_write_regs(sd, \
			(unsigned char *) mt9d111_regs_sharpness_bias[ctrl->value], \
			sizeof(mt9d111_regs_sharpness_bias[ctrl->value]));
		break;

	default:
		dev_err(&client->dev, "%s: no such control\n", __func__);
		break;
	}

	if (err < 0)
		goto out;
	else
		return 0;

out:
	dev_dbg(&client->dev, "%s: vidioc_s_ctrl failed\n", __func__);
	return err;
#else
	return 0;
#endif
}

static int mt9d111_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;
	printk("\t%s\n", __func__);
	ret = mt9d111_set_regs(client, mt9d111Init);
	ret = mt9d111_set_regs(client, mt9d111_pll);
	return ret;
}

/*
 * s_config subdev ops
 * With camera device, we need to re-initialize every single opening time therefor,
 * it is not necessary to be initialized on probe time. except for version checking
 * NOTE: version checking is optional
 */
static int mt9d111_s_config(struct v4l2_subdev *sd, int irq, void *platform_data)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct mt9d111_state *state = to_state(sd);
	struct mt9d111_platform_data *pdata;

	dev_info(&client->dev, "fetching platform data\n");

	pdata = client->dev.platform_data;

	if (!pdata) {
		dev_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	/*
	 * Assign default format and resolution
	 * Use configured default information in platform data
	 * or without them, use default information in driver
	 */
	if (!(pdata->default_width && pdata->default_height)) {
		/* TODO: assign driver default resolution */
	} else {
		state->pix.width = pdata->default_width;
		state->pix.height = pdata->default_height;
	}

	if (!pdata->pixelformat)
		state->pix.pixelformat = DEFAULT_FMT;
	else
		state->pix.pixelformat = pdata->pixelformat;

	if (!pdata->freq)
		state->freq = 24000000;	/* 24MHz default */
	else
		state->freq = pdata->freq;

	if (!pdata->is_mipi) {
		state->is_mipi = 0;
		dev_info(&client->dev, "parallel mode\n");
	} else
		state->is_mipi = pdata->is_mipi;

	return 0;
}

static const struct v4l2_subdev_core_ops mt9d111_core_ops = {
	.init = mt9d111_init,	/* initializing API */
	.s_config = mt9d111_s_config,	/* Fetch platform data */
	.queryctrl = mt9d111_queryctrl,
	.querymenu = mt9d111_querymenu,
	.g_ctrl = mt9d111_g_ctrl,
	.s_ctrl = mt9d111_s_ctrl,
};

static const struct v4l2_subdev_video_ops mt9d111_video_ops = {
	.s_crystal_freq = mt9d111_s_crystal_freq,
	.g_fmt = mt9d111_g_fmt,
	.s_fmt = mt9d111_s_fmt,
	.enum_framesizes = mt9d111_enum_framesizes,
	.enum_frameintervals = mt9d111_enum_frameintervals,
	.enum_fmt = mt9d111_enum_fmt,
	.try_fmt = mt9d111_try_fmt,
	.g_parm = mt9d111_g_parm,
	.s_parm = mt9d111_s_parm,
};

static const struct v4l2_subdev_ops mt9d111_ops = {
	.core = &mt9d111_core_ops,
	.video = &mt9d111_video_ops,
};

/*
 * mt9d111_probe
 * Fetching platform data is being done with s_config subdev call.
 * In probe routine, we just register subdev device
 */
static int mt9d111_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct mt9d111_state *state;
	struct v4l2_subdev *sd;

	state = kzalloc(sizeof(struct mt9d111_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	sd = &state->sd;
	strcpy(sd->name, MT9D111_DRIVER_NAME);

	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &mt9d111_ops);
	g_state = state;/*SDKIM */
	mt9d111_init(sd, 0);/*SDKIM */
	dev_info(&client->dev, "mt9d111 has been probed\n");
	return 0;
}


static int mt9d111_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));
	return 0;
}

static const struct i2c_device_id mt9d111_id[] = {
	{ MT9D111_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, mt9d111_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data = {
	.name = MT9D111_DRIVER_NAME,
	.probe = mt9d111_probe,
	.remove = mt9d111_remove,
	.id_table = mt9d111_id,
};

MODULE_DESCRIPTION("Microvision MT9D111 UXGA camera driver");
MODULE_AUTHOR("Jinsung Yang <jsgood.yang@samsung.com>");
MODULE_LICENSE("GPL");

