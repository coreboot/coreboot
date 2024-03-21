/* SPDX-License-Identifier: BSD-3-Clause */

#include <security/tpm/tspi.h>
#include <security/tpm/tss.h>
#include <security/vboot/antirollback.h>
#include <vb2_api.h>

#include "secdata_tpm_private.h"

/**
 * Similarly to safe_write(), this ensures we don't fail a DefineSpace because
 * we hit the TPM write limit. This is even less likely to happen than with
 * writes because we only define spaces once at initialization, but we'd
 * rather be paranoid about this.
 */
static tpm_result_t safe_define_space(uint32_t index, uint32_t perm, uint32_t size)
{
	tpm_result_t rc = tlcl1_define_space(index, perm, size);
	if (rc == TPM_MAXNVWRITES) {
		RETURN_ON_FAILURE(tpm_clear_and_reenable());
		return tlcl1_define_space(index, perm, size);
	} else {
		return rc;
	}
}

tpm_result_t factory_initialize_tpm1(struct vb2_context *ctx)
{
	TPM_PERMANENT_FLAGS pflags;
	tpm_result_t rc;

	vb2api_secdata_firmware_create(ctx);
	vb2api_secdata_kernel_create_v0(ctx);

	rc = tlcl1_get_permanent_flags(&pflags);
	if (rc != TPM_SUCCESS)
		return rc;

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
		RETURN_ON_FAILURE(tlcl1_set_nv_locked());
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
