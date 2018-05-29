/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <compiler.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/intel/common/vbt.h>

enum pixel_format {
	pixel_rgbx_8bpc = 0,
	pixel_bgrx_8bpc = 1,
	pixel_bitmask = 2,		/* defined by <rgb>_mask values */
};

static const uint8_t fsp_graphics_info_guid[16] = {
	0xce, 0x2c, 0xf6, 0x39, 0x25, 0x68, 0x69, 0x46,
	0xbb, 0x56, 0x54, 0x1a, 0xba, 0x75, 0x3a, 0x07
};

struct hob_graphics_info {
	uint64_t framebuffer_base;
	uint32_t framebuffer_size;
	uint32_t version;
	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;
	uint32_t pixel_format;		/* See enum pixel_format */
	uint32_t red_mask;
	uint32_t green_mask;
	uint32_t blue_mask;
	uint32_t reserved_mask;
	uint32_t pixels_per_scanline;
} __packed;

struct pixel {
	uint8_t pos;
	uint8_t size;
};

static const struct fsp_framebuffer {
	struct pixel red;
	struct pixel green;
	struct pixel blue;
	struct pixel rsvd;
} fsp_framebuffer_format_map[] = {
	[pixel_rgbx_8bpc] = { {0, 8}, {8, 8}, {16, 8}, {24, 8} },
	[pixel_bgrx_8bpc] = { {16, 8}, {8, 8}, {0, 8}, {24, 8} },
};

enum cb_err fsp_fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	size_t size;
	const struct hob_graphics_info *ginfo;
	const struct fsp_framebuffer *fbinfo;

	ginfo = fsp_find_extension_hob_by_guid(fsp_graphics_info_guid, &size);

	if (!ginfo) {
		printk(BIOS_ALERT, "Graphics hand-off block not found\n");
		return CB_ERR;
	}

	if (ginfo->pixel_format >= ARRAY_SIZE(fsp_framebuffer_format_map)) {
		printk(BIOS_ALERT, "FSP set unknown framebuffer format: %d\n",
		       ginfo->pixel_format);
		return CB_ERR;
	}

	fbinfo = fsp_framebuffer_format_map + ginfo->pixel_format;

	framebuffer->physical_address = ginfo->framebuffer_base;
	framebuffer->x_resolution = ginfo->horizontal_resolution;
	framebuffer->y_resolution = ginfo->vertical_resolution;
	framebuffer->bytes_per_line = ginfo->pixels_per_scanline * 4;
	framebuffer->bits_per_pixel = 32;
	framebuffer->red_mask_pos = fbinfo->red.pos;
	framebuffer->red_mask_size = fbinfo->red.size;
	framebuffer->green_mask_pos = fbinfo->green.pos;
	framebuffer->green_mask_size = fbinfo->green.size;
	framebuffer->blue_mask_pos = fbinfo->blue.pos;
	framebuffer->blue_mask_size = fbinfo->blue.size;
	framebuffer->reserved_mask_pos = fbinfo->rsvd.pos;
	framebuffer->reserved_mask_size = fbinfo->rsvd.pos;

	return CB_SUCCESS;
}

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	enum cb_err ret;
	uintptr_t framebuffer_bar;

	/* Pci enumeration happens after silicon init.
	 * After enumeration graphic framebuffer base may be relocated.
	 * Get framebuffer base from soc.
	 */
	framebuffer_bar = fsp_soc_get_igd_bar();

	if (!framebuffer_bar) {
		printk(BIOS_ALERT, "Framebuffer BAR invalid\n");
		return -1;
	}

	ret = fsp_fill_lb_framebuffer(framebuffer);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ALERT, "FSP did not return a valid framebuffer\n");
		return -1;
	}

	/* Resource allocator can move the BAR around after FSP configures it */
	framebuffer->physical_address = framebuffer_bar;
	printk(BIOS_DEBUG, "Graphics framebuffer located at 0x%llx\n",
		framebuffer->physical_address);

	return 0;
}

__weak uintptr_t fsp_soc_get_igd_bar(void)
{
	return 0;
}
