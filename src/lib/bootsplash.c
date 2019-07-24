/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Johanna Schander <coreboot@mimoja.de>
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

#include <cbfs.h>
#include <vbe.h>
#include <console/console.h>
#include <endian.h>
#include <bootsplash.h>

#include "jpeg.h"

void set_vesa_bootsplash(void)
{
	const vbe_mode_info_t *mode_info = vbe_mode_info();
	if (mode_info != NULL) {
		printk(BIOS_INFO, "Setting up bootsplash\n");
		unsigned int x_resolution = le16_to_cpu(mode_info->vesa.x_resolution);
		unsigned int y_resolution = le16_to_cpu(mode_info->vesa.y_resolution);
		unsigned int fb_resolution = mode_info->vesa.bits_per_pixel;
		unsigned char *framebuffer =
			(unsigned char *)le32_to_cpu(mode_info->vesa.phys_base_ptr);

		set_bootsplash(framebuffer, x_resolution, y_resolution, fb_resolution);
	} else {
		printk(BIOS_ERR, "VBE modeinfo invalid\n");
	}
}


void set_bootsplash(unsigned char *framebuffer, unsigned int x_resolution,
		    unsigned int y_resolution, unsigned int fb_resolution)
{
	struct jpeg_decdata *decdata;
	unsigned char *jpeg =
		cbfs_boot_map_with_leak("bootsplash.jpg", CBFS_TYPE_BOOTSPLASH, NULL);
	if (!jpeg) {
		printk(BIOS_ERR, "Could not find bootsplash.jpg\n");
		return;
	}

	decdata = malloc(sizeof(*decdata));
	int ret = jpeg_decode(jpeg, framebuffer, x_resolution, y_resolution, fb_resolution,
			      decdata);
	if (ret != 0) {
		printk(BIOS_ERR, "Bootsplash could not be decoded. jpeg_decode returned %d.\n",
		       ret);
		return;
	}
	printk(BIOS_INFO, "Bootsplash loaded\n");
}
