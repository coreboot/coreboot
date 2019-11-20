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

struct vboot_working_data *vboot_get_working_data(void)
{
	struct vboot_working_data *wd = NULL;

	if (cbmem_possibly_online())
		wd = cbmem_find(CBMEM_ID_VBOOT_WORKBUF);

	if (wd == NULL && CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) &&
	    preram_symbols_available())
		wd = (struct vboot_working_data *)_vboot2_work;

	assert(wd != NULL);

	return wd;
}

static inline void *vboot_get_workbuf(struct vboot_working_data *wd)
{
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}

struct vb2_context *vboot_get_context(void)
{
	struct vboot_working_data *wd;

	/* Return if context has already been initialized/restored. */
	if (vboot_ctx)
		return vboot_ctx;

	wd = vboot_get_working_data();

	/* Restore context from a previous stage. */
	if (vboot_logic_executed()) {
		assert(vb2api_reinit(vboot_get_workbuf(wd),
				     &vboot_ctx) == VB2_SUCCESS);
		return vboot_ctx;
	}

	assert(verification_should_run());

	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	memset(wd, 0, sizeof(*wd));
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);

	/* Initialize vb2_shared_data and friends. */
	assert(vb2api_init(vboot_get_workbuf(wd),
			   VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE -
			   wd->buffer_offset,
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

#if CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)
/*
 * For platforms that do not employ VBOOT_STARTS_IN_ROMSTAGE, vboot
 * verification occurs before CBMEM is brought online, using pre-RAM.
 * In order to make vboot data structures available downstream, copy
 * vboot_working_data from SRAM/CAR into CBMEM.
 */
static void vboot_migrate_cbmem(int unused)
{
	const size_t cbmem_size = VB2_KERNEL_WORKBUF_RECOMMENDED_SIZE;
	struct vboot_working_data *wd_preram =
		(struct vboot_working_data *)_vboot2_work;
	struct vboot_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF, cbmem_size);
	assert(wd_cbmem != NULL);
	memcpy(wd_cbmem, wd_preram, sizeof(struct vboot_working_data));
	vb2api_relocate(vboot_get_workbuf(wd_cbmem),
			vboot_get_workbuf(wd_preram),
			cbmem_size - wd_cbmem->buffer_offset,
			&vboot_ctx);
}
ROMSTAGE_CBMEM_INIT_HOOK(vboot_migrate_cbmem)
#else
static void vboot_setup_cbmem(int unused)
{
	struct vboot_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF,
			  VB2_KERNEL_WORKBUF_RECOMMENDED_SIZE);
	assert(wd_cbmem != NULL);
}
ROMSTAGE_CBMEM_INIT_HOOK(vboot_setup_cbmem)
#endif
