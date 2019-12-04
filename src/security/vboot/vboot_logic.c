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
 */

#include <arch/exception.h>
#include <assert.h>
#include <bootmode.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/vtxprintf.h>
#include <fmap.h>
#include <string.h>
#include <timestamp.h>
#include <vb2_api.h>
#include <security/vboot/misc.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_crtm.h>
#include <security/vboot/tpm_common.h>

#include "antirollback.h"

/* The max hash size to expect is for SHA512. */
#define VBOOT_MAX_HASH_SIZE VB2_SHA512_DIGEST_SIZE

#define TODO_BLOCK_SIZE 1024

/* exports */

void vb2ex_printf(const char *func, const char *fmt, ...)
{
	va_list args;

	if (func)
		printk(BIOS_INFO, "VB2:%s() ", func);

	va_start(args, fmt);
	vprintk(BIOS_INFO, fmt, args);
	va_end(args);

	return;
}

vb2_error_t vb2ex_read_resource(struct vb2_context *ctx,
				enum vb2_resource_index index,
				uint32_t offset,
				void *buf,
				uint32_t size)
{
	struct region_device rdev;
	const char *name;

	switch (index) {
	case VB2_RES_GBB:
		name = "GBB";
		break;
	case VB2_RES_FW_VBLOCK:
		if (vboot_is_firmware_slot_a(ctx))
			name = "VBLOCK_A";
		else
			name = "VBLOCK_B";
		break;
	default:
		return VB2_ERROR_EX_READ_RESOURCE_INDEX;
	}

	if (fmap_locate_area_as_rdev(name, &rdev))
		return VB2_ERROR_EX_READ_RESOURCE_SIZE;

	if (rdev_readat(&rdev, buf, offset, size) != size)
		return VB2_ERROR_EX_READ_RESOURCE_SIZE;

	return VB2_SUCCESS;
}

void vb2ex_abort(void)
{
	die("vboot has aborted execution; exit\n");
}

/* No-op stubs that can be overridden by SoCs with hardware crypto support. */
__weak vb2_error_t vb2ex_hwcrypto_digest_init(enum vb2_hash_algorithm hash_alg,
					      uint32_t data_size)
{
	return VB2_ERROR_EX_HWCRYPTO_UNSUPPORTED;
}

__weak vb2_error_t vb2ex_hwcrypto_digest_extend(const uint8_t *buf,
						uint32_t size)
{
	BUG(); /* Should never get called if init() returned an error. */
	return VB2_ERROR_UNKNOWN;
}

__weak vb2_error_t vb2ex_hwcrypto_digest_finalize(uint8_t *digest,
						  uint32_t digest_size)
{
	BUG(); /* Should never get called if init() returned an error. */
	return VB2_ERROR_UNKNOWN;
}

static int handle_digest_result(void *slot_hash, size_t slot_hash_sz)
{
	int is_resume;

	/*
	 * Chrome EC is the only support for vboot_save_hash() &
	 * vboot_retrieve_hash(), if Chrome EC is not enabled then return.
	 */
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return 0;

	/*
	 * Nothing to do since resuming on the platform doesn't require
	 * vboot verification again.
	 */
	if (!CONFIG(RESUME_PATH_SAME_AS_BOOT))
		return 0;

	/*
	 * Assume that if vboot doesn't start in bootblock verified
	 * RW memory init code is not employed. i.e. memory init code
	 * lives in RO CBFS.
	 */
	if (!CONFIG(VBOOT_STARTS_IN_BOOTBLOCK))
		return 0;

	is_resume = vboot_platform_is_resuming();

	if (is_resume > 0) {
		uint8_t saved_hash[VBOOT_MAX_HASH_SIZE];
		const size_t saved_hash_sz = sizeof(saved_hash);

		assert(slot_hash_sz == saved_hash_sz);

		printk(BIOS_DEBUG, "Platform is resuming.\n");

		if (vboot_retrieve_hash(saved_hash, saved_hash_sz)) {
			printk(BIOS_ERR, "Couldn't retrieve saved hash.\n");
			return -1;
		}

		if (memcmp(saved_hash, slot_hash, slot_hash_sz)) {
			printk(BIOS_ERR, "Hash mismatch on resume.\n");
			return -1;
		}
	} else if (is_resume < 0)
		printk(BIOS_ERR, "Unable to determine if platform resuming.\n");

	printk(BIOS_DEBUG, "Saving vboot hash.\n");

	/* Always save the hash for the current boot. */
	if (vboot_save_hash(slot_hash, slot_hash_sz)) {
		printk(BIOS_ERR, "Error saving vboot hash.\n");
		/* Though this is an error don't report it up since it could
		 * lead to a reboot loop. The consequence of this is that
		 * we will most likely fail resuming because of EC issues or
		 * the hash digest not matching. */
		return 0;
	}

	return 0;
}

