/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <antirollback.h>
#include <stdlib.h>
#include <string.h>
#include <tpm_lite/tlcl.h>
#include <vb2_api.h>
#include <console/console.h>

#ifndef offsetof
#define offsetof(A,B) __builtin_offsetof(A,B)
#endif

#ifdef FOR_TEST
#include <stdio.h>
#define VBDEBUG(format, args...) printf(format, ## args)
#else
#include <console/console.h>
#define VBDEBUG(format, args...) \
	printk(BIOS_INFO, "%s():%d: " format,  __func__, __LINE__, ## args)
#endif

#define RETURN_ON_FAILURE(tpm_cmd) do {				\
		uint32_t result_;					\
		if ((result_ = (tpm_cmd)) != TPM_SUCCESS) {		\
			VBDEBUG("Antirollback: %08x returned by " #tpm_cmd \
				 "\n", (int)result_);			\
			return result_;					\
		}							\
	} while (0)


static uint32_t safe_write(uint32_t index, const void *data, uint32_t length);

uint32_t tpm_extend_pcr(struct vb2_context *ctx, int pcr,
			enum vb2_pcr_digest which_digest)
{
	uint8_t buffer[VB2_PCR_DIGEST_RECOMMENDED_SIZE];
	uint32_t size = sizeof(buffer);
	int rv;

	rv = vb2api_get_pcr_digest(ctx, which_digest, buffer, &size);
	if (rv != VB2_SUCCESS)
		return rv;
	if (size < TPM_PCR_DIGEST)
		return VB2_ERROR_UNKNOWN;

	return tlcl_extend(pcr, buffer, NULL);
}

static uint32_t read_space_firmware(struct vb2_context *ctx)
{
	int attempts = 3;

	while (attempts--) {
		RETURN_ON_FAILURE(tlcl_read(FIRMWARE_NV_INDEX, ctx->secdata,
		                            VB2_SECDATA_SIZE));

		if (vb2api_secdata_check(ctx) == VB2_SUCCESS)
			return TPM_SUCCESS;

		VBDEBUG("TPM: %s() - bad CRC\n", __func__);
	}

	VBDEBUG("TPM: %s() - too many bad CRCs, giving up\n", __func__);
	return TPM_E_CORRUPTED_STATE;
}

static uint32_t write_secdata(uint32_t index,
			      const uint8_t *secdata,
			      uint32_t len)
{
	uint8_t sd[32];
	uint32_t rv;
	int attempts = 3;

	if (len > sizeof(sd)) {
		VBDEBUG("TPM: %s() - data is too large\n", __func__);
		return TPM_E_WRITE_FAILURE;
	}

	while (attempts--) {
		rv = safe_write(index, secdata, len);
		/* Can't write, not gonna try again */
		if (rv != TPM_SUCCESS)
			return rv;

		/* Read it back to be sure it got the right values. */
		rv = tlcl_read(index, sd, len);
		if (rv == TPM_SUCCESS && memcmp(secdata, sd, len) == 0)
			return rv;

		VBDEBUG("TPM: %s() failed. trying again\n", __func__);
		/* Try writing it again. Maybe it was garbled on the way out. */
	}

	VBDEBUG("TPM: %s() - too many failures, giving up\n", __func__);

	return TPM_E_CORRUPTED_STATE;
}

/*
 * This is derived from rollback_index.h of vboot_reference. see struct
 * RollbackSpaceKernel for details.
 */
static const uint8_t secdata_kernel[] = {
	0x02,
	0x4C, 0x57, 0x52, 0x47,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	0xE8,
};

#if IS_ENABLED(CONFIG_TPM2)

/* Nothing special in the TPM2 path yet. */
static uint32_t safe_write(uint32_t index, const void *data, uint32_t length)
{
	return tlcl_write(index, data, length);
}

