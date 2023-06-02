/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_RAS_H
#define MAINBOARD_RAS_H

struct fru {
	const char *str;
};

struct fru *mainboard_ras_get_ch_map(void);

#endif