static vb2_error_t hash_body(struct vb2_context *ctx,
			     struct region_device *fw_main)
{
	uint64_t load_ts;
	uint32_t expected_size;
	uint8_t block[TODO_BLOCK_SIZE];
	uint8_t hash_digest[VBOOT_MAX_HASH_SIZE];
	const size_t hash_digest_sz = sizeof(hash_digest);
	size_t block_size = sizeof(block);
	size_t offset;
	vb2_error_t rv;

	/* Clear the full digest so that any hash digests less than the
	 * max have trailing zeros. */
	memset(hash_digest, 0, hash_digest_sz);

	/*
	 * Since loading the firmware and calculating its hash is intertwined,
	 * we use this little trick to measure them separately and pretend it
	 * was first loaded and then hashed in one piece with the timestamps.
	 * (This split won't make sense with memory-mapped media like on x86.)
	 */
	load_ts = timestamp_get();
	timestamp_add(TS_START_HASH_BODY, load_ts);

	expected_size = region_device_sz(fw_main);
	offset = 0;

	/* Start the body hash */
	rv = vb2api_init_hash(ctx, VB2_HASH_TAG_FW_BODY, &expected_size);
	if (rv)
		return rv;

	/*
	 * Honor vboot's RW slot size. The expected size is pulled out of
	 * the preamble and obtained through vb2api_init_hash() above. By
	 * creating sub region the RW slot portion of the boot media is
	 * limited.
	 */
	if (rdev_chain(fw_main, fw_main, 0, expected_size)) {
		printk(BIOS_ERR, "Unable to restrict CBFS size.\n");
		return VB2_ERROR_UNKNOWN;
	}

	/* Extend over the body */
	while (expected_size) {
		uint64_t temp_ts;
		if (block_size > expected_size)
			block_size = expected_size;

		temp_ts = timestamp_get();
		if (rdev_readat(fw_main, block, offset, block_size) < 0)
			return VB2_ERROR_UNKNOWN;
		load_ts += timestamp_get() - temp_ts;

		rv = vb2api_extend_hash(ctx, block, block_size);
		if (rv)
			return rv;

		expected_size -= block_size;
		offset += block_size;
	}

	timestamp_add(TS_DONE_LOADING, load_ts);
	timestamp_add_now(TS_DONE_HASHING);

	/* Check the result (with RSA signature verification) */
	rv = vb2api_check_hash_get_digest(ctx, hash_digest, hash_digest_sz);
	if (rv)
		return rv;

	timestamp_add_now(TS_END_HASH_BODY);

	if (handle_digest_result(hash_digest, hash_digest_sz))
		return VB2_ERROR_UNKNOWN;

	return VB2_SUCCESS;
}

void vboot_save_nvdata_only(struct vb2_context *ctx)
{
	assert(!(ctx->flags & (VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED |
			       VB2_CONTEXT_SECDATA_KERNEL_CHANGED)));

	if (ctx->flags & VB2_CONTEXT_NVDATA_CHANGED) {
		printk(BIOS_INFO, "Saving nvdata\n");
		save_vbnv(ctx->nvdata);
		ctx->flags &= ~VB2_CONTEXT_NVDATA_CHANGED;
	}
}