static uint32_t set_firmware_space(const void *firmware_blob)
{
	RETURN_ON_FAILURE(tlcl_define_space(FIRMWARE_NV_INDEX,
					    VB2_SECDATA_SIZE));
	RETURN_ON_FAILURE(safe_write(FIRMWARE_NV_INDEX, firmware_blob,
				     VB2_SECDATA_SIZE));
	return TPM_SUCCESS;
}

static uint32_t set_kernel_space(const void *kernel_blob)
{
	RETURN_ON_FAILURE(tlcl_define_space(KERNEL_NV_INDEX,
					    sizeof(secdata_kernel)));
	RETURN_ON_FAILURE(safe_write(KERNEL_NV_INDEX, kernel_blob,
				     sizeof(secdata_kernel)));
	return TPM_SUCCESS;
}

static uint32_t _factory_initialize_tpm(struct vb2_context *ctx)
{
	RETURN_ON_FAILURE(tlcl_force_clear());
	RETURN_ON_FAILURE(set_firmware_space(ctx->secdata));
	RETURN_ON_FAILURE(set_kernel_space(secdata_kernel));
	return TPM_SUCCESS;
}

uint32_t tpm_clear_and_reenable(void)
{
	VBDEBUG("TPM: Clear and re-enable\n");
	return TPM_SUCCESS;
}

uint32_t antirollback_lock_space_firmware(void)
{
	return tlcl_lock_nv_write(FIRMWARE_NV_INDEX);
}

#else

uint32_t tpm_clear_and_reenable(void)
{
	VBDEBUG("TPM: Clear and re-enable\n");
	RETURN_ON_FAILURE(tlcl_force_clear());
	RETURN_ON_FAILURE(tlcl_set_enable());
	RETURN_ON_FAILURE(tlcl_set_deactivated(0));

	return TPM_SUCCESS;
}

/**
 * Like tlcl_write(), but checks for write errors due to hitting the 64-write
 * limit and clears the TPM when that happens.  This can only happen when the
 * TPM is unowned, so it is OK to clear it (and we really have no choice).
 * This is not expected to happen frequently, but it could happen.
 */

static uint32_t safe_write(uint32_t index, const void *data, uint32_t length)
{
	uint32_t result = tlcl_write(index, data, length);
	if (result == TPM_E_MAXNVWRITES) {
		RETURN_ON_FAILURE(tpm_clear_and_reenable());
		return tlcl_write(index, data, length);
	} else {
		return result;
	}
}

/**
 * Similarly to safe_write(), this ensures we don't fail a DefineSpace because
 * we hit the TPM write limit. This is even less likely to happen than with
 * writes because we only define spaces once at initialization, but we'd
 * rather be paranoid about this.
 */
static uint32_t safe_define_space(uint32_t index, uint32_t perm, uint32_t size)
{
	uint32_t result = tlcl_define_space(index, perm, size);
	if (result == TPM_E_MAXNVWRITES) {
		RETURN_ON_FAILURE(tpm_clear_and_reenable());
		return tlcl_define_space(index, perm, size);
	} else {
		return result;
	}
}

