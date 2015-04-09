/* Copyright (c) 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <antirollback.h>
#include <stdlib.h>
#include <vb2_api.h>

uint32_t tpm_extend_pcr(struct vb2_context *ctx, int pcr,
			enum vb2_pcr_digest which_digest)
{
	return TPM_SUCCESS;
}

uint32_t tpm_clear_and_reenable(void)
{
	return TPM_SUCCESS;
}

uint32_t safe_write(uint32_t index, const void *data, uint32_t length)
{
	return TPM_SUCCESS;
}

uint32_t safe_define_space(uint32_t index, uint32_t perm, uint32_t size)
{
	return TPM_SUCCESS;
}

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	vb2api_secdata_create(ctx);
	return TPM_SUCCESS;
}

uint32_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	return TPM_SUCCESS;
}

uint32_t antirollback_lock_space_firmware()
{
	return TPM_SUCCESS;
}
