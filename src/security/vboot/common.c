/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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

#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <fmap.h>
#include <stdint.h>
#include <string.h>
#include <symbols.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

static struct vb2_context *vboot_ctx;

static void *vboot_get_workbuf(void)
{
	void *wb = NULL;

	if (cbmem_possibly_online())
		wb = cbmem_find(CBMEM_ID_VBOOT_WORKBUF);

	if (wb == NULL && CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) &&
	    preram_symbols_available())
		wb = _vboot2_work;

	assert(wb != NULL);

	return wb;
}

struct vb2_context *vboot_get_context(void)
{
	void *wb;

	/* Return if context has already been initialized/restored. */
	if (vboot_ctx)
		return vboot_ctx;

	wb = vboot_get_workbuf();

	/* Restore context from a previous stage. */
	if (vboot_logic_executed()) {
		assert(vb2api_reinit(wb, &vboot_ctx) == VB2_SUCCESS);
		return vboot_ctx;
	}

	assert(verification_should_run());

	/* Initialize vb2_shared_data and friends. */
	assert(vb2api_init(wb, VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE,
			   &vboot_ctx) == VB2_SUCCESS);

	return vboot_ctx;
}

int vboot_locate_firmware(const struct vb2_context *ctx,
			  struct region_device *fw)
{
	const char *name;

	if (vboot_is_firmware_slot_a(ctx))
		name = "FW_MAIN_A";
	else
		name = "FW_MAIN_B";

	return fmap_locate_area_as_rdev(name, fw);
}

static void vboot_setup_cbmem(int unused)
{
	const size_t cbmem_size = VB2_KERNEL_WORKBUF_RECOMMENDED_SIZE;
	void *wb_cbmem = cbmem_add(CBMEM_ID_VBOOT_WORKBUF, cbmem_size);
	assert(wb_cbmem != NULL);
	/*
	 * For platforms where VBOOT_STARTS_IN_BOOTBLOCK, vboot verification
	 * occurs before CBMEM is brought online, using pre-RAM. In order to
	 * make vboot data structures available downstream, copy vboot workbuf
	 * from SRAM/CAR into CBMEM.
	 */
	if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK))
		assert(vb2api_relocate(wb_cbmem, _vboot2_work, cbmem_size,
				       &vboot_ctx) == VB2_SUCCESS);
}
ROMSTAGE_CBMEM_INIT_HOOK(vboot_setup_cbmem)