static uint32_t _factory_initialize_tpm(struct vb2_context *ctx)
{
	TPM_PERMANENT_FLAGS pflags;
	uint32_t result;

	result = tlcl_get_permanent_flags(&pflags);
	if (result != TPM_SUCCESS)
		return result;

	/*
	 * TPM may come from the factory without physical presence finalized.
	 * Fix if necessary.
	 */
	VBDEBUG("TPM: physicalPresenceLifetimeLock=%d\n",
		 pflags.physicalPresenceLifetimeLock);
	if (!pflags.physicalPresenceLifetimeLock) {
		VBDEBUG("TPM: Finalizing physical presence\n");
		RETURN_ON_FAILURE(tlcl_finalize_physical_presence());
	}

	/*
	 * The TPM will not enforce the NV authorization restrictions until the
	 * execution of a TPM_NV_DefineSpace with the handle of
	 * TPM_NV_INDEX_LOCK.  Here we create that space if it doesn't already
	 * exist. */
	VBDEBUG("TPM: nvLocked=%d\n", pflags.nvLocked);
	if (!pflags.nvLocked) {
		VBDEBUG("TPM: Enabling NV locking\n");
		RETURN_ON_FAILURE(tlcl_set_nv_locked());
	}

	/* Clear TPM owner, in case the TPM is already owned for some reason. */
	VBDEBUG("TPM: Clearing owner\n");
	RETURN_ON_FAILURE(tpm_clear_and_reenable());

	/* Define the backup space. No need to initialize it, though. */
	RETURN_ON_FAILURE(safe_define_space(BACKUP_NV_INDEX,
					    TPM_NV_PER_PPWRITE,
					    VB2_NVDATA_SIZE));

	/* Define and initialize the kernel space */
	RETURN_ON_FAILURE(safe_define_space(KERNEL_NV_INDEX,
					    TPM_NV_PER_PPWRITE,
					    sizeof(secdata_kernel)));
	RETURN_ON_FAILURE(write_secdata(KERNEL_NV_INDEX,
					secdata_kernel,
					sizeof(secdata_kernel)));

	/* Defines and sets vb2 secdata space */
	vb2api_secdata_create(ctx);
	RETURN_ON_FAILURE(safe_define_space(FIRMWARE_NV_INDEX,
	                                    TPM_NV_PER_GLOBALLOCK |
	                                    TPM_NV_PER_PPWRITE,
	                                    VB2_SECDATA_SIZE));
	RETURN_ON_FAILURE(write_secdata(FIRMWARE_NV_INDEX,
					ctx->secdata,
					VB2_SECDATA_SIZE));
	return TPM_SUCCESS;
}

uint32_t antirollback_lock_space_firmware(void)
{
	return tlcl_set_global_lock();
}
#endif

uint32_t factory_initialize_tpm(struct vb2_context *ctx)
{
	uint32_t result;

	/* Defines and sets vb2 secdata space */
	vb2api_secdata_create(ctx);

	VBDEBUG("TPM: factory initialization\n");

	/*
	 * Do a full test.  This only happens the first time the device is
	 * turned on in the factory, so performance is not an issue.  This is
	 * almost certainly not necessary, but it gives us more confidence
	 * about some code paths below that are difficult to
	 * test---specifically the ones that set lifetime flags, and are only
	 * executed once per physical TPM.
	 */
	result = tlcl_self_test_full();
	if (result != TPM_SUCCESS)
		return result;

	result = _factory_initialize_tpm(ctx);
	if (result != TPM_SUCCESS)
		return result;

	VBDEBUG("TPM: factory initialization successful\n");

	return TPM_SUCCESS;
}

/*
 * SetupTPM starts the TPM and establishes the root of trust for the
 * anti-rollback mechanism.  SetupTPM can fail for three reasons.  1 A bug. 2 a
 * TPM hardware failure. 3 An unexpected TPM state due to some attack.  In
 * general we cannot easily distinguish the kind of failure, so our strategy is
 * to reboot in recovery mode in all cases.  The recovery mode calls SetupTPM
 * again, which executes (almost) the same sequence of operations.  There is a
 * good chance that, if recovery mode was entered because of a TPM failure, the
 * failure will repeat itself.  (In general this is impossible to guarantee
 * because we have no way of creating the exact TPM initial state at the
 * previous boot.)  In recovery mode, we ignore the failure and continue, thus
 * giving the recovery kernel a chance to fix things (that's why we don't set
 * bGlobalLock).  The choice is between a knowingly insecure device and a
 * bricked device.
 *
 * As a side note, observe that we go through considerable hoops to avoid using
 * the STCLEAR permissions for the index spaces.  We do this to avoid writing
 * to the TPM flashram at every reboot or wake-up, because of concerns about
 * the durability of the NVRAM.
 */
