/*
 * Copyright (C) 2015 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SRC_DRIVERS_VIDEO_WW_RING__H__
#define __SRC_DRIVERS_VIDEO_WW_RING__H__

#if IS_ENABLED(CONFIG_CHROMEOS)
#include <vboot_api.h>

/*
 * ww_ring_display_pattern
 *
 * Display pattern on the ring LEDs.
 */
int ww_ring_display_pattern(unsigned i2c_bus, enum VbScreenType_t screen_type);

#else
static inline int ww_ring_display_pattern(unsigned i2c_bus, int screen_type) { return 0;}
#endif
#endif
