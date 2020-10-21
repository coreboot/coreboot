/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <security/vboot/antirollback.h>
#include <security/vboot/tpm_common.h>
#include <security/tpm/tspi.h>
#include <security/tpm/tss.h>
#include <security/tpm/tss/tcg-1.2/tss_structures.h>
#include <vb2_api.h>
#include <console/console.h>

#define VBDEBUG(format, args...) \
	printk(BIOS_INFO, "%s():%d: " format,  __func__, __LINE__, ## args)

#define RETURN_ON_FAILURE(tpm_cmd) do {				\
		uint32_t result_;					\
		if ((result_ = (tpm_cmd)) != TPM_SUCCESS) {		\
			VBDEBUG("Antirollback: %08x returned by " #tpm_cmd \
				 "\n", (int)result_);			\
			return result_;					\
		}							\
	} while (0)

static uint32_t safe_write(uint32_t index, const void *data, uint32_t length);

static uint32_t read_space_firmware(struct vb2_context *ctx)
{
	RETURN_ON_FAILURE(tlcl_read(FIRMWARE_NV_INDEX,
				    ctx->secdata_firmware,
				    VB2_SECDATA_FIRMWARE_SIZE));
	return TPM_SUCCESS;
}

uint32_t antirollback_read_space_kernel(struct vb2_context *ctx)
{
	if (!CONFIG(TPM2)) {
		/*
		 * Before reading the kernel space, verify its permissions. If
		 * the kernel space has the wrong permission, we give up. This
		 * will need to be fixed by the recovery kernel. We will have
		 * to worry about this because at any time (even with PP turned
		 * off) the TPM owner can remove and redefine a PP-protected
		 * space (but not write to it).
		 */
		uint32_t perms;

		RETURN_ON_FAILURE(tlcl_get_permissions(KERNEL_NV_INDEX,
						       &perms));
		if (perms != TPM_NV_PER_PPWRITE) {
			printk(BIOS_ERR,
			       "TPM: invalid secdata_kernel permissions\n");
			return TPM_E_CORRUPTED_STATE;
		}
	}

	uint8_t size = VB2_SECDATA_KERNEL_MIN_SIZE;

	RETURN_ON_FAILURE(tlcl_read(KERNEL_NV_INDEX, ctx->secdata_kernel,
				    size));

	if (vb2api_secdata_kernel_check(ctx, &size)
	    == VB2_ERROR_SECDATA_KERNEL_INCOMPLETE)
		/* Re-read. vboot will run the check and handle errors. */
		RETURN_ON_FAILURE(tlcl_read(KERNEL_NV_INDEX,
					    ctx->secdata_kernel, size));

	return TPM_SUCCESS;
}

#if CONFIG(TPM2)

static uint32_t read_space_mrc_hash(uint32_t index, uint8_t *data)
{
	RETURN_ON_FAILURE(tlcl_read(index, data,
				    HASH_NV_SIZE));
	return TPM_SUCCESS;
}

/*
 * This is used to initialize the TPM space for recovery hash after defining
 * it. Since there is no data available to calculate hash at the point where TPM
 * space is defined, initialize it to all 0s.
 */
static const uint8_t mrc_hash_data[HASH_NV_SIZE] = { };

/*
 * Different sets of NVRAM space attributes apply to the "ro" spaces,
 * i.e. those which should not be possible to delete or modify once
 * the RO exits, and the rest of the NVRAM spaces.
 */
static const TPMA_NV ro_space_attributes = {
	.TPMA_NV_PPWRITE = 1,
	.TPMA_NV_AUTHREAD = 1,
	.TPMA_NV_PPREAD = 1,
	.TPMA_NV_PLATFORMCREATE = 1,
	.TPMA_NV_WRITE_STCLEAR = 1,
	.TPMA_NV_POLICY_DELETE = 1,
};

static const TPMA_NV rw_space_attributes = {
	.TPMA_NV_PPWRITE = 1,
	.TPMA_NV_AUTHREAD = 1,
	.TPMA_NV_PPREAD = 1,
	.TPMA_NV_PLATFORMCREATE = 1,
};

/*
 * This policy digest was obtained using TPM2_PolicyPCR
 * selecting only PCR_0 with a value of all zeros.
 */
static const uint8_t pcr0_unchanged_policy[] = {
	0x09, 0x93, 0x3C, 0xCE, 0xEB, 0xB4, 0x41, 0x11, 0x18, 0x81, 0x1D,
	0xD4, 0x47, 0x78, 0x80, 0x08, 0x88, 0x86, 0x62, 0x2D, 0xD7, 0x79,
	0x94, 0x46, 0x62, 0x26, 0x68, 0x8E, 0xEE, 0xE6, 0x6A, 0xA1};

