/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __JINLON_SKU_H__
#define __JINLON_SKU_H__

/*
 * SKU definition taken from
 * https://buganizer.corp.google.com/issues/145688887#comment16
 */
enum {
	JINLON_SKU_01 = 1,  /* No LTE, No view-angle-manegement */
	JINLON_SKU_02 = 2,  /* No LTE, view-angle-manegement */
	JINLON_SKU_21 = 21, /* LTE, No view-angle-manegement */
	JINLON_SKU_22 = 22, /* LTE, view-angle-manegement */
};

#endif /* __JINLON_SKU_H__ */
