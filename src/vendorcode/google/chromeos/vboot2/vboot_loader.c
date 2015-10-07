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

#include <assets.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <rmodule.h>
#include <rules.h>
#include <string.h>
#include "misc.h"
#include "../vboot_handoff.h"
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

static int vboot_active(struct asset *asset)
{
	int run_verification;

	run_verification = verification_should_run();

	if (run_verification) {
		verstage_main();
	} else if (verstage_should_load()) {
		struct cbfsf file;
		struct prog verstage =
			PROG_INIT(ASSET_VERSTAGE,
				CONFIG_CBFS_PREFIX "/verstage");

		printk(BIOS_DEBUG, "VBOOT: Loading verstage.\n");

		/* load verstage from RO */
		if (cbfs_boot_locate(&file, prog_name(&verstage), NULL))
			die("failed to load verstage");

		cbfs_file_data(prog_rdev(&verstage), &file);

		if (cbfs_prog_stage_load(&verstage))
			die("failed to load verstage");

		/* verify and select a slot */
		prog_run(&verstage);

		/* This is not actually possible to hit this condition at
		 * runtime, but this provides a hint to the compiler for dead
		 * code elimination below. */
		if (!IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
			return 0;
	}

	/*
	 * Fill in vboot cbmem objects before moving to ramstage so all
	 * downstream users have access to vboot results. This path only
	 * applies to platforms employing VBOOT_DYNAMIC_WORK_BUFFER because
	 * cbmem comes online prior to vboot verification taking place. For
	 * other platforms the vboot cbmem objects are initialized when
	 * cbmem comes online.
	 */
	if (ENV_ROMSTAGE && IS_ENABLED(CONFIG_VBOOT_DYNAMIC_WORK_BUFFER)) {
		vb2_store_selected_region();
		vboot_fill_handoff();
	}

	return vboot_is_slot_selected();
}

static int vboot_locate_by_components(const struct region_device *fw_main,
					struct asset *asset)
{
	struct vboot_components *fw_info;
	size_t metadata_sz;
	size_t offset;
	size_t size;
	struct region_device *fw = asset_rdev(asset);
	int fw_index = 0;

	if (asset_type(asset) == ASSET_ROMSTAGE)
		fw_index = CONFIG_VBOOT_ROMSTAGE_INDEX;
	else if (asset_type(asset) == ASSET_RAMSTAGE)
		fw_index = CONFIG_VBOOT_RAMSTAGE_INDEX;
	else if (asset_type(asset) == ASSET_PAYLOAD)
		fw_index = CONFIG_VBOOT_BOOT_LOADER_INDEX;
	else if (asset_type(asset) == ASSET_REFCODE)
		fw_index = CONFIG_VBOOT_REFCODE_INDEX;
	else if (asset_type(asset) == ASSET_BL31)
		fw_index = CONFIG_VBOOT_BL31_INDEX;
	else
		die("Invalid program type for vboot.");

	metadata_sz = sizeof(*fw_info);
	metadata_sz += MAX_PARSED_FW_COMPONENTS * sizeof(fw_info->entries[0]);

	fw_info = rdev_mmap(fw_main, 0, metadata_sz);

	if (fw_info == NULL) {
		printk(BIOS_INFO, "No component metadata.\n");
		return -1;
	}

	if (fw_index >= fw_info->num_components) {
		printk(BIOS_INFO, "invalid index: %d\n", fw_index);
		rdev_munmap(fw_main, fw_info);
		return -1;
	}

	offset = fw_info->entries[fw_index].offset;
	size = fw_info->entries[fw_index].size;
	rdev_munmap(fw_main, fw_info);

	if (rdev_chain(fw, fw_main, offset, size)) {
		printk(BIOS_INFO, "invalid offset or size\n");
		return -1;
	}

	return 0;
}

static int vboot_locate_by_multi_cbfs(const struct region_device *fw_main,
					struct asset *asset)
{
	struct cbfsf file;

	if (cbfs_locate(&file, fw_main, asset_name(asset), NULL))
		return -1;

	cbfs_file_data(asset_rdev(asset), &file);

	return 0;
}

static int vboot_asset_locate(const struct region_device *fw_main,
				struct asset *asset)
{
	if (IS_ENABLED(CONFIG_MULTIPLE_CBFS_INSTANCES))
		return vboot_locate_by_multi_cbfs(fw_main, asset);
	else
		return vboot_locate_by_components(fw_main, asset);
}

/* This function is only called when vboot_active() returns 1. That
 * means we are taking vboot paths. */
static int vboot_locate(struct asset *asset)
{
	struct region_device fw_main;

	/* Code size optimization. We'd never actually get called under the
	 * followin cirumstances because verstage was loaded and ran -- never
	 * returning. */
	if (verstage_should_load() && !IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
		return 0;

	if (vb2_get_selected_region(&fw_main))
		die("failed to reference selected region\n");

	return vboot_asset_locate(&fw_main, asset);
}

const struct asset_provider vboot_provider = {
	.name = "VBOOT",
	.is_active = vboot_active,
	.locate = vboot_locate,
};