/* Nothing special in the TPM2 path yet. */
static uint32_t safe_write(uint32_t index, const void *data, uint32_t length)
{
	return tlcl_write(index, data, length);
}

static uint32_t set_space(const char *name, uint32_t index, const void *data,
			  uint32_t length, const TPMA_NV nv_attributes,
			  const uint8_t *nv_policy, size_t nv_policy_size)
{
	uint32_t rv;

	rv = tlcl_define_space(index, length, nv_attributes, nv_policy,
			       nv_policy_size);
	if (rv == TPM_E_NV_DEFINED) {
		/*
		 * Continue with writing: it may be defined, but not written
		 * to. In that case a subsequent tlcl_read() would still return
		 * TPM_E_BADINDEX on TPM 2.0. The cases when some non-firmware
		 * space is defined while the firmware space is not there
		 * should be rare (interrupted initialization), so no big harm
		 * in writing once again even if it was written already.
		 */
		VBDEBUG("%s: %s space already exists\n", __func__, name);
		rv = TPM_SUCCESS;
	}

	if (rv != TPM_SUCCESS)
		return rv;

	return safe_write(index, data, length);
}

static uint32_t set_firmware_space(const void *firmware_blob)
{
	return set_space("firmware", FIRMWARE_NV_INDEX, firmware_blob,
			 VB2_SECDATA_FIRMWARE_SIZE, ro_space_attributes,
			 pcr0_unchanged_policy, sizeof(pcr0_unchanged_policy));
}

static uint32_t set_kernel_space(const void *kernel_blob)
{
	return set_space("kernel", KERNEL_NV_INDEX, kernel_blob,
			 VB2_SECDATA_KERNEL_SIZE, rw_space_attributes, NULL, 0);
}

static uint32_t set_mrc_hash_space(uint32_t index, const uint8_t *data)
{
	if (index == MRC_REC_HASH_NV_INDEX) {
		return set_space("RO MRC Hash", index, data, HASH_NV_SIZE,
				 ro_space_attributes, pcr0_unchanged_policy,
				 sizeof(pcr0_unchanged_policy));
	} else {
		return set_space("RW MRC Hash", index, data, HASH_NV_SIZE,
				 rw_space_attributes, NULL, 0);
	}
}

static uint32_t _factory_initialize_tpm(struct vb2_context *ctx)
{
	vb2api_secdata_kernel_create(ctx);

	RETURN_ON_FAILURE(tlcl_force_clear());

	/*
	 * Of all NVRAM spaces defined by this function the firmware space
	 * must be defined last, because its existence is considered an
	 * indication that TPM factory initialization was successfully
	 * completed.
	 */
	RETURN_ON_FAILURE(set_kernel_space(ctx->secdata_kernel));

	/*
	 * Define and set rec hash space, if available.  No need to
	 * create the RW hash space because we will definitely boot
	 * once in normal mode before shipping, meaning that the space
	 * will get created with correct permissions while still in in
	 * our hands.
	 */
	if (CONFIG(VBOOT_HAS_REC_HASH_SPACE))
		RETURN_ON_FAILURE(set_mrc_hash_space(MRC_REC_HASH_NV_INDEX, mrc_hash_data));

	RETURN_ON_FAILURE(set_firmware_space(ctx->secdata_firmware));

	return TPM_SUCCESS;
}

uint32_t antirollback_lock_space_firmware(void)
{
	return tlcl_lock_nv_write(FIRMWARE_NV_INDEX);
}

uint32_t antirollback_read_space_mrc_hash(uint32_t index, uint8_t *data, uint32_t size)
{
	if (size != HASH_NV_SIZE) {
		VBDEBUG("TPM: Incorrect buffer size for hash idx 0x%x. "
			"(Expected=0x%x Actual=0x%x).\n", index, HASH_NV_SIZE,
			size);
		return TPM_E_READ_FAILURE;
	}
	return read_space_mrc_hash(index, data);
}

uint32_t antirollback_write_space_mrc_hash(uint32_t index, const uint8_t *data, uint32_t size)
{
	uint8_t spc_data[HASH_NV_SIZE];
	uint32_t rv;

	if (size != HASH_NV_SIZE) {
		VBDEBUG("TPM: Incorrect buffer size for hash idx 0x%x. "
			"(Expected=0x%x Actual=0x%x).\n", index, HASH_NV_SIZE,
			size);
		return TPM_E_WRITE_FAILURE;
	}

	rv = read_space_mrc_hash(index, spc_data);
	if (rv == TPM_E_BADINDEX) {
		/*
		 * If space is not defined already for hash, define
		 * new space.
		 */
		VBDEBUG("TPM: Initializing hash space.\n");
		return set_mrc_hash_space(index, data);
	}

	if (rv != TPM_SUCCESS)
		return rv;

	return safe_write(index, data, size);
}

