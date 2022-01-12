/* SPDX-License-Identifier: MIT */

#include <console/console.h>
#include <edid.h>
#include <boot/coreboot_tables.h>
#include <framebuffer_info.h>
#include <string.h>
#include <stdlib.h>
#include <list.h>

struct fb_info {
	struct list_node node;
	struct lb_framebuffer fb;
};
static struct list_node list;

/*
 * Allocate a new framebuffer info struct on heap.
 * Returns NULL on error.
 */
static struct fb_info *fb_new_framebuffer_info(void)
{
	struct fb_info *ret;
	ret = malloc(sizeof(struct fb_info));
	if (ret)
		memset(ret, 0, sizeof(struct fb_info));

	return ret;
}

/*
 * Fills a provided framebuffer info struct and adds it to the internal list if it's
 * valid. Returns NULL on error.
 */
struct fb_info *
fb_add_framebuffer_info_ex(const struct lb_framebuffer *fb)
{
	struct fb_info *info;
	uint8_t bpp_mask;

	/* Validate input */
	if (!fb || !fb->x_resolution || !fb->y_resolution || !fb->bytes_per_line ||
	    !fb->bits_per_pixel) {
		printk(BIOS_ERR, "%s: Invalid framebuffer data provided\n", __func__);
		return NULL;
	}

	bpp_mask = fb->blue_mask_size + fb->green_mask_size + fb->red_mask_size +
		fb->reserved_mask_size;
	if (bpp_mask > fb->bits_per_pixel) {
		printk(BIOS_ERR,
		       "%s: channel bit mask=%d is greater than BPP=%d ."
		       " This is a driver bug. Framebuffer is invalid.\n",
		       __func__, bpp_mask, fb->bits_per_pixel);
		return NULL;
	} else if (bpp_mask != fb->bits_per_pixel) {
		printk(BIOS_WARNING,
		       "%s: channel bit mask=%d and BPP=%d don't match."
		       " This is a driver bug.\n",
		       __func__, bpp_mask, fb->bits_per_pixel);
	}

	info = fb_new_framebuffer_info();
	if (!info)
		return NULL;

	printk(BIOS_INFO, "framebuffer_info: bytes_per_line: %d, bits_per_pixel: %d\n "
			  "                  x_res x y_res: %d x %d, size: %d at 0x%llx\n",
			fb->bytes_per_line, fb->bits_per_pixel, fb->x_resolution,
			fb->y_resolution, (fb->bytes_per_line * fb->y_resolution),
			fb->physical_address);

	/* Update */
	info->fb = *fb;

	list_insert_after(&info->node, &list);

	return info;
}

/*
 * Allocates a new framebuffer info struct and fills it for 32/24/16bpp framebuffers.
 * Intended for drivers that only support reporting the current information or have a single
 * modeset invocation.
 *
 * Complex drivers should use fb_add_framebuffer_info_ex() instead.
 */
struct fb_info *
fb_add_framebuffer_info(uintptr_t fb_addr, uint32_t x_resolution,
			uint32_t y_resolution, uint32_t bytes_per_line,
			uint8_t bits_per_pixel)
{
	struct fb_info *info = NULL;

	switch (bits_per_pixel) {
	case 32:
	case 24: {
		/* FIXME: 24 BPP might be RGB8 or XRGB8 */
		/* packed into 4-byte words */

		const struct lb_framebuffer fb = {
			.physical_address    = fb_addr,
			.x_resolution        = x_resolution,
			.y_resolution        = y_resolution,
			.bytes_per_line      = bytes_per_line,
			.bits_per_pixel      = bits_per_pixel,
			.red_mask_pos        = 16,
			.red_mask_size       = 8,
			.green_mask_pos      = 8,
			.green_mask_size     = 8,
			.blue_mask_pos       = 0,
			.blue_mask_size      = 8,
			.reserved_mask_pos   = 24,
			.reserved_mask_size  = 8,
			.orientation         = LB_FB_ORIENTATION_NORMAL,
		};

		info = fb_add_framebuffer_info_ex(&fb);
		break;
	}
	case 16: {
		/* packed into 2-byte words */
		const struct lb_framebuffer fb = {
			.physical_address   = fb_addr,
			.x_resolution       = x_resolution,
			.y_resolution       = y_resolution,
			.bytes_per_line     = bytes_per_line,
			.bits_per_pixel     = 16,
			.red_mask_pos       = 11,
			.red_mask_size      = 5,
			.green_mask_pos     = 5,
			.green_mask_size    = 6,
			.blue_mask_pos      = 0,
			.blue_mask_size     = 5,
			.reserved_mask_pos  = 0,
			.reserved_mask_size = 0,
			.orientation        = LB_FB_ORIENTATION_NORMAL,
		};
		info = fb_add_framebuffer_info_ex(&fb);
		break;
	}
	default:
		printk(BIOS_ERR, "%s: unsupported BPP %d\n", __func__, bits_per_pixel);
	}
	if (!info)
		printk(BIOS_ERR, "%s: failed to add framebuffer info\n", __func__);

	return info;
}

void fb_set_orientation(struct fb_info *info, enum lb_fb_orientation orientation)
{
	if (!info)
		return;

	info->fb.orientation = orientation;
}

/*
 * Take an edid, and create a framebuffer.
 */
struct fb_info *fb_new_framebuffer_info_from_edid(const struct edid *edid,
							 uintptr_t fb_addr)
{
	return fb_add_framebuffer_info(fb_addr, edid->x_resolution, edid->y_resolution,
		edid->bytes_per_line, edid->framebuffer_bits_per_pixel);
}

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	struct fb_info *i;

	list_for_each(i, list, node) {
		//TODO: Add support for advertising all framebuffers in this list
		*framebuffer = i->fb;
		return 0;
	}
	return -1;
}