void vboot_save_data(struct vb2_context *ctx)
{
	if (ctx->flags & VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED) {
		printk(BIOS_INFO, "Saving secdata\n");
		antirollback_write_space_firmware(ctx);
		ctx->flags &= ~VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED;
	}

	vboot_save_nvdata_only(ctx);
}

static uint32_t extend_pcrs(struct vb2_context *ctx)
{
	return vboot_extend_pcr(ctx, 0, BOOT_MODE_PCR) ||
		   vboot_extend_pcr(ctx, 1, HWID_DIGEST_PCR);
}

static void vboot_log_and_clear_recovery_mode_switch(int unused)
{
	/* Log the recovery mode switches if required, before clearing them. */
	log_recovery_mode_switch();

	/*
	 * The recovery mode switch is cleared (typically backed by EC) here
	 * to allow multiple queries to get_recovery_mode_switch() and have
	 * them return consistent results during the verified boot path as well
	 * as dram initialization. x86 systems ignore the saved dram settings
	 * in the recovery path in order to start from a clean slate. Therefore
	 * clear the state here since this function is called when memory
	 * is known to be up.
	 */
	clear_recovery_mode_switch();
}
#if !CONFIG(VBOOT_STARTS_IN_ROMSTAGE)
ROMSTAGE_CBMEM_INIT_HOOK(vboot_log_and_clear_recovery_mode_switch)
#endif

/**
 * Verify and select the firmware in the RW image
 *
 * TODO: Avoid loading a stage twice (once in hash_body & again in load_stage).
 * when per-stage verification is ready.
 */
