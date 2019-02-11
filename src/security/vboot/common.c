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
#include <reset.h>
#include <stdint.h>
#include <string.h>
#include <symbols.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

struct selected_region {
	uint32_t offset;
	uint32_t size;
};

/*
 * this is placed at the start of the vboot work buffer. selected_region is used
 * for the verstage to return the location of the selected slot. buffer is used
 * by the vboot2 core. Keep the struct CPU architecture agnostic as it crosses
 * stage boundaries.
 */
struct vb2_working_data {
	struct selected_region selected_region;
	/* offset of the buffer from the start of this struct */
	uint32_t buffer_offset;
	uint32_t buffer_size;
};

/* TODO(kitching): Use VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE instead. */
static size_t vb2_working_data_size(void)
{
	if (CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		return 12 * KiB;

	else if (CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) &&
		 preram_symbols_available())
		return REGION_SIZE(vboot2_work);

	die("impossible!");
}

static struct vb2_working_data * const vb2_get_working_data(void)
{
	struct vb2_working_data *wd = NULL;

	if (cbmem_possibly_online())
		wd = cbmem_find(CBMEM_ID_VBOOT_WORKBUF);

	if (wd == NULL && CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) &&
	    preram_symbols_available())
		wd = (struct vb2_working_data *)_vboot2_work;

	assert(wd != NULL);

	return wd;
}

void vb2_init_work_context(struct vb2_context *ctx)
{
	struct vb2_working_data *wd;

	/* First initialize the working data struct. */
	wd = vb2_get_working_data();
	memset(wd, 0, sizeof(struct vb2_working_data));

	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);
	wd->buffer_size = vb2_working_data_size() - wd->buffer_offset;

	/* Initialize the vb2_context. */
	memset(ctx, 0, sizeof(*ctx));
	ctx->workbuf = (void *)vb2_get_shared_data();
	ctx->workbuf_size = wd->buffer_size;
}

void vb2_finalize_work_context(struct vb2_context *ctx)
{
	/*
	 * Shrink buffer_size so that vb2_migrate_cbmem knows how much
	 * of vb2_working_data needs to be copied into CBMEM (if applicable),
	 * and so that downstream users know how much of the workbuf is
	 * currently used.
	 */
	vb2_get_working_data()->buffer_size = ctx->workbuf_used;
}

struct vb2_shared_data *vb2_get_shared_data(void)
{
	struct vb2_working_data *wd = vb2_get_working_data();
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}

int vb2_get_selected_region(struct region *region)
{
	const struct selected_region *reg =
		&vb2_get_working_data()->selected_region;

	if (reg == NULL)
		return -1;

	if (reg->offset == 0 && reg->size == 0)
		return -1;

	region->offset = reg->offset;
	region->size = reg->size;

	return 0;
}

void vb2_set_selected_region(const struct region *region)
{
	struct selected_region *reg = &vb2_get_working_data()->selected_region;

	assert(reg != NULL);

	reg->offset = region_offset(region);
	reg->size = region_sz(region);
}

int vb2_is_slot_selected(void)
{
	struct selected_region *reg = &vb2_get_working_data()->selected_region;

	assert(reg != NULL);

	return reg->size > 0;
}

#if CONFIG(VBOOT_MIGRATE_WORKING_DATA)
/*
 * For platforms that do not employ VBOOT_STARTS_IN_ROMSTAGE, vboot
 * verification occurs before CBMEM is brought online, using pre-RAM.
 * In order to make vboot data structures available downstream, copy
 * vb2_working_data from SRAM/CAR into CBMEM on platforms where this
 * memory later becomes unavailable.
 */
static void vb2_migrate_cbmem(int unused)
{
	const struct vb2_working_data *wd_preram =
		(struct vb2_working_data *)_vboot2_work;
	size_t cbmem_size = wd_preram->buffer_offset + wd_preram->buffer_size;
	struct vb2_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF, cbmem_size);
	printk(BIOS_DEBUG,
	       "VBOOT: copying vb2_working_data (%zu bytes) to CBMEM...\n",
	       cbmem_size);
	memcpy(wd_cbmem, wd_preram, cbmem_size);
	assert(wd_cbmem != NULL);
}
ROMSTAGE_CBMEM_INIT_HOOK(vb2_migrate_cbmem)
#elif CONFIG(VBOOT_STARTS_IN_ROMSTAGE)
static void vb2_setup_cbmem(int unused)
{
	struct vb2_working_data *wd_cbmem =
		cbmem_add(CBMEM_ID_VBOOT_WORKBUF, vb2_working_data_size());
	assert(wd_cbmem != NULL);
}
ROMSTAGE_CBMEM_INIT_HOOK(vb2_setup_cbmem)
#endif
