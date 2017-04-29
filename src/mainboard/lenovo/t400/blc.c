/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 arthur@aheymans.xyz
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

#include <stdint.h>
#include <commonlib/helpers.h>
#include <northbridge/intel/gm45/gm45.h>

static const struct blc_pwm_t blc_entries[] = {
	/* Vendor BIOS seems to intend to set PWM freq at 220Hz */
	/* T400 */
	{"B141EW04 V7 ", 110},
	{"LP141WX3-TLR1", 110},
	{"LP141WX5-TLP2", 450},
	{"LTN141AT12401", 450},
	{"LTN141W1-L05", 110},
	{"LTN141WD-L05", 110},
	/* T500 */
	{"LP154W02-TL06", 110},
	{"LTN154P3-L02", 110},
};

int get_blc_values(const struct blc_pwm_t **entries)
{
	*entries = blc_entries;
	return ARRAY_SIZE(blc_entries);
}
