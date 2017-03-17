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
#include <cbmem.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <rmodule.h>
#include <rules.h>
#include <string.h>
#include <vboot/misc.h>
#include <vboot/symbols.h>
#include <vboot/vboot_common.h>

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

static int vboot_executed CAR_GLOBAL;

int vb2_logic_executed(void)
{
	/* If this stage is supposed to run the vboot logic ensure it has been
	 * executed. */
	if (verification_should_run() && car_get_var(vboot_executed))
		return 1;

	/* If this stage is supposed to load verstage and verstage is returning
	 * back to the calling stage check that it has been executed. */
	if (verstage_should_load() && IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
		if (car_get_var(vboot_executed))
			return 1;

	/* Handle all other stages post vboot execution. */
	if (!ENV_BOOTBLOCK) {
		if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK))
			return 1;
		if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE) &&
				!ENV_ROMSTAGE)
			return 1;
	}

	return 0;
}

static void vboot_prepare(void)
{
	if (verification_should_run()) {
		/*
		 * Note that this path isn't taken when
		 * CONFIG_RETURN_FROM_VERSTAGE is employed.
		 */
		verstage_main();
		car_set_var(vboot_executed, 1);
		vb2_save_recovery_reason_vbnv();

		/*
		 * Avoid double memory retrain when the EC is running RW code
		 * and a recovery request came in through an EC host event. The
		 * double retrain happens because the EC won't be rebooted
		 * until kernel verification notices the EC isn't running RO
		 * code which is after memory training. Therefore, reboot the
		 * EC after we've saved the potential recovery request so it's
		 * not lost. Lastly, only perform this sequence on x86
		 * platforms since those are the ones that currently do a
		 * costly memory training in recovery mode.
		 */
		if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC) &&
			IS_ENABLED(CONFIG_ARCH_X86))
			google_chromeec_early_init();

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
		if (!IS_ENABLED(CONFIG_RETURN_FROM_VERSTAGE))
			return;

		car_set_var(vboot_executed, 1);
	}

	/*
	 * Fill in vboot cbmem objects before moving to ramstage so all
	 * downstream users have access to vboot results. This path only
	 * applies to platforms employing VBOOT_STARTS_IN_ROMSTAGE because
	 * cbmem comes online prior to vboot verification taking place. For
	 * other platforms the vboot cbmem objects are initialized when
	 * cbmem comes online.
	 */
	if (ENV_ROMSTAGE && IS_ENABLED(CONFIG_VBOOT_STARTS_IN_ROMSTAGE)) {
		vb2_store_selected_region();
		vboot_fill_handoff();
	}
}

static int vboot_locate(struct cbfs_props *props)
{
	struct region selected_region;

	/* Don't honor vboot results until the vboot logic has run. */
	if (!vb2_logic_executed())
		return -1;

	if (vb2_get_selected_region(&selected_region))
		return -1;

	props->offset = region_offset(&selected_region);
	props->size = region_sz(&selected_region);

	return 0;
}

const struct cbfs_locator vboot_locator = {
	.name = "VBOOT",
	.prepare = vboot_prepare,
	.locate = vboot_locate,
};
