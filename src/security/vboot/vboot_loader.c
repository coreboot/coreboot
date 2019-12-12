/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/bsd/cbfs_private.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <rmodule.h>
#include <security/vboot/misc.h>
#include <security/vboot/symbols.h>
#include <security/vboot/vboot_common.h>

/* Ensure vboot configuration is valid: */
_Static_assert(CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) +
	       CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) +
	       CONFIG(VBOOT_STARTS_IN_ROMSTAGE) == 1,
	       "vboot must start in bootblock, PSP or romstage (but only one!)");
_Static_assert(!CONFIG(VBOOT_SEPARATE_VERSTAGE) || CONFIG(VBOOT_STARTS_IN_BOOTBLOCK) ||
	       CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK),
	       "stand-alone verstage must start in or before bootblock ");
_Static_assert(!CONFIG(VBOOT_RETURN_FROM_VERSTAGE) ||
	       CONFIG(VBOOT_SEPARATE_VERSTAGE),
	       "return from verstage only makes sense for separate verstages");

int vboot_executed;

static void build_rw_mcache(void)
{
	if (CONFIG(NO_CBFS_MCACHE))
		return;

	const struct cbfs_boot_device *cbd = vboot_get_cbfs_boot_device();
	if (!cbd)	/* Don't build RW mcache in recovery mode. */
		return;
	cb_err_t err = cbfs_mcache_build(&cbd->rdev, cbd->mcache,
					 cbd->mcache_size, NULL);
	if (err && err != CB_CBFS_CACHE_FULL)
		die("Failed to build RW mcache.");	/* TODO: -> recovery? */
}

void vboot_run_logic(void)
{
	if (verification_should_run()) {
		/* Note: this path is not used for VBOOT_RETURN_FROM_VERSTAGE */
		verstage_main();
		vboot_executed = 1;
		build_rw_mcache();
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

		vboot_executed = 1;
		build_rw_mcache();
	}
}

const struct cbfs_boot_device *vboot_get_cbfs_boot_device(void)
{
	/* Don't honor vboot results until the vboot logic has run. */
	if (!vboot_logic_executed())
		return NULL;

	static struct cbfs_boot_device cbd;
	if (region_device_sz(&cbd.rdev))
		return &cbd;

	struct vb2_context *ctx = vboot_get_context();
	if (ctx->flags & VB2_CONTEXT_RECOVERY_MODE)
		return NULL;

	boot_device_init();
	if (vboot_locate_firmware(ctx, &cbd.rdev))
		return NULL;

	cbfs_boot_device_find_mcache(&cbd, CBMEM_ID_CBFS_RW_MCACHE);

	return &cbd;
}
