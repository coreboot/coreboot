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
	/* corrected to 320MHz CDClk, vendor set 753; works fine at both: */
	{"LTD121EQ3B", 447},
	{"LTD121EWVB", 165},
	{"B121EW03 V6 ", 165},
	/* datasheets: between 100 and 20k, typical 200 */
	/* TESTED: works best at 400 */
	{"B121EW09 V3 ", 400},
	{"HV121WX4-120", 110}, /* Aftermarket AFFS lcd, works well at low pwm */
	{"LTN121AT03001", 110},
	{"LTN121AP03001", 750},
	/* TODO: untested panels found on thinkwiki */
	/* Generally CCFL runs best at lower PWM */
	/* {"B121EW09 V2 ", 450}, */
	/* {"N121IB-L05", 450}, */
	{"LP121WX3-TLC1", 400}, /* TESTED to work best at this value */
	/* {"LP121WX3-TLA1" 450}, */
	/* {"B121EW03 V3 " 110}, */
	/* {"LTN121AP02001" 110}, */
};

int get_blc_values(const struct blc_pwm_t **entries)
{
	*entries = blc_entries;
	return ARRAY_SIZE(blc_entries);
}