uint32_t setup_tpm(struct vb2_context *ctx)
{
	uint8_t disable;
	uint8_t deactivated;
	uint32_t result;

	RETURN_ON_FAILURE(tlcl_lib_init());

	/* Handle special init for S3 resume path */
	if (ctx->flags & VB2_CONTEXT_S3_RESUME) {
		result = tlcl_resume();
		if (result == TPM_E_INVALID_POSTINIT)
			printk(BIOS_DEBUG, "TPM: Already initialized.\n");
		return TPM_SUCCESS;
	}

#ifdef TEGRA_SOFT_REBOOT_WORKAROUND
	result = tlcl_startup();
	if (result == TPM_E_INVALID_POSTINIT) {
		/*
		 * Some prototype hardware doesn't reset the TPM on a CPU
		 * reset.  We do a hard reset to get around this.
		 */
		VBDEBUG("TPM: soft reset detected\n", result);
		ctx->flags |= VB2_CONTEXT_SECDATA_WANTS_REBOOT;
		return TPM_E_MUST_REBOOT;
	} else if (result != TPM_SUCCESS) {
		VBDEBUG("TPM: tlcl_startup returned %08x\n", result);
		return result;
	}
#else
	RETURN_ON_FAILURE(tlcl_startup());
#endif

	/*
	 * Some TPMs start the self test automatically at power on. In that case
	 * we don't need to call ContinueSelfTest. On some (other) TPMs,
	 * continue_self_test may block. In that case, we definitely don't want
	 * to call it here. For TPMs in the intersection of these two sets, we
	 * are screwed. (In other words: TPMs that require manually starting the
	 * self-test AND block will have poor performance until we split
	 * tlcl_send_receive() into send() and receive(), and have a state
	 * machine to control setup.)
	 *
	 * This comment is likely to become obsolete in the near future, so
	 * don't trust it. It may have not been updated.
	 */
#ifdef TPM_MANUAL_SELFTEST
#ifdef TPM_BLOCKING_CONTINUESELFTEST
#warning "lousy TPM!"
#endif
	RETURN_ON_FAILURE(tlcl_continue_self_test());
#endif
	result = tlcl_assert_physical_presence();
	if (result != TPM_SUCCESS) {
		/*
		 * It is possible that the TPM was delivered with the physical
		 * presence command disabled.  This tries enabling it, then
		 * tries asserting PP again.
		 */
		RETURN_ON_FAILURE(tlcl_physical_presence_cmd_enable());
		RETURN_ON_FAILURE(tlcl_assert_physical_presence());
	}

	/* Check that the TPM is enabled and activated. */
	RETURN_ON_FAILURE(tlcl_get_flags(&disable, &deactivated, NULL));
	if (disable || deactivated) {
		VBDEBUG("TPM: disabled (%d) or deactivated (%d). Fixing...\n",
			disable, deactivated);
		RETURN_ON_FAILURE(tlcl_set_enable());
		RETURN_ON_FAILURE(tlcl_set_deactivated(0));
		VBDEBUG("TPM: Must reboot to re-enable\n");
		ctx->flags |= VB2_CONTEXT_SECDATA_WANTS_REBOOT;
		return TPM_E_MUST_REBOOT;
	}

	VBDEBUG("TPM: SetupTPM() succeeded\n");
	return TPM_SUCCESS;
}

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	uint32_t rv;

	rv = setup_tpm(ctx);
	if (rv)
		return rv;

	/* Read the firmware space. */
	rv = read_space_firmware(ctx);
	if (rv == TPM_E_BADINDEX) {
		/*
		 * This seems the first time we've run. Initialize the TPM.
		 */
		VBDEBUG("TPM: Not initialized yet.\n");
		RETURN_ON_FAILURE(factory_initialize_tpm(ctx));
	} else if (rv != TPM_SUCCESS) {
		VBDEBUG("TPM: Firmware space in a bad state; giving up.\n");
		//RETURN_ON_FAILURE(factory_initialize_tpm(ctx));
		return TPM_E_CORRUPTED_STATE;
	}

	return TPM_SUCCESS;
}

uint32_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	return write_secdata(FIRMWARE_NV_INDEX, ctx->secdata, VB2_SECDATA_SIZE);
}
