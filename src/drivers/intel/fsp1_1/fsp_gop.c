/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <boot/coreboot_tables.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

/* Reading VBT table from flash */
const optionrom_vbt_t *fsp_get_vbt(uint32_t *vbt_len)
{
	size_t vbt_size;
	union {
		const optionrom_vbt_t *data;
		uint32_t *signature;
	} vbt;

	/* Locate the vbt file in cbfs */
	vbt.data = cbfs_boot_map_with_leak("vbt.bin", CBFS_TYPE_OPTIONROM,
						&vbt_size);
	if (!vbt.data) {
		printk(BIOS_INFO,
			"FSP_INFO: VBT data file (vbt.bin) not found in CBFS");
		return NULL;
	}

	/* Validate the vbt file */
	if (*vbt.signature != VBT_SIGNATURE) {
		printk(BIOS_WARNING,
			"FSP_WARNING: Invalid signature in VBT data file (vbt.bin)!\n");
		return NULL;
	}
	*vbt_len = vbt_size;
	printk(BIOS_DEBUG, "FSP_INFO: VBT found at %p, 0x%08x bytes\n",
		vbt.data, *vbt_len);

#if IS_ENABLED(CONFIG_DISPLAY_VBT)
	/* Display the vbt file contents */
	printk(BIOS_DEBUG, "VBT Data:\n");
	hexdump(vbt.data, *vbt_len);
	printk(BIOS_DEBUG, "\n");
#endif

	/* Return the pointer to the vbt file data */
	return vbt.data;
}

void lb_framebuffer(struct lb_header *header)
{
	struct lb_framebuffer *framebuffer;
	framebuffer = (struct lb_framebuffer *)lb_new_record(header);

	VOID *hob_list_ptr;
	hob_list_ptr = get_hob_list();
	const EFI_GUID vbt_guid = EFI_PEI_GRAPHICS_INFO_HOB_GUID;
	u32 *vbt_hob;
	EFI_PEI_GRAPHICS_INFO_HOB *vbt_gop;
	vbt_hob = get_next_guid_hob(&vbt_guid, hob_list_ptr);
	if (vbt_hob == NULL) {
		printk(BIOS_ERR, "FSP_ERR: Graphics Data HOB is not present\n");
		return;
	} else {
		printk(BIOS_DEBUG, "FSP_DEBUG: Graphics Data HOB present\n");
		vbt_gop = GET_GUID_HOB_DATA(vbt_hob);
	}

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
	framebuffer->tag = LB_TAG_FRAMEBUFFER;
	framebuffer->size = sizeof(*framebuffer);
}
