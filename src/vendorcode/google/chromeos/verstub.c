/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/stages.h>
#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include "chromeos.h"

static struct vb2_working_data *init_vb2_working_data(void)
{
	struct vb2_working_data *wd;

	wd = vboot_get_working_data();
	memset(wd, 0, CONFIG_VBOOT_WORK_BUFFER_SIZE);
	/* 8-byte alignment for ARMv7 */
	wd->buffer = (uint8_t *)ALIGN_UP((uintptr_t)&wd[1], 8);
	wd->buffer_size = CONFIG_VBOOT_WORK_BUFFER_SIZE + (uintptr_t)wd
			- (uintptr_t)wd->buffer;

	return wd;
}

/**
 * Verify a slot and jump to the next stage
 *
 * This could be either part of the (1) bootblock or the (2) verstage, depending
 * on CONFIG_RETURN_FROM_VERSTAGE.
 *
 * 1) It jumps to the verstage and comes back, then, loads the romstage over the
 * verstage space and exits to it. (note the cbfs cache is trashed on return
 * from the verstage.)
 *
 * 2) We're already in the verstage. Verify firmware, then load the romstage and
 * exits to it.
 */
void vboot2_verify_firmware(void)
{
	void *entry;
	struct vb2_working_data *wd;

	wd = init_vb2_working_data();

#if CONFIG_RETURN_FROM_VERSTAGE
	/* load verstage from RO */
	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				CONFIG_CBFS_PREFIX "/verstage");
	if (entry == -1)
		die("failed to load verstage");

	/* verify and select a slot */
	stage_exit(entry);
#else
	verstage_main();
#endif /* CONFIG_RETURN_FROM_VERSTAGE */

	/* jump to the selected slot */
	entry = NULL;
	if (vboot_is_slot_selected(wd)) {
		/* RW A or B */
		struct vboot_components *fw_info =
				vboot_locate_components(&wd->selected_region);
		if (fw_info == NULL)
			die("failed to locate firmware components\n");
		entry = vboot_load_stage(CONFIG_VBOOT_ROMSTAGE_INDEX,
					 &wd->selected_region, fw_info);
	} else if (vboot_is_readonly_path(wd)) {
		/* RO */
		entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
					CONFIG_CBFS_PREFIX "/romstage");
	}

	if (entry != NULL && entry != (void *)-1)
		stage_exit(entry);

	die("failed to exit from stage\n");
}
