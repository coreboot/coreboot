/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Siemens AG
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

#include "rx6110sa.h"

struct drivers_i2c_rx6110sa_config {
	/* The day (of the week) is indicated by 7 bits, bit 0 to bit 6. */
	unsigned char user_weekday;	/* User day of the week to set */
	unsigned char user_day;		/* User day to set */
	unsigned char user_month;	/* User month to set */
	unsigned char user_year;	/* User year to set */
	unsigned char set_user_date;	/* Use user date from device tree */
	unsigned char cof_selection;	/* Set up "clock out" frequency */
};
