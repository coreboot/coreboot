/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <fsp/graphics.h>
#include <fsp/util.h>
#include <soc/intel/common/vbt.h>
#include <types.h>
#include <framebuffer_info.h>

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


void fsp_report_framebuffer_info(const uintptr_t framebuffer_bar)
{
	size_t size;
	const struct hob_graphics_info *ginfo;
	const struct fsp_framebuffer *fbinfo;

	/*
	 * Pci enumeration happens after silicon init.
	 * After enumeration graphic framebuffer base may be relocated.
	 */
	if (!framebuffer_bar) {
		printk(BIOS_ALERT, "Framebuffer BAR invalid\n");
		return;
	}

	ginfo = fsp_find_extension_hob_by_guid(fsp_graphics_info_guid, &size);

	if (!ginfo) {
		printk(BIOS_ALERT, "Graphics hand-off block not found\n");
		return;
	}

	if (ginfo->pixel_format >= ARRAY_SIZE(fsp_framebuffer_format_map)) {
		printk(BIOS_ALERT, "FSP set unknown framebuffer format: %d\n",
		       ginfo->pixel_format);
		return;
	}

	fbinfo = fsp_framebuffer_format_map + ginfo->pixel_format;

	const struct lb_framebuffer fb = {
		.physical_address    = framebuffer_bar,
		.x_resolution        = ginfo->horizontal_resolution,
		.y_resolution        = ginfo->vertical_resolution,
		.bytes_per_line      = ginfo->pixels_per_scanline * 4,
		.bits_per_pixel      = fbinfo->rsvd.size + fbinfo->red.size +
				       fbinfo->green.size + fbinfo->blue.size,
		.red_mask_pos        = fbinfo->red.pos,
		.red_mask_size       = fbinfo->red.size,
		.green_mask_pos      = fbinfo->green.pos,
		.green_mask_size     = fbinfo->green.size,
		.blue_mask_pos       = fbinfo->blue.pos,
		.blue_mask_size      = fbinfo->blue.size,
		.reserved_mask_pos   = fbinfo->rsvd.pos,
		.reserved_mask_size  = fbinfo->rsvd.size,
		.orientation         = LB_FB_ORIENTATION_NORMAL,
	};

	fb_add_framebuffer_info_ex(&fb);
}
