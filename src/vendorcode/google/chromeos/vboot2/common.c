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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cbfs.h>
#include <console/console.h>
#include <reset.h>
#include "../chromeos.h"
#include "../symbols.h"
#include "../vboot_handoff.h"
#include "misc.h"

void *vboot_load_stage(int stage_index,
		       struct vboot_region *fw_main,
		       struct vboot_components *fw_info)
{
	struct cbfs_media default_media, *media = &default_media;
	uintptr_t fc_addr;
	uint32_t fc_size;
	void *entry;

	if (stage_index >= fw_info->num_components) {
		printk(BIOS_INFO, "invalid stage index\n");
		return NULL;
	}

	fc_addr = fw_main->offset_addr + fw_info->entries[stage_index].offset;
	fc_size = fw_info->entries[stage_index].size;
	if (fc_size == 0 ||
	    fc_addr + fc_size > fw_main->offset_addr + fw_main->size) {
		printk(BIOS_INFO, "invalid stage address or size\n");
		return NULL;
	}

	init_default_cbfs_media(media);

	/* we're making cbfs access offset outside of the region managed by
	 * cbfs. this works because cbfs_load_stage_by_offset does not check
	 * the offset. */
	entry = cbfs_load_stage_by_offset(media, fc_addr);
	if (entry == (void *)-1)
		entry = NULL;
	return entry;
}

struct vb2_working_data * const vboot_get_working_data(void)
{
	return (struct vb2_working_data *)_vboot2_work;
}

void *vboot_get_work_buffer(struct vb2_working_data *wd)
{
	return (void *)((uintptr_t)wd + wd->buffer_offset);
}

void vboot_reboot(void)
{
	hard_reset();
	die("failed to reboot");
}
