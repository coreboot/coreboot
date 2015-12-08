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
 */

#include <arch/stages.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <fmap.h>
#include <stdlib.h>
#include <timestamp.h>
#define NEED_VB20_INTERNALS  /* TODO: remove me! */
#include <vb2_api.h>
#include <vboot_struct.h>
#include "../chromeos.h"
#include "../vboot_handoff.h"
#include "misc.h"

/**
 * Sets vboot_handoff based on the information in vb2_shared_data
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
	vb_sd->fw_version_tpm = vb2_sd->fw_version_secdata;

	if (get_write_protect_state())
		vb_sd->flags |= VBSD_BOOT_FIRMWARE_WP_ENABLED;
	if (get_sw_write_protect_state())
		vb_sd->flags |= VBSD_BOOT_FIRMWARE_SW_WP_ENABLED;

	if (vb2_sd->recovery_reason) {
		vb_sd->firmware_index = 0xFF;
		if (vb2_sd->flags & VB2_SD_FLAG_MANUAL_RECOVERY)
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
	if (CONFIG_EC_SOFTWARE_SYNC)
		vb_sd->flags |= VBSD_EC_SOFTWARE_SYNC;
	if (!CONFIG_PHYSICAL_REC_SWITCH)
		vb_sd->flags |= VBSD_BOOT_REC_SWITCH_VIRTUAL;

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
		fp = (struct vb2_fw_preamble *)((uintptr_t)vb2_sd +
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

void vboot_fill_handoff(void)
{
	struct vboot_handoff *vh;
	struct vb2_shared_data *sd;

	sd = vb2_get_shared_data();
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
}

/*
 * For platforms that employ VBOOT_DYNAMIC_WORK_BUFFER, the vboot
 * verification doesn't happen until after cbmem is brought online.
 * Therefore, the vboot results would not be initialized so don't
 * automatically add results when cbmem comes online.
 */
#if !IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER)
static void vb2_fill_handoff_cbmem(int unused)
{
	vboot_fill_handoff();
}
ROMSTAGE_CBMEM_INIT_HOOK(vb2_fill_handoff_cbmem)
#endif
