/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <console/console.h>
#include <security/tpm/tspi.h>
#include <security/vboot/antirollback.h>
#include <vb2_api.h>

#include "secdata_tpm_private.h"

tpm_result_t antirollback_read_space_kernel(struct vb2_context *ctx)
{
	if (tlcl_get_family() == TPM_1) {
		/*
		 * Before reading the kernel space, verify its permissions. If
		 * the kernel space has the wrong permission, we give up. This
		 * will need to be fixed by the recovery kernel. We will have
		 * to worry about this because at any time (even with PP turned
		 * off) the TPM owner can remove and redefine a PP-protected
		 * space (but not write to it).
		 */
		uint32_t perms;

		RETURN_ON_FAILURE(tlcl1_get_permissions(KERNEL_NV_INDEX, &perms));
		if (perms != TPM_NV_PER_PPWRITE) {
			printk(BIOS_ERR,
			       "TPM: invalid secdata_kernel permissions\n");
			return TPM_CB_CORRUPTED_STATE;
		}
	}

	uint8_t size = VB2_SECDATA_KERNEL_SIZE;
	tpm_result_t rc;

	/* Start with the version 1.0 size used by all modern Cr50/Ti50 boards. */
	rc = tlcl_read(KERNEL_NV_INDEX, ctx->secdata_kernel, size);
	if (rc == TPM_CB_RANGE) {
		/* Fallback to version 0.2(minimum) size and re-read. */
		VBDEBUG("Antirollback: NV read out of range, trying min size\n");
		size = VB2_SECDATA_KERNEL_MIN_SIZE;
		rc = tlcl_read(KERNEL_NV_INDEX, ctx->secdata_kernel, size);
	}
	RETURN_ON_FAILURE(rc);

	if (vb2api_secdata_kernel_check(ctx, &size) == VB2_ERROR_SECDATA_KERNEL_INCOMPLETE)
		/* Re-read. vboot will run the check and handle errors. */
		RETURN_ON_FAILURE(tlcl_read(KERNEL_NV_INDEX, ctx->secdata_kernel, size));

	return TPM_SUCCESS;
}

tpm_result_t safe_write(uint32_t index, const void *data, uint32_t length)
{
	tpm_result_t rc = tlcl_write(index, data, length);
	if (tlcl_get_family() == TPM_1 && rc == TPM_MAXNVWRITES) {
		/**
		 * Clear the TPM on write error due to hitting the 64-write
		 * limit.  This can only happen when the TPM is unowned, so it
		 * is OK to clear it (and we really have no choice).  This is
		 * not expected to happen frequently, but it could happen.
		 */
		RETURN_ON_FAILURE(tpm_clear_and_reenable());
		rc = tlcl_write(index, data, length);
	}
	return rc;
}

static uint32_t _factory_initialize_tpm(struct vb2_context *ctx)
{
	if (tlcl_get_family() == TPM_1)
		return factory_initialize_tpm1(ctx);
	if (tlcl_get_family() == TPM_2)
		return factory_initialize_tpm2(ctx);
	return TPM_CB_CORRUPTED_STATE;
}

uint32_t antirollback_lock_space_firmware(void)
{
	if (tlcl_get_family() == TPM_1)
		return tlcl1_set_global_lock();
	if (tlcl_get_family() == TPM_2)
		return tlcl2_lock_nv_write(FIRMWARE_NV_INDEX);
	return TPM_CB_CORRUPTED_STATE;
}

/**
 * Perform one-time initializations.
 *
 * Create the NVRAM spaces, and set their initial values as needed.  Sets the
 * nvLocked bit and ensures the physical presence command is enabled and
 * locked.
 */
static tpm_result_t factory_initialize_tpm(struct vb2_context *ctx)
{
	tpm_result_t rc;

	VBDEBUG("TPM: factory initialization\n");

	/*
	 * Do a full test.  This only happens the first time the device is
	 * turned on in the factory, so performance is not an issue.  This is
	 * almost certainly not necessary, but it gives us more confidence
	 * about some code paths below that are difficult to
	 * test---specifically the ones that set lifetime flags, and are only
	 * executed once per physical TPM.
	 */
	rc = tlcl_self_test_full();
	if (rc != TPM_SUCCESS)
		return rc;

	rc = _factory_initialize_tpm(ctx);
	if (rc != TPM_SUCCESS)
		return rc;

	/* _factory_initialize_tpm() writes initial secdata values to TPM
	   immediately, so let vboot know that it's up to date now. */
	ctx->flags &= ~(VB2_CONTEXT_SECDATA_FIRMWARE_CHANGED |
			VB2_CONTEXT_SECDATA_KERNEL_CHANGED);

	VBDEBUG("TPM: factory initialization successful\n");

	return TPM_SUCCESS;
}

tpm_result_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	tpm_result_t rc;

	rc = tlcl_read(FIRMWARE_NV_INDEX, ctx->secdata_firmware, VB2_SECDATA_FIRMWARE_SIZE);
	if (rc == TPM_BADINDEX) {
		/* This seems the first time we've run. Initialize the TPM. */
		VBDEBUG("TPM: Not initialized yet\n");
		RETURN_ON_FAILURE(factory_initialize_tpm(ctx));
	} else if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Failed to read firmware space: %#x\n", rc);
		return TPM_CB_CORRUPTED_STATE;
	}

	return rc;
}

tpm_result_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	if (CONFIG(TPM_GOOGLE_IMMEDIATELY_COMMIT_FW_SECDATA))
		tlcl_cr50_enable_nvcommits();
	return safe_write(FIRMWARE_NV_INDEX, ctx->secdata_firmware,
			  VB2_SECDATA_FIRMWARE_SIZE);
}

tpm_result_t antirollback_write_space_kernel(struct vb2_context *ctx)
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
	if (CONFIG(TPM_GOOGLE_IMMEDIATELY_COMMIT_FW_SECDATA))
		tlcl_cr50_enable_nvcommits();

	return safe_write(KERNEL_NV_INDEX, ctx->secdata_kernel, size);
}

vb2_error_t vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	printk(BIOS_INFO, "Clearing TPM owner\n");
	return tpm_clear_and_reenable() == TPM_SUCCESS ? VB2_SUCCESS : VB2_ERROR_EX_TPM_CLEAR_OWNER;
}
