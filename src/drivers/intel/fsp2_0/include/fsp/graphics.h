/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_GRAPHICS_H_
#define _FSP2_0_GRAPHICS_H_

#include <types.h>

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

/*
 * Report the fsp_graphics_info_guid HOB to framebuffer info.
 *
 * Must be called after PCI enumeration to make sure that the BAR
 * doesn't change any more.
 */
void fsp_report_framebuffer_info(const uintptr_t framebuffer_bar,
				 enum lb_fb_orientation orientation);

/* SoC Overrides */
/*
 * Check and report if an external display is attached
 *
 * Possible return values:
 * 1 - An external device is attached.
 * 0 - On-board display alone.
 */
int fsp_soc_report_external_display(void);

#endif /* _FSP2_0_GRAPHICS_H_ */
