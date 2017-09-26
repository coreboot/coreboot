/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_GFX_H_
#define _SOC_GFX_H_

/*
 * PCI config registers.
 */

#define GGC		0x50
# define GGC_VAMEN		(1 << 14)	/* Enable acceleration mode */
# define GGC_GTT_SIZE_MASK	(3 << 8)	/* GTT graphics memory size */
# define GGC_GTT_SIZE_0MB	(0 << 8)
# define GGC_GTT_SIZE_2MB	(1 << 8)
# define GGC_GTT_SIZE_4MB	(2 << 8)
# define GGC_GTT_SIZE_8MB	(3 << 8)
# define GGC_GSM_SIZE_MASK	(0x1f << 3)	/* Main memory use */
# define GGC_GSM_SIZE_0MB	(0 << 3)
# define GGC_GSM_SIZE_32MB	(1 << 3)
# define GGC_GSM_SIZE_64MB	(2 << 3)
# define GCC_GSM_SIZE_96MB	(3 << 3)
# define GGC_GSM_SIZE_128MB	(4 << 3)
# define GGC_GSM_SIZE_160MB	(5 << 3)
# define GGC_GSM_SIZE_192MB	(6 << 3)
# define GGC_GSM_SIZE_224MB	(7 << 3)
# define GGC_GSM_SIZE_256MB	(8 << 3)
# define GGC_GSM_SIZE_288MB	(9 << 3)
# define GGC_GSM_SIZE_320MB	(0x0a << 3)
# define GGC_GSM_SIZE_352MB	(0x0b << 3)
# define GGC_GSM_SIZE_384MB	(0x0c << 3)
# define GGC_GSM_SIZE_416MB	(0x0d << 3)
# define GGC_GSM_SIZE_448MB	(0x0e << 3)
# define GGC_GSM_SIZE_480MB	(0x0f << 3)
# define GGC_GSM_SIZE_512MB	(0x10 << 3)
# define GGC_VGA_DISABLE	(1 << 1)	/* VGA Disable */
# define GGC_GGCLCK             (1 << 0)	/* Prevent register writes */

#define GSM_BASE	0x5c
# define GSM_BDSM	0xfff00000	/* Base of stolen memory */
# define GSM_BDSM_LOCK	(1 << 0)	/* Prevent register writes */

#define GTT_BASE	0x70
# define GTT_BGSM	0xfff00000	/* Base of stolen memory */
# define GTT_BGSM_LOCK	(1 << 0)	/* Prevent register writes */

#define MSAC		0x62
#define  APERTURE_SIZE_MASK	(3 << 1)
#define  APERTURE_SIZE_128MB	(0 << 1)
#define  APERTURE_SIZE_256MB	(1 << 1)
#define  APERTURE_SIZE_512MB	(3 << 1)

/* Panel control registers */
#define HOTPLUG_CTRL		0x61110
#define PP_CONTROL		0x61204
# define PP_CONTROL_WRITE_PROTECT_KEY	0xffff0000 /* Enable display port VDD */
# define PP_CONTROL_UNLOCK		0xabcd0000
# define PP_CONTROL_EDP_FORCE_VDD	(1 << 3)   /* Enable display port VDD */
# define PP_CONTROL_BACKLIGHT_ENABLE	(1 << 2)
# define PP_CONTROL_POWER_DOWN_ON_RESET (1 << 1)
# define PP_CONTROL_POWER_STATE_TARGET	(1 << 0)   /* Power up/down (1/0) */

#define PP_ON_DELAYS		0x61208
#define PP_OFF_DELAYS		0x6120c
#define PP_DIVISOR		0x61210
#define BACKLIGHT_CTL2		0x61250
# define BACKLIGHT_PWM_ENABLE		(1 << 31)
# define BACKLIGHT_POLARITY		(1 << 28) /* Active low/high (1/0) */
# define BACKLIGHT_PHASE_IN_INT_STATUS	(1 << 26)
# define BACKLIGHT_PHASE_IN_ENABLE	(1 << 25)
# define BACKLIGHT_PHASE_IN_INT_ENABLE	(1 << 24)
# define BACKLIGHT_PHASE_IN_TIME_BASE	0x00ff0000
# define BACKLIGHT_PHASE_IN_COUNT	0x0000ff00
# define BACKLIGHT_PHASE_IN_INCREMENT	0x000000ff

#define BACKLIGHT_CTL		0x61254

#endif /* _SOC_GFX_H_ */
