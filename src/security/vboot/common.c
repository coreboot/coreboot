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

static struct vb2_context *vboot_ctx CAR_GLOBAL;

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
	struct vb2_context **vboot_ctx_ptr = car_get_var_ptr(&vboot_ctx);
	struct vboot_working_data *wd;

	/* Return if context has already been initialized/restored. */
	if (*vboot_ctx_ptr)
		return *vboot_ctx_ptr;

	wd = vboot_get_working_data();

	/* Restore context from a previous stage. */
	if (vboot_logic_executed()) {
		assert(vb2api_reinit(vboot_get_workbuf(wd),
				     vboot_ctx_ptr) == VB2_SUCCESS);
		return *vboot_ctx_ptr;
	}

	assert(verification_should_run());

	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	memset(wd, 0, sizeof(*wd));
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);
	wd->buffer_size = VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE
			  - wd->buffer_offset;

	/* Initialize vb2_shared_data and friends. */
	assert(vb2api_init(vboot_get_workbuf(wd), wd->buffer_size,
			   vboot_ctx_ptr) == VB2_SUCCESS);

	return *vboot_ctx_ptr;
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
	/*
	 * TODO(chromium:1021452): buffer_size is uint16_t and not large enough
	 * to hold the kernel verification workbuf size.  The only code which
	 * reads this value is in lb_vboot_workbuf() for lb_range->range_size.
	 * This value being zero doesn't cause any problems, since it is never
	 * read downstream.  Fix or deprecate vboot_working_data.
	 */
	wd_cbmem->buffer_size = 0;
	vb2api_relocate(vboot_get_workbuf(wd_cbmem),
			vboot_get_workbuf(wd_preram),
			cbmem_size - wd_cbmem->buffer_offset,
			car_get_var_ptr(&vboot_ctx));
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
