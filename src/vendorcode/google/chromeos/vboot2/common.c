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

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <reset.h>
#include <string.h>
#include <vb2_api.h>
#include "../chromeos.h"
#include "../symbols.h"
#include "../vboot_handoff.h"
#include "misc.h"

struct selected_region {
	uint32_t offset;
	uint32_t size;
};

/*
 * this is placed at the start of the vboot work buffer. selected_region is used
 * for the verstage to return the location of the selected slot. buffer is used
 * by the vboot2 core. Keep the struct cpu architecture agnostic as it crosses
 * stage boundaries.
 */
struct vb2_working_data {
	struct selected_region selected_region;
	/* offset of the buffer from the start of this struct */
	uint32_t buffer_offset;
	uint32_t buffer_size;
};

static const size_t vb_work_buf_size = 16 * KiB;

static struct vb2_working_data * const vboot_get_working_data(void)
{
	if (IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER))
		/* cbmem_add() does a cbmem_find() first. */
		return cbmem_add(CBMEM_ID_VBOOT_WORKBUF, vb_work_buf_size);
	else
		return (struct vb2_working_data *)_vboot2_work;
}

static size_t vb2_working_data_size(void)
{
	if (IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER))
		return vb_work_buf_size;
	else
		return _vboot2_work_size;
}

static struct selected_region *vb2_selected_region(void)
{
	struct selected_region *sel_reg = NULL;

	/* Ramstage always uses cbmem as a source of truth. */
	if (ENV_RAMSTAGE)
		sel_reg = cbmem_find(CBMEM_ID_VBOOT_SEL_REG);
	else if (ENV_ROMSTAGE) {
		/* Try cbmem first. Fall back on working data if not found. */
		sel_reg = cbmem_find(CBMEM_ID_VBOOT_SEL_REG);

		if (sel_reg == NULL) {
			struct vb2_working_data *wd = vboot_get_working_data();
			sel_reg = &wd->selected_region;
		}
	} else {
		/* Stages such as bootblock and verstage use working data. */
		struct vb2_working_data *wd = vboot_get_working_data();
		sel_reg = &wd->selected_region;
	}

	return sel_reg;
}

void vb2_init_work_context(struct vb2_context *ctx)
{
	struct vb2_working_data *wd;
	size_t work_size;

	/* First initialize the working data region. */
	work_size = vb2_working_data_size();
	wd = vboot_get_working_data();
	memset(wd, 0, work_size);

	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);
	wd->buffer_size = work_size - wd->buffer_offset;

	/* Initialize the vb2_context. */
	memset(ctx, 0, sizeof(*ctx));
	ctx->workbuf = (void *)vb2_get_shared_data();
	ctx->workbuf_size = wd->buffer_size;

}

struct vb2_shared_data *vb2_get_shared_data(void)
{
	struct vb2_working_data *wd = vboot_get_working_data();
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}

int vb2_get_selected_region(struct region *region)
{
	const struct selected_region *reg = vb2_selected_region();

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
	struct selected_region *reg = vb2_selected_region();
	reg->offset = region_offset(region);
	reg->size = region_sz(region);
}

int vboot_is_slot_selected(void)
{
	const struct selected_region *reg = vb2_selected_region();
	return reg->size > 0;
}

int vboot_is_readonly_path(void)
{
	return !vboot_is_slot_selected();
}

void vb2_store_selected_region(void)
{
	const struct vb2_working_data *wd;
	struct selected_region *sel_reg;

	/* Always use the working data in this path since it's the object
	 * which has the result.. */
	wd = vboot_get_working_data();

	sel_reg = cbmem_add(CBMEM_ID_VBOOT_SEL_REG, sizeof(*sel_reg));

	sel_reg->offset = wd->selected_region.offset;
	sel_reg->size = wd->selected_region.size;
}

/*
 * For platforms that employ VBOOT_DYNAMIC_WORK_BUFFER, the vboot
 * verification doesn't happen until after cbmem is brought online.
 * Therefore, the selected region contents would not be initialized
 * so don't automatically add results when cbmem comes online.
 */
#if !IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER)
static void vb2_store_selected_region_cbmem(int unused)
{
	vb2_store_selected_region();
}
ROMSTAGE_CBMEM_INIT_HOOK(vb2_store_selected_region_cbmem)
#endif
