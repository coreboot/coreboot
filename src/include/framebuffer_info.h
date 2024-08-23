/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __FRAMEBUFFER_INFO_H_
#define __FRAMEBUFFER_INFO_H_

#include <stdint.h>
#include <commonlib/coreboot_tables.h>

struct fb_info;

struct fb_info *
fb_add_framebuffer_info_ex(const struct lb_framebuffer *fb);

struct fb_info *fb_add_framebuffer_info(uintptr_t fb_addr, uint32_t x_resolution,
					uint32_t y_resolution, uint32_t bytes_per_line,
					uint8_t bits_per_pixel);
int fb_add_framebuffer_info_simple(uintptr_t fb_addr, uint32_t x_res, uint32_t y_res,
				   uint32_t bytes_per_line, uint8_t bits_per_pixel);

void fb_set_orientation(struct fb_info *info,
			enum lb_fb_orientation orientation);

struct edid;
struct fb_info *fb_new_framebuffer_info_from_edid(const struct edid *edid,
						  uintptr_t fb_addr);

#endif /* __FRAMEBUFFER_INFO_H_ */
