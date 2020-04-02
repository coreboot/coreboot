/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <fsp/util.h>

int fill_lb_framebuffer(struct lb_framebuffer *framebuffer)
{
	VOID *hob_list_ptr;
	hob_list_ptr = get_hob_list();
	const EFI_GUID vbt_guid = EFI_PEI_GRAPHICS_INFO_HOB_GUID;
	u32 *vbt_hob;
	EFI_PEI_GRAPHICS_INFO_HOB *vbt_gop;
	vbt_hob = get_next_guid_hob(&vbt_guid, hob_list_ptr);
	if (vbt_hob == NULL) {
		printk(BIOS_ERR, "FSP_ERR: Graphics Data HOB is not present\n");
		return -1;
	}
	printk(BIOS_DEBUG, "FSP_DEBUG: Graphics Data HOB present\n");
	vbt_gop = GET_GUID_HOB_DATA(vbt_hob);

	framebuffer->physical_address = vbt_gop->FrameBufferBase;
	framebuffer->x_resolution = vbt_gop->GraphicsMode.HorizontalResolution;
	framebuffer->y_resolution = vbt_gop->GraphicsMode.VerticalResolution;
	framebuffer->bytes_per_line = vbt_gop->GraphicsMode.PixelsPerScanLine
		* 4;
	framebuffer->bits_per_pixel = 32;
	framebuffer->red_mask_pos = 16;
	framebuffer->red_mask_size = 8;
	framebuffer->green_mask_pos = 8;
	framebuffer->green_mask_size = 8;
	framebuffer->blue_mask_pos = 0;
	framebuffer->blue_mask_size = 8;
	framebuffer->reserved_mask_pos = 24;
	framebuffer->reserved_mask_size = 8;

	return 0;
}
