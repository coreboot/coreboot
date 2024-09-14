/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <vbe.h>
#include <console/console.h>
#include <endian.h>
#include <bootsplash.h>

#include "jpeg.h"


void set_bootsplash(unsigned char *framebuffer, unsigned int x_resolution,
		    unsigned int y_resolution, unsigned int bytes_per_line,
		    unsigned int fb_resolution)
{
	if ((x_resolution > INT_MAX) || (y_resolution) > INT_MAX) {
		printk(BIOS_ERR, "Display resolution way too large.\n");
		return;
	}
	int xres = x_resolution, yres = y_resolution;
	printk(BIOS_INFO, "Setting up bootsplash in %dx%d@%d\n", x_resolution, y_resolution,
	       fb_resolution);
	size_t filesize;
	unsigned char *jpeg = cbfs_map("bootsplash.jpg", &filesize);
	if (!jpeg) {
		printk(BIOS_ERR, "Could not find bootsplash.jpg\n");
		return;
	}

	unsigned int image_width, image_height;
	const char *err = jpeg_fetch_size(jpeg, filesize, &image_width, &image_height);
	if (err != NULL) {
		printk(BIOS_ERR, "Could not parse bootsplash.jpg: %s\n", err);
		return;
	}

	printk(BIOS_DEBUG, "Bootsplash image resolution: %dx%d\n", image_width, image_height);

	if (image_width > xres || image_height > yres) {
		printk(BIOS_NOTICE, "Bootsplash image can't fit framebuffer.\n");
		cbfs_unmap(jpeg);
		return;
	}

	/* center image: */
	framebuffer += (yres - image_height) / 2 * bytes_per_line
		       + (xres - image_width) / 2 * (fb_resolution / 8);

	err = jpeg_decode(jpeg, filesize, framebuffer, image_width, image_height,
			  bytes_per_line, fb_resolution);
	cbfs_unmap(jpeg);
	if (err != NULL) {
		printk(BIOS_ERR, "Could not decode bootsplash.jpg: %s\n", err);
		return;
	}
	printk(BIOS_INFO, "Bootsplash loaded\n");
}
