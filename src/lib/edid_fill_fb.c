/* SPDX-License-Identifier: MIT */

#include <console/console.h>
#include <edid.h>
#include <boot/coreboot_tables.h>

static int fb_valid;
static struct lb_framebuffer edid_fb;

/*
 * Take an edid, and create a framebuffer. Set fb_valid to 1.
 */
void set_vbe_mode_info_valid(const struct edid *edid, uintptr_t fb_addr)
{
	edid_fb.physical_address = fb_addr;
	edid_fb.x_resolution = edid->x_resolution;
	edid_fb.y_resolution = edid->y_resolution;
	edid_fb.bytes_per_line = edid->bytes_per_line;
	/* In the case of (e.g.) 24 framebuffer bits per pixel, the convention
	 * nowadays seems to be to round it up to the nearest reasonable
	 * boundary, because otherwise the byte-packing is hideous.
	 * So, for example, in RGB with no alpha, the bytes are still
	 * packed into 32-bit words, the so-called 32bpp-no-alpha mode.
	 * Or, in 5:6:5 mode, the bytes are also packed into 32-bit words,
	 * and in 4:4:4 mode, they are packed into 16-bit words.
	 * Good call on the hardware guys part.
	 * It's not clear we're covering all cases here, but
	 * I'm not sure with grahpics you ever can.
	 */
	edid_fb.bits_per_pixel = edid->framebuffer_bits_per_pixel;
	edid_fb.reserved_mask_pos = 0;
	edid_fb.reserved_mask_size = 0;
	switch (edid->framebuffer_bits_per_pixel) {
	case 32:
	case 24:
		/* packed into 4-byte words */
		edid_fb.reserved_mask_pos = 24;
		edid_fb.reserved_mask_size = 8;
		edid_fb.red_mask_pos = 16;
		edid_fb.red_mask_size = 8;
		edid_fb.green_mask_pos = 8;
		edid_fb.green_mask_size = 8;
		edid_fb.blue_mask_pos = 0;
		edid_fb.blue_mask_size = 8;
		break;
	case 16:
		/* packed into 2-byte words */
		edid_fb.red_mask_pos = 11;
		edid_fb.red_mask_size = 5;
		edid_fb.green_mask_pos = 5;
		edid_fb.green_mask_size = 6;
		edid_fb.blue_mask_pos = 0;
		edid_fb.blue_mask_size = 5;
		break;
	default:
		printk(BIOS_SPEW, "%s: unsupported BPP %d\n", __func__,
		       edid->framebuffer_bits_per_pixel);
		return;
	}

	fb_valid = 1;
}

void set_vbe_framebuffer_orientation(enum lb_fb_orientation orientation)
{
	edid_fb.orientation = orientation;
}

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	if (!fb_valid)
		return -1;

	*framebuffer = edid_fb;

	return 0;
}
