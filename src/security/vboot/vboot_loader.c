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
#include <timestamp.h>

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

static void after_verstage(void)
{
	struct vb2_hash *metadata_hash = NULL;
	struct vb2_context *ctx = NULL;

	if (CONFIG(VBOOT_CBFS_INTEGRATION)) {
		ctx = vboot_get_context();
		vb2_error_t rv = vb2api_get_metadata_hash(ctx, &metadata_hash);
		if (rv)
			vboot_fail_and_reboot(ctx, VB2_RECOVERY_FW_PREAMBLE, rv);
	}

	vboot_executed = 1;	/* Mark verstage execution complete. */

	const struct cbfs_boot_device *cbd = vboot_get_cbfs_boot_device();
	if (!cbd)	/* Can't initialize RW CBFS in recovery mode. */
		return;

	enum cb_err err = cbfs_init_boot_device(cbd, metadata_hash);
	if (err && err != CB_CBFS_CACHE_FULL) {
		if (CONFIG(VBOOT_CBFS_INTEGRATION)) {
			printk(BIOS_ERR, "RW CBFS initialization failed: %d\n", err);
			/* CBFS error code does not fit in subcode. Use only lowest byte. */
			vboot_fail_and_reboot(ctx, VB2_RECOVERY_FW_BODY, err & 0xFF);
		} else {
			die("RW CBFS initialization failure: %d", err);
		}
	}
}

void vboot_run_logic(void)
{
	if (verification_should_run()) {
		/* Note: this path is not used for VBOOT_RETURN_FROM_VERSTAGE */
		verstage_main();
		after_verstage();
	} else if (verstage_should_load()) {
		struct prog verstage =
			PROG_INIT(PROG_VERSTAGE,
				CONFIG_CBFS_PREFIX "/verstage");

		printk(BIOS_DEBUG, "VBOOT: Loading verstage.\n");

		timestamp_add_now(TS_COPYVER_START);
		if (cbfs_prog_stage_load(&verstage))
			die("failed to load verstage");
		timestamp_add_now(TS_COPYVER_END);

		/* verify and select a slot */
		prog_run(&verstage);

		/* This is not actually possible to hit this condition at
		 * runtime, but this provides a hint to the compiler for dead
		 * code elimination below. */
		if (!CONFIG(VBOOT_RETURN_FROM_VERSTAGE))
			return;

		after_verstage();
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
