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

#ifndef __CHROMEOS_VBOOT2_MISC_H__
#define __CHROMEOS_VBOOT2_MISC_H__

#include "../vboot_common.h"

void *vboot2_verify_firmware(void);
void *vboot2_load_ramstage(void);
void verstage_main(void);
void *vboot_load_stage(int stage_index,
		       struct vboot_region *fw_main,
		       struct vboot_components *fw_info);
void vboot_reboot(void);

/*
 * this is placed at the start of the vboot work buffer. selected_region is used
 * for the verstage to return the location of the selected slot. buffer is used
 * by the vboot2 core. Keep the struct cpu architecture agnostic as it crosses
 * stage boundaries.
 */
struct vb2_working_data {
	uint32_t selected_region_offset;
	uint32_t selected_region_size;
	/* offset of the buffer from the start of this struct */
	uint32_t buffer_offset;
	uint32_t buffer_size;
};

struct vb2_working_data * const vboot_get_working_data(void);
void *vboot_get_work_buffer(struct vb2_working_data *wd);

static inline void vb2_get_selected_region(struct vb2_working_data *wd,
					   struct vboot_region *region)
{
	region->offset_addr = wd->selected_region_offset;
	region->size = wd->selected_region_size;
}

static inline void vb2_set_selected_region(struct vb2_working_data *wd,
					   struct vboot_region *region)
{
	wd->selected_region_offset = region->offset_addr;
	wd->selected_region_size = region->size;
}

static inline int vboot_is_slot_selected(struct vb2_working_data *wd)
{
	return wd->selected_region_size > 0;
}

static inline int vboot_is_readonly_path(struct vb2_working_data *wd)
{
	return wd->selected_region_size == 0;
}

#endif /* __CHROMEOS_VBOOT2_MISC_H__ */
