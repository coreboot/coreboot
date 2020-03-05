/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <vbe.h>
#include <console/console.h>
#include <endian.h>
#include <bootsplash.h>
#include <stdlib.h>

#include "jpeg.h"


void set_bootsplash(unsigned char *framebuffer, unsigned int x_resolution,
		    unsigned int y_resolution, unsigned int fb_resolution)
{
	printk(BIOS_INFO, "Setting up bootsplash in %dx%d@%d\n", x_resolution, y_resolution,
	       fb_resolution);
	struct jpeg_decdata *decdata;
	unsigned char *jpeg = cbfs_map("bootsplash.jpg", NULL);
	if (!jpeg) {
		printk(BIOS_ERR, "Could not find bootsplash.jpg\n");
		return;
	}

	int image_width, image_height;
	jpeg_fetch_size(jpeg, &image_width, &image_height);

	printk(BIOS_DEBUG, "Bootsplash image resolution: %dx%d\n", image_width, image_height);

	decdata = malloc(sizeof(*decdata));
	int ret = jpeg_decode(jpeg, framebuffer, x_resolution, y_resolution, fb_resolution,
			      decdata);
	cbfs_unmap(jpeg);
	if (ret != 0) {
		printk(BIOS_ERR, "Bootsplash could not be decoded. jpeg_decode returned %d.\n",
		       ret);
		return;
	}
	printk(BIOS_INFO, "Bootsplash loaded\n");
}
