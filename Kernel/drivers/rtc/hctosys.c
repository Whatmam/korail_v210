/*
 * RTC subsystem, initialize system time on startup
 *
 * Copyright (C) 2005 Tower Technologies
 * Author: Alessandro Zummo <a.zummo@towertech.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/rtc.h>

/* IMPORTANT: the RTC only stores whole seconds. It is arbitrary
 * whether it stores the most close value or the value with partial
 * seconds truncated. However, it is important that we use it to store
 * the truncated value. This is because otherwise it is necessary,
 * in an rtc sync function, to read both xtime.tv_sec and
 * xtime.tv_nsec. On some processors (i.e. ARM), an atomic read
 * of >32bits is not possible. So storing the most close value would
 * slow down the sync API. So here we have the truncated value and
 * the best guess is to add 0.5s.
 */

int rtc_hctosys_ret = -ENODEV;

static int __init rtc_hctosys(void)
{
	int err = -ENODEV;
	struct rtc_time tm;
	struct timespec tv = {
		.tv_nsec = NSEC_PER_SEC >> 1,
	};
	struct rtc_device *rtc = rtc_class_open(CONFIG_RTC_HCTOSYS_DEVICE);

	if (rtc == NULL) {
		pr_err("%s: unable to open rtc device (%s)\n",
			__FILE__, CONFIG_RTC_HCTOSYS_DEVICE);
		goto err_open;
	}

	err = rtc_read_time(rtc, &tm);
/*
    printk("============== RTC Debug Param (rtc_read_time = %d) =================\n", err);
    printk("tm_year = %d\n", tm.tm_year);   printk("tm_mon  = %d\n", tm.tm_mon);
    printk("tm_mday = %d\n", tm.tm_mday);   printk("tm_hour = %d\n", tm.tm_hour);
    printk("tm_min  = %d\n", tm.tm_min);    printk("tm_sec  = %d\n", tm.tm_sec);
    printk("================================================\n");
*/
	if (err) {
		dev_err(rtc->dev.parent,
			"hctosys: unable to read the hardware clock\n");
		goto err_read;

	}

	err = rtc_valid_tm(&tm);

    if(err || tm.tm_year <= 70 || tm.tm_year > 150) {   // 2050
        // RTC default value setting (2010-01-01, 0:0:0)
        tm.tm_year = 110;   tm.tm_mon = 0;      tm.tm_mday = 1;
        tm.tm_hour = 0;     tm.tm_min = 0;      tm.tm_sec = 0;
 
        err = rtc_valid_tm(&tm);
 
        if(err == 0)    {
/*			
            printk("============== RTC set to default system time =================\n");
            printk("tm_year = %d\n", tm.tm_year);       printk("tm_mon  = %d\n", tm.tm_mon);
            printk("tm_mday = %d\n", tm.tm_mday);       printk("tm_hour = %d\n", tm.tm_hour);
            printk("tm_min  = %d\n", tm.tm_min);        printk("tm_sec  = %d\n", tm.tm_sec);
            printk("===============================================================\n");
*/			
            rtc_set_time(rtc, &tm);
        }
    }
 
	if (err) {
		dev_err(rtc->dev.parent,
			"hctosys: invalid date/time\n");
		goto err_invalid;
	}

	rtc_tm_to_time(&tm, &tv.tv_sec);

	do_settimeofday(&tv);

	dev_info(rtc->dev.parent,
		"setting system clock to "
		"%d-%02d-%02d %02d:%02d:%02d (%u)\n",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec,
		(unsigned int) tv.tv_sec);

err_invalid:
err_read:
	rtc_class_close(rtc);

err_open:
	rtc_hctosys_ret = err;

	return err;
}

late_initcall(rtc_hctosys);