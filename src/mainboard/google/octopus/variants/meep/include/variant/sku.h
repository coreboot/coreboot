/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_SKU_H__
#define __MAINBOARD_SKU_H__

enum {
	SKU_1_MEEP = 1, /* Stylus + rear camera*/
	SKU_2_MEEP = 2, /* no Stylus + rear camera */
	SKU_3_MEEP = 3, /* no Stylus + no rear camera */
	SKU_4_MEEP = 4, /* Stylus + no rear camera */
	SKU_33_DORP = 33, /* HDMI */
	SKU_34_DORP = 34, /* HDMI+Kblight */
	SKU_35_DORP = 35, /* HDMI+TS */
	SKU_36_DORP = 36, /* HDMI+TS+KBlight */
	SKU_49_VORTININJA = 49, /* Stylus + rear camera */
	SKU_50_VORTININJA = 50, /* Stylus + no rear camera */
	SKU_51_VORTININJA = 51, /* no Stylus + rear camera */
	SKU_52_VORTININJA = 52, /* no Stylus + no rear camera */
	SKU_65_VORTICON = 65, /* no touchscreen */
	SKU_66_VORTICON = 66, /* with touchscreen */

};

#endif /* __MAINBOARD_SKU_H__ */
