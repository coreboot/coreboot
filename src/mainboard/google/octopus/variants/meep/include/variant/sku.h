/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#ifndef __MAINBOARD_SKU_H__
#define __MAINBOARD_SKU_H__

enum {
	SKU_4_VORTININJA = 4, /* Stylus + rear camera */
	SKU_5_VORTININJA = 5, /* Stylus + no rear camera */
	SKU_6_VORTININJA = 6, /* no Stylus + rear camera */
	SKU_7_VORTININJA = 7, /* no Stylus + no rear camera */
	SKU_33_DORP = 33, /* HDMI */
	SKU_34_DORP = 34, /* HDMI+Kblight */
	SKU_35_DORP = 35, /* HDMI+TS */
	SKU_36_DORP = 36, /* HDMI+TS+KBlight */
};

#endif /* __MAINBOARD_SKU_H__ */
