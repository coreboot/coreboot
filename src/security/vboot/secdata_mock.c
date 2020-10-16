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
	vb2api_secdata_kernel_create(ctx);
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