void verstage_main(void)
{
	struct vb2_context *ctx;
	struct region_device fw_main;
	vb2_error_t rv;

	timestamp_add_now(TS_START_VBOOT);

	/* Set up context and work buffer */
	ctx = vboot_get_context();

	/* Initialize and read nvdata from non-volatile storage. */
	vbnv_init(ctx->nvdata);

	/* Set S3 resume flag if vboot should behave differently when selecting
	 * which slot to boot.  This is only relevant to vboot if the platform
	 * does verification of memory init and thus must ensure it resumes with
	 * the same slot that it booted from. */
	if (CONFIG(RESUME_PATH_SAME_AS_BOOT) &&
		vboot_platform_is_resuming())
		ctx->flags |= VB2_CONTEXT_S3_RESUME;

	/* Read secdata from TPM. Initialize TPM if secdata not found. We don't
	 * check the return value here because vb2api_fw_phase1 will catch
	 * invalid secdata and tell us what to do (=reboot). */
	timestamp_add_now(TS_START_TPMINIT);
	if (vboot_setup_tpm(ctx) == TPM_SUCCESS)
		antirollback_read_space_firmware(ctx);
	timestamp_add_now(TS_END_TPMINIT);

	/* Enable measured boot mode */
	if (CONFIG(VBOOT_MEASURED_BOOT) &&
		!(ctx->flags & VB2_CONTEXT_S3_RESUME)) {
		if (vboot_init_crtm() != VB2_SUCCESS)
			die_with_post_code(POST_INVALID_ROM,
				"Initializing measured boot mode failed!");
	}

	if (get_recovery_mode_switch()) {
		ctx->flags |= VB2_CONTEXT_FORCE_RECOVERY_MODE;
		if (CONFIG(VBOOT_DISABLE_DEV_ON_RECOVERY))
			ctx->flags |= VB2_CONTEXT_DISABLE_DEVELOPER_MODE;
	}

	if (CONFIG(VBOOT_WIPEOUT_SUPPORTED) &&
		get_wipeout_mode_switch())
		ctx->flags |= VB2_CONTEXT_FORCE_WIPEOUT_MODE;

	if (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())
		ctx->flags |= VB2_CONTEXT_NOFAIL_BOOT;

	/* Mainboard/SoC always initializes display. */
	if (!CONFIG(VBOOT_MUST_REQUEST_DISPLAY) || CONFIG(VBOOT_ALWAYS_ENABLE_DISPLAY))
		ctx->flags |= VB2_CONTEXT_DISPLAY_INIT;

	/* Do early init (set up secdata and NVRAM, load GBB) */
	printk(BIOS_INFO, "Phase 1\n");
	rv = vb2api_fw_phase1(ctx);

	if (rv) {
		/*
		 * If vb2api_fw_phase1 fails, check for return value.
		 * If it is set to VB2_ERROR_API_PHASE1_RECOVERY, then continue
		 * into recovery mode.
		 * For any other error code, save context if needed and reboot.
		 */
		if (rv == VB2_ERROR_API_PHASE1_RECOVERY) {
			printk(BIOS_INFO, "Recovery requested (%x)\n", rv);
			vboot_save_data(ctx);
			extend_pcrs(ctx); /* ignore failures */
			goto verstage_main_exit;
		}

		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		vboot_save_data(ctx);
		vboot_reboot();
	}

	/* Determine which firmware slot to boot (based on NVRAM) */
	printk(BIOS_INFO, "Phase 2\n");
	rv = vb2api_fw_phase2(ctx);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		vboot_save_data(ctx);
		vboot_reboot();
	}

	/* Try that slot (verify its keyblock and preamble) */
	printk(BIOS_INFO, "Phase 3\n");
	timestamp_add_now(TS_START_VERIFY_SLOT);
	rv = vb2api_fw_phase3(ctx);
	timestamp_add_now(TS_END_VERIFY_SLOT);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		vboot_save_data(ctx);
		vboot_reboot();
	}

	printk(BIOS_INFO, "Phase 4\n");
	rv = vboot_locate_firmware(ctx, &fw_main);
	if (rv)
		die_with_post_code(POST_INVALID_ROM,
			"Failed to read FMAP to locate firmware");

	rv = hash_body(ctx, &fw_main);
	vboot_save_data(ctx);
	if (rv) {
		printk(BIOS_INFO, "Reboot requested (%x)\n", rv);
		vboot_reboot();
	}

	/* Only extend PCRs once on boot. */
	if (!(ctx->flags & VB2_CONTEXT_S3_RESUME)) {
		timestamp_add_now(TS_START_TPMPCR);
		rv = extend_pcrs(ctx);
		if (rv) {
			printk(BIOS_WARNING,
			       "Failed to extend TPM PCRs (%#x)\n", rv);
			vb2api_fail(ctx, VB2_RECOVERY_RO_TPM_U_ERROR, rv);
			vboot_save_data(ctx);
			vboot_reboot();
		}
		timestamp_add_now(TS_END_TPMPCR);
	}

	/* Lock TPM */

	timestamp_add_now(TS_START_TPMLOCK);
	rv = antirollback_lock_space_firmware();
	if (rv) {
		printk(BIOS_INFO, "Failed to lock TPM (%x)\n", rv);
		vb2api_fail(ctx, VB2_RECOVERY_RO_TPM_L_ERROR, 0);
		vboot_save_data(ctx);
		vboot_reboot();
	}
	timestamp_add_now(TS_END_TPMLOCK);

	/* Lock rec hash space if available. */
	if (CONFIG(VBOOT_HAS_REC_HASH_SPACE)) {
		rv = antirollback_lock_space_rec_hash();
		if (rv) {
			printk(BIOS_INFO, "Failed to lock rec hash space(%x)\n",
			       rv);
			vb2api_fail(ctx, VB2_RECOVERY_RO_TPM_REC_HASH_L_ERROR,
				    0);
			vboot_save_data(ctx);
			vboot_reboot();
		}
	}

	printk(BIOS_INFO, "Slot %c is selected\n",
	       vboot_is_firmware_slot_a(ctx) ? 'A' : 'B');

 verstage_main_exit:
	/* If CBMEM is not up yet, let the ROMSTAGE_CBMEM_INIT_HOOK take care
	   of running this function. */
	if (ENV_ROMSTAGE && CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		vboot_log_and_clear_recovery_mode_switch(0);

	/* Save recovery reason in case of unexpected reboots on x86. */
	vboot_save_recovery_reason_vbnv();

	timestamp_add_now(TS_END_VBOOT);
}
