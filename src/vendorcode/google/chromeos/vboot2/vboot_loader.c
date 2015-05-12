/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
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
 * Foundation, Inc.
 */

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <rmodule.h>
#include <rules.h>
#include <string.h>
#include "misc.h"
#include "../symbols.h"

/* The stage loading code is compiled and entered from multiple stages. The
 * helper functions below attempt to provide more clarity on when certain
 * code should be called. */

static int verification_should_run(void)
{
	if (ENV_VERSTAGE && IS_ENABLED(CONFIG_SEPARATE_VERSTAGE))
		return 1;

	if (!IS_ENABLED(CONFIG_SEPARATE_VERSTAGE)) {
		if (ENV_ROMSTAGE &&
		    IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE))
			return 1;
		if (ENV_BOOTBLOCK &&
		    IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK))
			return 1;
	}

	return 0;
}

static int verstage_should_load(void)
{
	if (!IS_ENABLED(CONFIG_SEPARATE_VERSTAGE))
		return 0;

	if (ENV_ROMSTAGE && IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE))
		return 1;

	if (ENV_BOOTBLOCK && IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK))
		return 1;

	return 0;
}

static void init_vb2_working_data(void)
{
	struct vb2_working_data *wd;
	size_t work_size;

	work_size = vb2_working_data_size();
	wd = vboot_get_working_data();
	memset(wd, 0, work_size);
	/*
	 * vboot prefers 16-byte alignment. This takes away 16 bytes
	 * from the VBOOT2_WORK region, but the vboot devs said that's okay.
	 */
	wd->buffer_offset = ALIGN_UP(sizeof(*wd), 16);
	wd->buffer_size = work_size - wd->buffer_offset;
}

static int vboot_loader_active(struct prog *prog)
{
	struct vb2_working_data *wd;
	int run_verification;

	run_verification = verification_should_run();

	if (run_verification) {
		init_vb2_working_data();
		verstage_main();
	} else if (verstage_should_load()) {
		struct prog verstage = {
			.type = PROG_VERSTAGE,
			.name = CONFIG_CBFS_PREFIX "/verstage",
		};

		/* load verstage from RO */
		if (cbfs_load_prog_stage(CBFS_DEFAULT_MEDIA, &verstage))
			die("failed to load verstage");

		/* verify and select a slot */
		prog_run(&verstage);

		/* This is not actually possible to hit this condition at
		 * runtime, but this provides a hint to the compiler for dead
		 * code elimination below. */
		if (!IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
			return 0;
	}

	/* Fill in vboot handoff structure before moving to ramstage so all
	 * downstream users have access to vboot results. */
	if (ENV_ROMSTAGE)
		vboot_fill_handoff();

	wd = vboot_get_working_data();

	if (vboot_is_slot_selected(wd)) {
		if (IS_ENABLED(CONFIG_MULTIPLE_CBFS_INSTANCES) &&
		    run_verification) {
			/* RW A or B */
			struct vboot_region fw_main;

			vb2_get_selected_region(wd, &fw_main);
			cbfs_set_header_offset(fw_main.offset_addr);
		}
		return 1;
	}

	return 0;
}

static uintptr_t vboot_fw_region(int fw_index, struct vboot_region *fw_main,
				struct vboot_components *fw_info, size_t *size)
{
	uintptr_t fc_addr;
	uint32_t fc_size;

	if (fw_index >= fw_info->num_components) {
		printk(BIOS_INFO, "invalid stage index: %d\n", fw_index);
		return 0;
	}

	fc_addr = fw_main->offset_addr + fw_info->entries[fw_index].offset;
	fc_size = fw_info->entries[fw_index].size;
	if (fc_size == 0 ||
	    fc_addr + fc_size > fw_main->offset_addr + fw_main->size) {
		printk(BIOS_INFO, "invalid stage address or size\n");
		return 0;
	}

	*size = fc_size;
	return fc_addr;
}

/* This function is only called when vboot_loader_active() returns 1. That
 * means we are taking vboot paths. */
static int vboot_prepare(struct prog *prog)
{
	struct vb2_working_data *wd;
	struct vboot_region fw_main;
	struct vboot_components *fw_info;

	/* Code size optimization. We'd never actually get called under the
	 * followin cirumstances because verstage was loaded and ran -- never
	 * returning. */
	if (verstage_should_load() && !IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
		return 0;

	/* In the multi cbfs case the cbfs offset pointer has already been
	 * updated after firmware verification. */
	if (IS_ENABLED(CONFIG_MULTIPLE_CBFS_INSTANCES)) {
		if (!ENV_RAMSTAGE &&
		    cbfs_load_prog_stage(CBFS_DEFAULT_MEDIA, prog) != 0)
			return -1;

		/* Need to load payload. */
		if (ENV_RAMSTAGE) {
			void *payload;
			size_t size;

			payload = cbfs_get_file_content(CBFS_DEFAULT_MEDIA,
							prog->name,
							CBFS_TYPE_PAYLOAD,
							&size);

			if (payload == NULL)
				die("Couldn't load payload\n");

			prog_set_area(prog, payload, size);
		}
		return 0;
	}

	wd = vboot_get_working_data();
	vb2_get_selected_region(wd, &fw_main);
	fw_info = vboot_locate_components(&fw_main);
	if (fw_info == NULL)
		die("failed to locate firmware components\n");

	/* Load payload in ramstage. */
	if (ENV_RAMSTAGE) {
		uintptr_t payload;
		void *payload_ptr;
		size_t size;

		payload = vboot_fw_region(CONFIG_VBOOT_BOOT_LOADER_INDEX,
						&fw_main, fw_info, &size);

		if (payload == 0)
			die("Couldn't load payload.");

		payload_ptr = vboot_get_region(payload, size, NULL);

		if (payload_ptr == NULL)
			die("Couldn't load payload.");

		prog_set_area(prog, payload_ptr, size);
	} else {
		uintptr_t stage;
		size_t size;
		int stage_index = 0;

		if (prog->type == PROG_ROMSTAGE)
			stage_index = CONFIG_VBOOT_ROMSTAGE_INDEX;
		else if (prog->type == PROG_RAMSTAGE)
			stage_index = CONFIG_VBOOT_RAMSTAGE_INDEX;
		else
			die("Invalid program type for vboot.");

		stage = vboot_fw_region(stage_index, &fw_main, fw_info, &size);

		if (stage == 0)
			die("Vboot stage load failed.");

		if (ENV_ROMSTAGE && IS_ENABLED(CONFIG_RELOCATABLE_RAMSTAGE)) {
			struct rmod_stage_load rmod_ram = {
				.cbmem_id = CBMEM_ID_RAMSTAGE,
				.prog = prog,
			};

			if (rmodule_stage_load(&rmod_ram, (void *)stage))
				die("Vboot couldn't load stage");
		} else if (cbfs_load_prog_stage_by_offset(CBFS_DEFAULT_MEDIA,
							prog, stage) < 0)
			die("Vboot couldn't load stage");
	}

	return 0;
}

const struct prog_loader_ops vboot_loader = {
	.name = "VBOOT",
	.is_loader_active = vboot_loader_active,
	.prepare = vboot_prepare,
};
