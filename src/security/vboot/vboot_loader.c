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
 */

#include <arch/early_variables.h>
#include <cbfs.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <rmodule.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

/* Ensure vboot configuration is valid: */
_Static_assert(CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) +
	       CONFIG(VBOOT_STARTS_IN_ROMSTAGE) == 1,
	       "vboot must either start in bootblock or romstage (not both!)");
_Static_assert(!CONFIG(VBOOT_SEPARATE_VERSTAGE) ||
	       CONFIG(VBOOT_STARTS_IN_BOOTBLOCK),
	       "stand-alone verstage must start in (i.e. after) bootblock");
_Static_assert(!CONFIG(VBOOT_RETURN_FROM_VERSTAGE) ||
	       CONFIG(VBOOT_SEPARATE_VERSTAGE),
	       "return from verstage only makes sense for separate verstages");

int vboot_executed CAR_GLOBAL;

void vboot_run_logic(void)
{
	if (verification_should_run()) {
		/* Note: this path is not used for VBOOT_RETURN_FROM_VERSTAGE */
		verstage_main();
		car_set_var(vboot_executed, 1);
	} else if (verstage_should_load()) {
		struct cbfsf file;
		struct prog verstage =
			PROG_INIT(PROG_VERSTAGE,
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
		if (!CONFIG(VBOOT_RETURN_FROM_VERSTAGE))
			return;

		car_set_var(vboot_executed, 1);
	}
}

static int vboot_locate(struct cbfs_props *props)
{
	struct region selected_region;

	/* Don't honor vboot results until the vboot logic has run. */
	if (!vboot_logic_executed())
		return -1;

	if (vboot_get_selected_region(&selected_region))
		return -1;

	props->offset = region_offset(&selected_region);
	props->size = region_sz(&selected_region);

	return 0;
}

const struct cbfs_locator vboot_locator = {
	.name = "VBOOT",
	.locate = vboot_locate,
};
