/*
 * This file is part of the coreboot project.
 *
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
