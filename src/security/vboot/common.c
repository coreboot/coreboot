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
#include <stdint.h>
#include <string.h>
#include <symbols.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

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

void vboot_init_work_context(struct vb2_context *ctx)
{
	struct vboot_working_data *wd;

	/* First initialize the working data region. */
	wd = vboot_get_working_data();
	memset(wd, 0, VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE);

	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);
	wd->buffer_size = VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE
			  - wd->buffer_offset;

	/* Initialize the vb2_context. */
	memset(ctx, 0, sizeof(*ctx));
	ctx->workbuf = (void *)vboot_get_shared_data();
	ctx->workbuf_size = wd->buffer_size;
}

void vboot_finalize_work_context(struct vb2_context *ctx)
{
	/*
	 * Shrink buffer_size so that vboot_migrate_cbmem knows how
	 * much of vboot_working_data needs to be copied into CBMEM
	 * (if applicable), and so that downstream users know how much
	 * of the workbuf is currently used.
	 */
	vboot_get_working_data()->buffer_size = ctx->workbuf_used;
}

struct vb2_shared_data *vboot_get_shared_data(void)
{
	struct vboot_working_data *wd = vboot_get_working_data();
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}

int vboot_get_selected_region(struct region *region)
{
	const struct selected_region *reg =
		&vboot_get_working_data()->selected_region;

	if (reg == NULL)
		return -1;

	if (reg->offset == 0 && reg->size == 0)
		return -1;

	region->offset = reg->offset;
	region->size = reg->size;

	return 0;
}

void vboot_set_selected_region(const struct region *region)
{
	struct selected_region *reg =
		&vboot_get_working_data()->selected_region;

	assert(reg != NULL);

	reg->offset = region_offset(region);
	reg->size = region_sz(region);
}

int vboot_is_slot_selected(void)
{
	struct selected_region *reg =
		&vboot_get_working_data()->selected_region;

	assert(reg != NULL);

	return reg->size > 0;
}

#if CONFIG(VBOOT_MIGRATE_WORKING_DATA)
/*
 * For platforms that do not employ VBOOT_STARTS_IN_ROMSTAGE, vboot
 * verification occurs before CBMEM is brought online, using pre-RAM.
 * In order to make vboot data structures available downstream, copy
 * vboot_working_data from SRAM/CAR into CBMEM on platforms where this
 * memory later becomes unavailable.
 */
static void vboot_migrate_cbmem(int unused)
{
	const struct vboot_working_data *wd_preram =
		(struct vboot_working_data *)_vboot2_work;
	size_t cbmem_size = wd_preram->buffer_offset + wd_preram->buffer_size;
	struct vboot_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF, cbmem_size);
	assert(wd_cbmem != NULL);

	printk(BIOS_DEBUG,
	       "VBOOT: copying vboot_working_data (%zu bytes) to CBMEM...\n",
	       cbmem_size);
	memcpy(wd_cbmem, wd_preram, cbmem_size);
}
ROMSTAGE_CBMEM_INIT_HOOK(vboot_migrate_cbmem)
#elif CONFIG(VBOOT_STARTS_IN_ROMSTAGE)
static void vboot_setup_cbmem(int unused)
{
	struct vboot_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF,
			  VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE);
	assert(wd_cbmem != NULL);
}
ROMSTAGE_CBMEM_INIT_HOOK(vboot_setup_cbmem)
#endif
