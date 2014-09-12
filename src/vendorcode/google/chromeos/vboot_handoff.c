/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <2recovery_reasons.h>
#include <2struct.h>
#include <arch/stages.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <stdlib.h>
#include <timestamp.h>
#include "chromeos.h"
#include "fmap.h"
#include "vboot_handoff.h"
#include <vboot_struct.h>

static void *load_ramstage(struct vboot_handoff *vboot_handoff,
			   struct vboot_region *fw_main)
{
	struct vboot_components *fw_info;
	int i;

	fw_info = vboot_locate_components(fw_main);
	if (fw_info == NULL)
		die("failed to locate firmware components\n");

	/* these offset & size are used to load a rw boot loader */
	for (i = 0; i < fw_info->num_components; i++) {
		vboot_handoff->components[i].address =
			fw_main->offset_addr + fw_info->entries[i].offset;
		vboot_handoff->components[i].size = fw_info->entries[i].size;
	}

	return vboot_load_stage(CONFIG_VBOOT_RAMSTAGE_INDEX, fw_main, fw_info);
}

/**
 * Sets vboot_handoff based on the information in vb2_shared_data
 *
 * TODO: Read wp switch to set VBSD_BOOT_FIRMWARE_WP_ENABLED
 */
static void fill_vboot_handoff(struct vboot_handoff *vboot_handoff,
			       struct vb2_shared_data *vb2_sd)
{
	VbSharedDataHeader *vb_sd =
		(VbSharedDataHeader *)vboot_handoff->shared_data;
	uint32_t *oflags = &vboot_handoff->init_params.out_flags;

	vb_sd->flags |= VBSD_BOOT_FIRMWARE_VBOOT2;

	vboot_handoff->selected_firmware = vb2_sd->fw_slot;

	vb_sd->firmware_index = vb2_sd->fw_slot;

	vb_sd->magic = VB_SHARED_DATA_MAGIC;
	vb_sd->struct_version = VB_SHARED_DATA_VERSION;
	vb_sd->struct_size = sizeof(VbSharedDataHeader);
	vb_sd->data_size = VB_SHARED_DATA_MIN_SIZE;
	vb_sd->data_used = sizeof(VbSharedDataHeader);

	if (vb2_sd->recovery_reason) {
		vb_sd->firmware_index = 0xFF;
		if (vb2_sd->recovery_reason == VB2_RECOVERY_RO_MANUAL)
			vb_sd->flags |= VBSD_BOOT_REC_SWITCH_ON;
		*oflags |= VB_INIT_OUT_ENABLE_RECOVERY;
		*oflags |= VB_INIT_OUT_CLEAR_RAM;
		*oflags |= VB_INIT_OUT_ENABLE_DISPLAY;
		*oflags |= VB_INIT_OUT_ENABLE_USB_STORAGE;
	}
	if (vb2_sd->flags & VB2_SD_DEV_MODE_ENABLED) {
		*oflags |= VB_INIT_OUT_ENABLE_DEVELOPER;
		*oflags |= VB_INIT_OUT_CLEAR_RAM;
		*oflags |= VB_INIT_OUT_ENABLE_DISPLAY;
		*oflags |= VB_INIT_OUT_ENABLE_USB_STORAGE;
		vb_sd->flags |= VBSD_BOOT_DEV_SWITCH_ON;
		vb_sd->flags |= VBSD_LF_DEV_SWITCH_ON;
	}
	/* TODO: Set these in depthcharge */
	if (CONFIG_VIRTUAL_DEV_SWITCH)
		vb_sd->flags |= VBSD_HONOR_VIRT_DEV_SWITCH;
	if (CONFIG_EC_SOFTWARE_SYNC) {
		vb_sd->flags |= VBSD_EC_SOFTWARE_SYNC;
		vb_sd->flags |= VBSD_BOOT_REC_SWITCH_VIRTUAL;
	}
	/* In vboot1, VBSD_FWB_TRIED is
	 * set only if B is booted as explicitly requested. Therefore, if B is
	 * booted because A was found bad, the flag should not be set. It's
	 * better not to touch it if we can only ambiguously control it. */
	/* if (vb2_sd->fw_slot)
		vb_sd->flags |= VBSD_FWB_TRIED; */

	/* copy kernel subkey if it's found */
	if (vb2_sd->workbuf_preamble_size) {
		struct vb2_fw_preamble *fp;
		uintptr_t dst, src;
		printk(BIOS_INFO, "Copying FW preamble\n");
		fp = (struct vb2_fw_preamble *)( (uintptr_t)vb2_sd +
				vb2_sd->workbuf_preamble_offset);
		src = (uintptr_t)&fp->kernel_subkey +
				fp->kernel_subkey.key_offset;
		dst = (uintptr_t)vb_sd + sizeof(VbSharedDataHeader);
		assert(dst + fp->kernel_subkey.key_size <=
		       (uintptr_t)vboot_handoff + sizeof(*vboot_handoff));
		memcpy((void *)dst, (void *)src,
		       fp->kernel_subkey.key_size);
		vb_sd->data_used += fp->kernel_subkey.key_size;
		vb_sd->kernel_subkey.key_offset =
				dst - (uintptr_t)&vb_sd->kernel_subkey;
		vb_sd->kernel_subkey.key_size = fp->kernel_subkey.key_size;
		vb_sd->kernel_subkey.algorithm = fp->kernel_subkey.algorithm;
		vb_sd->kernel_subkey.key_version =
				fp->kernel_subkey.key_version;
	}

	vb_sd->recovery_reason = vb2_sd->recovery_reason;
}

/**
 * Load ramstage and return the entry point
 */
void *vboot_load_ramstage(void)
{
	struct vboot_handoff *vh;
	struct vb2_shared_data *sd;
	struct vboot_region fw_main;
	struct vb2_working_data *wd = vboot_get_working_data();

	sd = (struct vb2_shared_data *)(uintptr_t)wd->buffer;
	sd->workbuf_hash_offset = 0;
	sd->workbuf_hash_size = 0;

	printk(BIOS_INFO, "creating vboot_handoff structure\n");
	vh = cbmem_add(CBMEM_ID_VBOOT_HANDOFF, sizeof(*vh));
	if (vh == NULL)
		/* we don't need to failover gracefully here because this
		 * shouldn't happen with the image that has passed QA. */
		die("failed to allocate vboot_handoff structure\n");

	memset(vh, 0, sizeof(*vh));

	/* needed until we finish transtion to vboot2 for kernel verification */
	fill_vboot_handoff(vh, sd);

	if (vboot_is_readonly_path(wd))
		/* we're on recovery path. continue to ro-ramstage. */
		return NULL;

	printk(BIOS_INFO,
	       "loading ramstage from Slot %c\n", sd->fw_slot ? 'B' : 'A');
	vb2_get_selected_region(wd, &fw_main);

	return load_ramstage(vh, &fw_main);
}
