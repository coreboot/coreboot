/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <security/tpm/tspi.h>
#include <vb2_api.h>

#include "antirollback.h"

vb2_error_t vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	vb2api_secdata_firmware_create(ctx);
	return VB2_SUCCESS;
}

vb2_error_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_read_space_kernel(struct vb2_context *ctx)
{
	/*
	 * The new kernel secdata v1 stores the last read EC hash, and reboots the
	 * device during EC software sync when that hash didn't match the currently
	 * active hash on the EC (this is used with TPM_GOOGLE to support EC-EFS2 and
	 * pretty much a no-op for other devices). Generally, of course the whole
	 * point of secdata is always that it persists across reboots, but with
	 * MOCK_SECDATA we can't do that. Previously we always happened to somewhat
	 * get away with presenting freshly-reinitialized data for MOCK_SECDATA on
	 * every boot, but with the EC hash feature in secdata v1, that would cause
	 * a reboot loop. The simplest solution is to just pretend we're a secdata
	 * v0 device when using MOCK_SECDATA.
	 */
	vb2api_secdata_kernel_create_v0(ctx);
	return VB2_SUCCESS;
}

vb2_error_t antirollback_write_space_kernel(struct vb2_context *ctx)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_lock_space_firmware(void)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_lock_space_mrc_hash(uint32_t index)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_read_space_mrc_hash(uint32_t index, uint8_t *data, uint32_t size)
{
	return VB2_SUCCESS;
}

vb2_error_t antirollback_write_space_mrc_hash(uint32_t index, const uint8_t *data,
					      uint32_t size)
{
	return VB2_SUCCESS;
}
