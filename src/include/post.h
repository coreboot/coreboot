/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __POST_H__
#define __POST_H__

#include <stdint.h>
#include <device/device.h>

void cmos_post_init(void);
void cmos_post_code(u8 value);
void cmos_post_extra(u32 value);
void cmos_post_path(const struct device *dev);
int cmos_post_previous_boot(u8 *code, u32 *extra);

static inline void post_log_path(const struct device *dev)
{
	if (CONFIG(CMOS_POST) && dev)
		cmos_post_path(dev);
}

static inline void post_log_clear(void)
{
	if (CONFIG(CMOS_POST))
		cmos_post_extra(0);
}

#endif
