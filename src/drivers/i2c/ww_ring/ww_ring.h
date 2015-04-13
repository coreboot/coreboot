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

/*
 * Different types of display patterns to be shown by the LED ring while
 * contrlled by coreboot.
 */
enum display_pattern {
	WWR_ALL_OFF,		/* Turn the LEDs off. */
	WWR_RECOVERY_PUSHED,	/* Recovery button push detected on start up. */
	WWR_WIPEOUT_REQUEST,	/* Held long enough for wipout request. */
	WWR_RECOVERY_REQUEST,	/* Held long enough for recovery request. */
	WWR_NORMAL_BOOT		/* No buttons pressed, normal boot sequence. */
};
/*
 * ww_ring_display_pattern
 *
 * Display pattern on the ring LEDs.
 */
int ww_ring_display_pattern(unsigned i2c_bus, enum display_pattern pattern);

#endif