uint32_t antirollback_lock_space_mrc_hash(uint32_t index)
{
	return tlcl_lock_nv_write(index);
}

#else

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

	vb2api_secdata_kernel_create_v0(ctx);

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

	/* Define and write secdata_kernel space. */
	RETURN_ON_FAILURE(safe_define_space(KERNEL_NV_INDEX,
					    TPM_NV_PER_PPWRITE,
					    VB2_SECDATA_KERNEL_SIZE_V02));
	RETURN_ON_FAILURE(safe_write(KERNEL_NV_INDEX,
				     ctx->secdata_kernel,
				     VB2_SECDATA_KERNEL_SIZE_V02));

	/* Define and write secdata_firmware space. */
	RETURN_ON_FAILURE(safe_define_space(FIRMWARE_NV_INDEX,
					    TPM_NV_PER_GLOBALLOCK |
					    TPM_NV_PER_PPWRITE,
					    VB2_SECDATA_FIRMWARE_SIZE));
	RETURN_ON_FAILURE(safe_write(FIRMWARE_NV_INDEX,
				     ctx->secdata_firmware,
					VB2_SECDATA_FIRMWARE_SIZE));

	return TPM_SUCCESS;
}

uint32_t antirollback_lock_space_firmware(void)
{
	return tlcl_set_global_lock();
}

#endif

/**
 * Perform one-time initializations.
 *
 * Create the NVRAM spaces, and set their initial values as needed.  Sets the
 * nvLocked bit and ensures the physical presence command is enabled and
 * locked.
 */
static uint32_t factory_initialize_tpm(struct vb2_context *ctx)
{
	uint32_t result;

	/*
	 * Set initial values of secdata_firmware space.
	 * kernel space is created in _factory_initialize_tpm().
	 */
	vb2api_secdata_firmware_create(ctx);

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

	/* _factory_initialize_tpm() writes initial secdata values to TPM
	   immediately, so let vboot know that it's up to date now. */
	ctx->flags &= ~(VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED |
			VB2_CONTEXT_SECDATA_KERNEL_CHANGED);

	VBDEBUG("TPM: factory initialization successful\n");

	return TPM_SUCCESS;
}

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	uint32_t rv;

	/* Read the firmware space. */
	rv = read_space_firmware(ctx);
	if (rv == TPM_E_BADINDEX) {
		/* This seems the first time we've run. Initialize the TPM. */
		VBDEBUG("TPM: Not initialized yet.\n");
		RETURN_ON_FAILURE(factory_initialize_tpm(ctx));
	} else if (rv != TPM_SUCCESS) {
		VBDEBUG("TPM: Firmware space in a bad state; giving up.\n");
		return TPM_E_CORRUPTED_STATE;
	}

	return TPM_SUCCESS;
}

uint32_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	if (CONFIG(CR50_IMMEDIATELY_COMMIT_FW_SECDATA))
		tlcl_cr50_enable_nvcommits();
	return safe_write(FIRMWARE_NV_INDEX, ctx->secdata_firmware,
			  VB2_SECDATA_FIRMWARE_SIZE);
}

uint32_t antirollback_write_space_kernel(struct vb2_context *ctx)
{
	/* Learn the expected size. */
	uint8_t size = VB2_SECDATA_KERNEL_MIN_SIZE;
	vb2api_secdata_kernel_check(ctx, &size);

	/*
	 * Ensure that the TPM actually commits our changes to NVMEN in case
	 * there is a power loss or other unexpected event. The AP does not
	 * write to the TPM during normal boot flow; it only writes during
	 * recovery, software sync, or other special boot flows. When the AP
	 * wants to write, it is imporant to actually commit changes.
	 */
	if (CONFIG(CR50_IMMEDIATELY_COMMIT_FW_SECDATA))
		tlcl_cr50_enable_nvcommits();

	return safe_write(KERNEL_NV_INDEX, ctx->secdata_kernel, size);
}

vb2_error_t vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	uint32_t rv;
	printk(BIOS_INFO, "Clearing TPM owner\n");
	rv = tpm_clear_and_reenable();
	if (rv)
		return VB2_ERROR_EX_TPM_CLEAR_OWNER;
	return VB2_SUCCESS;
}
