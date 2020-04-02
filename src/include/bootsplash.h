/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __BOOTSPLASH_H__
#define __BOOTSPLASH_H__

#include <types.h>

/**
 * Sets up the framebuffer with the bootsplash.jpg from cbfs.
 * Returns 0 on success
 * CB_ERR on cbfs errors
 * and >0 on jpeg errors.
 */
void set_bootsplash(unsigned char *framebuffer, unsigned int x_resolution,
		    unsigned int y_resolution, unsigned int fb_resolution);

#endif
