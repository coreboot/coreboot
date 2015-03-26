/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _BAYTRAIL_GFX_H_
#define _BAYTRAIL_GFX_H_

/*
 * PCI config registers.
 */

#define GGC		0x50
# define GGC_VGA_DISABLE	(1 << 1)
# define GGC_GTT_SIZE_MASK	(3 << 8)
# define GGC_GTT_SIZE_0MB	(0 << 8)
# define GGC_GTT_SIZE_1MB	(1 << 8)
# define GGC_GTT_SIZE_2MB	(2 << 8)
# define GGC_GSM_SIZE_MASK	(0x1f << 3)
# define GGC_GSM_SIZE_0MB	(0 << 3)
# define GGC_GSM_SIZE_32MB	(1 << 3)
# define GGC_GSM_SIZE_64MB	(2 << 3)
# define GGC_GSM_SIZE_128MB	(4 << 3)

#define GSM_BASE	0x5c
#define GTT_BASE	0x70

#define MSAC		0x62
#define  APERTURE_SIZE_MASK	(3 << 1)
#define  APERTURE_SIZE_128MB	(0 << 1)
#define  APERTURE_SIZE_256MB	(1 << 1)
#define  APERTURE_SIZE_512MB	(3 << 1)

#define VLV_DISPLAY_BASE	0x180000
#define PIPEA_REG(reg)		(VLV_DISPLAY_BASE + (reg))
#define PIPEB_REG(reg)		(VLV_DISPLAY_BASE + 0x100 + (reg))

/* Panel control registers */
#define HOTPLUG_CTRL		0x61110
#define PP_CONTROL		0x61204
#define  PP_CONTROL_UNLOCK		0xabcd0000
#define  PP_CONTROL_EDP_FORCE_VDD	(1 << 3)
#define PP_ON_DELAYS		0x61208
#define PP_OFF_DELAYS		0x6120c
#define PP_DIVISOR		0x61210
#define BACKLIGHT_CTL2		0x61250
#define  BACKLIGHT_ENABLE		(1 << 31)
#define BACKLIGHT_CTL		0x61254

#endif /* _BAYTRAIL_GFX_H_ */
